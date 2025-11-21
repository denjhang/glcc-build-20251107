# GLCC编译器使用指南

## 概述

GLCC是Gigatron的C语言编译器，用于将C代码编译为Gigatron VCPU可执行的.gt1格式文件。

## 编译hello.c的步骤

### 1. 创建源代码文件

首先创建一个简单的hello.c文件：

```c
#include <stdio.h>

int main() {
    printf("Hello, Gigatron!\n");
    return 0;
}
```

### 2. 使用glcc编译

在命令行中执行以下命令：

```bash
glcc.cmd hello.c
```

这将生成默认的输出文件a.gt1。

### 3. 指定输出文件名

如果想要指定输出文件名，可以使用-o选项：

```bash
glcc.cmd hello.c -o hello.gt1
```

### 4. 查看编译结果

使用gt1dump工具查看编译后的文件内容：

```bash
gt1dump.cmd hello.gt1
```

## 编译过程中遇到的问题

### 文件末尾换行符警告

在编译过程中可能会看到以下警告：

```
cpp: hello.c:6 No newline at end of file
```

这个警告不会影响编译结果，但建议在源文件末尾添加换行符以消除警告。

## GLCC编译器选项

### 基本选项

- `-o GT1FILE`: 指定输出文件名（默认为a.gt1）
- `-h, --help`: 显示帮助信息
- `-V, --version`: 显示版本信息

### 目标平台选项

- `-cpu CPU`: 选择目标vCPU（4, 5, 6, 7）
- `-rom ROM`: 选择目标ROM版本（v4, v5a, v6, dev7等）
- `-map MAP`: 选择链接器映射（32k, 64k, sim等）

### 调试选项

- `--symbols, --syms`: 输出符号列表
- `--fragments, --frags`: 输出内存映射
- `--debug-messages, -d`: 启用调试输出

## 工具链说明

### 主要工具

- `glcc.cmd`: C语言编译器
- `glink.cmd`: 链接器，将目标文件链接为.gt1文件
- `gt1dump.cmd`: 用于查看.gt1文件的内容
- `gtsim.cmd`: Gigatron模拟器
- `gtprof.cmd`: 性能分析工具

### 文件格式

- `.c`: C源代码文件
- `.o/.s`: 编译后的目标文件
- `.a`: 库文件
- `.gt1`: Gigatron可执行文件

## 示例项目结构

```
bin/
├── glcc.cmd          # 编译器
├── glink.cmd         # 链接器
├── gt1dump.cmd       # 查看工具
├── gtsim.cmd         # 模拟器
├── gtprof.cmd        # 性能分析
├── hello.c           # 源代码
├── hello.gt1         # 编译后的可执行文件
└── make.exe          # 构建工具
```

## 运行程序

编译完成后，可以使用Gigatron模拟器运行程序：

```bash
gtsim.cmd hello.gt1
```

## 注意事项

1. 确保源文件末尾有换行符以避免警告
2. GLCC是为Gigatron平台定制的编译器，生成的代码只能在Gigatron VCPU上运行
3. 编译过程中可能会产生一些警告，但通常不影响程序功能
4. 建议使用适当的ROM版本和CPU选项以获得最佳兼容性