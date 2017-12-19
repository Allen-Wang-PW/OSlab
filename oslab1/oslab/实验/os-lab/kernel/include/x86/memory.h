#ifndef __X86_MEMORY_H__
#define __X86_MEMORY_H__

#include "types.h"
#define DPL_KERNEL              0
#define DPL_USER                3


// Application segment byte bits
#define STA_X					0x8
#define STA_W					0x2
#define STA_R					0x2

// System segment type bits
#define STS_T32A				0x9
#define STS_IG32				0xE
#define STS_TG32				0xF

// GDT entries
#define NR_SEGMENTS             7
#define SEG_KERNEL_CODE         1 
#define SEG_KERNEL_DATA         2
#define SEG_USER_CODE			3
#define SEG_USER_DATA			4
#define SEG_TSS					5

// Selectors
#define KSEL(desc) (((desc) << 3) | DPL_KERNEL)
#define USEL(desc) (((desc) << 3) | DPL_USER)

struct GateDescriptor {
	uint32_t offset_15_0      : 16;
	uint32_t segment          : 16;
	uint32_t pad0             : 8;
	uint32_t type             : 4;
	uint32_t system           : 1;
	uint32_t privilege_level  : 2;
	uint32_t present          : 1;
	uint32_t offset_31_16     : 16;
};

struct TrapFrame {
	uint32_t ds, es;
	uint32_t edi, esi, ebp, xxx, ebx, edx, ecx, eax;
	int32_t irq;
	uint32_t error_code, eip, cs, eflags, esp, ss;
};

/*
struct SegDesc {
	uint32_t lim_15_0		:16;
	uint32_t base_15_0		:16;
	uint32_t base_23_16		:8;
	uint32_t type			:4;
	uint32_t s				:1;
	uint32_t dpl			:2;
	uint32_t p				:1;
	uint32_t lim_19_16		:4;
	uint32_t avl			:1;
	uint32_t rsv1			:1;
	uint32_t db				:1;
	uint32_t g				:1;
	uint32_t base_31_24		:8;
};
typedef struct SegDesc SegDesc;


#define SEG(type, base, lim, dpl) (SegDesc) {				\
	((lim) >> 12) & 0xffff, (uint32_t)(base) & 0xffff,		\
	((uint32_t)(base) >> 16) & 0xff, type, 1, dpl, 1,		\
	(uint32_t)(lim) >> 28, 0, 0, 1, 1, (uint32_t)(base) > 24\
}

#define SEG16(type, base, lim, dpl) (SegDesc) {				\
	(lim) & 0xffff, (uint32_t)(base) & 0xffff,				\
	((uint32_t)(base) >> 16) & 0xff, type, 0, dpl, 1,		\
	(uint32_t)(lim) >> 16, 0, 0, 1, 0, (uint32_t)(base) >> 24\
}
*/
// Task state segment format
/*
struct TSS {
	uint32_t link;
	uint32_t esp0;
	uint32_t ss0;
	union {
		struct {
			char dontcare[88];
		};
		struct {
			uint32_t esp1, ss1, esp2, ss2;
			uint32_t cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
			uint32_t es, cs, ss, ds, fs, gs, ldt;
		};
	};
};
typedef struct TSS TSS;

static inline void
set_gdt(SegDesc *gdt, uint32_t size) {
	volatile static uint16_t data[3];
	data[0] = size - 1;
	data[1] = (uint32_t)gdt;
	data[2] = (uint32_t)gdt >> 16;
	asm volatile("lgdt (%0)" : : "r"(data));
}

static inline void
lldt(uint16_t sel) {
	asm volatile("lldt %0" : : "r"(sel));
}

static inline void
ltr(uint16_t sel) {
	asm volatile("ltr %0" : : "r"(sel));
}
*/
#endif
