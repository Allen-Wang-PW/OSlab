#ifndef __COMMON_H__
#define __COMMON_H__

#include "types.h"
#include "const.h"
#include "assert.h"


void printk(const char *ctl, ...);
//void vfprintf(void (*pointer)(char), const char *, void **);
uint32_t loadmain(void);

#define SYS_write 1
//#define SYS_time 203
#define SYS_keyboard 2
#define SYS_screen 3
#define SYS_fork 4
#define SYS_sleep 5
#define SYS_exit 6
#define SYS_createsem 7
#define SYS_destroysem 8
#define SYS_P 9
#define SYS_V 10
#define SYS_createThread 11
#define FILE_OPEN 12
#define FILE_CLOSE 13
#define FILE_READ 14
#define FILE_WRITE 15
#define FILE_SEEK 16
#define FILE_GET 17
#define SIZE_GET 18

#endif
