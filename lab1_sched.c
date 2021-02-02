/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student name : 안상혁
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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

extern int *result; // 수행 결과를 저장하는 배열
void Input_Process(Process *p, int Psize)
{ // 프로세스 정보 입력
    int i;
    for(i=0;i<Psize;i++)
    {
        p[i].Order = i;
        printf("%c의 Arrival time과 Service time, Tickets (ex)0 3 30 : ", 65+i);
        scanf("%d %d %d", &p[i].Arrival_Time, &p[i].Service_Time, &p[i].Tickets);
    }
}
void Initialization(CPU *cpu, Process *p, int Psize, int Rsize)
{ // 프로세스와 CPU 관련 변수 초기화
    int i;
    (*cpu).System_Time = 0;
    (*cpu).Sel_Pid = -1; // 미선택(빈시간)은 기본 -1
    for(i=0;i<Psize;i++)
    {
        p[i].Remaining_Service = p[i].Service_Time;
        p[i].Wait_Time = 0;
        p[i].Priority = -1;
        p[i].Lottery = -1;
    }
    for(i=0;i<Rsize;i++) result[i] = -1;
}
void Draw(const CPU *cpu, const Process *p, int Psize)
{ // 스케줄링 결과 표시
    int i, j;
    for(i=0;i<Psize;i++)
    {
        printf("%c: |", 65+i);
        for(j=0;j<(*cpu).System_Time;j++)
        {
            if(result[j] < 0) printf(" |");
            else if(p[result[j]].Order == i) printf("%c|", 65+i);
            else printf(" |");
        }
        printf("\n");
    }
}
void Run(CPU *cpu, Process *p, int Psize)
{ // 한 타임(1 System_Time) 동안의 스케줄링 결과 수행
    int i;
    result[(*cpu).System_Time] = (*cpu).Sel_Pid; // 결과 배열에 해당 시간에 스케줄링된 프로세스 인덱스 입력
    if((*cpu).Sel_Pid != -1) // 스케줄러가 선택할 프로세스가 없을 때(빈 시간)가 아닌 경우
    {
        p[(*cpu).Sel_Pid].Remaining_Service--; // 실행된 프로세스의 남은 서비스 시간 감소
        p[(*cpu).Sel_Pid].Wait_Time = 0; // 실행된 프로세스의 대기시간 초기화(RR용)
        if(p[(*cpu).Sel_Pid].Remaining_Service == 0) p[(*cpu).Sel_Pid].Lottery = -1; // 남은 서비스가 없다면 확률 미적용(Lottery용)
    }
    (*cpu).System_Time++;
    for(i=0;i<Psize;i++)
    {
        if((*cpu).Sel_Pid != i && (*cpu).System_Time >= p[i].Arrival_Time && p[i].Remaining_Service)
            p[i].Wait_Time++; // 자신이 수행되지 못 했다면 대기한 시간 증가
    }
}
int Check_Result_Size(const Process *p, int Psize)
{ // 스케줄링되어야 할 총 횟수를 구하는 함수(도착 시간에 따른 정렬 이후에 가능)
    int i, Rsize = 0;
    for(i=0;i<Psize;i++)
    {
        if(Rsize < p[i].Arrival_Time) Rsize += (p[i].Arrival_Time - Rsize); // 빈 시간 고려
        Rsize += p[i].Service_Time;
    }
    return Rsize;
}
int IPow(int base, int exp)
{ // int형 거듭제곱 함수
    int i, r = 1;
    for(i=1;i<=exp;i++) r *= base;
    return r;
}
int Check_Priority(const CPU *cpu, Process *p, int Psize)
{ // Ready 상태인 프로세스가 자신 뿐인지 검사하는 함수
    int i;
    for(i=0;i<Psize;i++)
        if((*cpu).Sel_Pid != i) // 방금 실행했던 프로세스만 존재하는 경우가 아닐 때
            if(p[i].Arrival_Time <= (*cpu).System_Time && p[(*cpu).Sel_Pid].Remaining_Service > 0) return 1;
    return 0; // 방금 실행했던 프로세스만 존재하는 경우
}
int Arrival_Compare(const void *a, const void *b)
{ // 도착 시간을 비교하는 함수
    if((*(Process*)a).Arrival_Time > (*(Process*)b).Arrival_Time) return 1;
    else if((*(Process*)a).Arrival_Time < (*(Process*)b).Arrival_Time) return -1;
    else return 0;
}
int Service_Compare(const void *a, const void *b)
{ // 총 서비스 시간을 비교하는 함수
    if((*(Process*)a).Service_Time > (*(Process*)b).Service_Time) return 1;
    else if((*(Process*)a).Service_Time < (*(Process*)b).Service_Time) return -1;
    else return 0;
}
int Remaining_Compare(const void *a, const void *b)
{ // 남은 서비스 시간을 비교하는 함수
    if((*(Process*)a).Remaining_Service > (*(Process*)b).Remaining_Service) return 1;
    else if((*(Process*)a).Remaining_Service < (*(Process*)b).Remaining_Service) return -1;
    else return 0;
}
int Wait_Compare(const void *a, const void *b)
{ // 대기한 시간을 비교하는 함수
    if((*(Process*)a).Wait_Time > (*(Process*)b).Wait_Time) return 1;
    else if((*(Process*)a).Wait_Time < (*(Process*)b).Wait_Time) return -1;
    else return 0;
}
int Priority_Compare(const void *a, const void *b)
{ // 우선순위를 비교하는 함수(같다면 대기시간까지)
    if((*(Process*)a).Priority > (*(Process*)b).Priority) return 1;
    else if((*(Process*)a).Priority < (*(Process*)b).Priority) return -1;
    else if(Wait_Compare(a, b) < 0) return 1; // 우선순위가 같다면 RR로 스케줄링하기 위함
    else if(Wait_Compare(a, b) > 0) return -1;
    else return 0;
}
void Sched_FIFO(CPU *cpu, Process *p, int Psize)
{ // FIFO 스케줄링을 수행하는 함수
    int i, j;
    printf("FIFO\n");
    for(i=0;i<Psize;i++)
    { // 도착 순으로 정렬되어 있기 때문에 바로 프로세스 배열의 순서대로 실행
        if((*cpu).System_Time < p[i].Arrival_Time)
            (*cpu).System_Time = p[i].Arrival_Time; // 빈 시간 고려
        (*cpu).Sel_Pid = i;
        for(j=p[i].Remaining_Service;p[i].Remaining_Service>0;j--) Run(cpu, p, Psize); // 선택된 프로세스 수행
    }
    Draw(cpu, p, Psize); // 수행 결과 출력
    printf("\n\n");
}
void Sched_SJF(CPU *cpu, Process *p, int Psize, int Rsize)
{ // SJF 스케줄링을 수행하는 함수
    int i, min; // min = 총 서비스 시간이 제일 짧은(스케줄링 될) 프로세스 인덱스
    printf("SJF\n");
    while(Rsize-(*cpu).System_Time) // 총 수행할 횟수만큼 스케줄링할 때까지 반복
    {
        min = -1;
        for(i=0;i<Psize;i++)
        { // Service_Time이 제일 적은 Process 선택 과정
            if(p[i].Remaining_Service)
                if((*cpu).System_Time >= p[i].Arrival_Time) // 서비스를 완료하지 않고, 도착한 이후라면
                {
                    if(min == -1) min = i; // 현재 min이 빈 시간이면 바로 넣어 줌
                    if(Service_Compare(&p[i], &p[min]) < 0) min = i; // 총 서비스 시간 비교 후 min 갱신
                }
        }
        (*cpu).Sel_Pid = min;
        if((*cpu).Sel_Pid == -1)
        { // 빈 시간일 경우
            (*cpu).System_Time++;
            continue;
        }
        for(i=p[min].Remaining_Service;i;i--) Run(cpu, p, Psize); // 빈 시간이 아니라면 수행
    }
    Draw(cpu, p, Psize); // 수행 결과 표시
    printf("\n\n");
}
void Sched_STCF(CPU *cpu, Process *p, int Psize, int Rsize)
{ // STCF 스케줄링을 수행하는 함수
    int i, min; // min = 남은 서비스 시간이 제일 짧은(스케줄링 될) 프로세스 인덱스
    printf("SCTF\n");
    while(Rsize-(*cpu).System_Time) // 총 수행할 횟수만큼 스케줄링할 때까지 반복
    {
        min = -1;
        for(i=0;i<Psize;i++)
        { // Remaining_Service가 제일 적은 Process 선택 과정
            if(p[i].Remaining_Service)
                if((*cpu).System_Time >= p[i].Arrival_Time) // 서비스를 완료하지 않고, 도착한 이후라면
                {
                    if(min == -1) min = i; // 현재 min이 빈 시간이면 바로 넣어 줌
                    if(Remaining_Compare(&p[i], &p[min]) < 0) min = i; // 남은 서비스 시 비교 후 min 갱신
                }
        }
        (*cpu).Sel_Pid = min;
        Run(cpu, p, Psize); // 빈 시간이 아니더라도 한 번만 실행하므로 동일하게 처리
    }
    Draw(cpu, p, Psize); // 수행 결과 표시
    printf("\n\n");
}
void Sched_RR(CPU *cpu, Process *p, int Psize, int Rsize, int slice)
{ // RR 스케줄링을 수행하는 함수
    int i, max; // max = 대기한 시간이 제일 긴(스케줄링 될) 프로세스 인덱스
    printf("RR(%d)\n", slice); // 괄호 안에 숫자는 스케줄링된 프로세스가 수행될 time slice
    while(Rsize-(*cpu).System_Time) // 총 수행할 횟수만큼 스케줄링할 때까지 반복
    {
        max = -1;
        for(i=0;i<Psize;i++)
        { // Wait_Time이 제일 큰 Process 선택 과정
            if(p[i].Remaining_Service)
                if((*cpu).System_Time >= p[i].Arrival_Time) // 서비스를 완료하지 않고, 도착한 이후라면
                {
                    if(max == -1) max = i; // 현재 max가 빈 시간이면 바로 넣어 줌
                    if(Wait_Compare(&p[i], &p[max]) > 0) max = i; // 대기한 시간 비교 후 max 갱신
                }
        }
        (*cpu).Sel_Pid = max;
        if((*cpu).Sel_Pid == -1)
        { // 빈 시간일 경우
            (*cpu).System_Time++;
            continue;
        }
        for(i=0;i<slice;i++)
        { // 빈 시간이 아니라면 time slice 만큼 수행
            if(p[(*cpu).Sel_Pid].Remaining_Service <= 0) break; // 도중에 서비스를 마치면 수행 종료
            Run(cpu, p, Psize);
        }
    }
    Draw(cpu, p, Psize); // 수행 결과 표시
    printf("\n\n");
}
void Sched_MLFQ(CPU *cpu, Process *p, int Psize, int Rsize, int level, int slice_opt)
{ // MLFQ 스케줄링을 수행하는 함수(slice_opt가 0이면 모든 큐의 time slice가 같음)
    int i, min; // min = 우선순위가 높고(Priority 값이 작고) 대기한 시간이 긴(스케줄링 될) 프로세스 인덱스
    if(slice_opt) printf("MLFQ(%d, 2^i)\n", level); // 인자로 준 큐의 개수와 큐 별 time slice 같이 표시
    else printf("MLFQ(%d, 1)\n", level);
    while(Rsize-(*cpu).System_Time) // 총 수행할 횟수만큼 스케줄링할 때까지 반복
    {
        min = -1;
        for(i=0;i<Psize;i++)
        { // Priority가 제일 작은 Process 선택 과정
            if(p[i].Remaining_Service)
                if((*cpu).System_Time >= p[i].Arrival_Time) // 서비스를 완료하지 않고, 도착한 이후라면
                {
                    if(p[i].Priority == -1) p[i].Priority++; // 처음 도착하였다면 우선순위 갱신
                    if(min == -1) min = i; // 현재 min이 빈 시간이면 바로 넣어 줌
                    if(Priority_Compare(&p[i], &p[min]) < 0) min = i; // 우선순위 비교(같다면 대기 시간까지) 후 min 갱신
                }
        }
        (*cpu).Sel_Pid = min;
        if((*cpu).Sel_Pid == -1)
        { // 빈 시간일 경우
            (*cpu).System_Time++;
            continue;
        }
        if(slice_opt)
        { // 큐의 레벨에 따라 수행 횟수가 다른 경우
            for(i=0;i<IPow(2, p[(*cpu).Sel_Pid].Priority);i++)
            {
                if(p[(*cpu).Sel_Pid].Remaining_Service <= 0) break;
                Run(cpu, p, Psize);
            }
        }
        else Run(cpu, p, Psize); // 수행 횟수가 같은 경우
        if(Check_Priority(cpu, p, Psize)) // 우선순위 향상 전 도착한 프로세스가 자신만인지 체크(기본 예제에서는 자신밖에 없다면 한 번 더 수행하였기 때문)
            if(p[(*cpu).Sel_Pid].Priority < level-1) p[(*cpu).Sel_Pid].Priority++;
    }
    Draw(cpu, p, Psize); // 수행 결과 표시
    printf("\n\n");
}
void Sched_LOTTERY(CPU *cpu, Process *p, int Psize, int Rsize)
{ // LOTTERY 스케줄링을 수행하는 함수
    int i, winner, total, temp; // total = 현재 시간에서 대기 중인 프로세스의 총 티켓 수
    printf("LOTTERY\n");
    srand((unsigned)time(NULL)); // rand를 변하는 시간을 기준으로 수행하기 위함
    while(Rsize-(*cpu).System_Time) // 총 수행할 횟수만큼 스케줄링할 때까지 반복
    {
        temp = total = 0;
        for(i=0;i<Psize;i++)
        {
            if(p[i].Arrival_Time <= (*cpu).System_Time && p[i].Remaining_Service)
            { // 현재 수행 가능한 프로세스라면(도착하지 않았거나 수행을 완료 하였다면 반영되지 않음)
                total += p[i].Tickets; // 총 티켓 수 추가
                p[i].Lottery = temp + p[i].Tickets-1; // 티켓을 범위로 환산
                temp += p[i].Tickets;
            }
        }
        if(total == 0)
        { // 수행할 프로세스가 없다면
            (*cpu).System_Time++;
            continue;
        }
        winner = rand()%total; // 당첨 결과 값 저장
        for(i=0;i<Psize;i++)
        {
            if(p[i].Lottery >= winner)
            { // 당첨되었다면
                (*cpu).Sel_Pid = i;
                Run(cpu, p, Psize);
                break;
            }
        }
    }
    Draw(cpu, p, Psize); // 수행 결과 표시
    printf("\n\n");
}
