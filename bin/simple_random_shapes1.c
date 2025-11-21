/* 简单随机矩形程序 - 确保所有像素点都在屏幕以内 */

#include <stdlib.h>
#include <gigatron/console.h>
#include <gigatron/sys.h>

#define HEIGHT 120
#define WIDTH 160

/* 绘制像素点，带边界检查 */
void drawPixel(int x, int y, int color)
{
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    screenMemory[y][x] = color;
  }
}

/* 绘制矩形 */
void draw_rect(int x, int y, int w, int h, int color)
{
  int i, j;
  /* 确保矩形完全在屏幕内 */
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x + w >= WIDTH) w = WIDTH - x - 1;
  if (y + h >= HEIGHT) h = HEIGHT - y - 1;
  
  /* 确保矩形有最小尺寸 */
  if (w <= 0) w = 1;
  if (h <= 0) h = 1;
  
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      drawPixel(x + j, y + i, color);
    }
  }
}

/* 随机绘制矩形 */
void random_rect(void)
{
  int x = rand() % WIDTH;
  int y = rand() % HEIGHT;
  int w = 5 + rand() % 20;
  int h = 5 + rand() % 20;
  int color = 1 + rand() % 15;
  
  draw_rect(x, y, w, h, color);
}

/* 主函数 */
void main(void)
{
  int i, n;
  
  /* 设置视频模式 */
  SYS_SetMode(1);
  
  /* 主循环 - 不清屏，直接开始绘制 */
  for(n = 0; n < 100; n++) {
    /* 随机绘制10个矩形 */
    for (i = 0; i < 10; i++) {
      random_rect();
    }
    
    /* 简单延迟 */
    for (i = 0; i < 5000; i++);
  }
}
