#include "x86/x86.h"
#include "process.h"

#define NR_HARD_INTR 16
extern PCB pcbpool[];

void load_code(uint32_t);

static void (*do_timer)(void);
static void (*do_keyboard)(int);

void timer(void) {
	if (current != &idle) {
		(current->run_time)--;
//		printk("run_time:%d\n",current->run_time);
		if (current->run_time == 0) {
			rescheduled = 1;
//			switch_ready_queue(current);
			list_del(&(current->list));
			list_add_before(&ready, &(current->list));
		}
	}

	ListHead *curlist = block.next;
	while (curlist != &block) {
		PCB *curpcb = list_to_pcb(curlist);
		curlist = curlist->next;
		(curpcb->sleep_time)--;
		if (curpcb->sleep_time == 0) {
//			switch_ready_queue(curpcb);
			list_del(&(curpcb->list));
			list_add_before(&ready, &(curpcb->list));
		}
	}
}

void
set_timer_intr_handler( void (*ptr)(void) ) {
	do_timer = ptr;
}
void
set_keyboard_intr_handler( void (*ptr)(int) ) {
	do_keyboard = ptr;
}

void
do_syscall(struct TrapFrame *);

/* TrapFrame的定义在include/x86/memory.h
 * 请仔细理解这段程序的含义，这些内容将在后续的实验中被反复使用。 */

void
irq_handle(struct TrapFrame *tf) {
//	printk("enter irq_handle\n");
	int irq = tf->irq;
//	printk("irq:%x\n",irq);
//	printk("tf->eip:%x\n",tf->eip);
	if (irq < 0) {
		printk("%s, %d: Unhandled exception!\n", __FUNCTION__, __LINE__);
		assert(0);
	}
	else if (irq == 0x80) {
		do_syscall(tf);
	}
	else if (irq < 1000) {
		printk("%s, %d: Unexpected exception #%d!\n", __FUNCTION__, __LINE__, irq);
//		if (irq != 13)
		assert(0);
	}
	else if (irq == 1000) {
//		printk("enter time\n");
		timer();
	}
	else if (irq == 1001) {
		uint32_t scancode = in_byte(0x60);
		uint32_t val = in_byte(0x61);
		out_byte(0x61, val | 0x80);
		out_byte(0x61, val);
		load_code(scancode);
	}
	else if (irq == 1014){
	}
	else {
		assert(0);
	}

	current->tf = tf;
	schedule();
}
