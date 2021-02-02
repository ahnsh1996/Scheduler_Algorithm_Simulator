/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student name : 안상혁
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H
typedef struct CPU
{
    int System_Time; // 현재 진행 시간
    int Sel_Pid; // 스케줄링 결과 선택된 프로세스의 id(미선택(빈시간)은 기본 -1)
} CPU;
typedef struct Process
{
    int Order; // 정보를 입력한 순서(입력 후 도착시간에 따른 정렬을 하기 때문)
    int Arrival_Time; // 도착 시간
    int Service_Time; // 총 서비스 시간
    int Remaining_Service; // 남은 서비스 시간
    int Wait_Time; // 대기한 시간(RR용)
    int Priority; // 큐의 우선순위(MLFQ용)
    int Tickets; // 보유 티켓 수(LOTTERY용)
    int Lottery; // 티켓을 환산한 범위 값
} Process;
void Input_Process(Process *p, int Psize); // 프로세스 정보 입력
void Initialization(CPU *cpu, Process *p, int Psize, int Rsize); // 프로세스와 CPU 관련 변수 초기화
void Draw(const CPU *cpu, const Process *p, int Psize); // 스케줄링 결과 표시
void Run(CPU *cpu, Process *p, int Psize); // 한 타임(1 System_Time) 동안의 스케줄링 결과 수행
int Check_Result_Size(const Process *p, int Psize); // 스케줄링되어야 할 총 횟수를 구하는 함수(도착 시간에 따른 정렬 이후에 가능)
int IPow(int base, int exp); // int형 거듭제곱 함수
int Check_Priority(const CPU *cpu, Process *p, int Psize); // Ready 상태인 프로세스가 자신 뿐인지 검사하는 함수
int Arrival_Compare(const void *a, const void *b); // 도착 시간을 비교하는 함수
int Serivce_Compare(const void *a, const void *b); // 총 서비스 시간을 비교하는 함수
int Remaining_Compare(const void *a, const void *b); // 남은 서비스 시간을 비교하는 함수
int Wait_Compare(const void *a, const void *b); // 대기한 시간을 비교하는 함수
int Priority_Compare(const void *a, const void *b); // 우선순위를 비교하는 함수(같다면 대기시간까지)
void Sched_FIFO(CPU *cpu, Process *p, int Psize); // FIFO 스케줄링을 수행하는 함수
void Sched_SJF(CPU *cpu, Process *p, int Psize, int Rsize); // SJF 스케줄링을 수행하는 함수
void Sched_STCF(CPU *cpu, Process *p, int Psize, int Rsize); // STCF 스케줄링을 수행하는 함수
void Sched_RR(CPU *cpu, Process *p, int Psize, int Rsize, int slice); // RR 스케줄링을 수행하는 함수
void Sched_MLFQ(CPU *cpu, Process *p, int Psize, int Rsize, int level, int slice_opt); // MLFQ 스케줄링을 수행하는 함수
void Sched_LOTTERY(CPU *cpu, Process *p, int Psize, int Rsize); // LOTTERY 스케줄링을 수행하는 함수
#endif /* LAB1_HEADER_H*/



