/* Plane War - 飞机大战游戏 (简化直接绘图模式) */

#include <stdlib.h>
#include <string.h>
#include <gigatron/sys.h>
#include <gigatron/libc.h>

#pragma glcc option("KBGET_AUTOBTN")

/* Game constants */
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120
#define MAX_BULLETS 5
#define MAX_ENEMIES 8
#define PLAYER_Y 100

/* Game objects */
typedef struct {
    int x, y;
    int active;
} bullet_t;

typedef struct {
    int x, y;
    int active;
    int type;
} enemy_t;

/* State variables */
int player_x;
int score;
int level;
int game_over;
int fire_cooldown;

bullet_t bullets[MAX_BULLETS];
enemy_t enemies[MAX_ENEMIES];

/* Colors */
#define COLOR_BLACK   0x00
#define COLOR_BLUE    0x10
#define COLOR_GREEN   0x20
#define COLOR_YELLOW  0x30
#define COLOR_RED     0x40
#define COLOR_MAGENTA 0x50
#define COLOR_WHITE   0x60

static void clear_screen(void)
{
    SYS_Fill(makew(8, 0), COLOR_BLACK, makew(SCREEN_HEIGHT, SCREEN_WIDTH));
}

static void draw_rect(int x, int y, int w, int h, int color)
{
    unsigned int addr;
    
    if (x < 0 || y < 0 || x + w > SCREEN_WIDTH || y + h > SCREEN_HEIGHT)
        return;
    
    addr = makew(y + 8, x);
    SYS_Fill(addr, color, makew(h, w));
}

static void draw_player(void)
{
    draw_rect(player_x - 2, PLAYER_Y, 5, 2, COLOR_GREEN);
    draw_rect(player_x - 1, PLAYER_Y - 2, 3, 2, COLOR_GREEN);
    draw_rect(player_x, PLAYER_Y - 4, 1, 2, COLOR_GREEN);
}

static void draw_bullet(int x, int y)
{
    draw_rect(x, y, 2, 2, COLOR_YELLOW);
}

static void draw_enemy(int x, int y, int type)
{
    int color = (type == 1) ? COLOR_MAGENTA : COLOR_RED;
    draw_rect(x - 2, y, 5, 2, color);
    draw_rect(x - 1, y + 2, 3, 2, color);
    draw_rect(x, y + 4, 1, 2, color);
}

static void init_game(void)
{
    int i;
    
    player_x = SCREEN_WIDTH / 2;
    score = 0;
    level = 1;
    game_over = 0;
    fire_cooldown = 0;
    
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }
    
    for (i = 0; i < 3 + level; i++) {
        if (i < MAX_ENEMIES) {
            enemies[i].x = 10 + rand() % (SCREEN_WIDTH - 20);
            enemies[i].y = 10 + rand() % 30;
            enemies[i].active = 1;
            enemies[i].type = (rand() % 3 == 0) ? 1 : 0;
        }
    }
}

static void spawn_enemy(void)
{
    int i;
    
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x = 10 + rand() % (SCREEN_WIDTH - 20);
            enemies[i].y = 10;
            enemies[i].active = 1;
            enemies[i].type = (rand() % 3 == 0) ? 1 : 0;
            break;
        }
    }
}

static void update_bullets(void)
{
    int i;
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].y -= 3;
            if (bullets[i].y <= 0) {
                bullets[i].active = 0;
            }
        }
    }
}

static void update_enemies(void)
{
    int i;
    
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].y += (enemies[i].type == 1) ? 2 : 1;
            
            if (enemies[i].y >= PLAYER_Y + 10) {
                enemies[i].active = 0;
            }
            
            if (enemies[i].y >= PLAYER_Y - 5 && 
                enemies[i].y <= PLAYER_Y + 5 &&
                abs(enemies[i].x - player_x) < 8) {
                game_over = 1;
            }
        }
    }
}

static void check_collisions(void)
{
    int i, j;
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (abs(bullets[i].x - enemies[j].x) < 5 && 
                        abs(bullets[i].y - enemies[j].y) < 5) {
                        bullets[i].active = 0;
                        enemies[j].active = 0;
                        score += (enemies[j].type == 1) ? 20 : 10;
                        
                        if (score % 100 == 0) {
                            level++;
                        }
                        break;
                    }
                }
            }
        }
    }
}

static void handle_input(void)
{
    int key;
    
    if ((key = kbget()) >= 0) {
        switch(key) {
        case 0xfd: /* Left */
            if (player_x > 10) player_x -= 3;
            break;
        case 0xfe: /* Right */
            if (player_x < SCREEN_WIDTH - 10) player_x += 3;
            break;
        case 0x7f: /* A button (fire) */
            if (fire_cooldown == 0) {
                int i;
                for (i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].x = player_x;
                        bullets[i].y = PLAYER_Y - 5;
                        bullets[i].active = 1;
                        fire_cooldown = 8;
                        break;
                    }
                }
            }
            break;
        }
    }
    
    if (fire_cooldown > 0) fire_cooldown--;
}

static void render_game(void)
{
    int i;
    
    clear_screen();
    
    if (!game_over) {
        draw_player();
    }
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            draw_bullet(bullets[i].x, bullets[i].y);
        }
    }
    
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            draw_enemy(enemies[i].x, enemies[i].y, enemies[i].type);
        }
    }
    
    /* Draw score indicator */
    draw_rect(10, 5, 40, 8, COLOR_WHITE);
    draw_rect(15, 7, score, 4, COLOR_YELLOW);
    
    if (game_over) {
        draw_rect(SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2 - 10, 60, 10, COLOR_RED);
        draw_rect(SCREEN_WIDTH/2 - 25, SCREEN_HEIGHT/2, 50, 10, COLOR_WHITE);
    }
}

int main(void)
{
    int enemy_spawn_timer = 0;
    int i;
    
    for(;;) {
        init_game();
        
        while(!game_over) {
            handle_input();
            update_bullets();
            update_enemies();
            check_collisions();
            
            enemy_spawn_timer++;
            if (enemy_spawn_timer >= 60 - level * 5) {
                spawn_enemy();
                enemy_spawn_timer = 0;
            }
            
            render_game();
            
            for (i = 0; i < 1000; i++);
        }
        
        while(kbget() >= 0) ;
        while(kbget() < 0) ;
    }
    
    return 0;
}