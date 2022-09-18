#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>

#define FILE_SIZE 21474836500
#define PAGE_SIZE 4096
#define NO_OF_TRIALS 100
#define NO_OF_ITERATIONS 10000
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
        char c;
        int fd = open("data2.txt", O_RDONLY);
        if(fd < 0 ){
            printf("Cannot Open File");
            exit(1);
        }
        char *ptr = (char *) mmap(NULL, FILE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
        if(ptr == MAP_FAILED){
            printf("Mapping Failed\n");
            exit(1);
        }
        long int stride = FILE_SIZE/NO_OF_ITERATIONS;
        long int counter = FILE_SIZE;
        
        asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
        for(int j=0; j<NO_OF_ITERATIONS; j++){
            counter -= stride;
            c = ptr[counter];
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
        munmap(ptr, FILE_SIZE);
        posix_fadvise(fd, 0, FILE_SIZE, POSIX_FADV_DONTNEED);     
    }
    outputFile = fopen("page-fault-service-time.txt", "w");
    for(int i=0; i<NO_OF_TRIALS; i++){
         fprintf(outputFile, "%ld\n", total[i]); // reading in each trial
    }
    fclose(outputFile);
}
