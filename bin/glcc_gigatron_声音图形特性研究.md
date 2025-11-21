# glcc支持Gigatron的声音和图形特性研究

## 概述

本文档详细研究了glcc编译器为Gigatron平台提供的声音和图形API，通过分析头文件和示例代码，总结其游戏开发能力。

## 图形特性

### 基本显示能力

#### 屏幕规格
- **分辨率**: 160×120像素 (qqVgaWidth = 160, qqVgaHeigh = 120)
- **颜色系统**: 6位颜色，支持前景色和背景色组合
- **视频内存**: 256行的屏幕缓冲区 (`screenMemory[256][256]`)

### 核心绘图函数

#### 1. SYS_Fill() - 快速矩形填充
```c
extern void SYS_Fill(unsigned int addr, unsigned int color, unsigned int hw);
```
- **功能**: 快速填充矩形区域
- **参数**: 
  - `addr`: 目标地址 (使用makew()构造)
  - `color`: 填充颜色 (6位值)
  - `hw`: 尺寸和高度 (16位值，高8位=高度，低8位=宽度)
- **用途**: 背景清除、UI元素绘制、游戏地图生成

#### 2. SYS_VDrawBits() - 精确位图绘制
```c
extern void SYS_VDrawBits(int fgbg, int bits, char *address);
```
- **功能**: 像素级精确绘图控制
- **参数**:
  - `fgbg`: 前景色和背景色组合
  - `bits`: 8位位图数据
  - `address`: 目标地址
- **用途**: 精细像素操作、自定义字体绘制、精确碰撞检测

#### 3. SYS_Blit() - 块传送功能
```c
extern void* SYS_Blit(unsigned int src, unsigned int dst, unsigned int hw);
```
- **功能**: 内存块快速复制
- **参数**:
  - `src`: 源地址
  - `dst`: 目标地址  
  - `hw`: 传送尺寸和高度
- **用途**: 屏幕滚动、精灵动画、地图加载、视差效果

#### 4. SYS_Sprite6系列 - 硬件精灵绘制

Gigatron提供6×6像素精灵绘制函数系列：

```c
// 基础精灵绘制
extern void* SYS_Sprite6(const void *srcpix, void *dst);

// X轴翻转
extern void* SYS_Sprite6x(const void *srcpix, void *dst);

// Y轴翻转  
extern void* SYS_Sprite6y(const void *srcpix, void *dst);

// XY轴翻转
extern void* SYS_Sprite6xy(const void *srcpix, void *dst);
```

- **精灵规格**: 6×6像素，每像素2位颜色
- **性能**: 硬件加速，适合游戏角色和道具
- **用途**: 
  - 游戏角色动画
  - 道具和物品
  - 粒子效果
  - UI元素

### 高级图形特性

#### 视频模式控制
```c
extern void SYS_SetMode(int mode);
```
- **模式支持**: 
  - 模式0: 标准文本模式
  - 模式1: 160×120位图模式
  - 模式2: 扩展图形模式
  - 模式3: 64色模式

#### 内存扩展支持
- **跨页内存访问**: 通过SYS函数访问64K内存空间
- **扩展卡控制**: 通过SYS_ExpanderControl()和SYS_SpiExchangeBytes()访问硬件扩展

## 声音特性

### 声音通道系统
Gigatron提供4个独立的音频声道，每个声道可独立控制：

#### 声道结构
```c
typedef struct channel_s {
    char wavA, wavX;    // 波形参数
    char keyL, keyH;      // 音高参数（低字节/高字节）
    #if STRUCT_CHANNEL_HAS_KEYW
    word keyW;             // 16位音高参数
    #endif
    #if STRUCT_CHANNEL_HAS_OSC
    char oscL, oscH;      // 振荡器参数
    #endif
};
```

#### 声道控制
- **4个独立声道**: channel1-channel4
- **波形合成**: 每个声道支持2种波形叠加
- **音高控制**: 独立的音高参数，支持音符播放
- **振荡器支持**: 可选的振荡器功能

#### 音符系统
- **音符表**: `notesTable`位于0x0900，包含预定义的音符频率
- **音符播放**: 通过设置keyL/keyH参数播放不同音高

#### 声音控制
- **声音定时器**: `soundTimer`用于精确控制声音播放时长
- **LED反馈**: `ledState`和`ledTempo`提供视觉反馈

## 输入系统

### 控制台支持
```c
// 8个按钮常量定义
enum {
    buttonRight  = 1,   // 右
    buttonLeft   = 2,   // 左
    buttonDown   = 4,   // 下
    buttonUp     = 8,   // 上
    buttonStart  = 16,  // 开始
    buttonSelect = 32,  // 选择
    buttonB      = 64,   // B键
    buttonA      = 128   // A键
};
```

### 键盘输入系统
glcc提供多层次的键盘输入支持：

#### 1. kbgeta() - 简单键盘模式
```c
extern int kbgeta(void);
```
- **功能**: 报告所有与上次不同的按键值
- **适用**: 文本输入、简单控制

#### 2. kbgetb() - 自动按钮/键盘切换
```c
extern int kbgetb(void);
```
- **功能**: 
  - 键盘模式：类似kbgeta()
  - 按钮模式：独立处理每个按钮按下
  - 自动切换：检测到手柄信号后自动切换到按钮模式
- **适用**: 动作游戏、需要同时处理方向和动作

#### 3. kbgetc() - 带自动重复的按钮模式
```c
extern int kbgetc(void);
```
- **功能**: 在kbgetb基础上增加按键自动重复
- **适用**: 需要长按按键的游戏（如射击游戏）

### 内存管理特性

### 内存布局控制
```c
// 防止跨页的变量属性
#define __nohop __attribute__((nohop))

// 低内存区域分配
#define __lomem __attribute__((place(0x0200,0x07ff)))

// 高内存区域分配  
#define __himem __attribute__((place(0x8000,0xffff)))

// 固定地址分配
#define __at(x) __attribute__((org(x)))
```

### 高效内存操作
- **块操作**: SYS_Fill()比逐像素填充快得多
- **位操作**: SYS_VDrawBits()提供硬件级位操作
- **跨页访问**: 通过SYS函数访问扩展内存

## 游戏开发优势

### 1. 完整的C开发环境
- **标准C89支持**: 完整实现ANSI C 1989标准
- **标准库**: printf、stdlib、string等完整C库
- **类型安全**: 强类型系统和内存管理

### 2. 高效的硬件抽象
- **直接硬件访问**: SYS函数直接调用Gigatron硬件功能
- **性能优异**: 硬件加速的图形和声音操作
- **零开销抽象**: 无运行时开销的硬件接口

### 3. 丰富的示例代码
项目包含多个完整的游戏示例：

#### 动作游戏示例
- **飞机大战** (`plane_war_simple.c`): 
  - 使用SYS_Fill()绘制背景和游戏元素
  - 使用kbgetb()处理玩家输入
  - 完整的游戏循环和碰撞检测
  - 演示响应式控制和精灵动画

#### 贪吃蛇游戏** (`snake.c`):
  - 使用console.h实现字符界面
  - 使用kbgeta()处理方向输入
  - 完整的游戏状态管理
  - 演示经典的贪吃蛇游戏机制

#### 图形演示示例
- **位图演示** (`TSTblit.c`): 展示SYS_Blit()的块传送效果
- **填充演示** (`TSTfill.c`): 展示随机颜色填充和性能
- **旋转演示** (`TSTrotator.s`): 高级图形变换效果

## 开发建议

### 游戏制作最佳实践

#### 1. 使用SYS_Fill()进行背景绘制
- **原因**: 比逐像素操作快10-100倍
- **建议**: 游戏背景、UI元素、大地图都使用块填充

#### 2. 合理使用精灵系统
- **6×6精灵**: 适合角色、道具、子弹等游戏对象
- **内存布局**: 将精灵数据放在__nohop区域避免页面跨越
- **动画**: 通过翻转函数实现帧动画

#### 3. 充分利用4声道音频
- **背景音乐**: channel1和2用于环境音效
- **音效**: channel3用于短促音效
- **主旋律**: channel4用于游戏主旋律
- **音符系统**: 使用notesTable实现音阶变化

#### 4. 选择合适的输入模式
- **动作游戏**: 使用kbgetb()获得方向+动作的独立控制
- **射击游戏**: 使用kbgetc()支持连发和自动重复
- **RPG/策略**: 使用kbgeta()处理菜单和移动

#### 5. 性能优化技巧
- **减少SYS_VDrawBits()调用**: 该函数较慢，尽量用块操作替代
- **缓存计算结果**: 避免重复的三角函数和除法运算
- **使用soundTimer**: 实现精确的音效时序控制
- **内存预分配**: 游戏开始时分配所有需要的内存块

## 结论

glcc为Gigatron游戏开发提供了功能完整、性能优异的C开发环境。其图形系统支持从基础的块填充到高级的精灵绘制，声音系统提供4个独立声道的多层次音频合成能力。通过合理利用这些API，开发者可以制作出各种类型的游戏，从简单的动作游戏到复杂的射击游戏和音乐游戏。

特别值得注意的是，glcc的硬件抽象层让开发者能够充分利用Gigatron的独特架构特性，实现零开销的图形和声音操作，这在8位游戏机上是极其宝贵的能力。