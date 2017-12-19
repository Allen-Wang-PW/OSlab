#include "common.h"

void vfprintf(void (*printer)(char), const char *ctl, void **args) {
	char buf[64] = {'\0'};
	char *str = NULL;
	char *ptr = NULL;
	int len = 64;
	unsigned int uinteger = 0;

	while (*ctl) {
		if (*ctl != '%') {
			printer(*ctl);
			ctl++;
		}
		else {
			switch(*(++ctl)) {
				case 'd':
					uinteger = *(unsigned int *)args;
					if ((int)uinteger < 0){
						printer('-');
						uinteger = -uinteger;
					}
					ptr = buf + sizeof(buf) - 1;
					while(uinteger){
						*(--ptr) = '0' + uinteger % 10;
						uinteger /= 10;
					}
					while(*ptr){
						printer(*ptr++);
					}
					args++;
					break;
				case 'x':
					len = 64;
					str = *(char **)args;
					if (*str == '0' && *(str + 1) == 'x'){
						str += 1;
						while (*(++str) != '\0')
							printer(*str);
					}
					else {
						uinteger = *(uint32_t *)args;
						while (uinteger){
							buf[--len] = ((uinteger % 16) < 10) ? (uinteger % 16 + '0') : ('a' + (uinteger % 16) - 10);
							uinteger >>= 4;
						}
						for (; len < 64; len++){
							printer(*str);
							str++;
						}
					}
					args++;
					break;
				case 's':
					str = *(char **)args;
					while (*str != '\0'){
						printer(*str);
						str++;
					}
					args++;
					break;
				case 'c':
					printer(*(char *)args);
					args++;
					break;
				default:
					printer(*ctl);
					break;
			}
			ctl++;
		}
	}
}

						

char buf[256];
int len;

void printc(char c) {
	buf[len++] = c;
}

void write(char*, int);

void __attribute__((__noinline__))
printf(const char *ctl, ...) {
	len = 0;
	void **args = (void **)&ctl + 1;
	vfprintf(printc, ctl, args);
	write(buf, len);
}
