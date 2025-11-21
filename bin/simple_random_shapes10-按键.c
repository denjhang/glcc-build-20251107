/* 简单随机图形程序 - 支持矩形和三角形精灵 */

#include <stdlib.h>
#include <gigatron/console.h>
#include <gigatron/sys.h>
#include <gigatron/kbget.h>

/* 启用自动按钮处理 */
#pragma glcc option("KBGET_AUTOBTN")

/* 按键定义 */
#define KEY_LEFT  (0xff ^ buttonLeft)
#define KEY_RIGHT (0xff ^ buttonRight)
#define KEY_UP    (0xff ^ buttonUp)
#define KEY_DOWN  (0xff ^ buttonDown)
#define KEY_A     (0xff ^ buttonA)
#define KEY_B     (0xff ^ buttonB)

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
static char test_sprite[12];

/* 生成随机6x6精灵数据 */
static void generate_random_sprite(void)
{
    int i;
    for (i = 0; i < 12; i++) {
        test_sprite[i] = rand() & 0xFF;  /* 随机填充每个字节 */
    }
    
    /* 在屏幕上打印数组内容 */
    for (i = 0; i < 12; i++) {
        printf("%02X ", test_sprite[i]);  /* 打印每个字节的十六进制值 */
    }
    printf("\n");  /* 换行 */
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

/* 处理按键输入 */
static void handle_input(void)
{
    int key;
    
    if ((key = kbget()) >= 0) {
        switch(key) {
        case KEY_LEFT:
            if (sprite_x > DRAW_START_X) sprite_x--;
            break;
        case KEY_RIGHT:
            if (sprite_x < WIDTH - BORDER - 6) sprite_x++;
            break;
        case KEY_UP:
            if (sprite_y > DRAW_START_Y) sprite_y--;
            break;
        case KEY_DOWN:
            if (sprite_y < HEIGHT - BORDER - 6) sprite_y++;
            break;
        case KEY_A:
            /* 按下A键，生成新的随机精灵 */
            if (sprite_cooldown == 0) {
                generate_and_draw_sprite();
                sprite_cooldown = 3; /* 冷却时间 */
            }
            break;
        case KEY_B:
            /* 按下B键，用白色快速填充屏幕 */
            clear_screen_white();
            draw_border();
            draw_white_border_rect();
            break;
        }
    }
    
    /* 更新冷却时间 */
    if (sprite_cooldown > 0) sprite_cooldown--;
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
  generate_and_draw_sprite();
  
  /* 主循环 - 无限循环，不停顿 */
  while(1) {
    /* 处理按键输入 */
    handle_input();
    
    /* 绘制精灵 */
    draw_sprite_at_position();
  }
}
