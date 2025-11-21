#include <gigatron/sys.h>
#include <gigatron/console.h>
#include <gigatron/kbget.h>
#include <stdlib.h>
#include <stdio.h>

/* 游戏配置 */
#define SCREEN_WIDTH 20
#define SCREEN_HEIGHT 15
#define MAX_SNAKE_LENGTH 100
#define INITIAL_SNAKE_LENGTH 3
#define GAME_SPEED 10 /* 游戏速度（帧数） */

/* 方向定义 */
#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

/* 游戏状态 */
typedef struct {
    int snake_x[MAX_SNAKE_LENGTH];
    int snake_y[MAX_SNAKE_LENGTH];
    int snake_length;
    int direction;
    int food_x;
    int food_y;
    int score;
    int game_over;
    int frame_counter;
} game_state_t;

/* 全局游戏状态 */
static game_state_t game;

/* 初始化游戏 */
void init_game(void) {
    int i;
    
    /* 初始化蛇的位置（屏幕中央） */
    game.snake_length = INITIAL_SNAKE_LENGTH;
    for (i = 0; i < game.snake_length; i++) {
        game.snake_x[i] = SCREEN_WIDTH / 2 - i;
        game.snake_y[i] = SCREEN_HEIGHT / 2;
    }
    game.direction = DIR_RIGHT;
    game.score = 0;
    game.game_over = 0;
    game.frame_counter = 0;
    
    /* 生成第一个食物 */
    game.food_x = rand() % SCREEN_WIDTH;
    game.food_y = rand() % SCREEN_HEIGHT;
}

/* 生成新食物 */
void generate_food(void) {
    int valid_position = 0;
    int i;
    
    while (!valid_position) {
        game.food_x = rand() % SCREEN_WIDTH;
        game.food_y = rand() % SCREEN_HEIGHT;
        valid_position = 1;
        
        /* 检查食物是否在蛇身上 */
        for (i = 0; i < game.snake_length; i++) {
            if (game.snake_x[i] == game.food_x && game.snake_y[i] == game.food_y) {
                valid_position = 0;
                break;
            }
        }
    }
}

/* 移动蛇 */
void move_snake(void) {
    int new_x, new_y;
    int i;
    
    /* 计算新的头部位置 */
    new_x = game.snake_x[0];
    new_y = game.snake_y[0];
    
    switch (game.direction) {
        case DIR_UP:
            new_y--;
            break;
        case DIR_DOWN:
            new_y++;
            break;
        case DIR_LEFT:
            new_x--;
            break;
        case DIR_RIGHT:
            new_x++;
            break;
    }
    
    /* 检查边界碰撞 */
    if (new_x < 0 || new_x >= SCREEN_WIDTH || 
        new_y < 0 || new_y >= SCREEN_HEIGHT) {
        game.game_over = 1;
        return;
    }
    
    /* 检查自身碰撞 */
    for (i = 0; i < game.snake_length; i++) {
        if (new_x == game.snake_x[i] && new_y == game.snake_y[i]) {
            game.game_over = 1;
            return;
        }
    }
    
    /* 移动蛇身 */
    for (i = game.snake_length - 1; i > 0; i--) {
        game.snake_x[i] = game.snake_x[i - 1];
        game.snake_y[i] = game.snake_y[i - 1];
    }
    
    /* 设置新的头部位置 */
    game.snake_x[0] = new_x;
    game.snake_y[0] = new_y;
    
    /* 检查是否吃到食物 */
    if (new_x == game.food_x && new_y == game.food_y) {
        game.score++;
        if (game.snake_length < MAX_SNAKE_LENGTH) {
            game.snake_length++;
        }
        generate_food();
    }
}

/* 处理输入 */
void handle_input(void) {
    int key;
    
    key = kbget();
    if (key == -1) return;
    
    /* 根据按键改变方向（防止反向移动） */
    switch (key) {
        case 0x3c: /* 上箭头 */
            if (game.direction != DIR_DOWN) game.direction = DIR_UP;
            break;
        case 0x3d: /* 下箭头 */
            if (game.direction != DIR_UP) game.direction = DIR_DOWN;
            break;
        case 0x3e: /* 左箭头 */
            if (game.direction != DIR_RIGHT) game.direction = DIR_LEFT;
            break;
        case 0x3f: /* 右箭头 */
            if (game.direction != DIR_LEFT) game.direction = DIR_RIGHT;
            break;
        case 0x20: /* 空格键 - 重新开始 */
            if (game.game_over) {
                init_game();
            }
            break;
    }
}

/* 绘制游戏画面 */
void draw_game(void) {
    int x, y, i;
    int is_snake, is_head;
    char score_text[32];
    
    /* 清屏 */
    console_clear_screen();
    
    /* 绘制边框 */
    console_state_set_cycx(0x0000);
    console_print("+", 1);
    for (i = 0; i < SCREEN_WIDTH; i++) {
        console_print("-", 1);
    }
    console_print("+", 1);
    
    for (y = 0; y < SCREEN_HEIGHT; y++) {
        console_state_set_cycx((y + 1) << 8);
        console_print("|", 1);
        for (x = 0; x < SCREEN_WIDTH; x++) {
            is_snake = 0;
            is_head = 0;
            
            /* 检查是否是蛇的一部分 */
            for (i = 0; i < game.snake_length; i++) {
                if (game.snake_x[i] == x && game.snake_y[i] == y) {
                    is_snake = 1;
                    if (i == 0) is_head = 1;
                    break;
                }
            }
            
            if (is_head) {
                console_print("@", 1); /* 蛇头 */
            } else if (is_snake) {
                console_print("o", 1); /* 蛇身 */
            } else if (game.food_x == x && game.food_y == y) {
                console_print("*", 1); /* 食物 */
            } else {
                console_print(" ", 1); /* 空格 */
            }
        }
        console_print("|", 1);
    }
    
    /* 绘制底边框 */
    console_state_set_cycx((SCREEN_HEIGHT + 1) << 8);
    console_print("+", 1);
    for (i = 0; i < SCREEN_WIDTH; i++) {
        console_print("-", 1);
    }
    console_print("+", 1);
    
    /* 显示分数 */
    console_state_set_cycx((SCREEN_HEIGHT + 2) << 8);
    sprintf(score_text, "Score: %d", game.score);
    console_print(score_text, 32);
    
    /* 如果游戏结束，显示游戏结束信息 */
    if (game.game_over) {
        console_state_set_cycx(((SCREEN_HEIGHT + 3) << 8) | 5);
        console_print("GAME OVER!", 10);
        console_state_set_cycx(((SCREEN_HEIGHT + 4) << 8) | 3);
        console_print("Press SPACE to restart", 23);
    }
}

/* 主游戏循环 */
void game_loop(void) {
    int i;
    
    while (1) {
        handle_input();
        
        if (!game.game_over) {
            game.frame_counter++;
            if (game.frame_counter >= GAME_SPEED) {
                move_snake();
                game.frame_counter = 0;
            }
        }
        
        draw_game();
        
        /* 简单的延迟 */
        for (i = 0; i < 1000; i++) {
            /* 空循环延迟 */
        }
    }
}

/* 主函数 */
int main(void) {
    /* 初始化随机数种子 */
    srand(frameCount);
    
    /* 初始化游戏 */
    init_game();
    
    /* 开始游戏循环 */
    game_loop();
    
    return 0;
}