/* 简单随机矩形程序 - 确保所有像素点都在屏幕以内 */

#include <stdlib.h>
#include <gigatron/console.h>
#include <gigatron/sys.h>

/* 辅助函数：构造地址 */
static unsigned int makew(unsigned char h, unsigned char l) {
  return (h << 8) | l;
}


#define HEIGHT 120
#define WIDTH 160
/* 定义绘制范围为距离屏幕边缘5像素的区域 */
#define BORDER 5
#define DRAW_START_X BORDER
#define DRAW_START_Y BORDER
#define DRAW_WIDTH (WIDTH - 2 * BORDER)
#define DRAW_HEIGHT (HEIGHT - 2 * BORDER)

/* 绘制像素点，带边界检查 */
void drawPixel(int x, int y, int color)
{
  /* 只在距离屏幕边缘20像素的区域内绘制 */
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

/* 随机绘制矩形 */
void random_rect(void)
{
  int x, y, w, h, color;
  int max_w, max_h;
  
  /* 随机生成矩形尺寸 */
  w = 5 + rand() % 20;
  h = 5 + rand() % 20;
  color = 1 + rand() % 15;
  
  /* 计算在绘制区域内允许的最大尺寸 */
  max_w = DRAW_WIDTH - w;
  max_h = DRAW_HEIGHT - h;
  
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
  
  draw_rect(x, y, w, h, color);
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
    /* 随机绘制10个矩形 */
    for (i = 0; i < 10; i++) {
      random_rect();
    }
  }
}
