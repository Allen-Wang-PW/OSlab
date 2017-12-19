#include "x86/x86.h"
#include "game.h"
#include "string.h"
#include "device/timer.h"
#include "device/video.h"
#include "process.h"

#include "puzzle/ren0.h"
#include "puzzle/ren1.h"
#include "puzzle/ren2.h"
#include "puzzle/ren3.h"
#include "puzzle/ren4.h"
#include "puzzle/ren5.h"
#include "puzzle/ren6.h"
#include "puzzle/ren7.h"
#include "puzzle/ren8.h"
#include "puzzle/ren9.h"
#include "puzzle/ren10.h"
#include "puzzle/ren11.h"
#include "puzzle/ren12.h"

#define FPS 30
#define CHARACTER_PER_SECOND 5
#define UPDATE_PER_SECOND 100

#define PIC_X 200
#define PIC_Y 200

#define SCREEN_SIZE 800*600

enum {SEEK_SET, SEEK_CUR, SEEK_END};

volatile int tick = 0;

static int puzzle_map[3][4] = {
	{2, 6, 4, 8},
	{9, 7, 11, 3},
	{10, 5, 12, 1}
};

void
timer_event(void) {
	tick ++;
}

static int real_fps;
void
set_fps(int value) {
	real_fps = value;
}
int
get_fps() {
	return real_fps;
}

void printf(const char *ctl, ...);
uint8_t screen_buf[3*SCREEN_SIZE];
bool update_keypress(void);
int fork();
void sleep(uint32_t);
void exit(int);
sem_t createSem(int);
void destroySem(int);
void P(int);
void V(int);
void create_thread(void*);
int open(const char *);
void close(int);
void mfread(int, void*, int);
void mfwrite(int, void*, int);
void seek(int, int, int);
void getfile(void*);
void getsize(void*);
void save_key_code(int*);
void printkey(int);

void
print_puzzle(void) {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++)
			printf("%d, ",puzzle_map[i][j]);
		printf("\n");
	}
}
void
init_puzzle(void) {
//	uint32_t physicbase = *((uint32_t *)(0x2000 + 40));
	int w, h, i, j;
    int count = 0;
    uint8_t *dibits = NULL;
    for (h = 0; h < 3; h++) {
    	for (w = 0; w < 4; w++) {
        	count++;
            for (i = 0; i < PIC_X; i++) {
               	for (j = 0; j < PIC_Y; j++) {
//                   	uint8_t *pixels = (void *)(physicbase + 3 * (i + PIC_X * w) + (j + PIC_Y * h) * 3 * 800);
					uint8_t *pixels = screen_buf + 3 * (i + PIC_X * w) + (j + PIC_Y * h) * 3 * 800;
                    switch (count) {
                    	case 12:	dibits = (void *)(__0_bgr + 3 * i +     j * 3 * PIC_Y);
                        		break;
                        case 1: dibits = (void *)(__2_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 8: dibits = (void *)(__3_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 3: dibits = (void *)(__4_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 10: dibits = (void *)(__5_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 2: dibits = (void *)(__6_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 6: dibits = (void *)(__7_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 4: dibits = (void *)(__8_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 5: dibits = (void *)(__9_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 9:    dibits = (void *)(__10_bgr + 3 * i + j * 3 * PIC_Y);	
								break;
						case 7:    dibits = (void *)(__11_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
                        case 11:    dibits = (void *)(__12_bgr + 3 * i + j * 3 * PIC_Y);
                                break;
//                        default:    assert(0);
					}       
//                      uint8_t *dibits = (void *)(__1_bgr + 3 * i + j * 600    );
//                    if (count != 1){
                        pixels[0] = dibits[0];
						pixels[1] = dibits[1];
						pixels[2] = dibits[2];
//					}
				}
			}
		}
	}
}

void
draw_pic(int no, int x, int y) {
	printf("Draw\n");
	printf("no:%d\n", no);
//	uint32_t physicbase = *((uint32_t *)(0x2000 + 40));
	int i, j;
	uint8_t *dibits=NULL;
	for (i = 0; i < PIC_X; i++) {
		for (j = 0; j < PIC_Y; j++) {
//			uint8_t *pixels = (void *)(physicbase + 3 * (i + PIC_X * x) + (j + PIC_Y * y) * 3 * 800);
			uint8_t *pixels = (void *)(screen_buf + 3 * (i + PIC_X * x) + (j + PIC_Y * y) * 3 * 800);
			switch (no) {
				case 1:	dibits = (void *)(__0_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 2: dibits = (void *)(__2_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 3:	dibits = (void *)(__3_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 4:	dibits = (void *)(__4_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 5:	dibits = (void *)(__5_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 6:	dibits = (void *)(__6_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 7:	dibits = (void *)(__7_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 8:	dibits = (void *)(__8_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 9:	dibits = (void *)(__9_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 10:	dibits = (void *)(__10_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 11:	dibits = (void *)(__11_bgr + 3 * i + j * 3 * PIC_Y);
						break;
				case 12:	dibits = (void *)(__12_bgr + 3 * i + j * 3 * PIC_Y);
						break;
//				default:	assert(0);
			}
//			if (no != 1) {
				pixels[0] = dibits[0];
				pixels[1] = dibits[1];
				pixels[2] = dibits[2];
//			}
		}
	}
}

void
uentry(void) {
/*
	int a;
	for (a = 0; a < 10; a++) {
		printf("sleep");
		sleep(10);
	}
	exit(0);
*/
	int i, ret;
	if ((ret = fork()) == 0) {
		for (i = 0; i < 10; i++) {
			printf("Pong\n");
			sleep(10);
		}
		exit(0);
	}
	for (i = 0; i < 10; i++) {
		printf("Ping\n");
		sleep(10);
	}
	exit(0);
}

int test_cnt = 0;
void
prod(void) {
	while (TRUE) {
		test_cnt++;
		printf("prod %d\n", test_cnt);
		sleep(10);
	}
}

void
cons(void) {
	while (TRUE) {
		test_cnt++;
		printf("cons %d\n", test_cnt);
		sleep(10);
	}
}

void
test_thread(void) {
	create_thread(prod);
	create_thread(cons);
	while (TRUE)
		;
}

#define NBUF 5
#define NR_PROD 4
#define NR_CONS 3
int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
sem_t empty, full, mutex;

int cnt = 0;
void
test_producer(void) {
	while (TRUE) {
//		printf("producer: mutex:%d full:%d empty:%d\n", mutex, full, empty);
		P(empty);
//		printf("producer: mutex:%d full:%d empty:%d\n", mutex, full, empty);
		P(mutex);
//		if (g % 10000 == 0)
		cnt++;
			printf("produce %d\n", cnt);
		buf[f++] = g++;
		f %= NBUF;
		V(mutex);
		V(full);
		sleep(10);
//		printf("producer: mutex:%d full:%d empty:%d\n", mutex, full, empty);
	}
}

void
test_consumer(void) {
	int get;
	while (TRUE) {
		P(full);
		P(mutex);
		get = buf[r++];
		cnt--;
		printf("consume %d\n", cnt);
		last = get;
		r %= NBUF;
		V(mutex);
		V(empty);
		sleep(10);
	}
}

void
test_setup(void) {
	printf("Enter the user program\n");
	full = createSem(0);
	empty = createSem(NBUF);
	mutex = createSem(1);
	printf("full:%d empty:%d mutex:%d \n", full, empty, mutex);
	
	create_thread(test_consumer);
	create_thread(test_producer);
	create_thread(test_producer);

	exit(0);
}

void test_file() {
	int fp = open("check.txt");
	char buf[512];
	mfread(fp, buf, 512);
	printf("%s\n", buf);
	char tmp[2]="Y";
	seek(fp, 0, SEEK_SET);
	mfwrite(fp, tmp, 1);
	printf("\n");
	close(fp);
	fp = open("check.txt");
	mfread(fp, buf, 512);
	printf("%s\n", buf);
	close(fp);
}

/* enter : 28
 * l : 38
 * s : 31
 * space : 57
 * - : 12
 * a : 30
 * c : 46
 * t : 20
 */

void
shell(void) {
	printf("Shell\n");
	int i;
	int key[256];
	while (TRUE) {
		for (i = 0; i < 256; i++)
			key[i] = 0;
		save_key_code(key);
//		for (i = 0; i < 256; i++)
//			printf("%d ", key[i]);
		if (key[0] == 38 && key[1] == 31 && key[2] == 28) {
			char buf[128];
			for (i = 0; i < 128; i++)
				buf[i] = '\0';
			getfile(buf);
			i = 0;
//          printf("%s\n", buf);
			while (buf[i] != '\0') {
				if (buf[i] == ' ')
					printf("\n");
				else
					printf("%c", buf[i]);
				i++;
			}
		}
		else if (key[0] == 38 && key[1] == 31 && key[2] == 57 && key[3] == 12 && key[4] == 30) {
//			printf("in\n");
			char buf[128];
			for (i = 0; i < 128; i++)
				buf[i] = '\0';
			getfile(buf);
			i = 0;
//			printf("%s\n", buf);
			while (buf[i] != '\0') {
				if (buf[i] == ' ')
					printf("\n");
				else
					printf("%c", buf[i]);
				i++;
			}
		}
		else if (key[0] == 38 && key[1] == 31 && key[2] == 57 && key[3] == 12 && key[4] == 38) {
			char buf[128];
			int size[128];
			for (i = 0; i < 128; i++) {
				buf[i] = '\0';
				size[i] = -1;
			}
			getfile(buf);
			getsize(size);
			i = 0;
			int cnt = 0;
			while (buf[i] != '\0') {
				if (buf[i] == ' ') {
					printf("\t%d\n", size[cnt++]);
				}
				else
					printf("%c", buf[i]);
				i++;
			}
		}
		else if (key[0] == 46 && key[1] == 30 && key[2] == 20) {
//			printf("cat\n");
			int size[128];
			getsize(size);
			if (key[4] == 34) {
				int fp = open("game.bin");
				char buf[512];
				i = 0;
				while (i * 512 < size[0]) {
					mfread(fp, buf, 512);
					printf("%s", buf);
					i++;
				}
				close(fp);
			}
			else if (key[4] == 30) {
				int fp = open("ans.dat");
				char buf[512];
				i = 0;
				while (i * 512 < size[1]) {
					mfread(fp, buf, 512);
					printf("%s", buf);
					i++;
				}
				close(fp);
			}
			else if (key[4] == 46) {
				int fp = open("check.txt");
				char buf[512];
				i = 0;
				while (i * 512 < size[2]) {
					mfread(fp, buf, 512);
					printf("%s", buf);
					i++;
				}
				close(fp);
			}
		}
		else if (key[0] == 18 && key[1] == 46 && key[2] == 35 && key[3] == 24) {
			for (i = 5; key[i] != 28; i++) {
				printkey(key[i]);
			}
			printf("\n");
		}
		else
			;
	}
}
/*
		int code = press_key_code();
		if (code != -1) {
			printf("%d\n", code);
			switch (code) {
				case 38:
				   printf("l");
				   while (TRUE) {
					   code = press_key_code();
					   if (code == 31) {
						   printf("s");
						   while (TRUE) {
							   code = press_key_code();
							   if (code == 28) {
								   printf("\n");
								   char buf[128];
								   for (i = 0; i < 128; i++)
									   buf[i] = '\0';
								   getfile(buf);
								   i = 0;
								   while (buf[i] != '\0') {
									   if (buf[i] == ' ')
										   printf("\n");
									   else
										   printf("%c", buf[i]);
									   i++;
								   }
								   break;
							   }
						   }
						   break;
					   }
				   }
				   break;
			}
		}
	}
*/



void
main_loop(void) {
	printf("enter\n");
	
	test_file();
	int i, j, temp;
	bool flag = FALSE;
	init_puzzle();
	screendraw(screen_buf);

	while (TRUE) {

		while (TRUE) {
			int code = press_key_code();
			if (code != -1) {
				switch (code) {
					case 17:
							printf("up\n");
							for (i = 0; i < 3; i++) {
								for (j = 0; j < 4; j++) {
									if (puzzle_map[i][j] == 1) {
										if (i == 2)
											continue;
										else {
											draw_pic(puzzle_map[i+1][j], j, i);
											draw_pic(puzzle_map[i][j], j, i + 1);
											temp = puzzle_map[i+1][j];
											puzzle_map[i+1][j] = puzzle_map[i][j];
											puzzle_map[i][j] = temp;
											flag = TRUE;
//											print_puzzle();
											break;
										}
									}
								}
								if (flag) {
									flag = FALSE;
									break;
								}
							}
							break;
//					case 0x1f:							//down
					case 31:
							printf("down\n");
							for (i = 0; i  < 3; i++) {
								for (j = 0; j < 4; j++) {
									if (puzzle_map[i][j] == 1) {
										if (i == 0)
											continue;
										else {
											draw_pic(puzzle_map[i-1][j], j, i);
											draw_pic(puzzle_map[i][j], j, i - 1);
											temp = puzzle_map[i-1][j];
											puzzle_map[i-1][j] = puzzle_map[i][j];
											puzzle_map[i][j] = temp;
											flag = TRUE;
											break;
										}
									}
								}
								if (flag) {
									flag = FALSE;
									break;
								}
							}
							break;
//					case 0x1e:							//left
					case 30:
							printf("left\n");
							for (i = 0; i < 3; i++) {
								for (j = 0; j < 4; j++)	{
									if (puzzle_map[i][j] == 1) {
										if (j == 3)
											continue;
										else {
											draw_pic(puzzle_map[i][j+1], j, i);
											draw_pic(puzzle_map[i][j], j + 1, i);
											temp = puzzle_map[i][j+1];
											puzzle_map[i][j+1] = puzzle_map[i][j];
											puzzle_map[i][j] = temp;
											flag = TRUE;
											break;
										}
									}
								}
								if (flag) {
									flag = FALSE;
									break;
								}
							}
							break;
//					case 0x20:							//right
					case 32:	
							printf("right\n");      
                            for (i = 0; i < 3; i++) {
                                for (j = 0; j < 4; j++) {
                                    if (puzzle_map[i][j] == 1) {
                                        if (j == 0)
                                            continue;
                                        else {
                                            draw_pic(puzzle_map[i][j-1], j, i);                                         
                                            draw_pic(puzzle_map[i][j], j - 1, i);                                       
                                            temp = puzzle_map[i][j-1];
                                            puzzle_map[i][j-1] = puzzle_map[i][j]; 
                                            puzzle_map[i][j] = temp;
                                            flag = TRUE;
                                            break; 
                                        }   
                                    }   
                                }   
                                if (flag) {
                                    flag = FALSE;
                                    break; 
                                }   
                            } 
							break;
						default:
							printf("%d\n", code);
				}
				screendraw(screen_buf);
			}
		}
	}
}
