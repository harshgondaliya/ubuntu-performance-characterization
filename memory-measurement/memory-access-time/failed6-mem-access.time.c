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
#define MIN_STRIDE 16
#define MAX_STRIDE 262144
#define STRIDE_INCREMENT 1	
// arr size can be from 128 to 8000000
#define MIN_ARR_SIZE 512 // 1 kb array. then we keep shifting by 2. 2^10, 2^11, 2^12....
#define MAX_ARR_SIZE 16777216
#define ARR_SIZE_INCREMENT 1

#define LOOP_OVERHEAD 5.8
#define READ_OVERHEAD 37.1
#define EXTRA_INSTRUCTION_OVERHEAD 27.46
#define NO_OF_ACCESS_PER_ITERATION 1

unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time;
FILE *outputFile;

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
    for(int stride=MIN_STRIDE; stride<=MAX_STRIDE; stride = stride << STRIDE_INCREMENT){
        for(int arr_size=MIN_ARR_SIZE; arr_size<=MAX_ARR_SIZE; arr_size=arr_size << ARR_SIZE_INCREMENT){
            int *arr;
            arr = (int *)malloc(arr_size * sizeof(int));
            for(int k=0; k<arr_size; k++){
                arr[k] = 1;
            }
            asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
            for(int access=1; access<arr_size; access = access + stride){
                read = *(arr  + (rand() % access));
                x = read  + 1;
            }
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
                int no_of_iterations = ((int)(ceil((arr_size-1)/(stride * 1.0))));
                double time_minus_overhead = time - (no_of_iterations * LOOP_OVERHEAD) - READ_OVERHEAD - (EXTRA_INSTRUCTION_OVERHEAD*no_of_iterations);
                fprintf(outputFile, "%d, %d, %lf\n", stride*4, arr_size*4, time_minus_overhead/(no_of_iterations*NO_OF_ACCESS_PER_ITERATION));
            }
            //free(arr);
        }
    }
    fclose(outputFile);
}
