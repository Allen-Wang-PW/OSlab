#include "common.h"
//#include "stdlib.h"

/* implement this function to support printk */
static void vfprintf(void (*printer)(char), const char *ctl, void **args) {
//	const char *str = __FUNCTION__;
//	for(;*str != '\0'; str ++) printer(*str);

//	str = ": vfprintf() is not implemented!\n";
//	for(;*str != '\0'; str ++) printer(*str);
//	int integer = 0;
	char buf[64] = {'\0'};
	char *str = NULL;
	char *ptr = NULL;
//	int str_len = 0;
	int len = 64;
	unsigned int uinteger = 0;
//	int count = 0;
//	bool sign = 0;
	while (*ctl){
		if (*ctl != '%'){
			printer(*ctl);
			ctl ++;
		}
		else{
			switch (*(++ctl)){
				case 'd':
				/*
					len = 64;
					str = *(char **)args;
					if (*str == '0' && *(str + 1) == 'x'){
						str += 1;
						while (*(++str) != '\0')
							str_len ++;
						for (; count <= str_len; count ++){
							if (str[str_len] >= 'a' && str[str_len] <= 'f'){
								integer += str[str_len] - 'a' + 10;
							}
							else if (str[str_len] >= 'A' && str[str_len] <= 'F'){
								integer += str[str_len] - 'A' + 10;
							}
							else
								integer += str[str_len] - '0';
							integer <<= 4;
						}
					}
					else{
						integer = *(int *)args;
					}
					if (integer < 0){
						printer('-');	
						integer = -integer;
					}					
					while (integer){
						buf[--len] = integer % 10 + '0';
						integer /= 10;
					}
					for (; len <= 63 ; len ++){
						printer(buf[len]);
					}
				*/
					uinteger = *(unsigned int *)args;
					if ((int)uinteger < 0){
						printer('-');
						uinteger = -uinteger;
					}
//					if ((int)uinteger == 0) {
//						printer('0');
//						break;
//					}
					ptr = buf + sizeof(buf) - 1;
					while (uinteger){
						*(--ptr) = '0' + uinteger % 10;
						uinteger /= 10;
					}
					while (*ptr){
						printer(*ptr ++);
					}
					args ++;
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
						for (; len < 64; len ++){
							printer(buf[len]);
						}
					}
					args ++;
					break;

				case 's':
					str = *(char **)args;
					while (*str != '\0'){
						printer(*str);
						str ++;
					}
					args ++;		
					break;

				case 'c':
					printer(*(char *)args);
					args ++;
					break;

				default:
					printer(*ctl);
					break;
			}
			ctl ++;
		}	
	}
}

extern void serial_printc(char);

/* __attribute__((__noinline__))  here is to disable inlining for this function to avoid some optimization problems for gcc 4.7 */
void __attribute__((__noinline__)) 
printk(const char *ctl, ...) {
	void **args = (void **)&ctl + 1;
	vfprintf(serial_printc, ctl, args);
}
