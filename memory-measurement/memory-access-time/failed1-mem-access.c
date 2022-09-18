#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// arr size can be from 128 to 8000000
#define MIN_ARR_SIZE 10 // 1 kb array. then we keep shifting by 2. 2^10, 2^11, 2^12....
#define MAX_ARR_SIZE 2040
#define ARR_SIZE_INCREMENT 10

#define LOOP_OVERHEAD 5.8
#define READ_OVERHEAD 37.1
#define NO_OF_ACCESS_PER_ITERATION 10

unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time;
FILE *outputFile;

typedef struct Node{
    struct Node *next;
    int data[1024];
}Node;
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
    for(int arr_size=MIN_ARR_SIZE; arr_size<MAX_ARR_SIZE; arr_size+=ARR_SIZE_INCREMENT){
    	if((arr_size%10) != 0){
    	    break;
    	}
        Node clist[arr_size];
        for(int i=0; i<arr_size-1; i++){
            clist[i].next = &clist[i+1];
        }
        clist[arr_size-1].next = &clist[0];
        Node *head=&clist[0];
        int no_of_iterations = arr_size/NO_OF_ACCESS_PER_ITERATION;
        //printf("arr_size: %d, no_of_iterations: %d\n",arr_size, no_of_iterations);
        asm volatile ("CPUID\n\t"
                   "RDTSC\n\t"
                   "mov %%edx, %0\n\t"
                   "mov %%eax, %1\n\t" 
                   : "=r" (cycles_high0), "=r" (cycles_low0)::
                   "%rax", "%rbx", "%rcx", "%rdx");
        for(int k=0; k<no_of_iterations; k++){
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
            head = head->next;
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
            double time_minus_overhead = time - READ_OVERHEAD - (LOOP_OVERHEAD * no_of_iterations);
            fprintf(outputFile, "%d, %lf\n", arr_size*4096, time_minus_overhead/(no_of_iterations * NO_OF_ACCESS_PER_ITERATION));
        }
    }
}    
