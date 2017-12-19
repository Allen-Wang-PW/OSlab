//#include "game.h"
#include "irq.h"
#include "x86/x86.h"
#include "device/timer.h"
#include "device/palette.h"
#include "assert.h"
#include "mmu.h"
#include "process.h"

void init_serial(void);
void bootkernelmain(void);
void mem_init(void);
//void page_init(void);
void init_idt(void);
void enter_user_space(void);
void init_seg();
void load_file();
//void init_segment(void);

int
main(void) {
	printk("abc\n");
	init_serial();
	init_timer();
	init_idt();
	init_intr();
	init_seg();
	init_sem();
	printk("Segment has finished initialized!\n");
	load_file();
	init_proc();
//	set_timer_intr_handler(timer_event);
//	set_keyboard_intr_handler(keyboard_event);
//
	printk("Kernel has finished initialized!\n");
//	asm volatile ("movl %%eax, %%esp" : : "a"(idle.kstack + KSTACK_SIZE));
	enable_interrupt();
//	printk("aa\n");
//	enter_user_space();
//	printk("loader\n");

	while (1);
	assert(0);
	return 0;
}
