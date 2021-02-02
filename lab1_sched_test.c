/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student name : 안상혁
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

int *result; // 수행 결과를 저장하는 배열
int main(int argc, char *argv[]){
    int Psize, Rsize; // 프로세스 개수와 결과 배열의 크기
    int RR_slice, MaxLevel; // RR의 time slice와 MLFQ에서 큐의 개수
    CPU cpu;
    Process *p;
    
    printf("Process의 수 : ");
    scanf("%d", &Psize);
    p = (Process *)malloc(sizeof(Process)*Psize);

    printf("RR에서 time slice(1은 기본적으로 출력) : ");
    scanf("%d", &RR_slice);
    printf("MLFQ에서 큐의 개수 : ");
    scanf("%d", &MaxLevel);
    Input_Process(p, Psize); // 프로세스 정보 입력

    qsort(p, Psize, sizeof(Process), Arrival_Compare); // 도착 시간을 기준으로 프로세스 배열 정렬
    Rsize = Check_Result_Size(p, Psize); // 총 수행해야할 시간 체크
    result = (int *)malloc(sizeof(int)*Rsize);

    Initialization(&cpu, p, Psize, Rsize); // 매 스케줄링 전에 기존의 결과 초기화
    Sched_FIFO(&cpu, p, Psize);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_SJF(&cpu, p, Psize, Rsize);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_STCF(&cpu, p, Psize, Rsize);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_RR(&cpu, p, Psize, Rsize, 1);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_RR(&cpu, p, Psize, Rsize, RR_slice);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_MLFQ(&cpu, p, Psize, Rsize, MaxLevel, 0);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_MLFQ(&cpu, p, Psize, Rsize, MaxLevel, 1);
    Initialization(&cpu, p, Psize, Rsize);
    Sched_LOTTERY(&cpu, p, Psize, Rsize);

    free(p); // 사용한 동적 자원 반납
    free(result);
}

