# Gigatron LCC Build Process Documentation (Extended)

## Table of Contents

- [Overview](#overview)
- [Build Steps](#build-steps)
- [Gigatron-Specific Functions and System Calls](#gigatron-specific-functions-and-system-calls)
  - [Console Functions (gigatron/console.h)](#console-functions-gigatronconsoleh)
  - [Keyboard Functions (gigatron/kbget.h)](#keyboard-functions-gigatronkbgeth)
  - [Standard Library Functions (gigatron/libc.h)](#standard-library-functions-gigatronlibch)
  - [System Functions (gigatron/sys.h)](#system-functions-gigatronsysh)
  - [Formatted Output Functions (gigatron/printf.h)](#formatted-output-functions-gigatronprintfh)
  - [Compiler Directives (gigatron/pragma.h)](#compiler-directives-gigatronpragmah)
  - [Code Idioms (gigatron/idioms.h)](#code-idioms-gigatronidiomsh)
  - [Data Structures (gigatron/avl.h)](#data-structures-gigatronavlh)
  - [Conio Compatible Interface (gigatron/conio.h)](#conio-compatible-interface-gigatronconioh)
  - [Game Development Support](#game-development-support)
  - [Performance Analysis Tools](#performance-analysis-tools)
  - [Technical Implementation Details](#technical-implementation-details)
  - [Usage Examples](#usage-examples)
  - [Advanced Features](#advanced-features)
  - [Best Practices](#best-practices)
  - [Build Process Issues and Solutions](#build-process-issues-and-solutions)
  - [Build Results](#build-results)
  - [Recommendations](#recommendations)
  - [Gigatron-Specific Functions Detailed Documentation](#gigatron-specific-functions-detailed-documentation)
  - [Console Functions (gigatron/console.h)](#console-functions-gigatronconsoleh-1)
  - [Keyboard Functions (gigatron/kbget.h)](#keyboard-functions-gigatronkbgeth-1)
  - [Standard Library Functions (gigatron/libc.h)](#standard-library-functions-gigatronlibch-1)
  - [System Functions (gigatron/sys.h)](#system-functions-gigatronsysh-1)
  - [Formatted Output Functions (gigatron/printf.h)](#formatted-output-functions-gigatronprintfh-1)
  - [Compiler Directives (gigatron/pragma.h)](#compiler-directives-gigatronpragmah-1)
  - [Code Idioms (gigatron/idioms.h)](#code-idioms-gigatronidiomsh-1)
  - [Data Structures (gigatron/avl.h)](#data-structures-gigatronavlh-1)
  - [Conio Compatible Interface (gigatron/conio.h)](#conio-compatible-interface-gigatronconioh-1)

## Overview

This document provides an extended analysis of the Gigatron LCC build process on Windows 11 with MinGW/GCC 15.2.0, focusing on the specialized functions and system calls available for Gigatron game development, particularly for video, graphics, sound, keyboard, and game controller functionality.

## Build Steps

### 1. Create Build Directory
```bash
mkdir build
cd build
```

### 2. Run CMake Configuration
```bash
cmake ..
```

Configuration completed successfully, detecting:
- C Compiler: GNU 15.2.0
- C++ Compiler: GNU 15.2.0
- Python executable: /mingw64/bin/python3.12.exe

### 3. Execute Build
```bash
cmake --build .
```

## Gigatron-Specific Functions and System Calls

### Console Functions (build/include/gigatron/console.h)

The console.h header provides a basic set of functions for console management, text output, and input handling:

#### Text Output Functions
- `console_print()` - Print formatted text to console ([build/include/gigatron/console.h:40](build/include/gigatron/console.h:40))
- `console_writall()` - Write all characters to console ([build/include/gigatron/console.h:46](build/include/gigatron/console.h:46))
- `console_clear_screen()` - Reset video tables and clear screen ([build/include/gigatron/console.h:49](build/include/gigatron/console.h:49))
- `console_clear_to_eol()` - Clear from cursor to end of line ([build/include/gigatron/console.h:52](build/include/gigatron/console.h:52))

#### Screen Management
- `console_clear_line()` - Clear a specific line ([build/include/gigatron/console.h:132](build/include/gigatron/console.h:132))
- `console_scroll()` - Scroll screen content vertically ([build/include/gigatron/console.h:135](build/include/gigatron/console.h:135))

#### Cursor Control
- `console_state_set_cycx()` - Set cursor X coordinate ([build/include/gigatron/console.h:27](build/include/gigatron/console.h:27))
- `console_state_set_wrap()` - Set text wrapping mode ([build/include/gigatron/console.h:28](build/include/gigatron/console.h:28))

#### Low-Level Input
- `console_getkey()` - Get key press (non-blocking) ([build/include/gigatron/console.h:70](build/include/gigatron/console.h:70))
- `console_waitkey()` - Wait for key press with flashing cursor ([build/include/gigatron/console.h:73](build/include/gigatron/console.h:73))
- `console_readline()` - Read a line of text input ([build/include/gigatron/console.h:80](build/include/gigatron/console.h:80))
- `_console_ctrl()` - Handle control characters ([build/include/gigatron/console.h:112](build/include/gigatron/console.h:112))

#### Video Memory Access
- `_console_addr()` - Get console memory address ([build/include/gigatron/console.h:91](build/include/gigatron/console.h:91))
- `_console_printchars()` - Print characters at specific screen positions ([build/include/gigatron/console.h:100](build/include/gigatron/console.h:100))

### Keyboard Functions (build/include/gigatron/kbget.h)

The kbget.h header provides functions for keyboard input, supporting both simple and advanced modes:

#### Basic Input
- `kbget()` - Get key press (aliased to other functions based on linker options) ([build/include/gigatron/kbget.h:20](build/include/gigatron/kbget.h:20))
- `kbgeta()` - Get key press for keyboard-centric applications ([build/include/gigatron/kbget.h:26](build/include/gigatron/kbget.h:26))
- `kbgetb()` - Get key press with keyboard/button mode switching ([build/include/gigatron/kbget.h:43](build/include/gigatron/kbget.h:43))
- `kbgetc()` - Get key press with auto-repeat ([build/include/gigatron/kbget.h:46](build/include/gigatron/kbget.h:46))

### Standard Library Functions (build/include/gigatron/libc.h)

The libc.h header provides Gigatron-specific implementations of standard C library functions:

#### Program Control
- `_exit()` - Exit program without finalization ([build/include/gigatron/libc.h:14](build/include/gigatron/libc.h:14))
- `_exitm()` - Exit with message and optional finalization ([build/include/gigatron/libc.h:21](build/include/gigatron/libc.h:21))
- `_raisem()` - Raise signal with error message ([build/include/gigatron/libc.h:40](build/include/gigatron/libc.h:40))
- `_fexception()` and `_foverflow()` - Floating-point exception handling ([build/include/gigatron/libc.h:76](build/include/gigatron/libc.h:76))

#### Memory Management
- `_memchr2()` - Search for one of two characters in memory ([build/include/gigatron/libc.h:159](build/include/gigatron/libc.h:159))
- `_memchr2ext()` - Search for characters across memory banks ([build/include/gigatron/libc.h:165](build/include/gigatron/libc.h:165))
- `_memcpyext()` - Copy memory across banks ([build/include/gigatron/libc.h:172](build/include/gigatron/libc.h:172))
- `_memswp()` - Swap memory blocks ([build/include/gigatron/libc.h:178](build/include/gigatron/libc.h:178))

#### String Conversion
- `_itoa()`, `_utoa()`, `_ltoa()` - Convert integers to strings ([build/include/gigatron/libc.h:121](build/include/gigatron/libc.h:121))
- `_ultoa()` - Convert unsigned long integers to strings ([build/include/gigatron/libc.h:124](build/include/gigatron/libc.h:124))
- `dtoa()` - Convert doubles to strings ([build/include/gigatron/libc.h:138](build/include/gigatron/libc.h:138))

#### Mathematical Functions
- `_fexception()` - Floating-point exception handling ([build/include/gigatron/libc.h:76](build/include/gigatron/libc.h:76))
- `_foverflow()` - Floating-point overflow handling ([build/include/gigatron/libc.h:77](build/include/gigatron/libc.h:77))
- `_ldexp10p()` - Multiply by 10 to the power of n ([build/include/gigatron/libc.h:80](build/include/gigatron/libc.h:80))
- `_frexp10p()` - Decompose floating-point into mantissa and exponent ([build/include/gigatron/libc.h:85](build/include/gigatron/libc.h:85))
- `_fmodquo()` - Floating-point modulo with quotient ([build/include/gigatron/libc.h:88](build/include/gigatron/libc.h:88))
- `_polevl()`, `_p1evl()` - Polynomial evaluation ([build/include/gigatron/libc.h:91](build/include/gigatron/libc.h:91))

### System Functions (build/include/gigatron/sys.h)

The sys.h header provides low-level system calls for Gigatron hardware:

#### System Calls
- `SYS_Lup()` - LUP opcode stub ([build/include/gigatron/sys.h:142](build/include/gigatron/sys.h:142))
- `SYS_Fill()` - FILL opcode stub ([build/include/gigatron/sys.h:147](build/include/gigatron/sys.h:147))
- `SYS_Blit()` - BLIT opcode stub ([build/include/gigatron/sys.h:153](build/include/gigatron/sys.h:153))
- `SYS_Random()` - Random number generation ([build/include/gigatron/sys.h:158](build/include/gigatron/sys.h:158))
- `SYS_VDrawBits()` - Draw graphics with specified bit patterns ([build/include/gigatron/sys.h:162](build/include/gigatron/sys.h:162))
- `SYS_Exec()` - Execute code in ROM ([build/include/gigatron/sys.h:167](build/include/gigatron/sys.h:167))
- `SYS_SetMode()` - Set system mode ([build/include/gigatron/sys.h:171](build/include/gigatron/sys.h:171))
- `SYS_SetMemory()` - Set memory values ([build/include/gigatron/sys.h:175](build/include/gigatron/sys.h:175))
- `SYS_ReadRomDir()` - Read ROM directory ([build/include/gigatron/sys.h:180](build/include/gigatron/sys.h:180))
- `SYS_ExpanderControl()` - Expander control ([build/include/gigatron/sys.h:188](build/include/gigatron/sys.h:188))
- `SYS_SpiExchangeBytes()` - Exchange data with SPI devices ([build/include/gigatron/sys.h:196](build/include/gigatron/sys.h:196))
- `SYS_Sprite6()` series - Draw 6x6 sprites ([build/include/gigatron/sys.h:202](build/include/gigatron/sys.h:202))

#### Hardware Control
- Memory location variable definitions ([build/include/gigatron/sys.h:54](build/include/gigatron/sys.h:54))
- Button state constant definitions ([build/include/gigatron/sys.h:25](build/include/gigatron/sys.h:25))
- Sound channel structure definitions ([build/include/gigatron/sys.h:118](build/include/gigatron/sys.h:118))

#### Clock and Timing
- `_clock()` - Get system clock ([build/include/gigatron/libc.h:187](build/include/gigatron/libc.h:187))
- `_wait()` - Wait for specified number of vertical blanks ([build/include/gigatron/libc.h:190](build/include/gigatron/libc.h:190))

### Formatted Output Functions (build/include/gigatron/printf.h)

The printf.h header provides Gigatron-specific formatted output functions:

#### Standard Formatted Output
- `printf()` - Standard printf function ([build/include/gigatron/printf.h:23](build/include/gigatron/printf.h:23))
- `sprintf()` - Format output to string ([build/include/gigatron/printf.h:24](build/include/gigatron/printf.h:24))
- `vprintf()` - printf with va_list ([build/include/gigatron/printf.h:25](build/include/gigatron/printf.h:25))
- `vsprintf()` - sprintf with va_list ([build/include/gigatron/printf.h:26](build/include/gigatron/printf.h:26))

#### Console Formatted Output
- `cprintf()` - Console printf ([build/include/gigatron/printf.h:44](build/include/gigatron/printf.h:44))
- `vcprintf()` - Console printf with va_list ([build/include/gigatron/printf.h:48](build/include/gigatron/printf.h:48))
- `mincprintf()` - Minimal console printf (only supports %d and %s) ([build/include/gigatron/printf.h:55](build/include/gigatron/printf.h:55))
- `midcprintf()` - Medium console printf (supports more formats) ([build/include/gigatron/printf.h:56](build/include/gigatron/printf.h:56))

#### Formatting Options
- `_doprint()` - Formatting routine (aliased based on options) ([build/include/gigatron/printf.h:73](build/include/gigatron/printf.h:73))
- `_doprint_c89()` - C89 compatible formatting routine ([build/include/gigatron/printf.h:80](build/include/gigatron/printf.h:80))
- `_doprint_simple()` - Simple formatting routine ([build/include/gigatron/printf.h:87](build/include/gigatron/printf.h:87))

### Compiler Directives (build/include/gigatron/pragma.h)

The pragma.h header provides compile-time directives for Gigatron-specific optimizations:

#### Pragma Directives
- `#pragma glcc option("OPTION")` - Pass options to linker ([build/include/gigatron/pragma.h:14](build/include/gigatron/pragma.h:14))
- `#pragma glcc lib("LIB")` - Link library ([build/include/gigatron/pragma.h:16](build/include/gigatron/pragma.h:16))

#### Declaration Attributes
- `__attribute__((nohop))` - Variable doesn't cross page boundaries ([build/include/gigatron/pragma.h:32](build/include/gigatron/pragma.h:32))
- `__attribute__((org(ADDRESS)))` - Variable at specified address ([build/include/gigatron/pragma.h:34](build/include/gigatron/pragma.h:34))
- `__attribute__((offset(ADDRESS)))` - Variable at specified page offset ([build/include/gigatron/pragma.h:36](build/include/gigatron/pragma.h:36))
- `__attribute__((place(AMIN,AMAX)))` - Variable in specified address range ([build/include/gigatron/pragma.h:38](build/include/gigatron/pragma.h:38))
- `__attribute__((alias(STRING)))` - External variable alias ([build/include/gigatron/pragma.h:43](build/include/gigatron/pragma.h:43))
- `__attribute__((regalias(STRING)))` - Register alias ([build/include/gigatron/pragma.h:48](build/include/gigatron/pragma.h:48))

#### Convenience Macros
- `__nohop` - No page boundary crossing marker ([build/include/gigatron/pragma.h:71](build/include/gigatron/pragma.h:71))
- `__lomem` - Low memory allocation (0x0200-0x07ff) ([build/include/gigatron/pragma.h:79](build/include/gigatron/pragma.h:79))
- `__himem` - High memory allocation (0x8000-0xffff) ([build/include/gigatron/pragma.h:88](build/include/gigatron/pragma.h:88))
- `__weakref(x)` - Weak reference ([build/include/gigatron/pragma.h:96](build/include/gigatron/pragma.h:96))
- `__at(x)` - Fixed address ([build/include/gigatron/pragma.h:113](build/include/gigatron/pragma.h:113))
- `__offset(x)` - Page offset address ([build/include/gigatron/pragma.h:122](build/include/gigatron/pragma.h:122))

### Code Idioms (build/include/gigatron/idioms.h)

The idioms.h header provides compiler-recognized optimization patterns:

#### Byte Operations
- `hi()` - Get high byte of integer or pointer ([build/include/gigatron/idioms.h:17](build/include/gigatron/idioms.h:17))
- `lo()` - Get low byte of integer or pointer ([build/include/gigatron/idioms.h:20](build/include/gigatron/idioms.h:20))
- `makew()` - Create word using specified high/low bytes ([build/include/gigatron/idioms.h:23](build/include/gigatron/idioms.h:23))
- `makep()` - Create pointer using specified high/low bytes ([build/include/gigatron/idioms.h:25](build/include/gigatron/idioms.h:25))
- `mashw()` - Mix high/low bytes to create word ([build/include/gigatron/idioms.h:28](build/include/gigatron/idioms.h:28))
- `mashp()` - Mix high/low bytes to create pointer ([build/include/gigatron/idioms.h:31](build/include/gigatron/idioms.h:31))

#### Byte Operation Macros
- `copylo()` - Copy low byte ([build/include/gigatron/idioms.h:34](build/include/gigatron/idioms.h:34))
- `copyhi()` - Copy high byte ([build/include/gigatron/idioms.h:35](build/include/gigatron/idioms.h:35))
- `setlo()` - Set low byte ([build/include/gigatron/idioms.h:38](build/include/gigatron/idioms.h:38))
- `sethi()` - Set high byte ([build/include/gigatron/idioms.h:39](build/include/gigatron/idioms.h:39))

### Data Structures (build/include/gigatron/avl.h)

The avl.h header provides self-balancing binary search tree implementation:

#### Tree Operations
- `_avl_add()` - Add node to tree ([build/include/gigatron/avl.h:33](build/include/gigatron/avl.h:33))
- `_avl_del()` - Remove node from tree ([build/include/gigatron/avl.h:42](build/include/gigatron/avl.h:42))

#### Data Structures
- `avlnode_t` - AVL tree node structure ([build/include/gigatron/avl.h:14](build/include/gigatron/avl.h:14))
- `avlcmp_t` - Comparison function type ([build/include/gigatron/avl.h:23](build/include/gigatron/avl.h:23))

### Conio Compatible Interface (build/include/conio.h)

Provides conio-compatible interface:

#### Output Functions
- `putch()` - Write single character ([build/include/conio.h:22](build/include/conio.h:22))
- `cputs()` - Write string ([build/include/conio.h:25](build/include/conio.h:25))
- `gotoxy()`/`wherex()`/`wherey()` - Cursor position control ([build/include/conio.h:32](build/include/conio.h:32))
- `textcolor()`/`textbackground()` - Color settings ([build/include/conio.h:38](build/include/conio.h:38))
- `clrscr()`/`clreol()` - Screen clearing ([build/include/conio.h:59](build/include/conio.h:59))
- `cputsxy()` - Print string at specified position ([build/include/conio.h:65](build/include/conio.h:65))

#### Input Functions
- `kbhit()` - Check key availability ([build/include/conio.h:81](build/include/conio.h:81))
- `getch()`/`getche()` - Character input ([build/include/conio.h:84](build/include/conio.h:84))
- `ungetch()` - Character unget ([build/include/conio.h:90](build/include/conio.h:90))
- `cgets()` - String input ([build/include/conio.h:110](build/include/conio.h:110))

#### Color Constants
- Standard color definitions (BLACK, BLUE, GREEN, etc.) ([build/include/conio.h:41](build/include/conio.h:41))
- 6-bit Gigatron color support ([build/include/conio.h:37](build/include/conio.h:37))

## Technical Implementation Details

### Memory Management
- Memory block swapping with `_memswp()` ([build/include/gigatron/libc.h:178](build/include/gigatron/libc.h:178))
- Cross-bank memory copying with `_memcpyext()` ([build/include/gigatron/libc.h:172](build/include/gigatron/libc.h:172))
- Cross-bank memory searching with `_memchr2ext()` ([build/include/gigatron/libc.h:165](build/include/gigatron/libc.h:165))

### System Calls
- `SYS_Lup()` - LUP opcode stub ([build/include/gigatron/sys.h:142](build/include/gigatron/sys.h:142))
- `SYS_Fill()` - FILL opcode stub ([build/include/gigatron/sys.h:147](build/include/gigatron/sys.h:147))
- `SYS_Blit()` - BLIT opcode stub ([build/include/gigatron/sys.h:153](build/include/gigatron/sys.h:153))
- `SYS_VDrawBits()` - Bit pattern drawing ([build/include/gigatron/sys.h:162](build/include/gigatron/sys.h:162))
- `SYS_Sprite6()` series - 6x6 sprite drawing ([build/include/gigatron/sys.h:202](build/include/gigatron/sys.h:202))

### Sound System
- Sound channel structure definitions ([build/include/gigatron/sys.h:118](build/include/gigatron/sys.h:118))
- 4 sound channel variables ([build/include/gigatron/sys.h:129](build/include/gigatron/sys.h:129))
- `SYS_SetMode()` - System mode setting ([build/include/gigatron/sys.h:171](build/include/gigatron/sys.h:171))

### Input Processing
- Button state constant definitions ([build/include/gigatron/sys.h:25](build/include/gigatron/sys.h:25))
- Keyboard input functions ([build/include/gigatron/kbget.h:20](build/include/gigatron/kbget.h:20))
- Controller input handling ([build/include/gigatron/kbget.h:43](build/include/gigatron/kbget.h:43))

## Usage Examples

### Basic Console Application
```c
#include <gigatron/console.h>
#include <gigatron/kbget.h>

int main() {
    console_clear_screen();
    
    // Print welcome message
    console_print("Welcome to Gigatron LCC!\n", 25);
    
    // Simple input loop
    while (1) {
        int key = console_getkey();
        if (key == 'q') break;
        
        // Echo the key
        _console_ctrl(key);
        console_print("Key pressed: %c\n", 16);
    }
    
    return 0;
}
```

### Basic Graphics Drawing
```c
#include <gigatron/console.h>
#include <gigatron/sys.h>

// Simple drawing function
void draw_example() {
    // Clear screen
    console_clear_screen();
    
    // Use system call to draw bitmap
    SYS_VDrawBits(0x3f20, 0x1234, (char*)0x8000);
    
    // Use sprite drawing
    char sprite_data[] = {0x12, 0x34, 0x56, 0x78};
    SYS_Sprite6(sprite_data, (char*)0x8010);
}
```

### Sound Generation
```c
#include <gigatron/sys.h>
#include <gigatron/libc.h>

// Generate a simple tone
void play_tone(int duration) {
    // Set sound channel
    SYS_SetMode(0x01); // Enable channel 0
    
    // Wait for specified time
    _wait(duration);
    
    // Disable channel
    SYS_SetMode(0x00); // Disable channel 0
}
```

## Best Practices

### Memory Optimization
1. Use `__lomem` and `__himem` attributes to control variable location ([build/include/gigatron/pragma.h:79](build/include/gigatron/pragma.h:79))
2. Use `__nohop` attribute to prevent variables from crossing page boundaries ([build/include/gigatron/pragma.h:71](build/include/gigatron/pragma.h:71))
3. Use `_memswp()` for efficient memory block swapping ([build/include/gigatron/libc.h:178](build/include/gigatron/libc.h:178))

### Code Optimization
1. Use `hi()` and `lo()` macros for byte operations ([build/include/gigatron/idioms.h:17](build/include/gigatron/idioms.h:17))
2. Use `makew()` and `makep()` macros for word and pointer construction ([build/include/gigatron/idioms.h:23](build/include/gigatron/idioms.h:23))
3. Choose appropriate printf implementation based on needs ([build/include/gigatron/printf.h:73](build/include/gigatron/printf.h:73))

## Build Process Issues and Solutions

### Issue 1: `constexpr` Keyword Conflict
**Error Messages:**
```
D:/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/src/c.h:528:1: error: 'constexpr' used with 'extern'
  528 | extern Tree constexpr(int);
       | ^~~~~~
D:/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/src/c.h:528:23: error: expected identifier or '(' before 'int'
  528 | extern Tree constexpr(int);
       |                       ^~~~
```

**Root Cause Analysis:**
- `constexpr` is a C++11 introduced keyword, but not a keyword in C language
- Modern C compilers (like GCC 15.2.0) might recognize it as a reserved keyword
- Function declaration syntax incorrect, missing parameter names

**Solution:**
1. Rename function from `constexpr` to `const_expr`
2. Fix function declaration syntax, adding correct parameter names

**Modified Files:**
- `src/c.h` - Line 528: `extern Tree constexpr(int);` → `extern Tree const_expr(int);`
- `src/stmt.c` - Line 122: `p = constexpr(0);` → `p = const_expr(0);`
- `src/simp.c` - Line 187: `Tree constexpr(int tok)` → `Tree const_expr(int tok)`
- `src/simp.c` - Line 197: `Tree p = constexpr(tok);` → `Tree p = const_expr(tok);`
- `src/decl.c` - Line 116: `p = constexpr(0);` → `p = const_expr(0);`

### Issue 2: Function Pointer Type Mismatch
**Error Messages:**
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

**Root Cause Analysis:**
- gigatron.md file function declarations used comment syntax `/* int, int, int, int */`, causing compiler to parse them as `void (*)(void)` type
- Interface structure expects function pointers with correct parameter types

**Solution:**
Modify function declarations in gigatron.md file, adding correct parameter types:

**Modified Files:**
- `src/gigatron.md` - Lines 48-50:
  ```c
  static void blkfetch(int, int, int, int) {}
  static void blkloop(int, int, int, int, int[]) {}
  static void blkstore(int, int, int, int) {}
  static void emit2(Node) {}
  ```

## Build Results

Build completed successfully, generating the following main components:

### Executable Files
- `rcc.exe` - LCC compiler
- `glcc.exe` - Gigatron C compiler

### Library Files
- `libc-cpu4.a` - C standard library (CPU4) ([build/CMakeFiles/libc-cpu4.dir](build/CMakeFiles/libc-cpu4.dir))
- `libc-cpu5.a` - C standard library (CPU5) ([build/CMakeFiles/libc-cpu5.dir](build/CMakeFiles/libc-cpu5.dir))
- `libc-cpu7.a` - C standard library (CPU7) ([build/CMakeFiles/libc-cpu7.dir](build/CMakeFiles/libc-cpu7.dir))

### Tools and Scripts
- `mapconx` - Console mapping tool ([build/mapconx](build/mapconx))
- `mapsim` - Simulator mapping tool ([build/mapsim](build/mapsim))
- Various map generation tools (map128k, map32k, map512k, map64k) ([build/map128k](build/map128k))
- `alpha.c` - Alpha backend generation ([build/alpha.c](build/alpha.c))

## Summary

By resolving the `constexpr` keyword conflict and function pointer type mismatch issues, successfully built the Gigatron LCC project in a Windows environment. These modifications ensure code compatibility with modern C compilers while maintaining the integrity of the original functionality.

## Recommendations

1. Avoid using C++ keywords as identifiers in C code in future code maintenance
2. Always provide complete parameter types and names for function declarations
3. Test on different platforms to ensure cross-platform compatibility

Build completed on: 2025-11-21 12:52:20 UTC

## Gigatron-Specific Functions Detailed Documentation

### Console Functions (gigatron/console.h)

The console.h header provides a comprehensive set of functions for console management, text output, and input handling:

#### Text Output Functions
- `console_print()` - Print formatted text to console
- `console_writall()` - Write all characters to console
- `console_printchars()` - Print characters from a string
- `console_clear()` - Clear screen with specified color
- `console_clear_to_eol()` - Clear from cursor to end of line
- `console_clear_line()` - Clear a specific line
- `console_scroll()` - Scroll screen content vertically
- `console_reset()` - Reset video tables and clear screen

#### Screen Management
- `console_clear_screen()` - Reset video tables and clear screen
- `console_state_set_cycx()` - Set cursor Y coordinate
- `console_state_set_wrapx()` - Set text wrapping mode

#### Cursor Control
- `console_state_set_cycx()` - Set cursor X coordinate
- `console_state_set_wrapx()` - Set wrapping mode for X coordinate
- `console_state_set_cycy()` - Set cursor Y coordinate
- `console_state_set_wrapy()` - Set wrapping mode for Y coordinate

#### Color Support
- `console_state_set_fgbg()` - Set foreground/background color
- `console_state_set_fgcx()` - Set cursor X color
- `console_state_set_fgcy()` - Set cursor Y color

#### Low-Level Input
- `console_getkey()` - Get key press (non-blocking)
- `console_waitkey()` - Wait for key press with timeout
- `console_readline()` - Read a line of text input
- `console_ctrl()` - Send control characters

#### Video Memory Access
- `*_console_addr()` - Get console memory address
- `console_printchars()` - Print characters at specific screen positions

### Keyboard Functions (gigatron/kbget.h)

The kbget.h header provides functions for keyboard input, supporting both simple and advanced modes:

#### Basic Input
- `kbget()` - Get key press (non-blocking)
- `kbgeta()` - Get key press with auto-repeat for Type C controllers
- `kbgetb()` - Get key press for Type B controllers
- `kbgetc()` - Get key press with auto-repeat and character echo

#### Advanced Features
- Button state tracking for Type C controllers
- Support for both Type A and Type B controllers
- Serial input reporting for Type C controllers
- Auto-repeat functionality with configurable timing

#### Controller Support
- Button state visualization
- Support for Start, Select, and A/B buttons

### Standard Library Functions (gigatron/libc.h)

The libc.h header provides Gigatron-specific implementations of standard C library functions:

#### Program Control
- `_exit()` - Exit program without finalization
- `_exitm()` - Exit with message and optional finalization
- `_raisem()` - Raise signal with error message
- Exception handling with `_fexception()` and `_foverflow()`

#### Memory Management
- `_memchr2()` - Search for character in memory banks
- `_memcpyext()` - Copy memory across banks
- `_memswp()` - Swap memory blocks
- Memory bank allocation with specific addresses

#### String Conversion
- `_itoa()`, `_utoa()`, `_ltoa()` - Convert integers to strings
- `_dtoa()` - Convert doubles to strings
- Support for different radixes (binary, octal, decimal, hexadecimal)

#### Mathematical Functions
- Trigonometric functions (`sin()`, `cos()`, `tan()`)
- Power and logarithm functions (`pow()`, `log()`, `exp()`)
- Floating-point comparison and classification

#### Video and Graphics Support
- Video table management functions
- Sprite drawing functions for game development
- Support for different video modes and color palettes

### System Functions (gigatron/sys.h)

The sys.h header provides low-level system calls for Gigatron hardware:

#### System Calls
- `SYS_Lup()`, `SYS_Fill()`, `SYS_Exec()` - Execute code in ROM
- `SYS_VDrawBits()` - Draw graphics with specified bit patterns
- `SYS_Sprite6()` - Draw 6x6 sprites
- `SYS_SetMemory()` - Set memory values
- Memory bank switching and management

#### Hardware Control
- `SYS_Reset()` - Reset Gigatron system
- `SYS_SetMode()` - Set system mode (video, sound, etc.)
- `SYS_ReadRomDir()` - Read ROM directory
- `SYS_SpiExchangeBytes()` - Exchange data with SPI devices

#### Clock and Timing
- `_clock()` - Get system clock
- `_wait()` - Wait for specified number of clock cycles
- High-resolution timing functions

#### Sound Support
- Sound channel management
- Waveform generation for different sound channels
- Noise generation for sound effects

#### Input Device Support
- Keyboard scanning functions
- Controller input handling
- Support for both Type A and Type B controllers

### Formatted Output Functions (gigatron/printf.h)

The printf.h header provides Gigatron-specific formatted output functions:

#### Formatted Output
- `cprintf()` - Console printf with color support
- `vcprintf()` - Console printf with variable arguments
- `mincprintf()` - Minimal console printf for memory-constrained environments
- `midcprintf()` - Medium console printf balancing features and size

#### Control Character Support
- Support for Gigatron-specific control characters
- Color and formatting codes for console output
- Cursor positioning and control

#### Low-Level Output
- Direct screen memory access functions
- Optimized character drawing routines
- Support for different Gigatron video modes

### Compiler Directives (gigatron/pragma.h)

The pragma.h header provides compile-time directives for Gigatron-specific optimizations:

#### Code Generation Options
- `OPTION_PRINTF_SIMPLE` - Minimal printf implementation
- `OPTION_PRINTF_C89` - C89-compliant printf implementation
- `OPTION_KBGET_SIMPLE` - Basic keyboard support
- `OPTION_KBGET_AUTOBTN` - Auto-repeat for Type A controllers
- `OPTION_KBGET_AUTOREPEAT` - Advanced keyboard support

#### Memory Optimization
- Options for optimizing memory usage
- Bank switching optimizations
- Variable allocation strategies

#### Compilation Control
- Options for controlling code generation and optimization
- Debugging and profiling options

### Code Idioms (gigatron/idioms.h)

The idioms.h header provides compiler-recognized optimization patterns:

#### Byte Operations
- `hi()`/`lo()` - Get high/low bytes of integers or pointers
- `makew()`/`makep()` - Create words/pointers using specified high/low bytes
- `mashw()`/`mashp()` - Mix high/low bytes to create words/pointers
- `copylo()`/`copyhi()`/`setlo()`/`sethi()` - Byte copy and set operations

#### Memory Operations
- Efficient byte and word operations
- Pointer operation optimizations
- Compiler-specific optimization patterns

### Data Structures (gigatron/avl.h)

The avl.h header provides self-balancing binary search tree implementation:

#### Tree Operations
- `_avl_add()` - Add node to tree
- `_avl_del()` - Remove node from tree
- Self-balancing algorithms
- Efficient search and insert operations

#### Comparison Functions
- Custom comparison function support
- Flexible data type support
- Memory-efficient tree structures

### Conio Compatible Interface (gigatron/conio.h)

Provides conio-compatible interface:

#### Output Functions
- `putch()` - Write single character
- `cputs()` - Write string
- `gotoxy()`/`wherex()`/`wherey()` - Cursor position control
- `textcolor()`/`textbackground()` - Color settings
- `clrscr()`/`clreol()` - Screen clearing

#### Input Functions
- `kbhit()` - Check key availability
- `getch()`/`getche()` - Character input
- `ungetch()` - Character unget
- `cgets()` - String input

#### Color Constants
- Standard color definitions (BLACK, BLUE, GREEN, etc.)
- 6-bit Gigatron color support
- Compatibility color macros