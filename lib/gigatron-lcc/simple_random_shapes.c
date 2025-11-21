/* 简单随机图形程序 - 支持矩形和三角形精灵 */

#include <stdlib.h>
#include <gigatron/console.h>
#include <gigatron/sys.h>
#include <gigatron/kbget.h>
#include <gigatron/libc.h>

/* 按键定义 - 直接使用buttonState的位值 */
#define KEY_LEFT  buttonLeft
#define KEY_RIGHT buttonRight
#define KEY_UP    buttonUp
#define KEY_DOWN  buttonDown
#define KEY_A     buttonA
#define KEY_B     buttonB
#define KEY_START buttonStart

#define HEIGHT 120
#define WIDTH 160
/* 定义绘制范围为距离屏幕边缘5像素的区域 */
#define BORDER 5
#define DRAW_START_X BORDER
#define DRAW_START_Y BORDER
#define DRAW_WIDTH (WIDTH - 2 * BORDER)
#define DRAW_HEIGHT (HEIGHT - 2 * BORDER)

/* 辅助函数：构造地址 */
static unsigned int makew(unsigned char h, unsigned char l) {
  return (h << 8) | l;
}

/* 精灵位置变量 */
static int sprite_x = WIDTH / 2 - 3;
static int sprite_y = HEIGHT / 2 - 3;
static int sprite_cooldown = 0;

/* 保存上一次的精灵位置，用于擦除轨迹 */
static int prev_sprite_x = WIDTH / 2 - 3;
static int prev_sprite_y = HEIGHT / 2 - 3;

/* 精灵颜色变量 - RGB222格式 */
static int sprite_r = 3;  /* 红色通道：0-3 */
static int sprite_g = 0;  /* 绿色通道：0-3 */
static int sprite_b = 0;  /* 蓝色通道：0-3 */
static int sprite_color = 0x30;  /* 初始为红色 (RGB222: 110000) */

/* 按键长按变量 */
static int last_key = -1;  /* 上一次的键值 */

/* 绘制像素点，带边界检查 */
void drawPixel(int x, int y, int color)
{
  /* 只在距离屏幕边缘5像素的区域内绘制 */
  if (x >= DRAW_START_X && x < WIDTH - BORDER && y >= DRAW_START_Y && y < HEIGHT - BORDER) {
    screenMemory[y][x] = color;
  }
}

/* 绘制矩形 */
void draw_rect(int x, int y, int w, int h, int color)
{
  int i;
  /* 确保矩形完全在绘制区域内 */
  if (x < DRAW_START_X) x = DRAW_START_X;
  if (y < DRAW_START_Y) y = DRAW_START_Y;
  if (x + w >= WIDTH - BORDER) w = WIDTH - BORDER - x;
  if (y + h >= HEIGHT - BORDER) h = HEIGHT - BORDER - y;
  
  /* 确保矩形有最小尺寸 */
  if (w <= 0) w = 1;
  if (h <= 0) h = 1;
  
  /* 使用SYS_SetMemory加速矩形绘制 */
  for (i = 0; i < h; i++) {
    SYS_SetMemory(w, color, &screenMemory[y + i][x]);
  }
}

/* 6x6精灵数据数组 - 每次生成新的随机数据 */
static char test_sprite[36];  /* 6x6像素需要36个字节 */

/* 绿色精灵数据数组 */
static char green_sprite[36];  /* 6x6像素需要36个字节 */

/* 绿色精灵位置和速度变量 */
static int green_x = WIDTH / 2 - 3;
static int green_y = HEIGHT / 2 - 3;
static int green_vx = 1;  /* X方向速度 */
static int green_vy = 1;  /* Y方向速度 */

/* 保存绿色精灵的上一次位置，用于擦除轨迹 */
static int prev_green_x = WIDTH / 2 - 3;
static int prev_green_y = HEIGHT / 2 - 3;

/* 生成6x6精灵数据 - 使用当前颜色 */
static void generate_sprite_with_color(void)
{
    int i;
    for (i = 0; i < 36; i++) {
        test_sprite[i] = sprite_color;  /* 使用当前颜色填充每个像素 */
    }
}

/* 生成随机6x6精灵数据 */
static void generate_random_sprite(void)
{
    int i;
    for (i = 0; i < 36; i++) {
        test_sprite[i] = rand() & 0xFF;  /* 随机填充每个字节 */
    }
}

/* 生成绿色精灵数据 */
static void generate_green_sprite(void)
{
    int i;
    for (i = 0; i < 36; i++) {
        green_sprite[i] = 0x0C;  /* 绿色 (RGB222: 001100) */
    }
}

/* 初始化绿色精灵的位置和速度 */
static void init_green_sprite(void)
{
    /* 随机位置，确保在绘制区域内 */
    green_x = DRAW_START_X + rand() % (DRAW_WIDTH - 6);
    green_y = DRAW_START_Y + rand() % (DRAW_HEIGHT - 6);
    
    /* 随机速度，范围-2到2（不包括0） */
    do {
        green_vx = (rand() % 5) - 2;  /* -2, -1, 0, 1, 2 */
    } while (green_vx == 0);
    
    do {
        green_vy = (rand() % 5) - 2;  /* -2, -1, 0, 1, 2 */
    } while (green_vy == 0);
    
    /* 初始化前一次位置 */
    prev_green_x = green_x;
    prev_green_y = green_y;
    
    /* 生成绿色精灵数据 */
    generate_green_sprite();
}

/* 更新绿色精灵位置并检测碰撞 */
static void update_green_sprite(void)
{
    /* 保存当前位置 */
    prev_green_x = green_x;
    prev_green_y = green_y;
    
    /* 更新位置 */
    green_x += green_vx;
    green_y += green_vy;
    
    /* 检测边界碰撞并反弹 */
    if (green_x <= DRAW_START_X) {
        green_x = DRAW_START_X;
        green_vx = -green_vx;  /* X方向反弹 */
    } else if (green_x >= WIDTH - BORDER - 6) {
        green_x = WIDTH - BORDER - 6;
        green_vx = -green_vx;  /* X方向反弹 */
    }
    
    if (green_y <= DRAW_START_Y) {
        green_y = DRAW_START_Y;
        green_vy = -green_vy;  /* Y方向反弹 */
    } else if (green_y >= HEIGHT - BORDER - 6) {
        green_y = HEIGHT - BORDER - 6;
        green_vy = -green_vy;  /* Y方向反弹 */
    }
    
    /* 检测与玩家精灵的碰撞 */
    /* 简单的矩形碰撞检测 */
    if (green_x < sprite_x + 6 && green_x + 6 > sprite_x &&
        green_y < sprite_y + 6 && green_y + 6 > sprite_y) {
        /* 发生碰撞，两个精灵都反弹 */
        green_vx = -green_vx;
        green_vy = -green_vy;
        
        /* 将绿色精灵移开，避免重复碰撞 */
        green_x = prev_green_x;
        green_y = prev_green_y;
    }
}

/* 擦除绿色精灵移动留下的轨迹 */
static void erase_green_trail(void)
{
    /* 确定移动方向并擦除相应的轨迹 */
    if (green_x > prev_green_x) {
        /* 向右移动，擦除左边缘 */
        int y;
        for (y = 0; y < 6; y++) {
            if (prev_green_y + y >= DRAW_START_Y && prev_green_y + y < HEIGHT - BORDER) {
                unsigned int addr = makew(prev_green_y + y + 8, prev_green_x);
                SYS_VDrawBits(0x3F3F, 0x01, (char*)addr);
            }
        }
    } else if (green_x < prev_green_x) {
        /* 向左移动，擦除右边缘 */
        int y;
        for (y = 0; y < 6; y++) {
            if (prev_green_y + y >= DRAW_START_Y && prev_green_y + y < HEIGHT - BORDER) {
                unsigned int addr = makew(prev_green_y + y + 8, prev_green_x + 5);
                SYS_VDrawBits(0x3F3F, 0x01, (char*)addr);
            }
        }
    } else if (green_y > prev_green_y) {
        /* 向下移动，擦除上边缘 */
        if (prev_green_y >= DRAW_START_Y && prev_green_y < HEIGHT - BORDER) {
            SYS_SetMemory(6, 0x3F, &screenMemory[prev_green_y][prev_green_x]);
        }
    } else if (green_y < prev_green_y) {
        /* 向上移动，擦除下边缘 */
        if (prev_green_y + 5 >= DRAW_START_Y && prev_green_y + 5 < HEIGHT - BORDER) {
            SYS_SetMemory(6, 0x3F, &screenMemory[prev_green_y + 5][prev_green_x]);
        }
    }
}

/* 绘制绿色精灵 */
static void draw_green_sprite(void)
{
    /* 确保精灵在绘制区域内 */
    if (green_x < DRAW_START_X) green_x = DRAW_START_X;
    if (green_y < DRAW_START_Y) green_y = DRAW_START_Y;
    if (green_x >= WIDTH - BORDER - 6) green_x = WIDTH - BORDER - 6;
    if (green_y >= HEIGHT - BORDER - 6) green_y = HEIGHT - BORDER - 6;
    
    /* 计算屏幕地址 */
    unsigned int addr = makew(green_y + 8, green_x);
    
    /* 使用SYS_Sprite6绘制精灵 */
    SYS_Sprite6(green_sprite, (void*)addr);
}

/* 用黑色清屏 */
void clear_screen_black(void)
{
    int i;
    for (i = 0; i < HEIGHT; i++) {
        SYS_SetMemory(WIDTH, 0, &screenMemory[i][0]);  /* 0是黑色 */
    }
}

/* 用白色快速填充屏幕 */
void clear_screen_white(void)
{
    int i;
    for (i = 0; i < HEIGHT; i++) {
        SYS_SetMemory(WIDTH, 0x3F, &screenMemory[i][0]);  /* 0x3F是白色 */
    }
}

/* 绘制白色边框矩形填充整个绘制区域 */
void draw_white_border_rect(void)
{
    /* 绘制整个绘制区域的白色边框 */
    draw_rect(DRAW_START_X, DRAW_START_Y, DRAW_WIDTH, DRAW_HEIGHT, 0x3F); /* 0x3F是白色 */
}

/* 使用SYS_Sprite6绘制精灵 - 在当前位置 */
static void draw_sprite_at_position(void)
{
    /* 确保精灵在绘制区域内 */
    if (sprite_x < DRAW_START_X) sprite_x = DRAW_START_X;
    if (sprite_y < DRAW_START_Y) sprite_y = DRAW_START_Y;
    if (sprite_x >= WIDTH - BORDER - 6) sprite_x = WIDTH - BORDER - 6;
    if (sprite_y >= HEIGHT - BORDER - 6) sprite_y = HEIGHT - BORDER - 6;
    
    /* 计算屏幕地址 */
    unsigned int addr = makew(sprite_y + 8, sprite_x);
    
    /* 使用SYS_Sprite6绘制精灵 */
    SYS_Sprite6(test_sprite, (void*)addr);
}

/* 生成新的随机精灵并绘制 */
static void generate_and_draw_sprite(void)
{
    /* 生成新的随机精灵数据 */
    generate_random_sprite();
    
    /* 绘制精灵 */
    draw_sprite_at_position();
}

/* 生成当前颜色的精灵并绘制 */
static void generate_color_sprite_and_draw(void)
{
    /* 生成当前颜色的精灵数据 */
    generate_sprite_with_color();
    
    /* 绘制精灵 */
    draw_sprite_at_position();
}

/* 随机绘制矩形 */
void random_rect(void)
{
  int x, y, w, h, color;
  
  /* 随机生成矩形尺寸 */
  w = 5 + rand() % 20;
  h = 5 + rand() % 20;
  
  /* 使用RGB222颜色模式生成64种颜色 */
  int r = rand() % 4;  /* 红色通道：0-3 */
  int g = rand() % 4;  /* 绿色通道：0-3 */
  int b = rand() % 4;  /* 蓝色通道：0-3 */
  color = (r << 4) | (g << 2) | b;  /* 组合为RGB222格式 */
  
  /* 计算在绘制区域内允许的最大尺寸 */
  int max_w = DRAW_WIDTH - w;
  int max_h = DRAW_HEIGHT - h;
  
  /* 确保矩形完全在绘制区域内 */
  if (max_w > 0) {
    x = DRAW_START_X + rand() % max_w;
  } else {
    x = DRAW_START_X;
    w = DRAW_WIDTH;
  }
  
  if (max_h > 0) {
    y = DRAW_START_Y + rand() % max_h;
  } else {
    y = DRAW_START_Y;
    h = DRAW_HEIGHT;
  }
  
  /* 直接绘制，不检查重叠 */
  draw_rect(x, y, w, h, color);
}

/* 随机绘制6x6精灵 */
void random_triangle(void)
{
  /* 随机生成位置 */
  sprite_x = DRAW_START_X + rand() % (DRAW_WIDTH - 6);
  sprite_y = DRAW_START_Y + rand() % (DRAW_HEIGHT - 6);
  
  /* 生成新的随机精灵数据并绘制 */
  generate_and_draw_sprite();
}

/* 绘制黑色边框 */
void draw_border(void)
{
  int i;
  
  /* 上边框 */
  for (i = 0; i < BORDER; i++) {
    SYS_SetMemory(WIDTH, 0, &screenMemory[i][0]);
  }
  
  /* 下边框 */
  for (i = HEIGHT - BORDER; i < HEIGHT; i++) {
    SYS_SetMemory(WIDTH, 0, &screenMemory[i][0]);
  }
  
  /* 左边框 */
  for (i = BORDER; i < HEIGHT - BORDER; i++) {
    SYS_SetMemory(BORDER, 0, &screenMemory[i][0]);
  }
  
  /* 右边框 */
  for (i = BORDER; i < HEIGHT - BORDER; i++) {
    SYS_SetMemory(BORDER, 0, &screenMemory[i][WIDTH - BORDER]);
  }
}

/* 擦除精灵移动留下的轨迹 - 使用SYS_VDrawBits优化 */
static void erase_sprite_trail(void)
{
    /* 确定移动方向并擦除相应的轨迹 */
    if (sprite_x > prev_sprite_x) {
        /* 向右移动，擦除左边缘 */
        /* 使用SYS_VDrawBits一次性擦除6个像素 */
        int y;
        for (y = 0; y < 6; y++) {
            if (prev_sprite_y + y >= DRAW_START_Y && prev_sprite_y + y < HEIGHT - BORDER) {
                /* 计算地址 */
                unsigned int addr = makew(prev_sprite_y + y + 8, prev_sprite_x);
                /* 使用SYS_VDrawBits擦除一个像素，前景色和背景色都设为白色 */
                SYS_VDrawBits(0x3F3F, 0x01, (char*)addr);
            }
        }
    } else if (sprite_x < prev_sprite_x) {
        /* 向左移动，擦除右边缘 */
        int y;
        for (y = 0; y < 6; y++) {
            if (prev_sprite_y + y >= DRAW_START_Y && prev_sprite_y + y < HEIGHT - BORDER) {
                /* 计算地址 */
                unsigned int addr = makew(prev_sprite_y + y + 8, prev_sprite_x + 5);
                /* 使用SYS_VDrawBits擦除一个像素，前景色和背景色都设为白色 */
                SYS_VDrawBits(0x3F3F, 0x01, (char*)addr);
            }
        }
    } else if (sprite_y > prev_sprite_y) {
        /* 向下移动，擦除上边缘 */
        /* 使用SYS_SetMemory一次性擦除6个像素 */
        if (prev_sprite_y >= DRAW_START_Y && prev_sprite_y < HEIGHT - BORDER) {
            /* 使用SYS_SetMemory擦除一行6个像素为白色 */
            SYS_SetMemory(6, 0x3F, &screenMemory[prev_sprite_y][prev_sprite_x]);
        }
    } else if (sprite_y < prev_sprite_y) {
        /* 向上移动，擦除下边缘 */
        /* 使用SYS_SetMemory一次性擦除6个像素 */
        if (prev_sprite_y + 5 >= DRAW_START_Y && prev_sprite_y + 5 < HEIGHT - BORDER) {
            /* 使用SYS_SetMemory擦除一行6个像素为白色 */
            SYS_SetMemory(6, 0x3F, &screenMemory[prev_sprite_y + 5][prev_sprite_x]);
        }
    }
}

/* 执行按键动作 */
static void execute_key_action(void)
{
    /* 保存当前位置 */
    prev_sprite_x = sprite_x;
    prev_sprite_y = sprite_y;
    
    /* 检查每个按键是否被按下 */
    if(!(buttonState & KEY_LEFT)) {
        if (sprite_x > DRAW_START_X) {
            sprite_x--;
            erase_sprite_trail();
            draw_sprite_at_position();
        }
    }
    if(!(buttonState & KEY_RIGHT)) {
        if (sprite_x < WIDTH - BORDER - 6) {
            sprite_x++;
            erase_sprite_trail();
            draw_sprite_at_position();
        }
    }
    if(!(buttonState & KEY_UP)) {
        if (sprite_y > DRAW_START_Y) {
            sprite_y--;
            erase_sprite_trail();
            draw_sprite_at_position();
        }
    }
    if(!(buttonState & KEY_DOWN)) {
        if (sprite_y < HEIGHT - BORDER - 6) {
            sprite_y++;
            erase_sprite_trail();
            draw_sprite_at_position();
        }
    }
    if(!(buttonState & KEY_A)) {
        int color_index = sprite_color;
        color_index = (color_index + 1) & 0x3F;
        sprite_color = color_index;
        
        sprite_r = (sprite_color >> 4) & 0x03;
        sprite_g = (sprite_color >> 2) & 0x03;
        sprite_b = sprite_color & 0x03;
        
        generate_sprite_with_color();
        draw_sprite_at_position();
    }
    if(!(buttonState & KEY_B)) {
        int color_index = sprite_color;
        color_index = (color_index - 1) & 0x3F;
        sprite_color = color_index;
        
        sprite_r = (sprite_color >> 4) & 0x03;
        sprite_g = (sprite_color >> 2) & 0x03;
        sprite_b = sprite_color & 0x03;
        
        generate_sprite_with_color();
        draw_sprite_at_position();
    }
    if(!(buttonState & KEY_START)) {
        /* 保存当前精灵位置和颜色 */
        int saved_x = sprite_x;
        int saved_y = sprite_y;
        int saved_color = sprite_color;
        
        /* 清屏 */
        clear_screen_black();
        
        /* 绘制黑色边框 */
        draw_border();
        
        /* 绘制白色边框矩形填充整个绘制区域 */
        draw_white_border_rect();
        
        /* 恢复精灵位置和颜色 */
        sprite_x = saved_x;
        sprite_y = saved_y;
        sprite_color = saved_color;
        
        /* 更新颜色变量 */
        sprite_r = (sprite_color >> 4) & 0x03;
        sprite_g = (sprite_color >> 2) & 0x03;
        sprite_b = sprite_color & 0x03;
        
        /* 更新前一次位置 */
        prev_sprite_x = sprite_x;
        prev_sprite_y = sprite_y;
        
        /* 重新生成精灵并绘制 */
        generate_sprite_with_color();
        draw_sprite_at_position();
    }
}

/* 处理按键输入 - 简化版本 */
static void handle_input(void)
{
    execute_key_action();
}

/* 主函数 */
void main(void)
{
  int i;
  
  /* 设置视频模式 */
  SYS_SetMode(1);
  
  /* 快速清屏 */
  {
    int i;
    for (i = 0; i < HEIGHT; i++) {
      SYS_SetMemory(WIDTH, 0, &screenMemory[i][0]);
    }
  }
  
  /* 绘制黑色边框 */
  draw_border();
  
  /* 绘制白色边框矩形填充整个绘制区域 */
  draw_white_border_rect();
  
  /* 生成初始精灵 */
  generate_color_sprite_and_draw();
  
  /* 初始化前一次位置 */
  prev_sprite_x = sprite_x;
  prev_sprite_y = sprite_y;
  
  /* 初始化绿色精灵 */
  init_green_sprite();
  
  /* 主循环 - 无限循环，不停顿 */
  while(1) {
    /* 处理按键输入 */
    handle_input();
    
    /* 更新绿色精灵位置 */
    update_green_sprite();
    
    /* 擦除绿色精灵轨迹 */
    erase_green_trail();
    
    /* 绘制精灵 */
    draw_sprite_at_position();
    
    /* 绘制绿色精灵 */
    draw_green_sprite();
  }
}
