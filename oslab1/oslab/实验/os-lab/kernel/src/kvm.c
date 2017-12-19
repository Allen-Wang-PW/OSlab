#include "x86/memory.h"
#include "mmu.h"
#include "boot.h"
#include "common.h"
#include "process.h"

//static void ide_read(uint8_t *addr, uint32_t offset, int32_t size);

#define SECTSIZE 512

static uint32_t user_entry;

Segdesc gdt[NR_SEGMENTS];
TSS tss;

void readseg(unsigned char *, int, int);
void readsect(void*, int);
uint32_t loadmain(void);

void
init_seg() {
	gdt[SEG_KERNEL_CODE] = SEG(STA_X | STA_R, 0, 0xffffffff, DPL_KERNEL);
	gdt[SEG_KERNEL_DATA] = SEG(STA_W,		  0, 0xffffffff, DPL_KERNEL);
	gdt[SEG_USER_CODE] = SEG(STA_X | STA_R, 0x3000000, 0xffffffff, DPL_USER);
	gdt[SEG_USER_DATA] = SEG(STA_W,			0x3000000, 0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,			&tss, sizeof(TSS) - 1, DPL_KERNEL);
	gdt[SEG_TSS].sd_s = 0;
	set_gdt(gdt, sizeof(gdt));

	int i;
	for (i = 0; i<NR_SEGMENTS;i++)
		printk("%x,%x\n", *(int*)&gdt[i],*((int*)&gdt[i]+1));

	// initialize TSS
	unsigned int temp;
	asm volatile("movl %%esp,%0":"=m"(temp):);
	tss.esp0 = temp;
	tss.ss0 = KSEL(SEG_KERNEL_DATA);
	ltr(KSEL(SEG_TSS));
//	asm volatile("ltr %%ax" : : "a"(KSEL(SEG_TSS)));
	// set correct segment register
	asm volatile("movw %%ax,%%es" : : "a"(KSEL(SEG_KERNEL_DATA)));
	asm volatile("movw %%ax,%%ds" : : "a"(KSEL(SEG_KERNEL_DATA)));
	asm volatile("movw %%ax,%%ss" : : "a"(KSEL(SEG_KERNEL_DATA)));

	lldt(0);
}

void
enter_user_space(void) {
	// prepare data needed by iret: eip, cs, eflags, esp, ss

	printk("prepare to enter user space\n");
	user_entry = loadmain();
	printk("user_entry : %x\n", user_entry);
	// set correct segment registers
	asm volatile("pushl %%eax" : : "a"(USEL(SEG_USER_DATA)));
	asm volatile("pushl %%eax" : : "a"(0x3000000));
	asm volatile("pushf");
	asm volatile("pushl %%eax" : : "a"(USEL(SEG_USER_CODE)));
	asm volatile("pushl %%eax" : : "a"(user_entry));
//	asm volatile("pushl %%eax" : : "a"(USEL(SEG_USER_DATA)));
//	asm volatile("pushl %%eax" : : "a"(USEL(SEG_USER_DATA)));
	asm volatile("movw %%ax,%%ds" : : "a"(USEL(SEG_USER_DATA)));
	asm volatile("movw %%ax,%%es" : : "a"(USEL(SEG_USER_DATA)));
	asm volatile("iret");
}

uint32_t
loadmain(void) {
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;
	unsigned char *pa, *i;

//	uint8_t buf[4096];
	elf = (struct ELFHeader*)0x8000;

//	while (1);
//	printk("enter\n");
//	while (1);
	readseg((unsigned char*)elf, 4096, 0x732400);

//	printk("out\n");
//	printk("entry : %x\n", elf->entry);
	ph = (struct ProgramHeader*)((char*)elf + elf->phoff);
	eph = ph + elf->phnum;
	for (; ph < eph; ph++) {
		pa = 0x3000000 + (unsigned char*)ph->paddr;
		readseg(pa, ph->filesz, 0x732400 + ph->off);
		for (i = pa + ph->filesz; i < pa + ph->memsz; *i ++ = 0);
	}
	return elf->entry;
}

void
waitdisk(void) {
	 while((in_byte(0x1F7) & 0xC0) != 0x40);
}

void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

/*
void
readseg(unsigned char *pa, int count, int offset) {
	unsigned char *epa;
	epa = pa + count;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 1;
//	printk("in\n");
	for(; pa < epa; pa += SECTSIZE, offset ++)
		readsect(pa, offset);
}
*/

void
readseg(unsigned char *pa, int count, int offset) {
	int i, op = 0, end = 0;
	uint32_t off = offset / SECTSIZE;
	uint32_t td = (uint32_t)pa - offset % SECTSIZE;
	uint32_t final = (uint32_t)pa + count;

	uint8_t sect[512];
	for (; td < final; td += SECTSIZE, off++) {
		readsect(sect, off);
		if (td > (uint32_t)pa)
			op = td;
		else
			op = (uint32_t)pa;
		if (td + SECTSIZE < final)
			end = td + SECTSIZE;
		else
			end = final;
		for (i = op; i < end; i++)
			((uint8_t*)i)[0] = sect[i - td];
	}
}
