#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>

#define NO_OF_ITERATIONS 100
#define NO_OF_FILES 18
unsigned int cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total_time;
FILE *outputFile;

void main(){    
    outputFile = fopen("local-seq-file-read-rev.txt", "w");
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
    int id = 1;
    while(id <= NO_OF_FILES){
        char file_name[100];
        uint64_t file_sizes[NO_OF_FILES] = {131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648, 4294967296, 8589934592, 17179869184};
        char buf[4096];
        sprintf(file_name, "test-%d.img",(id));
        int fd = open(file_name, O_SYNC | __O_DIRECT);
        uint64_t chunks = file_sizes[id-1]/4096;
        // printf("chunks: %ld\n",chunks);
        int n; char c;
        total_time = 0;
        for(int i=0; i<NO_OF_ITERATIONS; i++){
            int e = system("sync; echo 3 > /proc/sys/vm/drop_caches");
            posix_fadvise(fd, 0, file_sizes[id-1], POSIX_FADV_DONTNEED);
            if(e==-1){
                printf("Error in clearing cache");
                exit(1);
            }
            asm volatile ("CPUID\n\t"
                    "RDTSC\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t" 
                    : "=r" (cycles_high0), "=r" (cycles_low0)::
                    "%rax", "%rbx", "%rcx", "%rdx");
            for(long long int j=chunks; j>=0; j--){
                // printf("j: %ld\n",j);
                n = pread(fd, buf, 4096, j*4096);
                // if(n ==-1){
                //     printf("Error while reading");
                //     exit(1);
                // }
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
                total_time = total_time + (end_time - start_time); 
            }
        }
        fprintf(outputFile, "%ld, %lf\n", (chunks), (total_time*0.344)/(chunks*NO_OF_ITERATIONS));
        c = buf[0];
        id+=1;
    }
    fclose(outputFile);
}
