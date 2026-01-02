#!/bin/bash
# GTest 安装脚本

echo "=== 安装 Google Test (GTest) ==="

# 检查是否已安装
if [ -f /usr/include/gtest/gtest.h ] || [ -f /usr/local/include/gtest/gtest.h ]; then
    echo "GTest 已安装，检查库文件..."
    
    # 检查库文件
    if [ -f /usr/lib/libgtest.a ] || [ -f /usr/local/lib/libgtest.a ]; then
        echo "✅ GTest 已完整安装"
        exit 0
    fi
fi

echo "开始安装 GTest..."

# 方法1: 使用包管理器安装（推荐）
if command -v apt-get &> /dev/null; then
    echo "使用 apt-get 安装..."
    sudo apt-get update
    sudo apt-get install -y libgtest-dev
    
    # 编译 GTest 库
    echo "编译 GTest 库..."
    cd /usr/src/gtest
    sudo cmake .
    sudo make
    sudo cp lib/*.a /usr/lib
    
    echo "✅ GTest 安装完成"
    exit 0
fi

# 方法2: 从源码编译安装
echo "从源码编译安装 GTest..."
cd /tmp
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install

echo "✅ GTest 安装完成"
echo ""
echo "如果安装失败，请手动检查："
echo "1. GTest 头文件位置: /usr/local/include/gtest/gtest.h"
echo "2. GTest 库文件位置: /usr/local/lib/libgtest.a"
echo "3. 如果路径不同，请修改 Makefile 中的 GTEST_DIR 变量"

