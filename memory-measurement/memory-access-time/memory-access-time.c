#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MIN_ARR_SIZE 512 // 1 kb array. then we keep shifting by 2. 2^10, 2^11, 2^12....
#define MAX_ARR_SIZE 134217728
#define ARR_SIZE_INCREMENT 4096

#define READ_OVERHEAD 37.1
#define EXTRA_INS_OVERHEAD 1.6
#define NO_OF_INS 99

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
        for(int arr_size=MIN_ARR_SIZE; arr_size<=MAX_ARR_SIZE; arr_size=arr_size + ARR_SIZE_INCREMENT){
            int *arr;
            int no_of_access=0;
            int stride;
            double e = log2((double)arr_size);
            stride = arr_size / 100;
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
            for(int k=arr_size-1; k>stride; k-=stride){
                random = k - (rand()%stride);
                // printf("k: %d\nrandom: %d\n",k,random);
                ptr[no_of_access] = &arr[random];
                no_of_access++;
            }
            asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
                read = *ptr[0]; x = read  + 1; read = *ptr[1]; x = read  + 1; read = *ptr[2]; x = read  + 1; read = *ptr[3]; x = read  + 1; read = *ptr[4]; x = read  + 1; 
                read = *ptr[5]; x = read  + 1; read = *ptr[6]; x = read  + 1; read = *ptr[7]; x = read  + 1; read = *ptr[8]; x = read  + 1; read = *ptr[9]; x = read  + 1;
                read = *ptr[10]; x = read  + 1; read = *ptr[11]; x = read  + 1; read = *ptr[12]; x = read  + 1; read = *ptr[13]; x = read  + 1; read = *ptr[14]; x = read  + 1; 
                read = *ptr[15]; x = read  + 1; read = *ptr[16]; x = read  + 1; read = *ptr[17]; x = read  + 1; read = *ptr[18]; x = read  + 1; read = *ptr[19]; x = read  + 1;
                read = *ptr[20]; x = read  + 1; read = *ptr[21]; x = read  + 1; read = *ptr[22]; x = read  + 1; read = *ptr[23]; x = read  + 1; read = *ptr[24]; x = read  + 1; 
                read = *ptr[25]; x = read  + 1; read = *ptr[26]; x = read  + 1; read = *ptr[27]; x = read  + 1; read = *ptr[28]; x = read  + 1; read = *ptr[29]; x = read  + 1;
                read = *ptr[30]; x = read  + 1; read = *ptr[31]; x = read  + 1; read = *ptr[32]; x = read  + 1; read = *ptr[33]; x = read  + 1; read = *ptr[34]; x = read  + 1; 
                read = *ptr[35]; x = read  + 1; read = *ptr[36]; x = read  + 1; read = *ptr[37]; x = read  + 1; read = *ptr[38]; x = read  + 1; read = *ptr[39]; x = read  + 1;
                read = *ptr[40]; x = read  + 1; read = *ptr[41]; x = read  + 1; read = *ptr[42]; x = read  + 1; read = *ptr[43]; x = read  + 1; read = *ptr[44]; x = read  + 1; 
                read = *ptr[45]; x = read  + 1; read = *ptr[46]; x = read  + 1; read = *ptr[47]; x = read  + 1; read = *ptr[48]; x = read  + 1; read = *ptr[49]; x = read  + 1;
                read = *ptr[50]; x = read  + 1; read = *ptr[51]; x = read  + 1; read = *ptr[52]; x = read  + 1; read = *ptr[53]; x = read  + 1; read = *ptr[54]; x = read  + 1; 
                read = *ptr[55]; x = read  + 1; read = *ptr[56]; x = read  + 1; read = *ptr[57]; x = read  + 1; read = *ptr[58]; x = read  + 1; read = *ptr[59]; x = read  + 1;
                read = *ptr[60]; x = read  + 1; read = *ptr[61]; x = read  + 1; read = *ptr[62]; x = read  + 1; read = *ptr[63]; x = read  + 1; read = *ptr[64]; x = read  + 1; 
                read = *ptr[65]; x = read  + 1; read = *ptr[66]; x = read  + 1; read = *ptr[67]; x = read  + 1; read = *ptr[68]; x = read  + 1; read = *ptr[69]; x = read  + 1;
                read = *ptr[70]; x = read  + 1; read = *ptr[71]; x = read  + 1; read = *ptr[72]; x = read  + 1; read = *ptr[73]; x = read  + 1; read = *ptr[74]; x = read  + 1; 
                read = *ptr[75]; x = read  + 1; read = *ptr[76]; x = read  + 1; read = *ptr[77]; x = read  + 1; read = *ptr[78]; x = read  + 1; read = *ptr[79]; x = read  + 1;
                read = *ptr[80]; x = read  + 1; read = *ptr[81]; x = read  + 1; read = *ptr[82]; x = read  + 1; read = *ptr[83]; x = read  + 1; read = *ptr[84]; x = read  + 1; 
                read = *ptr[85]; x = read  + 1; read = *ptr[86]; x = read  + 1; read = *ptr[87]; x = read  + 1; read = *ptr[88]; x = read  + 1; read = *ptr[89]; x = read  + 1;
                read = *ptr[90]; x = read  + 1; read = *ptr[91]; x = read  + 1; read = *ptr[92]; x = read  + 1; read = *ptr[93]; x = read  + 1; read = *ptr[94]; x = read  + 1; 
                read = *ptr[95]; x = read  + 1; read = *ptr[96]; x = read  + 1; read = *ptr[97]; x = read  + 1; read = *ptr[98]; x = read  + 1;
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
                double time_minus_overhead = time - READ_OVERHEAD - (EXTRA_INS_OVERHEAD * NO_OF_INS);
                // printf("noting the measurement timestamp\n");
                if(no_of_access > 0){ // time_minus_overhead/(batch_access*6)
                fprintf(outputFile, "%ld, %ld, %lf\n", (long int)stride*4, (long int)arr_size*4, time_minus_overhead/NO_OF_INS);
            	}
            }
            free(arr);
        }
    fclose(outputFile);
}
