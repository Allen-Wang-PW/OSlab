#include "fs.h"
#include "string.h"
#include "common.h"
#include "assert.h"

extern Bitmap bitmap;
extern Dir dir;
void readseg(unsigned char *, int, int);
void readsect(void *, int);
void load_file(void) {
	readseg((unsigned char*)&bitmap, sizeof(bitmap), 401 * 512);
	readseg((unsigned char*)&dir, sizeof(dir), 2449 * 512);
	printk("%s\n", dir.entries[0].filename);
	assert(strcmp(dir.entries[0].filename, "game.bin") == 0);
}
