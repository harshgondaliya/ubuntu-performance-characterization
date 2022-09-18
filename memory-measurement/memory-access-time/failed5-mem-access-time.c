#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
// arr size must be at least the stride
// stride must be between 32 (64b) to 128 (512b)
#define MIN_STRIDE 128
#define MAX_STRIDE 4194304
#define STRIDE_INCREMENT 1
// arr size can be from 128 to 8000000
#define MIN_ARR_SIZE 512 // 1 kb array. then we keep shifting by 2. 2^10, 2^11, 2^12....
#define MAX_ARR_SIZE 33554432
#define ARR_SIZE_INCREMENT 4096

#define LOOP_OVERHEAD 5.8
#define READ_OVERHEAD 37.1
#define EXTRA_INS_OVERHEAD 1.6
#define NO_OF_INS 1

unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time;
FILE *outputFile;

int *ptr[10000000];
void main(){    
    // warm instruction cache
    asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");

    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 "CPUID\n\t"
                 : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                 "%rbx", "%rcx", "%rdx");
    
    asm volatile ("CPUID\n\t"
                  "RDTSC\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                  : "=r" (cycles_high0), "=r" (cycles_low0)::
                  "%rax", "%rbx", "%rcx", "%rdx");
    
    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 "CPUID\n\t"
                 : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                 "%rbx", "%rcx", "%rdx");
   
    outputFile = fopen("memory-access-time.txt", "w");
    int read;
    int x;
    //for(int stride=MIN_STRIDE; stride<=MAX_STRIDE; stride = stride << STRIDE_INCREMENT){
        for(int arr_size=MIN_ARR_SIZE; arr_size<=MAX_ARR_SIZE; arr_size=arr_size + ARR_SIZE_INCREMENT){
            int *arr;
            int no_of_access=0;
            int stride;
            double e = log2((double)arr_size);
            // if( e <= 5){
            //     stride = arr_size >> 3;
            // }else if(e <= 10){
            //     stride = arr_size >> 5;
            // }else if(e <= 15){
            //     stride = arr_size >> 10;
            // }else if(e <= 20){
            //     stride = arr_size >> 15;
            // } else if(e <= 25){
            //     stride = arr_size >> 20;
            // } else if(e <= 30){
            //     stride = arr_size >> 25;
            // }
            stride = arr_size >> 6;
            //printf("array size: %d\n", arr_size);
            arr = (int *)malloc(arr_size * sizeof(int));
            if(arr == NULL){printf("Unable to allocate memory\n");}
            //int arr[arr_size];
            //printf("allocated array of size %d\n", arr_size);
            for(int k=0; k<arr_size; k++){
                arr[k] = 1;
            }
            //int no_of_access = arr_size>>7;
            int random;
            for(int k=0; k<arr_size-stride; k+=stride){
                random = k + (rand()%stride);
                // printf("k: %d\nrandom: %d\n",k,random);
                ptr[no_of_access] = &arr[random];
                no_of_access++;
            }
            int batch_access = no_of_access/NO_OF_INS;
            // int temp = arr[0];
            // temp+=1;
            // int temp2 = arr[1];
            // temp2+=1;
            // int temp3 = arr[2];
            // temp3+=1;
            // int temp4 = arr[3];
            // temp4+=1;
            // int temp5 = arr[4];
            // temp5+=1;
            // int temp6 = arr[5];
            // temp6+=1;
            // printf("initialized pointer list");
            asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
            // printf("entering access loop\n");
            for(int access=0; access<batch_access; access = access + NO_OF_INS){
                // printf("accessing %d\n", access);
                read = *ptr[access];
                x = read  + 1;
                // read = *ptr[access+1];
                // x = read  + 1;
                // read = *ptr[access+2];
                // x = read  + 1;
                // read = *ptr[access+3];
                // x = read  + 1;
                // read = *ptr[access+4];
                // x = read  + 1;
                // read = *ptr[access+5];
                // x = read  + 1;
                // printf("accessed %d\n", access);
            }
            // printf("exiting access loop\n");
            asm volatile("RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t"
                        : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                        "%rbx", "%rcx", "%rdx");
            x++;
            start_time = ( ((__uint64_t)cycles_high0 << 32) | cycles_low0 );
            end_time = ( ((__uint64_t)cycles_high1 << 32) | cycles_low1 );
            if(end_time - start_time < 0){
                printf("Critical Error in Taking Measurement");
            } else{
                double time = end_time - start_time;
                double time_minus_overhead = time - ((batch_access) * LOOP_OVERHEAD) - READ_OVERHEAD - ((batch_access) * EXTRA_INS_OVERHEAD * NO_OF_INS);
                // printf("noting the measurement timestamp\n");
                if(no_of_access > 0){ // time_minus_overhead/(batch_access*6)
                fprintf(outputFile, "%ld, %ld, %lf, %d, %d\n", (long int)stride*4, (long int)arr_size*4, time_minus_overhead/(batch_access*NO_OF_INS), batch_access, no_of_access);
            	}
            }
            free(arr);
        }
    //}
    fclose(outputFile);
}
