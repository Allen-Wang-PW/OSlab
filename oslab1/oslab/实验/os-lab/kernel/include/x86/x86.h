#ifndef __X86_H__
#define __x86_H__

#include "cpu.h"
#include "memory.h"
#include "io.h"

static __inline uint8_t inb(int port) __attribute__((always_inline));
static __inline void outb(int port, uint8_t data) __attribute__((always_inline));
static __inline void insl(int port, void *addr, int cnt) __attribute__((always_inline));
static __inline void outsl(int port, const void *addr, int cnt) __attribute__((always_inline));

static __inline uint8_t
inb(int port) {
	uint8_t data;
	__asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static __inline void
outb(int port, uint8_t data) {
	__asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static __inline void
insl(int port, void *addr, int cnt) {
	__asm __volatile("cld\n\trepne\n\tinsl"		:
			"=D" (addr), "=c" (cnt)		:
			"d" (port), "0" (addr), "1" (cnt)	:
			"memory", "cc");
}

static __inline void
outsl(int port, const void *addr, int cnt) {
	__asm __volatile("cld\n\trepne\n\toutsl"		:
			"=S" (addr), "=c" (cnt)		:
			"d" (port), "0" (addr), "1" (cnt)	:
			"cc");
}
#endif
