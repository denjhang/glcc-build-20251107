/* 随机绘制图形程序 - 三角形、长方形、圆形 */

#include <stdlib.h>
#include <gigatron/sys.h>
#include <gigatron/libc.h>

/* 屏幕尺寸 */
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120

/* 颜色定义 */
#define COLOR_BLACK   0x00
#define COLOR_BLUE    0x10
#define COLOR_GREEN   0x20
#define COLOR_YELLOW  0x30
#define COLOR_RED     0x40
#define COLOR_MAGENTA 0x50
#define COLOR_WHITE   0x60
#define COLOR_CYAN    0x70

/* 辅助函数：从两个字节创建一个字 */
static unsigned int makew(unsigned char h, unsigned char l) {
    return ((unsigned int)h << 8) | l;
}

/* 清屏函数 */
static void clear_screen(int color)
{
    SYS_SetMemory(SCREEN_HEIGHT * 256, color, (void*)0x0800);
}

/* 绘制像素点 */
static void draw_pixel(int x, int y, int color)
{
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        unsigned int addr = makew(y + 8, x);
        SYS_SetMemory(1, color, (void*)addr);
    }
}

/* 绘制水平线 */
static void draw_hline(int x1, int x2, int y, int color)
{
    int x;
    if (y < 0 || y >= SCREEN_HEIGHT) return;
    
    if (x1 > x2) { int temp = x1; x1 = x2; x2 = temp; }
    
    if (x1 < 0) x1 = 0;
    if (x2 >= SCREEN_WIDTH) x2 = SCREEN_WIDTH - 1;
    
    for (x = x1; x <= x2; x++) {
        draw_pixel(x, y, color);
    }
}

/* 绘制垂直线 */
static void draw_vline(int x, int y1, int y2, int color)
{
    int y;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    
    if (y1 > y2) { int temp = y1; y1 = y2; y2 = temp; }
    
    if (y1 < 0) y1 = 0;
    if (y2 >= SCREEN_HEIGHT) y2 = SCREEN_HEIGHT - 1;
    
    for (y = y1; y <= y2; y++) {
        draw_pixel(x, y, color);
    }
}

/* 绘制直线（Bresenham算法） */
static void draw_line(int x1, int y1, int x2, int y2, int color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        draw_pixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/* 绘制填充矩形 */
static void draw_filled_rect(int x, int y, int w, int h, int color)
{
    int i;
    if (x < 0 || y < 0 || x + w > SCREEN_WIDTH || y + h > SCREEN_HEIGHT)
        return;
    
    for (i = 0; i < h; i++) {
        unsigned int addr = makew(y + i + 8, x);
        SYS_SetMemory(w, color, (void*)addr);
    }
}

/* 绘制矩形边框 */
static void draw_rect(int x, int y, int w, int h, int color)
{
    draw_hline(x, x + w - 1, y, color);
    draw_hline(x, x + w - 1, y + h - 1, color);
    draw_vline(x, y, y + h - 1, color);
    draw_vline(x + w - 1, y, y + h - 1, color);
}

/* 绘制三角形 */
static void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color)
{
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x1, y1, color);
}

/* 绘制填充三角形 */
static void draw_filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color)
{
    int min_y = y1;
    int max_y = y1;
    
    if (y2 < min_y) min_y = y2;
    if (y3 < min_y) min_y = y3;
    if (y2 > max_y) max_y = y2;
    if (y3 > max_y) max_y = y3;
    
    if (min_y < 0) min_y = 0;
    if (max_y >= SCREEN_HEIGHT) max_y = SCREEN_HEIGHT - 1;
    
    int y;
    for (y = min_y; y <= max_y; y++) {
        int x_left = SCREEN_WIDTH;
        int x_right = -1;
        
        // 扫描三条边找到当前y坐标的左右边界
        int i;
        for (i = 0; i < 3; i++) {
            int x_start, y_start, x_end, y_end;
            if (i == 0) { x_start = x1; y_start = y1; }
            else if (i == 1) { x_start = x2; y_start = y2; }
            else { x_start = x3; y_start = y3; }
            
            if (i == 2) { x_end = x1; y_end = y1; }
            else if (i == 0) { x_end = x2; y_end = y2; }
            else { x_end = x3; y_end = y3; }
            
            if ((y >= y_start && y <= y_end) || (y >= y_end && y <= y_start)) {
                if (y_start != y_end) {
                    int x = x_start + (y - y_start) * (x_end - x_start) / (y_end - y_start);
                    if (x < x_left) x_left = x;
                    if (x > x_right) x_right = x;
                }
            }
        }
        
        if (x_left <= x_right) {
            if (x_left < 0) x_left = 0;
            if (x_right >= SCREEN_WIDTH) x_right = SCREEN_WIDTH - 1;
            draw_hline(x_left, x_right, y, color);
        }
    }
}

/* 绘制圆形 */
static void draw_circle(int cx, int cy, int radius, int color)
{
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        draw_pixel(cx + x, cy + y, color);
        draw_pixel(cx + y, cy + x, color);
        draw_pixel(cx - y, cy + x, color);
        draw_pixel(cx - x, cy + y, color);
        draw_pixel(cx - x, cy - y, color);
        draw_pixel(cx - y, cy - x, color);
        draw_pixel(cx + y, cy - x, color);
        draw_pixel(cx + x, cy - y, color);
        
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

/* 绘制填充圆形 */
static void draw_filled_circle(int cx, int cy, int radius, int color)
{
    int y;
    for (y = -radius; y <= radius; y++) {
        int x_square = radius * radius - y * y;
        int x = 0;
        // 简单的平方根近似
        while (x * x <= x_square) x++;
        x--;
        draw_hline(cx - x, cx + x, cy + y, color);
    }
}

/* 获取随机颜色 */
static int random_color(void)
{
    return (rand() % 8) * 0x10;
}

/* 随机绘制矩形 */
static void random_rect(void)
{
    int x = rand() % (SCREEN_WIDTH - 20);
    int y = rand() % (SCREEN_HEIGHT - 20);
    int w = 10 + rand() % 30;
    int h = 10 + rand() % 30;
    int color = random_color();
    int filled = rand() % 2;
    
    if (filled) {
        draw_filled_rect(x, y, w, h, color);
    } else {
        draw_rect(x, y, w, h, color);
    }
}

/* 随机绘制三角形 */
static void random_triangle(void)
{
    int x1 = rand() % SCREEN_WIDTH;
    int y1 = rand() % SCREEN_HEIGHT;
    int x2 = rand() % SCREEN_WIDTH;
    int y2 = rand() % SCREEN_HEIGHT;
    int x3 = rand() % SCREEN_WIDTH;
    int y3 = rand() % SCREEN_HEIGHT;
    int color = random_color();
    int filled = rand() % 2;
    
    if (filled) {
        draw_filled_triangle(x1, y1, x2, y2, x3, y3, color);
    } else {
        draw_triangle(x1, y1, x2, y2, x3, y3, color);
    }
}

/* 随机绘制圆形 */
static void random_circle(void)
{
    int cx = rand() % SCREEN_WIDTH;
    int cy = rand() % SCREEN_HEIGHT;
    int radius = 5 + rand() % 20;
    int color = random_color();
    int filled = rand() % 2;
    
    if (filled) {
        draw_filled_circle(cx, cy, radius, color);
    } else {
        draw_circle(cx, cy, radius, color);
    }
}

/* 主函数 */
int main(void)
{
    int i;
    
    /* 设置视频模式为160x120位图模式 */
    SYS_SetMode(1);
    
    /* 清除第一行视频内存以确保正确初始化 */
    SYS_SetMemory(256, 0x00, (void*)0x0800);
    
    /* 主循环 */
    while (1) {
        /* 清屏为黑色背景 */
        clear_screen(COLOR_BLACK);
        
        /* 随机绘制20个图形 */
        for (i = 0; i < 20; i++) {
            int shape_type = rand() % 3;
            
            switch (shape_type) {
            case 0:
                random_rect();
                break;
            case 1:
                random_triangle();
                break;
            case 2:
                random_circle();
                break;
            }
        }
        
        /* 简单的延迟循环 */
        for (i = 0; i < 10000; i++);
    }
    
    return 0;
}