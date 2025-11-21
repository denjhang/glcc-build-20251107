/* Plane War - 飞机大战游戏 (图形模式) */

#include <stdlib.h>
#include <string.h>
#include <gigatron/sys.h>
#include <gigatron/libc.h>

int kbget(void); // Explicit declaration for kbget

#pragma glcc option("KBGET_AUTOBTN")

/* Game constants */
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120
#define MAX_BULLETS 5
#define MAX_ENEMIES 8
#define PLAYER_Y 100
#define MAX_EXPLOSIONS 10

/* Colors */
#define COLOR_BLACK   0x00
#define COLOR_BLUE    0x10
#define COLOR_GREEN   0x20
#define COLOR_YELLOW  0x30
#define COLOR_RED     0x40
#define COLOR_MAGENTA 0x50
#define COLOR_WHITE   0x60
#define COLOR_CYAN    0x70

/* Sprite data (6x6 pixels, 2-bit color per pixel) */
// Player sprite - 飞机形状
const char player_sprite[] = {
    0b00110000, 0b00000000, // Row 0: 机翼
    0b01111000, 0b00000000, // Row 1: 机翼扩展
    0b11111100, 0b00000000, // Row 2: 机身
    0b01111000, 0b00000000, // Row 3: 机身
    0b00110000, 0b00000000, // Row 4: 尾翼
    0b00000000, 0b00000000  // Row 5: 尾翼尖端
};

// Bullet sprite - 子弹形状
const char bullet_sprite[] = {
    0b00000000, 0b00000000, // Row 0
    0b00000000, 0b00000000, // Row 1
    0b11110000, 0b00000000, // Row 2: 子弹主体
    0b11110000, 0b00000000, // Row 3: 子弹主体
    0b00000000, 0b00000000, // Row 4
    0b00000000, 0b00000000  // Row 5
};

// Enemy sprite - 敌机形状
const char enemy_sprite[] = {
    0b00000000, 0b00000000, // Row 0
    0b01111000, 0b00000000, // Row 1: 敌机机翼
    0b11111100, 0b00000000, // Row 2: 敌机机身
    0b01111000, 0b00000000, // Row 3: 敌机机身
    0b00110000, 0b00000000, // Row 4: 敌机尾翼
    0b00000000, 0b00000000  // Row 5
};

// Strong enemy sprite - 强敌形状
const char strong_enemy_sprite[] = {
    0b01111000, 0b00000000, // Row 0: 强敌机翼
    0b11111100, 0b00000000, // Row 1: 强敌机身
    0b11111100, 0b00000000, // Row 2: 强敌机身
    0b11111100, 0b00000000, // Row 3: 强敌机身
    0b01111000, 0b00000000, // Row 4: 强敌尾翼
    0b00110000, 0b00000000  // Row 5: 强敌尾翼尖端
};

// Explosion sprite - 爆炸效果
const char explosion_sprite[] = {
    0b11110000, 0b00000000, // Row 0
    0b11111100, 0b00000000, // Row 1
    0b11111100, 0b00000000, // Row 2
    0b11111100, 0b00000000, // Row 3
    0b11110000, 0b00000000, // Row 4
    0b11000000, 0b00000000  // Row 5
};

/* Game objects */
typedef struct {
    int x, y;
    int active;
} bullet_t;

typedef struct {
    int x, y;
    int active;
    int type; // 0 for normal, 1 for stronger
} enemy_t;

typedef struct {
    int x, y;
    int active;
    int frame;
} explosion_t;

/* State variables */
int player_x;
int score;
int level;
int game_over;
int fire_cooldown;

bullet_t bullets[MAX_BULLETS];
enemy_t enemies[MAX_ENEMIES];
explosion_t explosions[MAX_EXPLOSIONS];

// Helper function to make a word from two bytes
static unsigned int makew(unsigned char h, unsigned char l) {
    return ((unsigned int)h << 8) | l;
}

// Clear screen with background color
static void clear_screen(void)
{
    SYS_SetMemory(SCREEN_HEIGHT * 256, COLOR_BLACK, (void*)0x0800);
}

// Draw a filled rectangle
static void draw_rect(int x, int y, int w, int h, int color)
{
    unsigned int addr;
    int i;
    
    if (x < 0 || y < 0 || x + w > SCREEN_WIDTH || y + h > SCREEN_HEIGHT)
        return;
    
    for (i = 0; i < h; i++) {
        addr = makew(y + i + 8, x);
        SYS_SetMemory(w, color, (void*)addr);
    }
}

// Draw a 6x6 sprite at the specified position
static void draw_sprite(int x, int y, const char *sprite_data)
{
    // Ensure sprite is within screen bounds
    if (x < -5 || x >= SCREEN_WIDTH || y < -5 || y >= SCREEN_HEIGHT) {
        return;
    }
    
    // Calculate screen address (video memory starts at 0x0800)
    // Each row is 256 bytes, but only 160 pixels are visible
    // Each byte represents 4 pixels (2 bits per pixel)
    unsigned int addr = makew(y + 8, x);
    
    // Use SYS_Sprite6 to draw the sprite
    SYS_Sprite6(sprite_data, (void*)addr);
}

// Draw player sprite
static void draw_player(void)
{
    draw_sprite(player_x - 3, PLAYER_Y, player_sprite);
}

// Draw bullet sprite
static void draw_bullet(int x, int y)
{
    draw_sprite(x - 2, y, bullet_sprite);
}

// Draw enemy sprite based on type
static void draw_enemy(int x, int y, int type)
{
    if (type == 1) {
        draw_sprite(x - 3, y, strong_enemy_sprite);
    } else {
        draw_sprite(x - 3, y, enemy_sprite);
    }
}

// Draw explosion sprite
static void draw_explosion(int x, int y, int frame)
{
    // Use different colors for different explosion frames
    if (frame < 3) {
        draw_sprite(x - 3, y, explosion_sprite);
    }
}

// Initialize game state
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
    
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        explosions[i].active = 0;
    }
    
    // Spawn initial enemies
    for (i = 0; i < 3 + level; i++) {
        if (i < MAX_ENEMIES) {
            enemies[i].x = 10 + rand() % (SCREEN_WIDTH - 20);
            enemies[i].y = 10 + rand() % 30;
            enemies[i].active = 1;
            enemies[i].type = (rand() % 3 == 0) ? 1 : 0;
        }
    }
}

// Spawn new enemy
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

// Create explosion effect
static void create_explosion(int x, int y)
{
    int i;
    
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].active = 1;
            explosions[i].frame = 0;
            break;
        }
    }
}

// Update bullet positions
static void update_bullets(void)
{
    int i;
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].y -= 4;
            if (bullets[i].y <= 0) {
                bullets[i].active = 0;
            }
        }
    }
}

// Update enemy positions
static void update_enemies(void)
{
    int i;
    
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].y += (enemies[i].type == 1) ? 2 : 1;
            
            if (enemies[i].y >= PLAYER_Y + 10) {
                enemies[i].active = 0;
            }
            
            // Check collision with player
            if (enemies[i].y >= PLAYER_Y - 6 &&
                enemies[i].y <= PLAYER_Y + 6 &&
                abs(enemies[i].x - player_x) < 6) {
                game_over = 1;
                create_explosion(player_x, PLAYER_Y);
            }
        }
    }
}

// Update explosion animations
static void update_explosions(void)
{
    int i;
    
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            explosions[i].frame++;
            if (explosions[i].frame >= 6) {
                explosions[i].active = 0;
            }
        }
    }
}

// Check collisions between bullets and enemies
static void check_collisions(void)
{
    int i, j;
    
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    if (abs(bullets[i].x - enemies[j].x) < 6 &&
                        abs(bullets[i].y - enemies[j].y) < 6) {
                        bullets[i].active = 0;
                        enemies[j].active = 0;
                        score += (enemies[j].type == 1) ? 20 : 10;
                        create_explosion(enemies[j].x, enemies[j].y);
                        
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

// Handle player input
static void handle_input(void)
{
    int key;
    
    if ((key = kbget()) >= 0) {
        switch(key) {
        case 0xfd: /* Left */
            if (player_x > 5) player_x -= 4;
            break;
        case 0xfe: /* Right */
            if (player_x < SCREEN_WIDTH - 5) player_x += 4;
            break;
        case 0x7f: /* A button (fire) */
            if (fire_cooldown == 0) {
                int i;
                for (i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].x = player_x;
                        bullets[i].y = PLAYER_Y - 6;
                        bullets[i].active = 1;
                        fire_cooldown = 6;
                        break;
                    }
                }
            }
            break;
        }
    }
    
    if (fire_cooldown > 0) fire_cooldown--;
}

// Draw background stars
static void draw_background(void)
{
    static int star_offset = 0;
    int i;
    
    star_offset = (star_offset + 1) % 10;
    
    // Draw some stars
    for (i = 0; i < 5; i++) {
        int x = (i * 30 + star_offset) % SCREEN_WIDTH;
        int y = (i * 20) % (SCREEN_HEIGHT / 2);
        draw_rect(x, y, 1, 1, COLOR_WHITE);
    }
}

// Draw score display
static void draw_score(void)
{
    // Draw score as a bar at the top
    draw_rect(10, 5, 40, 8, COLOR_WHITE);
    draw_rect(15, 7, score / 10, 4, COLOR_YELLOW);
    
    // Draw level indicator
    draw_rect(SCREEN_WIDTH - 30, 5, 20, 8, COLOR_WHITE);
    draw_rect(SCREEN_WIDTH - 28, 7, level * 4, 4, COLOR_GREEN);
}

// Draw game over screen
static void draw_game_over(void)
{
    // Draw "GAME OVER" using rectangles
    draw_rect(SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2 - 10, 60, 10, COLOR_RED);
    draw_rect(SCREEN_WIDTH/2 - 25, SCREEN_HEIGHT/2, 50, 10, COLOR_WHITE);
    
    // Draw final score
    draw_rect(SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 + 15, 40, 8, COLOR_WHITE);
    draw_rect(SCREEN_WIDTH/2 - 18, SCREEN_HEIGHT/2 + 17, score / 20, 4, COLOR_YELLOW);
}

// Main render function
static void render_game(void)
{
    int i;
    
    clear_screen();
    draw_background();
    
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
    
    for (i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            draw_explosion(explosions[i].x, explosions[i].y, explosions[i].frame);
        }
    }
    
    draw_score();
    
    if (game_over) {
        draw_game_over();
    }
}

// Main game loop
int main(void)
{
    int enemy_spawn_timer = 0;
    int i;
    
    // Set video mode to 160x120 bitmap mode
    SYS_SetMode(1);
    // Explicitly clear the first line of video memory to ensure proper initialization
    SYS_SetMemory(256, 0x00, (void*)0x0800);
    
    for(;;) {
        init_game();
        
        while(!game_over) {
            handle_input();
            update_bullets();
            update_enemies();
            update_explosions();
            check_collisions();
            
            enemy_spawn_timer++;
            if (enemy_spawn_timer >= 60 - level * 5) {
                spawn_enemy();
                enemy_spawn_timer = 0;
            }
            
            render_game();
            
            // Simple delay loop
            for (i = 0; i < 800; i++);
        }
        
        // Wait for all keys to be released, then wait for a key press to restart
        while(kbget() >= 0) ;
        while(kbget() < 0) ;
    }
    
    return 0;
}
