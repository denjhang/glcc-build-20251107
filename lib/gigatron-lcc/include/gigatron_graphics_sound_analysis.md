# Gigatron 图形和声音功能函数详细研究

## 概述

本文档详细分析了Gigatron系统中的图形和声音相关功能函数，基于对gigatron/include目录下所有头文件的深入研究。

## 图形相关功能

### 1. 基本图形常量和定义

#### 显示分辨率
```c
enum {
    qqVgaWidth = 160,  // VGA宽度
    qqVgaHeigh = 120   // VGA高度
};
```
- 定义了Gigatron的基本显示分辨率为160x120像素

#### 视频相关内存位置
```c
extern __near byte videoY;        // 当前视频Y坐标
extern __near byte frameCount;    // 帧计数器
extern byte videoTable[];         // 视频表
extern byte screenMemory[][256];  // 屏幕内存
```

### 2. 图形绘制函数

#### SYS_VDrawBits - 位图绘制
```c
extern void SYS_VDrawBits(int fgbg, int bits, char *address);
#define has_SYS_VDrawBits() 1
```
- **功能**: 在指定地址绘制位图
- **参数**:
  - `fgbg`: 前景色和背景色
  - `bits`: 要绘制的位图数据
  - `address`: 目标地址
- **可用性**: 所有ROM版本都支持

#### SYS_Sprite系列函数 - 精灵绘制
```c
extern void* SYS_Sprite6(const void *srcpix, void *dst);
extern void* SYS_Sprite6x(const void *srcpix, void *dst);
extern void* SYS_Sprite6y(const void *srcpix, void *dst);
extern void* SYS_Sprite6xy(const void *srcpix, void *dst);
#define has_SYS_Sprite6() ((romType & 0xfc) >= romTypeValue_ROMv3)
```
- **功能**: 绘制6x6像素的精灵图形
- **变体**:
  - `SYS_Sprite6`: 基本精灵绘制
  - `SYS_Sprite6x`: X方向扩展精灵
  - `SYS_Sprite6y`: Y方向扩展精灵
  - `SYS_Sprite6xy`: XY方向扩展精灵
- **可用性**: ROMv3及以上版本支持

### 3. 控制台图形功能

#### 控制台状态管理
```c
extern __near struct console_state_s {
    int  fgbg;        // 前景色和背景色
    char cy, cx;      // 光标坐标
    char wrapy, wrapx; // 换行/滚动启用
} console_state;
```

#### 控制台绘制函数
```c
// 打印字符串到控制台
extern int console_print(const char *s, unsigned int len);

// 清除屏幕
extern void console_clear_screen(void);

// 清除到行尾
extern void console_clear_to_eol(void);

// 清除指定行
extern void console_clear_line(int y);

// 滚动行范围
extern void console_scroll(int y1, int y2, int n);
```

#### 低级绘制函数
```c
// 在指定地址绘制字符
extern int _console_printchars(int fgbg, const char *addr, const char *s, int len);

// 清除屏幕区域
extern void _console_clear(char *addr, int clr, int nl);
```

### 4. 视频模式控制

#### SYS_SetMode - 设置视频模式
```c
extern void SYS_SetMode(int);
#define has_SYS_SetMode 1
```
- **功能**: 设置Gigatron的视频模式
- **参数**: 模式参数（具体含义需参考ROM文档）

## 声音相关功能

### 1. 声音通道结构

#### 声音通道定义
```c
typedef struct channel_s {
    char wavA, wavX;           // 波形参数
#if STRUCT_CHANNEL_HAS_KEYW
    word keyW;                 // 音调（字形式）
#else
    char keyL, keyH;           // 音调（高低字节）
#endif
#if STRUCT_CHANNEL_HAS_OSC
    char oscL, oscH;           // 振荡器参数
#endif
} channel_t;
```

#### 声音通道变量
```c
extern channel_t channel1 __at(0x1fa);  // 通道1
extern channel_t channel2 __at(0x2fa);  // 通道2
extern channel_t channel3 __at(0x3fa);  // 通道3
extern channel_t channel4 __at(0x4fa);  // 通道4
extern channel_t *channel(int c);       // 获取通道指针
```

### 2. 声音相关内存位置

#### 声音控制变量
```c
extern __near byte soundTimer;    // 声音定时器
extern __near byte ledState;      // LED状态
extern __near byte ledTempo;      // LED节拍
extern byte soundTable[];          // 声音表
```

#### 音符表
```c
enum {
    notesTable = 0x0900,  // 音符表地址
};
```

### 3. 控制台声音功能

#### 控制台铃声
```c
// 响铃n帧
extern void _console_bell(int n);
```

## 系统功能函数

### 1. 内存操作

#### SYS_SetMemory - 设置内存
```c
extern void SYS_SetMemory(int count, int val, void *addr);
#define has_SYS_SetMemory 1
```
- **功能**: 设置内存区域的值
- **参数**:
  - `count`: 字节数
  - `val`: 要设置的值
  - `addr`: 目标地址

### 2. 系统控制

#### SYS_Exec - 执行ROM代码
```c
extern void SYS_Exec(void *romptr, void *vlr);
#define has_SYS_Exec() 1
```
- **功能**: 执行ROM中的代码
- **注意**: 当参数vlr为(void*)(-1)时返回

#### SYS_Lup - 查找ROM位置
```c
extern int SYS_Lup(unsigned int addr);
#define has_SYS_Lup() 1
```

#### SYS_Random - 随机数生成
```c
extern unsigned int SYS_Random(void);
#define has_SYS_Random() 1
```

## 输入功能

### 1. 按钮定义
```c
enum {
    buttonRight  = 1,   // 右
    buttonLeft   = 2,   // 左
    buttonDown   = 4,   // 下
    buttonUp     = 8,   // 上
    buttonStart  = 16,  // 开始
    buttonSelect = 32,  // 选择
    buttonB      = 64,  // B按钮
    buttonA      = 128  // A按钮
};
```

### 2. 输入状态
```c
extern __near byte buttonState;  // 按钮状态
extern __near byte serialRaw;     // 串行原始数据
```

### 3. 键盘输入函数
```c
extern int kbget(void);    // 主键盘输入函数
extern int kbgeta(void);   // 键盘中心模式
extern int kbgetb(void);   // 按钮模式
extern int kbgetc(void);   // 带自动重复的按钮模式
```

## 内存布局和特殊地址

### 1. ROM版本标识
```c
enum {
    romTypeValue_ROMv1  = 0x1c,
    romTypeValue_ROMv2  = 0x20,
    romTypeValue_ROMv3  = 0x28,
    romTypeValue_ROMv4  = 0x38,
    romTypeValue_ROMv5a = 0x40,
    romTypeValue_ROMv6  = 0x48,
    romTypeValue_DEVROM = 0xf8
};
```

### 2. 特殊内存位置
```c
extern __near byte romType;           // ROM类型
extern __near byte channelMask_v4;    // v4通道掩码
extern __near word sysFn;             // 系统函数
extern __near byte sysArgs0[8];       // 系统函数参数
```

## 使用建议

### 1. 图形编程
- 使用`SYS_VDrawBits`进行基本位图绘制
- 利用`SYS_Sprite6`系列函数绘制精灵图形
- 通过控制台函数实现文本显示
- 注意ROM版本兼容性（特别是精灵函数需要ROMv3+）

### 2. 声音编程
- 通过channel1-4结构体控制4个声音通道
- 设置wavA和wavX参数控制波形
- 使用keyL/keyH或keyW设置音调
- 利用soundTimer进行时序控制

### 3. 兼容性考虑
- 检查ROM版本：`(romType & 0xfc) >= romTypeValue_ROMvX`
- 使用`has_SYS_*`宏检查功能可用性
- 注意不同ROM版本的功能差异

## 总结

Gigatron提供了一套相对完整的图形和声音功能API，包括：

1. **图形功能**：
   - 基本位图绘制
   - 精灵图形支持
   - 控制台文本显示
   - 视频模式控制

2. **声音功能**：
   - 4个独立声音通道
   - 波形和音调控制
   - 系统铃声功能

3. **系统功能**：
   - 内存操作
   - ROM代码执行
   - 随机数生成

这些功能为Gigatron游戏和应用开发提供了坚实的基础，开发者可以根据需要组合使用这些API来创建丰富的多媒体体验。