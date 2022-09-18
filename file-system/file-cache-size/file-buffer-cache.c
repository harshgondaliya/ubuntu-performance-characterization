#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>

#define NO_OF_ITERATIONS 10
#define MIN_FILE_SIZE_GB 0.5
#define MAX_FILE_SIZE_GB 16.5
#define FILE_SIZE_INC_GB 0.5
unsigned int cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total[NO_OF_ITERATIONS];
FILE *outputFile;

void main(){    
    outputFile = fopen("file-buffer-size.txt", "w");
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
    float file_size = MIN_FILE_SIZE_GB;
    while(file_size <=MAX_FILE_SIZE_GB){
        char file_name[100];
        char buf[4096];
        sprintf(file_name, "test-%.2f.img",(file_size));
        // printf("filename: %s", file_name);
        int fd = open(file_name, O_RDONLY);
        // printf("%d\n",fd);
        long long int chunks = (((double)file_size*1024.0*1024.0)/(4096))*1024.0;
        chunks -=1;
        // printf("chunks %lld\n",chunks);
        int n; char c;
        for(long long int j=0; j<=chunks; j++){
            //lseek(fd, j*4096, SEEK_SET);
            n = pread(fd, buf, 4096, j*4096);
            if(n ==-1){
                printf("Error");
                exit(1);
            }
        }
        asm volatile ("CPUID\n\t"
                    "RDTSC\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t" 
                    : "=r" (cycles_high0), "=r" (cycles_low0)::
                    "%rax", "%rbx", "%rcx", "%rdx");
        for(int i=0; i<NO_OF_ITERATIONS; i++){
            for(long long int j=chunks; j>=0; j--){
                //lseek(fd, j*4096, SEEK_SET);
                pread(fd, buf, 4096, j*4096);
                // printf("%lld\n", j*4096);
                //c = buf[0];
            }
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
            fprintf(outputFile, "%.1fGB, %lld, %lf\n", (file_size), chunks, (0.344)*(end_time - start_time)/(NO_OF_ITERATIONS*1.0));
        }
        c = buf[0];
        int e = system("sync; echo 3 > /proc/sys/vm/drop_caches");
        if(e==-1){
            printf("Error");
            exit(1);
        }
        file_size+=FILE_SIZE_INC_GB;
    }
    fclose(outputFile);
}
