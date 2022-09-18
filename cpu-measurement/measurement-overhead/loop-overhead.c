#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define NO_OF_TRIALS 500
unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total[NO_OF_TRIALS];
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

    for(int i=0;i<NO_OF_TRIALS;i++){
        asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
        for(int j=0; j<1; j++){ }
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
            total[i] = (end_time - start_time);
        }        
    }
    outputFile = fopen("loop-overhead.txt", "w");
    double sum=0, mean=0, sum_squared_diff=0, stddev=0;
    for(int i=0; i<NO_OF_TRIALS; i++){
        fprintf(outputFile, "%ld\n", total[i]); // reading in each trial
    }
    fclose(outputFile);
    for(int i=0; i<NO_OF_TRIALS; i++){
        sum += total[i];   
    }
    mean = sum/(NO_OF_TRIALS *1.0); // mean computed
    for(int i=0; i<NO_OF_TRIALS; i++){
        sum_squared_diff += pow(total[i] - mean, 2);   
    }
    stddev = sqrt(sum_squared_diff/NO_OF_TRIALS);
    printf("\n\nTrials: %d\nMean Cycles for 1 Loop Iteration: %lf\nStddev: %lf\n", NO_OF_TRIALS, mean, stddev);
}
