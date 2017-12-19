#include "../include/common.h"

void printf(const char *ctl, ...);
int __attribute__((__noinline__))
syscall(int id, ...) {
	int ret;
	int *args = &id;
	asm volatile("int $0x80": "=a"(ret) : "a"(args[0]), "b"(args[1]), "c"(args[2]), "d"(args[3]));
	return ret;
}

void write(char *buf, int len) {
	syscall(SYS_write, (int)buf, len, 0);
}

void update_keyboard(int *key) {
	*key = syscall(SYS_keyboard, key, 0, 0);
}

void screendraw(uint8_t *buf) {
	syscall(SYS_screen, (int)buf, 0, 0);
}

int fork() {
	return syscall(SYS_fork, 0, 0, 0);
}

void sleep(uint32_t time) {
	syscall(SYS_sleep, (int)time, 0, 0);
}

void exit(int argv) {
	syscall(SYS_exit, argv, 0, 0);
}

sem_t createSem(int argv) {
	return syscall(SYS_createsem, argv, 0, 0);
}

void destroySem(int argv) {
	syscall(SYS_destroysem, argv, 0, 0);
}

void P(int argv) {
	syscall(SYS_P, argv, 0, 0);
}

void V(int argv) {
	syscall(SYS_V, argv, 0, 0);
}

void create_thread(void *argv) {
	syscall(SYS_createThread, (int)argv, 0, 0);
}

int open(const char *argv) {
	return syscall(FILE_OPEN, (int)argv, 0, 0);
}

void close(int argv) {
	syscall(FILE_CLOSE, argv, 0, 0);
}

void mfread(int fd, void *buf, int len) {
	syscall(FILE_READ, fd, (int)buf, len);
}

void mfwrite(int fd, void *buf, int len) {
	syscall(FILE_WRITE, fd, (int)buf, len);
}

void seek(int fd, int offset, int whence) {
	syscall(FILE_SEEK, fd, offset, whence);
}

void getfile(void *buf) {
	syscall(FILE_GET, (int)buf);
}

void getsize(void *size) {
	syscall(SIZE_GET, (int)size);
}

int
press_key_code(void) {
	int *key = NULL;
	update_keyboard(key);
	return *key;
}

void
printkey(int key) {
	switch (key) {
		case 28:
			  printf("\n");
			  break;
		case 38:
			  printf("l");
			  break;
		case 31:
			  printf("s");
			  break;
		case 57:
			  printf(" ");
			  break;
		case 12:
			  printf("-");
			  break;
		case 30:
			  printf("a");
			  break;
		case 20:
			  printf("t");
			  break;
		case 46:
			  printf("c");
			  break;
		case 45:
			  printf("x");
			  break;
		case 34:
			  printf("g");
			  break;
		case 50:
			  printf("m");
			  break;
		case 18:
			  printf("e");
			  break;
		case 52:
			  printf(".");
			  break;
		case 48:
			  printf("b");
			  break;
		case 23:
			  printf("i");
			  break;
		case 49:
			  printf("n");
			  break;
		case 35:
			  printf("h");
			  break;
		case 37:
			  printf("k");
			  break;
		case 32:
			  printf("d");
			  break;
		case 24:
			  printf("o");
			  break;
		case 33:
			  printf("f");
			  break;
		case 36:
			  printf("j");
			  break;
		case 25:
			  printf("p");
			  break;
		case 16:
			  printf("q");
			  break;
		case 13:
			  printf("r");
			  break;
		case 22:
			  printf("u");
			  break;
		case 47:
			  printf("v");
			  break;
		case 17:
			  printf("w");
			  break;
		case 21:
			  printf("y");
			  break;
		case 44:
			  printf("z");
			  break;
	}
}

void
save_key_code(int *code) {
	int cnt = 0;
	while (TRUE) {
		int temp = press_key_code();
		if (temp != -1 && temp < 100) {
			code[cnt++] = temp;
			printkey(temp);
			if (temp == 28) {
				break;
			}
		}
	}
}
