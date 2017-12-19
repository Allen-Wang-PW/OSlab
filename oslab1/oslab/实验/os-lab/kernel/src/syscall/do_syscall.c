#include "common.h"
#include "x86/x86.h"
#include "mmu.h"
#include "process.h"
#include "fs.h"

#define SRC_SIZE 800*600

uint32_t scancode;
extern uint32_t num_of_user_proc;

extern Segdesc gdt[NR_SEGMENTS];

bool newkey = FALSE;

void load_code(uint32_t val){
	scancode = val;
//	printk("key: %d\n", scancode);
	newkey = TRUE;
}

void serial_printc(char);
bool query_key(int);
uint32_t get_base(uint32_t index) {
	return gdt[index].sd_base_15_0 + (gdt[index].sd_base_23_16 << 16) + (gdt[index].sd_base_31_24 << 24);
}

void sys_printf(struct TrapFrame *tf) {
//	printk("%s", (const char *)tf->ebx);
	int i;
	for (i = 0; i < tf->ecx; i++) {
		serial_printc(*(char *)(tf->ebx + i + current->seg_base));
	}
	tf->eax = tf->ecx;
}

void sys_keyboard(struct TrapFrame *tf) {
	if (newkey) {
		tf->eax = scancode;
//		printk("%d\n",scancode);
		newkey = FALSE;
	}
	else {
		tf->eax = -1;
	}
}

void sys_screen(struct TrapFrame *tf) {
	uint32_t physicbase = *((uint32_t *)(0x2000 + 40));
	uint8_t *pixels = (void*)(physicbase);
	int i;
	for (i = 0; i < SRC_SIZE * 3; i++) 
		*(pixels + i) = *((uint8_t *)(tf->ebx + i + 0x3000000));	
}

void sys_fork(struct TrapFrame *tf) {
	PCB *child = new_pcb();
	child->tf = (struct TrapFrame*)(child->kstack + KSTACK_SIZE - sizeof(struct TrapFrame));
	*(child->tf) = *tf;
//	memcpy((void*)(get_base((child->tf->ds) >> 3)), (void*)(get_base((tf->ds) >> 3)), PROCESS_MEM_SIZE);
	memcpy((void*)(child->seg_base), (void*)(current->seg_base), PROCESS_MEM_SIZE);
	tf->eax = getpid(child);


	/* ensure the PCB data of child */
//	child->tf = (struct TrapFrame*)(child->kstack + KSTACK_SIZE - sizeof(struct TrapFrame));

	/* child process return value */
	child->tf->eax = 0;

	/* set segment registers */
//	child->tf->cs = USEL(seg_code);
//	child->tf->ds = USEL(seg_data);
//	child->tf->es = USEL(seg_data);
//	child->tf->ss = USEL(seg_data);
}

void sys_exit(void) {
//	switch_free_queue(current);
	list_del(&(current->list));
	list_add_before(&free, &(current->list));
	num_of_user_proc--;
	rescheduled = 1;
}

void sys_sleep(struct TrapFrame *tf) {
//	switch_block_queue(current);
	list_del(&(current->list));
	list_add_before(&block, &(current->list));
	current->sleep_time = tf->ebx * 100;
	rescheduled = 1;
}

void sys_createsem(struct TrapFrame *tf) {
//	printk("ebx: %d\n", tf->ebx);
	tf->eax = sem_open(tf->ebx);
}

void sys_destroysem(struct TrapFrame *tf) {
	sem_close(tf->ebx);
}

void sys_p(struct TrapFrame *tf) {
	sem_wait(tf->ebx);
}

void sys_v(struct TrapFrame *tf) {
	sem_post(tf->ebx);
}

void sys_createThread(struct TrapFrame *tf) {
//	printk("in\n");
//	printk("tf->ebx=%d\n",tf->ebx);
	create_kthread((void*)tf->ebx);
}

void sys_fileopen(struct TrapFrame *tf) {
	printk("%s\n", (char*)tf->ecx);
//	int i;
//	for ( i=0; i<10; i++)
//	printk("sys %c\n", *((char *)(tf->ecx)+i));
	tf->eax = fs_open((char *)(tf->ebx + 0x3000000));
}

void sys_fileclose(struct TrapFrame *tf) {
	fs_close(tf->ebx);
}

void sys_fileread(struct TrapFrame *tf) {
	fs_read(tf->ebx, (void *)(tf->ecx + 0x3000000), tf->edx);
}

void sys_filewrite(struct TrapFrame *tf) {
	fs_write(tf->ebx, (void *)(tf->ecx + 0x3000000), tf->edx);
}

void sys_fileseek(struct TrapFrame *tf) {
	fs_lseek(tf->ebx, tf->ecx, tf->edx);
}

void sys_tellfilename(struct TrapFrame *tf) {
	getfilename((void *)(tf->ebx + 0x3000000));
}

void sys_tellfilesize(struct TrapFrame *tf) {
	getfilesize((void *)(tf->ebx + 0x3000000));
}

void do_syscall(struct TrapFrame *tf) {
	switch(tf->eax){
		case SYS_write:
			sys_printf(tf);
			break;
//		case SYS_time:
//			break;
		case SYS_keyboard:
			sys_keyboard(tf);
			break;
		case SYS_screen:
			sys_screen(tf);
			break;
		case SYS_fork:
			sys_fork(tf);
			break;
		case SYS_exit:
			sys_exit();
			break;
		case SYS_sleep:
			sys_sleep(tf);
			break;
		case SYS_createsem:
			sys_createsem(tf);
			break;
		case SYS_destroysem:
			sys_destroysem(tf);
			break;
		case SYS_P:
			sys_p(tf);
			break;
		case SYS_V:
			sys_v(tf);
			break;
		case SYS_createThread:
			sys_createThread(tf);
			break;
		case FILE_OPEN:
			sys_fileopen(tf);
			break;
		case FILE_CLOSE:
			sys_fileclose(tf);
			break;
		case FILE_READ:
			sys_fileread(tf);
			break;
		case FILE_WRITE:
			sys_filewrite(tf);
			break;
		case FILE_SEEK:
			sys_fileseek(tf);
			break;
		case FILE_GET:
			sys_tellfilename(tf);
			break;
		case SIZE_GET:
			sys_tellfilesize(tf);
			break;
		default:
			printk("Unhandled system call: id = %d", tf->eax);
			assert(0);
	}
}
