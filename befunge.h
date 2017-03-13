void simulate (int** grid, int w, int h);
int simulate_start(int** grid, int* stack, int w, int h, int x, int y, int d, int sp);
int** parse(char* inp, int w, int h);
void* start_thread (void* args);

typedef struct bthread {
	int** grid;
	int* stack;
	int x;
	int y;
	int d;
	int h;
	int w;
	int sp;
} *bthread_t;
