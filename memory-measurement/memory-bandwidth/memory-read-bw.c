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
   
    outputFile = fopen("memory-read-bw.txt", "w");
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
                sum = arr[0] + arr[16] + arr[32] + arr[48] + arr[64] + arr[80] + arr[96] + arr[112] + 
                      arr[128] + arr[144] + arr[160]+ arr[176] + arr[192]+ arr[208] + arr[224]+ arr[240] + arr[256] + 
                      arr[272] + arr[288] + arr[304]+ arr[320] + arr[336]+ arr[352] + arr[368]+ arr[384] + arr[400] + 
                      arr[416] + arr[432] + arr[448]+ arr[464] + arr[480]+ arr[496] + arr[512] + arr[528] + 
                      arr[544] + arr[560] + arr[576]+ arr[592] + arr[608]+ arr[624] + arr[640]+ arr[656] + arr[672] + 
                      arr[688] + arr[704] + arr[720]+ arr[736] + arr[752]+ arr[768] + arr[784]+ arr[800] + arr[816] + 
                      arr[832] + arr[848] + arr[864]+ arr[880] + arr[896]+ arr[912] + arr[928] + arr[944] + arr[960] +
                      arr[976] + arr[992] + arr[1008]+ arr[1024] + arr[1040]+ arr[1056] + arr[1072] + arr[1088] + arr[1104] +
                      arr[1120] + arr[1136] + arr[1152]+ arr[1168] + arr[1184]+ arr[1200] + arr[1216] + arr[1232] + arr[1248] +
                      arr[1264] + arr[1280] + arr[1296]+ arr[1312] + arr[1328]+ arr[1344] + arr[1360] + arr[1376] + arr[1392] +
                      arr[1408] + arr[1424] + arr[1440]+ arr[1456] + arr[1472]+ arr[1488] + arr[1504] + arr[1520] + arr[1536] +
                      arr[1552] + arr[1568] + arr[1584];
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
