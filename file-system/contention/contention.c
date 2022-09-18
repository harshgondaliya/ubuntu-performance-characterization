#include <stdio.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>

#define NO_OF_PROCESSES 16
#define NO_OF_BLOCK_READS 1000
unsigned int cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total_time;
FILE *outputFile;

void main(){    
    outputFile = fopen("contention.txt", "w");
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
    int status = 0;
    int wpid;
    int pid;
    for(int k=1; k<=NO_OF_PROCESSES; k++){
        int pid[k];
        int fd;
        int n;
        char buf[4096];
        char file_name[100];
        pid_t parent_pid = getpid();
        int e = system("sync; echo 3 > /proc/sys/vm/drop_caches");
        asm volatile ("CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t" 
            : "=r" (cycles_high0), "=r" (cycles_low0)::
            "%rax", "%rbx", "%rcx", "%rdx");
        for(int i=1; i<=k; i++){
            pid[i-1] = fork();
            // if (pid[i-1]==-1){
            //     printf("Unable to fork");
            // }
            if(pid[i-1]==0){
                sprintf(file_name, "test-%d.img", i);
                fd = open(file_name, __O_DIRECT|O_SYNC);
                // if(fd==-1){
                //     printf("Error while opening file\n");
                // }
                for(int j=0; j<NO_OF_BLOCK_READS; j++){
                    n = pread(fd, buf, 4096, j*8096);
                    // if(n ==-1){
                    //     printf("Error while reading at offset %d\n",j*4096);
                    //     exit(1);
                    // }
                }
                exit(0);
            }
        }
        // while(wait(NULL) > 0);
        for(int p=1; p<=k; p++){
            wait(NULL);
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
            total_time = (end_time - start_time); 
        }
        printf("%d, %ld\n", k, total_time);
    }
    fclose(outputFile);
}
