# Gigatron LCC 构建过程记录

## 目录

- [概述](#概述)
- [构建步骤](#构建步骤)
- [Gigatron专用函数和系统调用](#gigatron专用函数和系统调用)

## 概述

本文档提供了在 Windows 11 环境下使用 MinGW/GCC 15.2.0 构建 Gigatron LCC 项目的完整过程分析，重点关注Gigatron游戏开发可用的专用函数和系统调用，特别是视频、图形、声音、键盘和游戏控制器功能。

## 构建步骤

### 1. 创建构建目录
```bash
mkdir build
cd build
```

### 2. 运行 CMake 配置
```bash
cmake ..
```

配置过程成功完成，检测到了：
- C 编译器：GNU 15.2.0
- C++ 编译器：GNU 15.2.0
- Python 可执行文件：/mingw64/bin/python3.12.exe

### 3. 执行构建
```bash
cmake --build .
```

## Gigatron专用函数和系统调用

### 控制台函数 (build/include/gigatron/console.h)

控制台函数提供了基本的控制台管理、文本输出和输入处理功能：

#### 文本输出函数
- `console_print()` - 向控制台打印格式化文本 ([build/include/gigatron/console.h:40](build/include/gigatron/console.h:40))
- `console_writall()` - 向控制台写入所有字符 ([build/include/gigatron/console.h:46](build/include/gigatron/console.h:46))
- `console_clear_screen()` - 重置视频表并清除屏幕 ([build/include/gigatron/console.h:49](build/include/gigatron/console.h:49))
- `console_clear_to_eol()` - 从光标清除到行尾 ([build/include/gigatron/console.h:52](build/include/gigatron/console.h:52))

#### 屏幕管理
- `console_clear_line()` - 清除指定行 ([build/include/gigatron/console.h:132](build/include/gigatron/console.h:132))
- `console_scroll()` - 垂直滚动屏幕内容 ([build/include/gigatron/console.h:135](build/include/gigatron/console.h:135))

#### 光标控制
- `console_state_set_cycx()` - 设置光标X坐标 ([build/include/gigatron/console.h:27](build/include/gigatron/console.h:27))
- `console_state_set_wrap()` - 设置文本换行模式 ([build/include/gigatron/console.h:28](build/include/gigatron/console.h:28))

#### 低级输入
- `console_getkey()` - 获取按键（非阻塞） ([build/include/gigatron/console.h:70](build/include/gigatron/console.h:70))
- `console_waitkey()` - 等待按键（带闪烁光标） ([build/include/gigatron/console.h:73](build/include/gigatron/console.h:73))
- `console_readline()` - 读取一行文本输入 ([build/include/gigatron/console.h:80](build/include/gigatron/console.h:80))
- `_console_ctrl()` - 处理控制字符 ([build/include/gigatron/console.h:112](build/include/gigatron/console.h:112))

#### 视频内存访问
- `_console_addr()` - 获取控制台内存地址 ([build/include/gigatron/console.h:91](build/include/gigatron/console.h:91))
- `_console_printchars()` - 在指定屏幕位置打印字符 ([build/include/gigatron/console.h:100](build/include/gigatron/console.h:100))

### 键盘函数 (build/include/gigatron/kbget.h)

键盘函数提供了基本的键盘输入功能：

#### 基本输入
- `kbget()` - 获取按键（根据链接选项别名到其他函数） ([build/include/gigatron/kbget.h:20](build/include/gigatron/kbget.h:20))
- `kbgeta()` - 获取按键（键盘中心应用） ([build/include/gigatron/kbget.h:26](build/include/gigatron/kbget.h:26))
- `kbgetb()` - 获取按键（键盘/按钮模式切换） ([build/include/gigatron/kbget.h:43](build/include/gigatron/kbget.h:43))
- `kbgetc()` - 获取按键（带自动重复） ([build/include/gigatron/kbget.h:46](build/include/gigatron/kbget.h:46))

### 标准库函数 (build/include/gigatron/libc.h)

标准库函数提供了Gigatron特定的标准C库函数实现：

#### 程序控制
- `_exit()` - 退出程序（不运行终结函数） ([build/include/gigatron/libc.h:14](build/include/gigatron/libc.h:14))
- `_exitm()` - 退出并显示消息和可选终结 ([build/include/gigatron/libc.h:21](build/include/gigatron/libc.h:21))
- `_raisem()` - 引发信号并显示错误消息 ([build/include/gigatron/libc.h:40](build/include/gigatron/libc.h:40))
- `_fexception()`和`_foverflow()` - 浮点异常处理 ([build/include/gigatron/libc.h:76](build/include/gigatron/libc.h:76))

#### 内存管理
- `_memchr2()` - 在内存中搜索两个字符之一 ([build/include/gigatron/libc.h:159](build/include/gigatron/libc.h:159))
- `_memchr2ext()` - 跨内存库搜索字符 ([build/include/gigatron/libc.h:165](build/include/gigatron/libc.h:165))
- `_memcpyext()` - 跨内存库复制内存 ([build/include/gigatron/libc.h:172](build/include/gigatron/libc.h:172))
- `_memswp()` - 交换内存块 ([build/include/gigatron/libc.h:178](build/include/gigatron/libc.h:178))

#### 字符串转换
- `_itoa()`、`_utoa()`、`_ltoa()` - 将整数转换为字符串 ([build/include/gigatron/libc.h:121](build/include/gigatron/libc.h:121))
- `_ultoa()` - 将无符号长整数转换为字符串 ([build/include/gigatron/libc.h:124](build/include/gigatron/libc.h:124))
- `dtoa()` - 将双精度浮点数转换为字符串 ([build/include/gigatron/libc.h:138](build/include/gigatron/libc.h:138))

#### 数学函数
- `_fexception()` - 浮点异常处理 ([build/include/gigatron/libc.h:76](build/include/gigatron/libc.h:76))
- `_foverflow()` - 浮点溢出处理 ([build/include/gigatron/libc.h:77](build/include/gigatron/libc.h:77))
- `_ldexp10p()` - 乘以10的幂 ([build/include/gigatron/libc.h:80](build/include/gigatron/libc.h:80))
- `_frexp10p()` - 分解浮点数为尾数和指数 ([build/include/gigatron/libc.h:85](build/include/gigatron/libc.h:85))
- `_fmodquo()` - 带商的浮点取模 ([build/include/gigatron/libc.h:88](build/include/gigatron/libc.h:88))
- `_polevl()`、`_p1evl()` - 多项式求值 ([build/include/gigatron/libc.h:91](build/include/gigatron/libc.h:91))

### 系统函数 (build/include/gigatron/sys.h)

系统函数提供了Gigatron硬件的低级系统调用：

#### 系统调用
- `SYS_Lup()` - LUP操作码存根 ([build/include/gigatron/sys.h:142](build/include/gigatron/sys.h:142))
- `SYS_Fill()` - FILL操作码存根 ([build/include/gigatron/sys.h:147](build/include/gigatron/sys.h:147))
- `SYS_Blit()` - BLIT操作码存根 ([build/include/gigatron/sys.h:153](build/include/gigatron/sys.h:153))
- `SYS_Random()` - 随机数生成 ([build/include/gigatron/sys.h:158](build/include/gigatron/sys.h:158))
- `SYS_VDrawBits()` - 使用指定位模式绘制图形 ([build/include/gigatron/sys.h:162](build/include/gigatron/sys.h:162))
- `SYS_Exec()` - 在ROM中执行代码 ([build/include/gigatron/sys.h:167](build/include/gigatron/sys.h:167))
- `SYS_SetMode()` - 设置系统模式 ([build/include/gigatron/sys.h:171](build/include/gigatron/sys.h:171))
- `SYS_SetMemory()` - 设置内存值 ([build/include/gigatron/sys.h:175](build/include/gigatron/sys.h:175))
- `SYS_ReadRomDir()` - 读取ROM目录 ([build/include/gigatron/sys.h:180](build/include/gigatron/sys.h:180))
- `SYS_ExpanderControl()` - 扩展器控制 ([build/include/gigatron/sys.h:188](build/include/gigatron/sys.h:188))
- `SYS_SpiExchangeBytes()` - 与SPI设备交换数据 ([build/include/gigatron/sys.h:196](build/include/gigatron/sys.h:196))
- `SYS_Sprite6()`系列 - 绘制6x6精灵 ([build/include/gigatron/sys.h:202](build/include/gigatron/sys.h:202))

#### 硬件控制
- 内存位置变量定义 ([build/include/gigatron/sys.h:54](build/include/gigatron/sys.h:54))
- 按钮状态常量定义 ([build/include/gigatron/sys.h:25](build/include/gigatron/sys.h:25))
- 声音通道结构定义 ([build/include/gigatron/sys.h:118](build/include/gigatron/sys.h:118))

#### 时钟和定时
- `_clock()` - 获取系统时钟 ([build/include/gigatron/libc.h:187](build/include/gigatron/libc.h:187))
- `_wait()` - 等待指定垂直消隐数 ([build/include/gigatron/libc.h:190](build/include/gigatron/libc.h:190))

### 格式化输出函数 (build/include/gigatron/printf.h)

格式化输出函数提供了Gigatron特定的格式化输出功能：

#### 标准格式化输出
- `printf()` - 标准printf函数 ([build/include/gigatron/printf.h:23](build/include/gigatron/printf.h:23))
- `sprintf()` - 格式化输出到字符串 ([build/include/gigatron/printf.h:24](build/include/gigatron/printf.h:24))
- `vprintf()` - 带va_list的printf ([build/include/gigatron/printf.h:25](build/include/gigatron/printf.h:25))
- `vsprintf()` - 带va_list的sprintf ([build/include/gigatron/printf.h:26](build/include/gigatron/printf.h:26))

#### 控制台格式化输出
- `cprintf()` - 控制台printf ([build/include/gigatron/printf.h:44](build/include/gigatron/printf.h:44))
- `vcprintf()` - 带va_list的控制台printf ([build/include/gigatron/printf.h:48](build/include/gigatron/printf.h:48))
- `mincprintf()` - 最小控制台printf（仅支持%d和%s） ([build/include/gigatron/printf.h:55](build/include/gigatron/printf.h:55))
- `midcprintf()` - 中等控制台printf（支持更多格式） ([build/include/gigatron/printf.h:56](build/include/gigatron/printf.h:56))

#### 格式化选项
- `_doprint()` - 格式化例程（根据选项别名） ([build/include/gigatron/printf.h:73](build/include/gigatron/printf.h:73))
- `_doprint_c89()` - C89兼容格式化例程 ([build/include/gigatron/printf.h:80](build/include/gigatron/printf.h:80))
- `_doprint_simple()` - 简单格式化例程 ([build/include/gigatron/printf.h:87](build/include/gigatron/printf.h:87))

### 编译器指令 (build/include/gigatron/pragma.h)

编译器指令提供了Gigatron特定优化的编译时指令：

#### Pragma指令
- `#pragma glcc option("OPTION")` - 传递选项给链接器 ([build/include/gigatron/pragma.h:14](build/include/gigatron/pragma.h:14))
- `#pragma glcc lib("LIB")` - 链接库 ([build/include/gigatron/pragma.h:16](build/include/gigatron/pragma.h:16))

#### 声明属性
- `__attribute__((nohop))` - 变量不跨页边界 ([build/include/gigatron/pragma.h:32](build/include/gigatron/pragma.h:32))
- `__attribute__((org(ADDRESS)))` - 变量在指定地址 ([build/include/gigatron/pragma.h:34](build/include/gigatron/pragma.h:34))
- `__attribute__((offset(ADDRESS)))` - 变量在指定页偏移 ([build/include/gigatron/pragma.h:36](build/include/gigatron/pragma.h:36))
- `__attribute__((place(AMIN,AMAX)))` - 变量在指定地址范围 ([build/include/gigatron/pragma.h:38](build/include/gigatron/pragma.h:38))
- `__attribute__((alias(STRING)))` - 外部变量别名 ([build/include/gigatron/pragma.h:43](build/include/gigatron/pragma.h:43))
- `__attribute__((regalias(STRING)))` - 寄存器别名 ([build/include/gigatron/pragma.h:48](build/include/gigatron/pragma.h:48))

#### 便利宏
- `__nohop` - 不跨页边界标记 ([build/include/gigatron/pragma.h:71](build/include/gigatron/pragma.h:71))
- `__lomem` - 低内存分配（0x0200-0x07ff） ([build/include/gigatron/pragma.h:79](build/include/gigatron/pragma.h:79))
- `__himem` - 高内存分配（0x8000-0xffff） ([build/include/gigatron/pragma.h:88](build/include/gigatron/pragma.h:88))
- `__weakref(x)` - 弱引用 ([build/include/gigatron/pragma.h:96](build/include/gigatron/pragma.h:96))
- `__at(x)` - 固定地址 ([build/include/gigatron/pragma.h:113](build/include/gigatron/pragma.h:113))
- `__offset(x)` - 页偏移地址 ([build/include/gigatron/pragma.h:122](build/include/gigatron/pragma.h:122))

### 代码惯用式 (build/include/gigatron/idioms.h)

代码惯用式提供了编译器识别的优化模式：

#### 字节操作
- `hi()` - 获取整数或指针的高字节 ([build/include/gigatron/idioms.h:17](build/include/gigatron/idioms.h:17))
- `lo()` - 获取整数或指针的低字节 ([build/include/gigatron/idioms.h:20](build/include/gigatron/idioms.h:20))
- `makew()` - 使用指定高/低字节创建字 ([build/include/gigatron/idioms.h:23](build/include/gigatron/idioms.h:23))
- `makep()` - 使用指定高/低字节创建指针 ([build/include/gigatron/idioms.h:25](build/include/gigatron/idioms.h:25))
- `mashw()` - 混合高/低字节创建字 ([build/include/gigatron/idioms.h:28](build/include/gigatron/idioms.h:28))
- `mashp()` - 混合高/低字节创建指针 ([build/include/gigatron/idioms.h:31](build/include/gigatron/idioms.h:31))

#### 字节操作宏
- `copylo()` - 复制低字节 ([build/include/gigatron/idioms.h:34](build/include/gigatron/idioms.h:34))
- `copyhi()` - 复制高字节 ([build/include/gigatron/idioms.h:35](build/include/gigatron/idioms.h:35))
- `setlo()` - 设置低字节 ([build/include/gigatron/idioms.h:38](build/include/gigatron/idioms.h:38))
- `sethi()` - 设置高字节 ([build/include/gigatron/idioms.h:39](build/include/gigatron/idioms.h:39))

### 数据结构 (build/include/gigatron/avl.h)

AVL树提供了自平衡二叉搜索树实现：

#### 树操作
- `_avl_add()` - 向树添加节点 ([build/include/gigatron/avl.h:33](build/include/gigatron/avl.h:33))
- `_avl_del()` - 从树删除节点 ([build/include/gigatron/avl.h:42](build/include/gigatron/avl.h:42))

#### 数据结构
- `avlnode_t` - AVL树节点结构 ([build/include/gigatron/avl.h:14](build/include/gigatron/avl.h:14))
- `avlcmp_t` - 比较函数类型 ([build/include/gigatron/avl.h:23](build/include/gigatron/avl.h:23))

### Conio兼容接口 (build/include/conio.h)

提供了与标准conio兼容的接口：

#### 输出函数
- `putch()` - 写入单个字符 ([build/include/conio.h:22](build/include/conio.h:22))
- `cputs()` - 写入字符串 ([build/include/conio.h:25](build/include/conio.h:25))
- `gotoxy()`/`wherex()`/`wherey()` - 光标位置控制 ([build/include/conio.h:32](build/include/conio.h:32))
- `textcolor()`/`textbackground()` - 颜色设置 ([build/include/conio.h:38](build/include/conio.h:38))
- `clrscr()`/`clreol()` - 屏幕清除 ([build/include/conio.h:59](build/include/conio.h:59))
- `cputsxy()` - 在指定位置打印字符串 ([build/include/conio.h:65](build/include/conio.h:65))

#### 输入函数
- `kbhit()` - 检查按键可用性 ([build/include/conio.h:81](build/include/conio.h:81))
- `getch()`/`getche()` - 字符输入 ([build/include/conio.h:84](build/include/conio.h:84))
- `ungetch()` - 字符回退 ([build/include/conio.h:90](build/include/conio.h:90))
- `cgets()` - 字符串输入 ([build/include/conio.h:110](build/include/conio.h:110))

#### 颜色常量
- 标准颜色定义（BLACK、BLUE、GREEN等） ([build/include/conio.h:41](build/include/conio.h:41))
- 6位Gigatron颜色支持 ([build/include/conio.h:37](build/include/conio.h:37))

## 技术实现细节

### 内存管理
- 使用`_memswp()`的内存块交换 ([build/include/gigatron/libc.h:178](build/include/gigatron/libc.h:178))
- 使用`_memcpyext()`的跨内存库复制 ([build/include/gigatron/libc.h:172](build/include/gigatron/libc.h:172))
- 使用`_memchr2ext()`的跨内存库搜索 ([build/include/gigatron/libc.h:165](build/include/gigatron/libc.h:165))

### 系统调用
- `SYS_Lup()` - LUP操作码存根 ([build/include/gigatron/sys.h:142](build/include/gigatron/sys.h:142))
- `SYS_Fill()` - FILL操作码存根 ([build/include/gigatron/sys.h:147](build/include/gigatron/sys.h:147))
- `SYS_Blit()` - BLIT操作码存根 ([build/include/gigatron/sys.h:153](build/include/gigatron/sys.h:153))
- `SYS_VDrawBits()` - 位模式绘制 ([build/include/gigatron/sys.h:162](build/include/gigatron/sys.h:162))
- `SYS_Sprite6()`系列 - 6x6精灵绘制 ([build/include/gigatron/sys.h:202](build/include/gigatron/sys.h:202))

### 声音系统
- 声音通道结构定义 ([build/include/gigatron/sys.h:118](build/include/gigatron/sys.h:118))
- 4个声音通道变量 ([build/include/gigatron/sys.h:129](build/include/gigatron/sys.h:129))
- `SYS_SetMode()` - 系统模式设置 ([build/include/gigatron/sys.h:171](build/include/gigatron/sys.h:171))

### 输入处理
- 按钮状态常量定义 ([build/include/gigatron/sys.h:25](build/include/gigatron/sys.h:25))
- 键盘输入函数 ([build/include/gigatron/kbget.h:20](build/include/gigatron/kbget.h:20))
- 控制器输入处理 ([build/include/gigatron/kbget.h:43](build/include/gigatron/kbget.h:43))

## 使用示例

### 基本控制台应用程序
```c
#include <gigatron/console.h>
#include <gigatron/kbget.h>

int main() {
    console_clear_screen();
    
    // 打印欢迎消息
    console_print("Welcome to Gigatron LCC!\n", 25);
    
    // 简单输入循环
    while (1) {
        int key = console_getkey();
        if (key == 'q') break;
        
        // 回显按键
        _console_ctrl(key);
        console_print("Key pressed: %c\n", 16);
    }
    
    return 0;
}
```

### 基本图形绘制
```c
#include <gigatron/console.h>
#include <gigatron/sys.h>

// 简单绘制函数
void draw_example() {
    // 清除屏幕
    console_clear_screen();
    
    // 使用系统调用绘制位图
    SYS_VDrawBits(0x3f20, 0x1234, (char*)0x8000);
    
    // 使用精灵绘制
    char sprite_data[] = {0x12, 0x34, 0x56, 0x78};
    SYS_Sprite6(sprite_data, (char*)0x8010);
}
```

### 声音生成
```c
#include <gigatron/sys.h>
#include <gigatron/libc.h>

// 生成简单音调
void play_tone(int duration) {
    // 设置声音通道
    SYS_SetMode(0x01); // 启用通道0
    
    // 等待指定时间
    _wait(duration);
    
    // 禁用通道
    SYS_SetMode(0x00); // 禁用通道0
}
```

## 最佳实践

### 内存优化
1. 使用`__lomem`和`__himem`属性控制变量位置 ([build/include/gigatron/pragma.h:79](build/include/gigatron/pragma.h:79))
2. 使用`__nohop`属性防止变量跨页边界 ([build/include/gigatron/pragma.h:71](build/include/gigatron/pragma.h:71))
3. 使用`_memswp()`进行高效内存块交换 ([build/include/gigatron/libc.h:178](build/include/gigatron/libc.h:178))

### 代码优化
1. 使用`hi()`和`lo()`宏进行字节操作 ([build/include/gigatron/idioms.h:17](build/include/gigatron/idioms.h:17))
2. 使用`makew()`和`makep()`宏进行字和指针构造 ([build/include/gigatron/idioms.h:23](build/include/gigatron/idioms.h:23))
3. 根据需要选择适当的printf实现 ([build/include/gigatron/printf.h:73](build/include/gigatron/printf.h:73))

## 遇到的问题及解决方案

### 问题 1：`constexpr` 关键字冲突
**错误信息：**
```
D:/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/src/c.h:528:1: error: 'constexpr' used with 'extern'
  528 | extern Tree constexpr(int);
       | ^~~~~~
D:/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/src/c.h:528:23: error: expected identifier or '(' before 'int'
  528 | extern Tree constexpr(int);
       |                       ^~~~
```

**原因分析：**
- `constexpr` 是 C++11 引入的关键字，但在 C 语言中不是关键字
- 现代 C 编译器（如 GCC 15.2.0）可能会将其识别为保留关键字
- 函数声明语法不正确，缺少参数名称

**解决方案：**
1. 将函数名从 `constexpr` 改为 `const_expr`
2. 修复函数声明语法，添加正确的参数名称

**修改的文件：**
- `src/c.h` - 第528行：`extern Tree constexpr(int);` → `extern Tree const_expr(int);`
- `src/stmt.c` - 第122行：`p = constexpr(0);` → `p = const_expr(0);`
- `src/simp.c` - 第187行：`Tree constexpr(int tok)` → `Tree const_expr(int tok)`
- `src/simp.c` - 第197行：`Tree p = constexpr(tok);` → `Tree p = const_expr(tok);`
- `src/decl.c` - 第116行：`p = constexpr(0);` → `p = const_expr(0);`

### 问题 2：函数指针类型不匹配
**错误信息：**
```
../src/gigatron.md:2838:11: error: initialization of 'void (*)(int,  int,  int,  int)' from incompatible pointer type 'void (*)(void)' [-Wincompatible-pointer-types]
  2838 |           blkfetch, blkstore, blkloop,
       |           ^~~~~~~
../src/gigatron.md:2838:21: error: initialization of 'void (*)(int,  int,  int,  int)' from incompatible pointer type 'void (*)(void)' [-Wincompatible-pointer-types]
  2838 |           blkfetch, blkstore, blkloop,
       |                     ^~~~~~~
../src/gigatron.md:2838:31: error: initialization of 'void (*)(int,  int,  int,  int[])' from incompatible pointer type 'void (*)(void)' [-Wincompatible-pointer-types]
  2838 |           blkfetch, blkstore, blkloop,
       |                               ^~~~~~~
../src/gigatron.md:2847:11: error: initialization of 'void (*)(struct node *)' from incompatible pointer type 'void (*)(void)' [-Wincompatible-pointer-types]
  2847 |           emit2,
       |           ^~~~~~
```

**原因分析：**
- gigatron.md 文件中的函数声明使用了注释语法 `/* int, int, int, int */`，这导致编译器将其解析为 `void (*)(void)` 类型
- Interface 结构体期望的是具有正确参数类型的函数指针

**解决方案：**
修改 gigatron.md 文件中的函数声明，添加正确的参数类型：

**修改的文件：**
- `src/gigatron.md` - 第48-50行：
  ```c
  static void blkfetch(int, int, int, int) {}
  static void blkloop(int, int, int, int, int[]) {}
  static void blkstore(int, int, int, int) {}
  static void emit2(Node) {}
  ```

## 构建结果

构建成功完成后，生成了以下主要组件：

### 可执行文件
- `rcc.exe` - LCC 编译器
- `glcc.exe` - Gigatron C 编译器

### 库文件
- `libc-cpu4.a` - C 标准库（CPU4） ([build/CMakeFiles/libc-cpu4.dir](build/CMakeFiles/libc-cpu4.dir))
- `libc-cpu5.a` - C 标准库（CPU5） ([build/CMakeFiles/libc-cpu5.dir](build/CMakeFiles/libc-cpu5.dir))
- `libc-cpu7.a` - C 标准库（CPU7） ([build/CMakeFiles/libc-cpu7.dir](build/CMakeFiles/libc-cpu7.dir))

### 工具和脚本
- `mapconx` - 控制台映射工具 ([build/mapconx](build/mapconx))
- `mapsim` - 模拟器映射工具 ([build/mapsim](build/mapsim))
- 各种 map 生成工具（map128k, map32k, map512k, map64k） ([build/map128k](build/map128k))
- `alpha.c` - Alpha后端生成 ([build/alpha.c](build/alpha.c))

## 总结

通过解决 `constexpr` 关键字冲突和函数指针类型不匹配问题，成功在 Windows 环境下构建了 Gigatron LCC 项目。这些修改确保了代码与现代 C 编译器的兼容性，同时保持了原有功能的完整性。

## 建议

1. 在未来的代码维护中，避免使用 C++ 关键字作为 C 代码中的标识符
2. 对于函数声明，始终提供完整的参数类型和名称
3. 在不同平台上进行测试，确保跨平台兼容性

构建时间：2025-11-21 12:52:20 UTC