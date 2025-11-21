/* 简单随机图形程序 - 支持矩形和三角形精灵 */

#include <stdlib.h>
#include <gigatron/console.h>
#include <gigatron/sys.h>

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

/* 生成随机6x6精灵数据 */
static void generate_random_sprite(char sprite_data[12])
{
    int i;
    for (i = 0; i < 12; i++) {
        sprite_data[i] = rand() & 0xFF;  /* 随机填充每个字节 */
    }
}

/* 使用SYS_Sprite6绘制随机精灵 */
static void draw_random_sprite(int x, int y)
{
    char random_sprite[12];
    
    /* 确保精灵在绘制区域内 */
    if (x < DRAW_START_X || x + 6 >= WIDTH - BORDER ||
        y < DRAW_START_Y || y + 6 >= HEIGHT - BORDER) {
        return;
    }
    
    /* 生成随机精灵数据 */
    generate_random_sprite(random_sprite);
    
    /* 计算屏幕地址 */
    unsigned int addr = makew(y + 8, x);
    
    /* 使用SYS_Sprite6绘制随机精灵 */
    SYS_Sprite6(random_sprite, (void*)addr);
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
  int x, y;
  
  /* 随机生成位置 */
  x = DRAW_START_X + rand() % (DRAW_WIDTH - 6);
  y = DRAW_START_Y + rand() % (DRAW_HEIGHT - 6);
  
  /* 使用随机精灵数据绘制 */
  draw_random_sprite(x, y);
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
  
  /* 主循环 - 无限循环，不停顿 */
  while(1) {
    /* 随机选择绘制矩形或三角形精灵 */
    if (rand() % 2) {
      random_rect();
    } else {
      random_triangle();
    }
  }
}
