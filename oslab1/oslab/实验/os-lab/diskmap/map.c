#include "map.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct bitmap Bitmap;
struct dir Dir;
int alloc_bitmap();

int main(int argc, char *argv[]) {
	FILE *fp;
	if ((fp = fopen(argv[1], "wb")) == NULL) {
		printf("Open File %s Failure!\n", argv[1]);
		return -1;
	}

	/* Expand the file to 2MB */
	char expansion[512];
	memset(expansion, 0, sizeof(expansion));
	int b;
	fseek(fp, 0, SEEK_SET);
	for (b = 0; b < 4 * 1024; b++) {
		fwrite(expansion, 1, 512, fp);
	}

	/* initialize the Bitmap.mask and Dir.entries */
//	printf("%d, %d\n", sizeof(Bitmap.mask), sizeof(Dir.entries));
	memset(Bitmap.mask, 0, sizeof(Bitmap.mask));
	memset(Dir.entries, 0, sizeof(Dir.entries));

	int i;

	/* initialize the filename, file_size and inode_offset of Dir.entries */
	for (i = 0; i < NR_ENTRIES; i++) {
		memset(Dir.entries[i].filename, '\0', sizeof(Dir.entries[i].filename));
		Dir.entries[i].file_size = 0;
		Dir.entries[i].inode_offset = 0;
	}

	struct inode biginode[NR_FILES];
	/* write files into the certain file */
	for (i = 2; i < argc; i++) {
		FILE *tempfp;
		if ((tempfp = fopen(argv[i], "rb")) == NULL) {
			printf("Open File %s Failure!", argv[i]);
			return -1;
		}
		/* write filename and filesize to Dir.entries */
		printf("%s..\n", argv[i]);
		strcpy(Dir.entries[i-2].filename, argv[i]);
//		printf("Dir.entries[%d].filename = %s.\n", i-2, Dir.entries[i-2].filename);
		struct stat temp;
		stat(argv[i], &temp);
		Dir.entries[i-2].file_size = temp.st_size;	
	//	printf("size = %d.\n", Dir.entries[i-2].file_size);
		fseek(tempfp, 0, SEEK_SET);

		int cnt = 0;
		char buf[512];
		int a;
		memset(buf, '\0', sizeof(buf));
		for (a = 0; a < Dir.entries[i-2].file_size / 512 + 1; a++) {
			fread(buf, 1, 512, tempfp);
			int j = alloc_bitmap();
//			printf("alloc %d\n", j);
//			printf("cnt %d\n",cnt);
			biginode[i-2].data_block_offsets[cnt++] = j;
//			fseek(fp, (j + 1) * 512 + sizeof(struct bitmap), SEEK_SET);
			fseek(fp, j * 512 + sizeof(struct bitmap) + 512 + sizeof(biginode), SEEK_SET);
//			struct stat file;
//			stat(argv[1], &file);
//			printf("file size %d\n", file.st_size);
			fwrite(buf, 1, 512, fp);
			memset(buf, '\0', sizeof(buf));
		}
//		int k = alloc_bitmap();
//		printf("k %d\n", k);
		Dir.entries[i-2].inode_offset = i - 2;
//		fseek(fp, (k + 1) * 512 + sizeof(struct bitmap), SEEK_SET);
//		fseek(fp, k * 512 + sizeof(struct bitmap) + sizeof(struct dir), SEEK_SET);
//		fwrite(&tempinode, 1, sizeof(struct inode), fp);

		fclose(tempfp);
	}
	fseek(fp, sizeof(struct bitmap) + 512, SEEK_SET);
	fwrite(&biginode, 1, sizeof(biginode), fp);
	printf("size %x\n", sizeof(biginode));
//	printf("Dir %s\n", Dir.entries[0].filename);
	fseek(fp, sizeof(struct bitmap), SEEK_SET);
//	printf("pos %d\n", ftell(fp));
	fwrite(&Dir, 1, 512, fp);
	fseek(fp, 0, SEEK_SET);
	fwrite(&Bitmap, 1, sizeof(struct bitmap), fp);

	fclose(fp);
	return 0;
}

int alloc_bitmap() {
	int i;
	for (i = 0; i < NR_BITMAP; i++) {
			if (!Bitmap.mask[i]) {
				Bitmap.mask[i] = 1;
				return i;
			}
		}
	printf("pass\n");
	return -1;
}
