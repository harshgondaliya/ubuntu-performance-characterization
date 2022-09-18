#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define NO_OF_ITERATIONS 1000
unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time, total[NO_OF_ITERATIONS];
FILE *outputFile;
#define PORT 5010
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
    
    
	for(int i=0;i<NO_OF_ITERATIONS;i++){
        sleep(0.1);
        int sock;
        struct sockaddr_in server;
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            printf("Could not create socket");
        }
        server.sin_addr.s_addr = inet_addr("34.221.92.235");
        server.sin_family = AF_INET;
        server.sin_port = htons( PORT );
        if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
            printf("connect error");
            exit(1);
        }
        asm volatile ("CPUID\n\t"
                "RDTSC\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t" 
                : "=r" (cycles_high0), "=r" (cycles_low0)::
                "%rax", "%rbx", "%rcx", "%rdx");
        close(sock);    
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
    outputFile = fopen("teardown-overhead-remote.txt", "w");
    for(int i=0; i<NO_OF_ITERATIONS; i++){
        // printf("%d: %ld\n",i, total[i]);
        fprintf(outputFile, "%ld\n", (total[i]));
    }
    fclose(outputFile);
}
