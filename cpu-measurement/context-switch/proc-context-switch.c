#include <stdio.h>
#include <inttypes.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define NO_OF_TRIALS 200
#define NO_OF_ITERATIONS 10000
unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total[NO_OF_TRIALS];
FILE *outputFile;

void main(){    
    // warm instruction cache
    int p_to_c[2];
    int arr[1000];
    int c_to_p[2];
    char token[] = "1234567891234567891234567891234";
    char read_buf[32], read_buf2[32];
    pid_t pid;
    pipe(c_to_p); // child uses c_to_p to write while use p_to_c for read
    pipe(p_to_c);   
    pid = fork();    
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
            if(pid != 0){ // since child will read early let him wait/block until something has been written
                write(p_to_c[1], token, (strlen(token)+1));
                read(c_to_p[0], read_buf, (strlen(token)+1));
            } else {
                read(p_to_c[0], read_buf2, (strlen(token)+1));
                write(c_to_p[1], token, (strlen(token)+1));
            }
        }
        asm volatile("RDTSCP\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    "CPUID\n\t"
                    : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                    "%rbx", "%rcx", "%rdx");
        // we wait until child finishes but need not include that in our time measurement.
        start_time = ( ((__uint64_t)cycles_high0 << 32) | cycles_low0 );
        end_time = ( ((__uint64_t)cycles_high1 << 32) | cycles_low1 );
        if(end_time - start_time < 0){
            printf("Critical Error in Taking Measurement");
        } else{
            total[i] = (end_time - start_time);
        }
    }
    if(pid == 0){ exit(0); } else {wait(NULL);} // cant put this inside the timestamps    
    outputFile = fopen("proc-context-switch-overhead.txt", "w");
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