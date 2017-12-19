#ifndef __PROCESS_H__
#define __PROCESS_H__
#include "adt/linklist.h"
#include "common.h"
#include "string.h"

#define KSTACK_SIZE 4096
#define PROCESS_MEM_SIZE 0x400000

typedef struct ListHead {
	struct ListHead *prev, *next;
} ListHead;

typedef struct Semaphore {
	int token;
	int used;
	ListHead wait;
} Semaphore;

typedef struct PCB {
	struct TrapFrame *tf;
	uint8_t kstack[KSTACK_SIZE];
	uint32_t run_time;
	uint32_t sleep_time;
	uint32_t seg_base;
	pid_t pid;
	ListHead list;
	Semaphore *sem;
} PCB;

extern PCB *current;
extern PCB idle;
extern ListHead ready, block, free;
extern uint8_t rescheduled;

void init_proc(void);
void init_pcbpool(void);
void schedule(void);
uint8_t list_empty(ListHead*);
void list_del(ListHead*);
void list_add_after(ListHead*, ListHead*);
void list_add_before(ListHead*, ListHead*);
PCB *new_pcb(void);
PCB *list_to_pcb(ListHead*);

void init_sem(void);
sem_t sem_open(int);
void sem_close(sem_t);
void sem_wait(sem_t);
void sem_post(sem_t);
void create_kthread(void*);

uint32_t getpid(PCB*);
#endif
