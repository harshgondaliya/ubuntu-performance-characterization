#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define ARR_SIZE 104857600 // 400 MB array
#define NO_OF_TRIALS 100
#define READ_OVERHEAD 37.1
#define LOOP_OVERHEAD 5.8

unsigned cycles_low0, cycles_high0, cycles_low1, cycles_high1;
__uint64_t start_time, end_time;
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
   
    outputFile = fopen("memory-write-bw.txt", "w");
    int sum = 0;
    //int *arr;
    for(int i=0;i<NO_OF_TRIALS;i++){
        //arr = (int *)malloc(ARR_SIZE * sizeof(int));
        //if(arr == NULL){printf("Unable to allocate memory\n");}
        int *arr = (int *)malloc(ARR_SIZE*sizeof(int));
        int *start_arr = arr;
        for(int k=0; k<ARR_SIZE; k++){
            arr[k] = 1;
        }
        int no_of_iterations = (ARR_SIZE/1600);
        asm volatile ("CPUID\n\t"
                "RDTSC\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t" 
                : "=r" (cycles_high0), "=r" (cycles_low0)::
                "%rax", "%rbx", "%rcx", "%rdx");
            int count =0;
            for(int j=0;j<no_of_iterations-10;j++){
                // printf("enter %d\n",count);
                      arr[0] =1; arr[16] =1; arr[32] =1; arr[48] =1; arr[64] =1; arr[80] =1; arr[96] =1; arr[112] =1; 
                      arr[128] =1; arr[144] =1; arr[160] = 1; arr[176] =1; arr[192] = 1; arr[208] =1; arr[224] = 1; arr[240] =1; arr[256] =1; 
                      arr[272] =1; arr[288] =1; arr[304] = 1; arr[320] =1; arr[336] = 1; arr[352] =1; arr[368] = 1; arr[384] =1; arr[400] =1; 
                      arr[416] =1; arr[432] =1; arr[448] = 1; arr[464] =1; arr[480] = 1; arr[496] =1; arr[512] =1; arr[528] =1; 
                      arr[544] =1; arr[560] =1; arr[576] = 1; arr[592] =1; arr[608] = 1; arr[624] =1; arr[640] = 1; arr[656] =1; arr[672] =1; 
                      arr[688] =1; arr[704] =1; arr[720] = 1; arr[736] =1; arr[752] = 1; arr[768] =1; arr[784] = 1; arr[800] =1; arr[816] =1; 
                      arr[832] =1; arr[848] =1; arr[864] = 1; arr[880] =1; arr[896] = 1; arr[912] =1; arr[928] =1; arr[944] =1; arr[960] =1;
                      arr[976] =1; arr[992] =1; arr[1008] = 1; arr[1024] =1; arr[1040] = 1; arr[1056] =1; arr[1072] =1; arr[1088] =1; arr[1104] =1;
                      arr[1120] =1; arr[1136] =1; arr[1152] = 1; arr[1168] =1; arr[1184] = 1; arr[1200] =1; arr[1216] =1; arr[1232] =1; arr[1248] =1;
                      arr[1264] =1; arr[1280] =1; arr[1296] = 1; arr[1312] =1; arr[1328] = 1; arr[1344] =1; arr[1360] =1; arr[1376] =1; arr[1392] =1;
                      arr[1408] =1; arr[1424] =1; arr[1440] = 1; arr[1456] =1; arr[1472] = 1; arr[1488] =1; arr[1504] =1; arr[1520] =1; arr[1536] =1;
                      arr[1552] =1; arr[1568] =1; arr[1584] = 1;
                // printf("exit: %d; updated count: %d\n",count, count+1600);
                count+=1600;
                arr += 1600; 
            }
        asm volatile("RDTSCP\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    "CPUID\n\t"
                    : "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                    "%rbx", "%rcx", "%rdx");
        
        int temp = sum;
        start_time = ( ((__uint64_t)cycles_high0 << 32) | cycles_low0 );
        end_time = ( ((__uint64_t)cycles_high1 << 32) | cycles_low1 );
        if(end_time - start_time < 0){
            printf("Critical Error in Taking Measurement");
        } else{
            __uint64_t time = end_time - start_time;
            __uint64_t time_minus_overhead = time - READ_OVERHEAD - (LOOP_OVERHEAD * no_of_iterations);
            fprintf(outputFile, "%ld\n", time_minus_overhead);
        }
        free(start_arr);
    }
    fclose(outputFile);
}
