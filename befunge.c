#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "befunge.h"

#define DUPE(newbef)\
	newbef = malloc(sizeof(struct bthread));\
	newbef->x = x;\
	newbef->y = y;\
	newbef->w = w;\
	newbef->h = h;\
	newbef->sp = sp;\
	newbef->grid = grid;\
	newbef->stack = malloc(sizeof(int)*1024);\
	for (i = 0; i < 1024; i++) {\
		newbef->stack[i] = stack[i];\
	}\

#define P(sem) sem_wait(sem);
#define V(sem) sem_post(sem);
#define UNLINK_ALL()\
	sem_unlink("/readers");\
	sem_unlink("/writers");\
	sem_unlink("/open_brace");\

int can_thread = 1;
sem_t* readers;
sem_t* writers;
int r_count = 0;

sem_t* openb;

int main (int argc, char* argv[]) {
	srand(time(NULL));

	int gridC;
	int gridR;

	if (argc > 1) {
		gridC = atoi(argv[1]);
	} else {
		gridC = 80;
	}

	if (argc > 2) {
		gridR = atoi(argv[2]);
	} else {	
		gridR = 25;
	}

	if (argc > 3) {
		can_thread = 0;
	}

	UNLINK_ALL();
	readers = sem_open("/readers", O_CREAT, 0777, 1);
	writers = sem_open("/writers", O_CREAT, 0777, 1);

	openb   = sem_open("/open_brace",  O_CREAT, 0777, 1);

	printf("Please input a grid of size %d x %d\n", gridC, gridR);
	
	char* grid = (char*)malloc(sizeof(char) * (gridC + 1) * gridR + 1); 
	FILE *inp = stdin;
 
	fread(grid, sizeof(char), (gridC + 1) * gridR, inp);
	grid[(gridC + 1) * gridR] = 0;

	printf("*-------------------*\n");

	simulate(parse(grid, gridC, gridR), gridC, gridR);

	UNLINK_ALL();
	printf("Terminated Successfully\n");

	return 0;
}

int pop (int stack[1024], int* sp) {
	if (*sp == 0) {
		return 0;
	}
	(*sp)--;
	return stack[*sp];
}

void push (int* stack, int* sp, int val) {
	stack[*sp] = val;
	(*sp)++;
}

void simulate (int** grid, int w, int h) {
	int* stack = malloc(sizeof(int) * 1024);
	simulate_start(grid, stack, w, h, 0, 0, 0, 0);
	putc('\n', stdout);
}

int read_grid(int** grid, int x, int y) {
		//Thread Safe Reading
		P(readers);
		r_count++;
		if (r_count == 1) {
			P(writers);
		}
		V(readers);
		int curr = grid[y][x];
		P(readers);
		r_count--;
		if (r_count == 0) {
			V(writers);
		}
		V(readers);
		return curr;
}

int simulate_start(int** grid, int* stack, int w, int h, int x, int y, int d, int sp) {
	int a1;
	int a2;
	int a3;

	char c1;

	int i;

	bool stringmode = false;

	while (true) {
		bool term = false;
		int curr = read_grid(grid, x, y);
		//printf("At: (%d, %d): %c\n", x, y, curr);

		if (stringmode) {
			if (curr == '"') {
				stringmode = false;
			} else {
				push(stack, &sp, curr);
			}

			if (d == 0 && x < (w-1)) {
				x++;
			} else if (d == 1 && y < (h-1)) {
				y++;
			} else if (d == 2 && x > 0) {
				x += (w-1);
			} else if (d == 3 && y > 0) {
				y += (h-1);
			} 	
			x %= w;
			y %= h;

			continue;
		}

		switch (curr) {
			case '+': 
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);

				push(stack, &sp, a1+a2);
				break;
			case '-':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);

				push(stack, &sp, a2-a1);
				break;
			case '*':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);

				push(stack, &sp, a2*a1);
				break;
			case '/':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);

				push(stack, &sp, a2/a1);
				break;
			case '%':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);

				push(stack, &sp, a2%a1);
				break;
			case '!':
				a1 = pop(stack, &sp);

				push(stack, &sp, a1 == 0 ? 1 : 0);
				break;
			case '`':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);

				push(stack, &sp, (a2 > a1) ? 1 : 0);
				break;
			case ',':
				putc(pop(stack, &sp), stdout);
				fflush(stdout);
				break;
			case '.':
				a1 = pop(stack, &sp);
				printf("%d", a1);
				fflush(stdout);
				break;
			case '@':
				term = true;
				break;
			case '>':
				d = 0;
				break;
			case 'v':
				d = 1;
				break;
			case '<':
				d = 2;
				break;
			case '^':
				d = 3;
				break;
			case '?':
				d = floor(rand()%4); 
				break;
			case '_':
				d = pop(stack, &sp) == 0 ? 0 : 2;
				break;
			case '|':
				d = pop(stack, &sp) == 0 ? 1 : 3;
				break;
			case '#':
				if (d == 0 && x < (w-1)) {
					x++;
				} else if (d == 1 && y < (h-1)) {
					y++;
				} else if (d == 2 && x > 0) {
					x += (w-1);
				} else if (d == 3 && y > 0) {
					y += (h-1);
				} 	
				x %= w;
				y %= h;

				break;
			case ':':
				a1 = pop(stack, &sp);
				push(stack, &sp, a1);
				push(stack, &sp, a1);
				break;
			case '\\':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);
				push(stack, &sp, a1);
				push(stack, &sp, a2);
				break;
			case '$':
				pop(stack, &sp);
				break;
			case 'g':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);
				if (a1 >= 0 && a1 < h && a2 >= 0 && a2 < w) {
					push(stack, &sp, read_grid(grid, a2, a1));
				} else {
					push(stack, &sp, 0);
				}
				break;
			case 'p':
				a1 = pop(stack, &sp);
				a2 = pop(stack, &sp);
				a3 = pop(stack, &sp);
				if (a1 >= 0 && a1 < h && a2 >= 0 && a2 < w) {
					P(writers);
					grid[a1][a2] = a3;
					V(writers);
				} else {
					printf("Error: Writing Out of Bounds to position: %d x %d\n", a2, a1);
					exit(-1);
				}
				break;
			case '&':
				c1 = getchar();
				c1 -= 0x30;
				push(stack, &sp, (int)c1);
				break;
			case '~':
				c1 = getchar();
				push(stack, &sp, (int)c1);
				break;
			case '"':
				stringmode = true;
				break;
			case '=': {
				if (!can_thread) {
					break;
				}
				bthread_t bef1;
				bthread_t bef2;
				DUPE(bef1)
				DUPE(bef2)
				bef1->d = 0;
				bef1->x = (bef1->x+1)%w;
				bef2->d = 2;
				bef2->x = (bef2->x+w-1)%w;

				pthread_t thread1;
				pthread_t thread2;
				int* r;

				pthread_create(&thread1, NULL, (void*)&start_thread, (void*) bef1);
				pthread_create(&thread2, NULL, (void*)&start_thread, (void*) bef2);

				pthread_join(thread1, (void**)&r);
				push(stack, &sp, *r);
				free(r);
				pthread_join(thread2, (void**)&r);
				push(stack, &sp, *r);
				free(r);
				break;
			}
			case '{': {
				//printf("Entering Mutex Protected Block\n");
				sem_wait(openb);
				//printf("Entered!\n");

				break;
			}
			case '}':
				//printf("Leaving Mutex Protected Block\n");
				V(openb);
				//printf("Left\n");
				break;
			default:
				if (0x30 <= curr && curr <= 0x39) {
					push(stack, &sp, curr - 0x30);
				}
		}

		if (term) {
			break;
		}

		if (d == 0 && x < (w-1)) {
			x++;
		} else if (d == 1 && y < (h-1)) {
			y++;
		} else if (d == 2 && x > 0) {
			x += (w-1);
		} else if (d == 3 && y > 0) {
			y += (h-1);
		} 	
		x %= w;
		y %= h;

	}
	return pop(stack, &sp);
}

void* start_thread(void* args) {
	bthread_t befinst = (bthread_t)args;
	//printf("Created New Thread at (%d, %d)\n", befinst->x, befinst->y);
	int res = simulate_start(befinst->grid, befinst->stack, befinst->w, befinst->h, befinst->x, befinst->y, befinst->d, befinst->sp);
	free(befinst->stack);
	free(befinst);
	int* top_v = malloc(sizeof(int));
	*top_v = res;
	//printf("Dying lol (%d)\n", res);
	return (void*)top_v;
}

int** parse (char* inp, int w, int h) {
	int** gridhead = (int**) malloc(sizeof(int*) * h);
	int i;
	int j;
	for (i = 0; i < h; i++) {
		gridhead[i] = (int*) malloc(sizeof(int) * w);
		for (j = 0; j < w; j++) {
			gridhead[i][j] = inp[i * (w + 1) + j];
			printf("%c", gridhead[i][j]);
		}
		printf("\n");
	}
	return gridhead;
}
