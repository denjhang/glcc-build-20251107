#include <gigatron/sys.h>
#include <gigatron/console.h>
#include <gigatron/kbget.h>
#include <stdlib.h>
#include <stdio.h>

/* This enables the code that handles simultaneous button presses
   and yet discriminates keypresses from button presses. */
KBGET_AUTOBTN;

#define BLANK ' '
#define HEAD  '0'
#define TAIL  '+'
#define FOOD  '@'
#define WALL  '#'
#define KEY_MOVE_UP    0xf7
#define KEY_MOVE_LEFT  0xfd
#define KEY_MOVE_DOWN  0xfb
#define KEY_MOVE_RIGHT 0xfe
#define WIDTH 26
#define HEIGHT 14
#define DEBUG 0

struct position {
	int x;
	int y;
} pos;

char field[WIDTH][HEIGHT];  /* changed to char for space */

void resetField(void);
void printField(void);
int getItem(int, int);
void setItem(int, int, int);
int move(int);
void generateFood(void);
int score = 0;
int foodGenerated = 0;

int main(void) {
	char i;
	int key = 0;
	int lastkey = 0;
	
	resetField();
	printField();
	pos.x = WIDTH/2;
	pos.y = HEIGHT/2;
	setItem(pos.x, pos.y, HEAD);
	
	/* Generate first food */
	generateFood();
	
	while(key != 'x') {
		
		if (kbhit())
			key = getch();

		/* This replace usleep.
		   One should monitor the keyboard often
		   enough to avoid losing key press events. */
		if (++i & 0x7f)
			continue;
		
		if (key != 'x') {
			if (key = move(key)) {
				if (key == 'f') { /* ate food */
					score++;
					lastkey = key;
					generateFood();
				} else {
					lastkey = key;
				}
			} else key = lastkey;
		}
		if(DEBUG) {
			console_state_set_cycx(((HEIGHT + 1) << 8) | 12);
			printf("%02x", key);
		}
		console_state_set_cycx(((HEIGHT + 1) << 8) | 1);
		printf("Score: %d  ", score);
	}
	return 0;
}

int move(int key) {
	int oldX = pos.x;
	int oldY = pos.y;

	setItem(pos.x, pos.y, TAIL);
	switch(key) {
	case KEY_MOVE_UP:    pos.y--; break;
	case KEY_MOVE_LEFT:  pos.x--; break;
	case KEY_MOVE_DOWN:  pos.y++; break;
	case KEY_MOVE_RIGHT: pos.x++; break;
	default: return 0; // do not display anything.
	}

    if (getItem(pos.x, pos.y) == BLANK) {
	    setItem(pos.x, pos.y, HEAD);
	    return key;
    } else if (getItem(pos.x, pos.y) == FOOD) {
	    setItem(pos.x, pos.y, HEAD);
	    return 'f'; /* ate food */
    } else {
	    setItem(oldX, oldY, HEAD); /* restore head position */
	    return 'x'; // snake has moved to non blank field - game over
    }
}

int getItem(int x, int y) {
	return field[x][y];
}

void setItem(int x, int y, int item) {
	field[x][y] = item;
	console_state_set_cycx((y << 8) | x);
	putch(item);
}

void generateFood(void) {
	int x, y;
	int valid = 0;
	
	while (!valid) {
		x = 1 + rand() % (WIDTH - 2);
		y = 1 + rand() % (HEIGHT - 2);
		
		if (getItem(x, y) == BLANK) {
			setItem(x, y, FOOD);
			valid = 1;
		}
	}
}

void resetField(void) {
	int x, y;
	for(y = 0; y < HEIGHT; y++) {
		for(x = 0; x < WIDTH; x++) {
			field[x][y] = WALL;
		}
	}
	for(y = 1; y < HEIGHT - 1; y++) {
		for(x = 1; x < WIDTH - 1; x++) {
			field[x][y] = BLANK;
		}
	}
}

void printField(void) {
	int i, y, x;
	
	console_clear_screen();
	for(y = 0; y < HEIGHT; y++) {
		for(x = 0; x < WIDTH; x++) {
			i = field[x][y];
			if (i != BLANK) {
				console_state_set_cycx((y << 8) | x);
				putch(i);
			}
		}
	}

	/* print debug things */
	if(DEBUG >= 2) {
		console_state_set_cycx(((HEIGHT + 2) << 8) | 1);
		printf("\n\nX: %3d   Y: %3d", pos.x, pos.y);
		printf("\n\nx: %3d   y: %3d", x, y);
	}
}