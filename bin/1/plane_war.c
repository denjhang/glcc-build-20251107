/* Plane War - 飞机大战游戏 */

#include <stdlib.h>
#include <string.h>
#include <conio.h>

#ifdef __gigatron

# include <gigatron/sys.h>

/* Field size */
# define FW 26
# define FH 14

/* Keys */
# define KEY_LEFT  (0xff ^ buttonLeft)
# define KEY_RIGHT (0xff ^ buttonRight)
# define KEY_FIRE  (0xff ^ buttonA)
# define KEY_UP    (0xff ^ buttonUp)
# define KEY_DOWN  (0xff ^ buttonDown)

/* For page zero variables */ 
# define near __near

/* For timing */
# define clock_t signed char
# define CLOCKS_PER_SEC 60
# define clock() frameCount

# pragma glcc option("PRINTF_SIMPLE")
# pragma glcc option("KBGET_AUTOBTN")

#else /* ! __gigatron */

# include <time.h>
# define FW 80
# define FH 23
# define KEY_LEFT  'a'
# define KEY_RIGHT 'd'
# define KEY_FIRE  ' '
# define KEY_UP    'w'
# define KEY_DOWN  's'
# define near

#endif /* ! __gigatron */

/* Game constants */
#define MAX_BULLETS 5
#define MAX_ENEMIES 8
#define PLAYER_Y (FH-2)

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
static char field[FH][FW];
near int player_x;
near int score;
near int level;
near int game_over;
near int fire_cooldown;

bullet_t bullets[MAX_BULLETS];
enemy_t enemies[MAX_ENEMIES];

enum what_s {
    blank  = 0x0,
    wall   = 0x1,
    player = 0x2,
    bullet = 0x3,
    enemy  = 0x4,
    explosion = 0x5
};

static const char chars[] = " \x7f#^*V+";
static const char colors[] = { BLUE, LIGHTGRAY, GREEN, YELLOW, RED, MAGENTA }; 

static void init_game(void)
{
    int i;
    
    /* Clear field */
    memset(field, 0, sizeof(field));
    
    /* Initialize player */
    player_x = FW / 2;
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
            enemies[i].x = rand() % (FW - 2) + 1;
            enemies[i].y = rand() % 4 + 1;
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
            enemies[i].x = rand() % (FW - 2) + 1;
            enemies[i].y = 1;
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
            bullets[i].y--;
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
            enemies[i].y++;
            
            /* Fast enemies move faster */
            if (enemies[i].type == 1 && (frameCount & 1) == 0) {
                enemies[i].y++;
            }
            
            /* Check if enemy reached bottom */
            if (enemies[i].y >= FH - 1) {
                enemies[i].active = 0;
            }
            
            /* Check collision with player */
            if (enemies[i].y == PLAYER_Y && 
                abs(enemies[i].x - player_x) < 2) {
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
                    if (bullets[i].x == enemies[j].x && 
                        bullets[i].y == enemies[j].y) {
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
    
    if ((key = kbget()) >= 0) {
        switch(key) {
        case KEY_LEFT:
            if (player_x > 1) player_x--;
            break;
        case KEY_RIGHT:
            if (player_x < FW - 2) player_x++;
            break;
        case KEY_FIRE:
            if (fire_cooldown == 0) {
                int i;
                /* Find inactive bullet slot */
                for (i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].x = player_x;
                        bullets[i].y = PLAYER_Y - 1;
                        bullets[i].active = 1;
                        fire_cooldown = 5; /* Cooldown frames */
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

static char prev_field[FH][FW]; /* Previous frame field */

static void render_game(void)
{
    int i, j;
    static int first_render = 1;
    
    /* Initialize on first render */
    if (first_render) {
        clrscr();
        memset(prev_field, 0, sizeof(prev_field));
        
        /* Draw walls (static) */
        for (i = 0; i < FW; i++) {
            gotoxy(i+1, 1);
            textcolor(colors[wall]);
            putch(chars[wall]);
            gotoxy(i+1, FH);
            textcolor(colors[wall]);
            putch(chars[wall]);
            prev_field[0][i] = wall;
            prev_field[FH-1][i] = wall;
        }
        for (i = 1; i < FH-1; i++) {
            gotoxy(1, i+1);
            textcolor(colors[wall]);
            putch(chars[wall]);
            gotoxy(FW, i+1);
            textcolor(colors[wall]);
            putch(chars[wall]);
            prev_field[i][0] = wall;
            prev_field[i][FW-1] = wall;
        }
        first_render = 0;
    }
    
    /* Clear field for current frame */
    for (i = 1; i < FH-1; i++) {
        for (j = 1; j < FW-1; j++) {
            field[i][j] = blank;
        }
    }
    
    /* Draw player */
    field[PLAYER_Y][player_x] = player;
    
    /* Draw bullets */
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            field[bullets[i].y][bullets[i].x] = bullet;
        }
    }
    
    /* Draw enemies */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            field[enemies[i].y][enemies[i].x] = enemy;
        }
    }
    
    /* Only render changed cells */
    for (i = 1; i < FH-1; i++) {
        for (j = 1; j < FW-1; j++) {
            if (field[i][j] != prev_field[i][j]) {
                gotoxy(j+1, i+1);
                if (field[i][j] == blank) {
                    textcolor(colors[blank]);
                    putch(chars[blank]);
                } else {
                    textcolor(colors[field[i][j]]);
                    putch(chars[field[i][j]]);
                }
                prev_field[i][j] = field[i][j];
            }
        }
    }
    
    /* Update score and level only when needed */
    static int prev_score = -1, prev_level = -1;
    if (score != prev_score || level != prev_level) {
        textcolor(WHITE);
        gotoxy(1, FH);
        cprintf("Score:%03d L:%d", score, level);
        prev_score = score;
        prev_level = level;
    }
    
    if (game_over) {
        textcolor(RED);
        gotoxy(FW/2-5, FH/2);
        cprintf("GAME OVER!");
        gotoxy(FW/2-8, FH/2+1);
        cprintf("Press any key");
    }
}

int main(void)
{
    clock_t clk, nclk;
    int enemy_spawn_timer = 0;
    
    for(;;) {
        init_game();
        
        while(!game_over) {
            clk = clock();
            
            /* Handle input */
            handle_input();
            
            /* Update game state every few frames */
            if ((int)(clock_t)(clk - nclk) >= 0) {
                nclk = clk + 3; /* Update every 3 frames */
                
                update_bullets();
                update_enemies();
                check_collisions();
                
                /* Spawn new enemies periodically */
                enemy_spawn_timer++;
                if (enemy_spawn_timer >= 30 - level * 2) {
                    spawn_enemy();
                    enemy_spawn_timer = 0;
                }
            }
            
            /* Render */
            render_game();
        }
        
        /* Wait for key press to restart */
        while(kbget() >= 0) ; /* Clear buffer */
        while(kbget() < 0) ; /* Wait for key */
    }
    
    return 0;
}