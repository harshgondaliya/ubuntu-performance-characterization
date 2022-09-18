#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
// arr size must be at least the stride
// stride must be between 32 (64b) to 128 (512b)
#define MIN_ARR_SIZE 1024
#define MAX_ARR_SIZE 1073741824
#define ARR_SIZE_INC 1

#define STRIDE 32
#define LOOP_OVERHEAD 5.8
#define READ_OVERHEAD 37.1

unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time;
FILE *outputFile;

void main(){
    for(int i=MIN_ARR_SIZE; i<=MAX_ARR_SIZE; i++){
    	int arr[i];
    	int no_of_access = i/STRIDE;
    	int *ptr[no_of_access];
    }
    int arr[1048576];
    int *ptr[4];
    ptr[0] = &arr[1024];
    ptr[1] = &arr[65536];
    ptr[2] = &arr[49000];
    ptr[3] = &arr[89010];
    //ptr[4] = &arr[86];
    //ptr[5] = &arr[121];
    int x;
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
    asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
    for(int j=0; j<4; j++){
        x = *ptr[j];
    }
    asm volatile("RDTSCP\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "CPUID\n\t"
                : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                "%rbx", "%rcx", "%rdx");
    start_time = ( ((__uint64_t)cycles_high0 << 32) | cycles_low0 );
    end_time = ( ((__uint64_t)cycles_high1 << 32) | cycles_low1 );
    if(end_time - start_time < 0){
        printf("Critical Error in Taking Measurement");
    } else{
        double time = end_time - start_time;
        //double time_minus_overhead = time - (no_of_iterations * LOOP_OVERHEAD) - READ_OVERHEAD - (EXTRA_INSTRUCTION_OVERHEAD*no_of_iterations);
        fprintf(outputFile, "%lf\n", (time - READ_OVERHEAD - (LOOP_OVERHEAD*4))/(4*1.0));
    }
    // for(int stride=MIN_STRIDE; stride<=MAX_STRIDE; stride = stride + STRIDE_INCREMENT){
    //     for(int arr_size=MIN_ARR_SIZE; arr_size<=MAX_ARR_SIZE; arr_size=arr_size + ARR_SIZE_INCREMENT){
    //         int *arr;
    //         arr = (int *)malloc(arr_size * sizeof(int));
    //         for(int k=0; k<arr_size; k++){
    //             arr[k] = 1;
    //         }
    //         asm volatile ("CPUID\n\t"
    //                "RDTSC\n\t"
    //                "mov %%edx, %0\n\t"
    //                "mov %%eax, %1\n\t" 
    //                : "=r" (cycles_high0), "=r" (cycles_low0)::
    //                "%rax", "%rbx", "%rcx", "%rdx");
    //         for(int access=1; access<arr_size; access = access + stride){
    //             read = *(arr  + access + (rand() % stride));
    //             x = read  + 1;
    //         }
    //         asm volatile("RDTSCP\n\t"
    //                     "mov %%edx, %0\n\t"
    //                     "mov %%eax, %1\n\t"
    //                     "CPUID\n\t"
    //                     : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
    //                     "%rbx", "%rcx", "%rdx");
    //         start_time = ( ((__uint64_t)cycles_high0 << 32) | cycles_low0 );
    //         end_time = ( ((__uint64_t)cycles_high1 << 32) | cycles_low1 );
    //         if(end_time - start_time < 0){
    //             printf("Critical Error in Taking Measurement");
    //         } else{
    //             double time = end_time - start_time;
    //             int no_of_iterations = ((int)(ceil((arr_size-1)/(stride * 1.0))));
    //             double time_minus_overhead = time - (no_of_iterations * LOOP_OVERHEAD) - READ_OVERHEAD - (EXTRA_INSTRUCTION_OVERHEAD*no_of_iterations);
    //             fprintf(outputFile, "%d, %d, %lf\n", stride*4, arr_size*4, time_minus_overhead/(no_of_iterations*NO_OF_ACCESS_PER_ITERATION));
    //         }
    //         free(arr);
    //     }
    // }
    fclose(outputFile);
}
