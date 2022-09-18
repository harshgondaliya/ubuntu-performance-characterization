#include <stdio.h>
#include <inttypes.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define NO_OF_TRIALS 200
#define NO_OF_ITERATIONS 10000
unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total[NO_OF_TRIALS];
FILE *outputFile;

void main(){    
    pid_t pid;
    int wait_res;
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
        for(int j=0; j<NO_OF_ITERATIONS; j++){
            pid = fork();
            if(pid == 0){ exit(0); }
            else if(pid != -1){ wait_res = wait(NULL);}
            else {  printf("Error while forking"); }
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
            total[i] = (end_time - start_time);
        }
    }
    outputFile = fopen("fork-overhead.txt", "w");
    for(int i=0; i<NO_OF_TRIALS; i++){
         fprintf(outputFile, "%ld\n", (total[i]));
    }
    fclose(outputFile);
    // for(int i=0; i<NO_OF_TRIALS; i++){
    //     sum += (total[i]/NO_OF_ITERATIONS*1.0);   
    // }
    // mean = sum/(NO_OF_TRIALS *1.0);
    // for(int i=0; i<NO_OF_TRIALS; i++){
    //     sum_squared_diff += pow((total[i]/(NO_OF_ITERATIONS*1.0)) - mean, 2);   
    // }
    // stddev = sqrt(sum_squared_diff/NO_OF_TRIALS);
    // printf("\n\nTrials: %d\nMean Cycles for Proc Call (0 arg): %lf\nStddev: %lf\n", NO_OF_TRIALS, mean, stddev);
}