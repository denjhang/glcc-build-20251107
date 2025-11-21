/* Plane War - 飞机大战游戏 (直接绘图模式) */

#include <stdlib.h>
#include <string.h>
#include <gigatron/sys.h>
#include <gigatron/libc.h>

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
    int type; /* 0=normal, 1=fast */
} enemy_t;

/* State variables */
near int player_x;
near int score;
near int level;
near int game_over;
near int fire_cooldown;

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

static void draw_pixel(int x, int y, int color)
{
    unsigned int addr;
    
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        addr = makew(y + 8, x);
        *(char*)addr = color;
    }
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
    /* Draw player as a triangle */
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
    /* Draw enemy as inverted triangle */
    draw_rect(x - 2, y, 5, 2, color);
    draw_rect(x - 1, y + 2, 3, 2, color);
    draw_rect(x, y + 4, 1, 2, color);
}

static void init_game(void)
{
    int i;
    
    /* Initialize player */
    player_x = SCREEN_WIDTH / 2;
    score = 0;
    level = 1;
    game_over = 0;
    fire_cooldown = 0;
    
    /* Initialize bullets */
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    
    /* Initialize enemies */
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }
    
    /* Create initial enemies */
    for (i = 0; i < 3 + level; i++) {
        if (i < MAX_ENEMIES) {
            enemies[i].x = 10 + rand() % (SCREEN_WIDTH - 20);
            enemies[i].y = 10 + rand() % 30;
            enemies[i].active = 1;
            enemies[i].type = (rand() % 3 == 0) ? 1 : 0; /* 33% chance of fast enemy */
        }
    }
}

static void spawn_enemy(void)
{
    int i;
    
    /* Find inactive enemy slot */
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
            bullets[i].y -= 3; /* Faster bullets */
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
            /* Move enemy down */
            enemies[i].y += (enemies[i].type == 1) ? 2 : 1;
            
            /* Check if enemy reached bottom */
            if (enemies[i].y >= PLAYER_Y + 10) {
                enemies[i].active = 0;
            }
            
            /* Check collision with player */
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
                        /* Hit! */
                        bullets[i].active = 0;
                        enemies[j].active = 0;
                        score += (enemies[j].type == 1) ? 20 : 10;
                        
                        /* Level up every 100 points */
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
    int i;
    
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
                /* Find inactive bullet slot */
                for (i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].x = player_x;
                        bullets[i].y = PLAYER_Y - 5;
                        bullets[i].active = 1;
                        fire_cooldown = 8; /* Cooldown frames */
                        break;
                    }
                }
            }
            break;
        }
    }
    
    /* Update cooldown */
    if (fire_cooldown > 0) fire_cooldown--;
}

static void render_game(void)
{
    int i;
    
    /* Clear screen */
    clear_screen();
    
    /* Draw player */
    if (!game_over) {
        draw_player();
    }
    
    /* Draw bullets */
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            draw_bullet(bullets[i].x, bullets[i].y);
        }
    }
    
    /* Draw enemies */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            draw_enemy(enemies[i].x, enemies[i].y, enemies[i].type);
        }
    }
    
    /* Draw score and level */
    /* Simple text drawing using pixels */
    int score_text_x = 10;
    int score_text_y = 5;
    
    /* Draw "S:" */
    draw_rect(score_text_x, score_text_y, 2, 6, COLOR_WHITE);
    draw_rect(score_text_x + 4, score_text_y, 6, 2, COLOR_WHITE);
    draw_rect(score_text_x, score_text_y + 6, 2, 2, COLOR_WHITE);
    draw_rect(score_text_x + 4, score_text_y + 6, 6, 2, COLOR_WHITE);
    
    /* Draw score digits (simplified) */
    int score_digit = (score / 100) % 10;
    draw_rect(score_text_x + 15, score_text_y, 6, 8, COLOR_YELLOW);
    
    score_digit = (score / 10) % 10;
    draw_rect(score_text_x + 25, score_text_y, 6, 8, COLOR_YELLOW);
    
    score_digit = score % 10;
    draw_rect(score_text_x + 35, score_text_y, 6, 8, COLOR_YELLOW);
    
    if (game_over) {
        /* Draw GAME OVER text */
        draw_rect(SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2 - 10, 60, 10, COLOR_RED);
        draw_rect(SCREEN_WIDTH/2 - 25, SCREEN_HEIGHT/2, 50, 10, COLOR_WHITE);
    }
}

int main(void)
{
    int enemy_spawn_timer = 0;
    
    for(;;) {
        init_game();
        
        while(!game_over) {
            /* Handle input */
            handle_input();
            
            /* Update game state */
            update_bullets();
            update_enemies();
            check_collisions();
            
            /* Spawn new enemies periodically */
            enemy_spawn_timer++;
            if (enemy_spawn_timer >= 60 - level * 5) {
                spawn_enemy();
                enemy_spawn_timer = 0;
            }
            
            /* Render */
            render_game();
            
            /* Simple delay */
            {
                int j;
                for (j = 0; j < 1000; j++);
            }
        }
        
        /* Wait for key press to restart */
        while(kbget() >= 0) ; /* Clear buffer */
        while(kbget() < 0) ; /* Wait for key */
    }
    
    return 0;
}