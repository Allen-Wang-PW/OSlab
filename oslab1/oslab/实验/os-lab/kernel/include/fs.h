#ifndef __FS_H__
#define __FS_H__

#include "common.h"

#define SECTSIZE 512
#define BLOCK_SIZE 512
#define NR_ENTRIES 4
#define NR_FILES 4
#define NR_BITMAP 512 * 256 * 8
#define NR_FCB 4
struct Bitmap {
	uint8_t mask[4096];
};
typedef struct Bitmap Bitmap;

struct Dirent {
	char filename[24];
	uint32_t file_size;
	uint32_t inode_offset;
};
typedef struct Dirent Dirent;

struct Dir {
	struct Dirent entries[512 / sizeof(struct Dirent)];
};
typedef struct Dir Dir;

struct iNode {
	uint32_t data_block_offsets[512 * 4096 / sizeof(uint32_t)];
};
typedef struct iNode iNode;

struct FCB {
	char filename[24];
	int opened;
	int offset;
	int size;
	uint32_t block[512 * 4096 / sizeof(uint32_t)];
};
typedef struct FCB FCB;

FCB fcbpool[NR_FCB];

enum {SEEK_SET, SEEK_CUR, SEEK_END};

void initfcb(void);
int fs_open(const char *);
int fs_read(int, void *, int);
int fs_write(int, void *, int);
int fs_close(int);
int fs_lseek(int ,int, int);
void getfilename(void*);
void getfilesize(void*);

Bitmap bitmap;
Dir dir;

int ide_read(uint32_t secno, void *dst, size_t nsecs);
int ide_write(uint32_t secno, const void *src, size_t nsecs);
#endif
