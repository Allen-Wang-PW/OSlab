#include "process.h"
#include "x86/x86.h"
#include "mmu.h"
#include "common.h"

extern TSS tss;

PCB idle;
PCB *current = &idle;

extern uint32_t get_base(uint32_t);
extern Segdesc gdt[NR_SEGMENTS];

uint8_t rescheduled = 0;

void
init_proc(){
	init_pcbpool();

	/* idle process */
	rescheduled = 1;
	current = &idle;

	uint32_t user_entry = loadmain();
	printk("user_entry:%x\n",user_entry);

	PCB *data = new_pcb();

	data->tf = (struct TrapFrame*)((uint32_t)data->kstack + KSTACK_SIZE - sizeof(struct TrapFrame));
	data->tf->eip = user_entry;
	data->tf->esp = 0x400000;
	data->tf->cs = USEL(SEG_USER_CODE);
	data->tf->ds = USEL(SEG_USER_DATA);
	data->tf->es = USEL(SEG_USER_DATA);
	data->tf->ss = USEL(SEG_USER_DATA);
	data->tf->eflags = 0x202;

}

void
schedule(void) {
//	printk("enter schedule\n");
//	printk("rescheduled:%d\n",rescheduled);
	if (rescheduled || current == &idle) {
		if (list_empty(&ready)) {
			current = &idle;
			rescheduled = 1;
			return ;
		}
		else {
//			printk("switch\n");
			current = list_to_pcb(ready.next);
			current->run_time = 10;			
			tss.esp0 = (uint32_t)(current->kstack + KSTACK_SIZE);
			rescheduled = 0;
//			printk("seg_base: %x\n", current->seg_base);
			gdt[SEG_USER_CODE] = SEG(STA_X | STA_R, current->seg_base, 0xffffffff, DPL_USER);
			gdt[SEG_USER_DATA] = SEG(STA_W,         current->seg_base, 0xffffffff, DPL_USER);
			return ;
		}
	}
}
