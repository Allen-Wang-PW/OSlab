#include "process.h"
#include "common.h"
#include "x86/memory.h"
#include "mmu.h"

#define NR_PCB 5
#define NR_SEM 5

#define offsetof(type, member) \
	((size_t)(&((type*)0)->member))

#define list_entry(ptr, type, member) \
	((type*)((char*)(ptr) - offsetof(type, member)))

#define list_to_sem(list) \
	list_entry((list), Semaphore, link)

PCB pcbpool[NR_PCB];
Semaphore sempool[NR_SEM];

uint32_t num_of_user_proc = 0;

ListHead ready, block, free;
ListHead sem_run, sem_free;
//static ListHead free;

static inline void
list_init(ListHead *list) {
	assert(list != NULL);
	list->prev = list->next = list;
}

uint8_t
list_empty(ListHead *list) {
	assert(list != NULL);
	return (list == list->next);
}

static inline void
list_add(ListHead *prev, ListHead *next, ListHead *data) {
	assert(data != NULL);
	data->prev = prev;
	data->next = next;
	if (prev != NULL)
		prev->next = data;
	if (next != NULL)
		next->prev = data;
}

void
list_add_before(ListHead *list, ListHead *data) {
	assert(list != NULL);
	list_add(list->prev, list, data);
}

void
list_add_after(ListHead *list, ListHead *data) {
	assert(list != NULL);
	list_add(list, list->next, data);
}

void
list_del(ListHead *data) {
	assert(data != NULL);
	ListHead *prev = data->prev;
	ListHead *next = data->next;
	data->next = data->prev = NULL;

	if (prev != NULL)
		prev->next = next;
	if (next != NULL)
		next->prev = prev;
}


PCB*
list_to_pcb(ListHead *data) {
	return ((PCB*)((char*)(data) - (int)(&((PCB*)0)->list)));
}

void
init_pcbpool(void) {
	list_init(&ready);
	list_init(&block);
	list_init(&free);

	int i;
	for (i = 0; i < NR_PCB; i++)
		list_add_before(&free, &(pcbpool[i].list));
}

PCB*
new_pcb(void) {
	assert(!list_empty(&free));

	ListHead *data = free.next;
	list_del(data);
	list_add_before(&ready, data);

	PCB *temp = list_to_pcb(data);
	temp->tf = (struct TrapFrame*)(temp->kstack + KSTACK_SIZE - sizeof(struct TrapFrame));
	temp->pid = getpid(temp);
	temp->seg_base = 0x3000000 + (temp->pid - 1)* PROCESS_MEM_SIZE;

	num_of_user_proc++;

	return temp;
}

uint32_t
getpid(PCB *proc) {
	return proc - pcbpool + 1;
}

void
init_sem() {
	int i;
	for (i = 0; i < NR_SEM; i++) {
		memset(sempool + i, 0, sizeof(Semaphore));
		sempool[i].used = 0;
//		list_add_before(&sem_free, &(sempool[i].link));
	}
}

sem_t
sem_open(int value) {
	int i;
	for (i = 0; i < NR_SEM; i++)
		if (!sempool[i].used)
			break;

//	printk("index:%d\n", i);
	sempool[i].token = value;
	sempool[i].used = 1;
	list_init(&(sempool[i].wait));
	return i;
}

/*
void
sem_close(sem_t index) {
	if (sempool[index] != NULL){
		list_del(&(sempool[index]->link));
		list_add_before(&sem_free, &(sempool[index]->link));
	}
	sempool[index] = NULL;
}
*/

void
sem_close(sem_t index) {
	if (sempool[index].used) {
		sempool[index].used = 0;
//		sempool[index] = NULL;
		list_init(&sempool[index].wait);
	}
}

void
sem_wait(sem_t index) {	
//	printk("wait\n");
	sempool[index].token--;
	if (sempool[index].token < 0) {
//		printk("in\n");
		list_del(&(current->list));
		list_add_before(&sempool[index].wait, &(current->list));
		rescheduled = 1;
	}
//	printk("After P, the sem[%d]->token is %d\n", index, sempool[index].token);
}

void
sem_post(sem_t index) {
//	if (index == 0)
//		printk("index=%d, token=%d\n", index, sempool[index].token);
	sempool[index].token++;
//	if (index == 0)
//		printk("index=%d, token=%d\n", index, sempool[index].token);
	if (sempool[index].token <= 0) {
		ListHead *temp = (sempool[index].wait).next;
		PCB *pcb = list_to_pcb(temp);
		list_del(&(pcb->list));
//		list_del(temp);
		list_add_before(&ready, &(pcb->list));
	}
//	printk("After V, the sem[%d]->token is %d\n", index, sempool[index].token);
}

void
create_kthread(void *fun) {
	PCB *pcb = new_pcb();
	pcb->seg_base = current->seg_base;
	struct TrapFrame *tf = (struct TrapFrame *)(pcb->kstack + KSTACK_SIZE - sizeof(struct TrapFrame));
	pcb->tf = tf;
	tf->eip = (uint32_t)fun;
	tf->esp = 0x3000000 + (pcb->pid - 1) * 0x400000;
	tf->cs = USEL(SEG_USER_CODE);
	tf->ds = USEL(SEG_USER_DATA);
	tf->es = USEL(SEG_USER_DATA);
	tf->ss = USEL(SEG_USER_DATA);
	tf->eflags = 0x202;
	
	list_add_before(&ready, &(pcb->list));
}
