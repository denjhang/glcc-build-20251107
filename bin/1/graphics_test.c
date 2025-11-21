/* 简单的图形测试程序 - 展示Gigatron图形功能 */

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

/* 绘制填充矩形 */
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

/* 绘制矩形边框 */
static void draw_frame(int x, int y, int w, int h, int color)
{
    unsigned int addr;
    int i;
    
    /* 上边框 */
    addr = makew(y + 8, x);
    SYS_SetMemory(w, color, (void*)addr);
    
    /* 下边框 */
    addr = makew(y + h + 7, x);
    SYS_SetMemory(w, color, (void*)addr);
    
    /* 左边框 */
    for (i = 0; i < h; i++) {
        addr = makew(y + i + 8, x);
        SYS_SetMemory(1, color, (void*)addr);
    }
    
    /* 右边框 */
    for (i = 0; i < h; i++) {
        addr = makew(y + i + 8, x + w - 1);
        SYS_SetMemory(1, color, (void*)addr);
    }
}

/* 简单的6x6精灵数据 - 小方块 */
const char square_sprite[] = {
    0b11110000, 0b00000000, // Row 0
    0b11110000, 0b00000000, // Row 1
    0b11110000, 0b00000000, // Row 2
    0b11110000, 0b00000000, // Row 3
    0b11110000, 0b00000000, // Row 4
    0b11110000, 0b00000000  // Row 5
};

/* 简单的6x6精灵数据 - 圆形 */
const char circle_sprite[] = {
    0b01100000, 0b00000000, // Row 0
    0b11110000, 0b00000000, // Row 1
    0b11110000, 0b00000000, // Row 2
    0b11110000, 0b00000000, // Row 3
    0b11110000, 0b00000000, // Row 4
    0b01100000, 0b00000000  // Row 5
};

/* 绘制6x6精灵 */
static void draw_sprite(int x, int y, const char *sprite_data)
{
    if (x < -5 || x >= SCREEN_WIDTH || y < -5 || y >= SCREEN_HEIGHT) {
        return;
    }
    
    unsigned int addr = makew(y + 8, x);
    SYS_Sprite6(sprite_data, (void*)addr);
}

/* 绘制彩色条纹 */
static void draw_stripes(void)
{
    int i;
    for (i = 0; i < SCREEN_WIDTH; i += 10) {
        int color = (i / 10) % 8;
        draw_rect(i, 10, 10, 20, color * 0x10);
    }
}

/* 绘制彩色方块网格 */
static void draw_grid(void)
{
    int x, y;
    for (x = 0; x < SCREEN_WIDTH; x += 20) {
        for (y = 40; y < SCREEN_HEIGHT; y += 20) {
            int color = ((x / 20) + (y / 20)) % 8;
            draw_rect(x, y, 18, 18, color * 0x10);
        }
    }
}

/* 绘制随机精灵 */
static void draw_random_sprites(void)
{
    int i;
    for (i = 0; i < 10; i++) {
        int x = rand() % (SCREEN_WIDTH - 6);
        int y = 50 + rand() % 40;
        int sprite_type = rand() % 2;
        int color = rand() % 8;
        
        if (sprite_type == 0) {
            draw_sprite(x, y, square_sprite);
        } else {
            draw_sprite(x, y, circle_sprite);
        }
    }
}

/* 绘制演示场景 */
static void draw_demo_scene(void)
{
    /* 清屏为黑色背景 */
    clear_screen(COLOR_BLACK);
    
    /* 绘制标题区域 */
    draw_rect(0, 0, SCREEN_WIDTH, 8, COLOR_BLUE);
    
    /* 绘制彩色条纹 */
    draw_stripes();
    
    /* 绘制网格 */
    draw_grid();
    
    /* 绘制一些边框 */
    draw_frame(10, 90, 40, 25, COLOR_WHITE);
    draw_frame(60, 90, 40, 25, COLOR_YELLOW);
    draw_frame(110, 90, 40, 25, COLOR_GREEN);
    
    /* 在边框内填充不同颜色 */
    draw_rect(12, 92, 36, 21, COLOR_RED);
    draw_rect(62, 92, 36, 21, COLOR_MAGENTA);
    draw_rect(112, 92, 36, 21, COLOR_CYAN);
    
    /* 绘制随机精灵 */
    draw_random_sprites();
}

/* 动画演示 */
static void animate_demo(void)
{
    int frame = 0;
    int i, j;
    
    while (1) {
        clear_screen(COLOR_BLACK);
        
        /* 绘制移动的彩色条 */
        for (i = 0; i < 8; i++) {
            int x = (i * 20 + frame) % SCREEN_WIDTH;
            draw_rect(x, 20, 15, 80, i * 0x10);
        }
        
        /* 绘制弹跳的精灵 */
        for (i = 0; i < 5; i++) {
            int x = (i * 30 + frame/2) % SCREEN_WIDTH;
            int y = 50 + (int)(10 * sin((frame + i * 20) * 0.1));
            if (i % 2 == 0) {
                draw_sprite(x, y, square_sprite);
            } else {
                draw_sprite(x, y, circle_sprite);
            }
        }
        
        /* 绘制旋转的颜色方块 */
        for (i = 0; i < 4; i++) {
            int cx = SCREEN_WIDTH / 2;
            int cy = SCREEN_HEIGHT / 2;
            int radius = 30;
            int angle = (frame + i * 90) % 360;
            int x = cx + (int)(radius * cos(angle * 3.14159 / 180));
            int y = cy + (int)(radius * sin(angle * 3.14159 / 180));
            draw_rect(x - 3, y - 3, 6, 6, i * 0x10 + 0x08);
        }
        
        frame++;
        
        /* 简单的延迟循环 */
        for (j = 0; j < 500; j++);
        
        /* 循环100帧后重新开始 */
        if (frame >= 100) {
            frame = 0;
        }
    }
}

/* 主函数 */
int main(void)
{
    /* 设置视频模式为160x120位图模式 */
    SYS_SetMode(1);
    
    /* 清除第一行视频内存以确保正确初始化 */
    SYS_SetMemory(256, 0x00, (void*)0x0800);
    
    /* 首先显示静态演示场景 */
    draw_demo_scene();
    
    /* 简单延迟 */
    int i;
    for (i = 0; i < 10000; i++);
    
    /* 然后进入动画演示 */
    animate_demo();
    
    return 0;
}