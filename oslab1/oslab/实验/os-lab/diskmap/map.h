#include "stdio.h"
#include "stdint.h"

#define BLOCK_SIZE 512
#define NR_BITMAP 512 * 256 * 8
#define NR_ENTRIES 16
#define NR_FILES 3 

#pragma pack(0)
struct bitmap {
	uint8_t mask[512 * 256 * 8];
};

struct dirent {
	char filename[24];
	uint32_t file_size;
	uint32_t inode_offset;
};

struct dir {
	struct dirent entries[512 / sizeof(struct dirent)];
};

struct inode {
	uint32_t data_block_offsets[512 * 4096 / sizeof(uint32_t)];
};

#pragma pack()
