#!/bin/bash
# GTest 安装脚本（修复版）

echo "=== 安装 Google Test (GTest) ==="

# 检查是否已安装
if [ -f /usr/include/gtest/gtest.h ] || [ -f /usr/local/include/gtest/gtest.h ]; then
    echo "检查 GTest 库文件..."
    if [ -f /usr/lib/libgtest.a ] || [ -f /usr/local/lib/libgtest.a ] || \
       [ -f /usr/lib/x86_64-linux-gnu/libgtest.a ]; then
        echo "✅ GTest 已完整安装"
        exit 0
    fi
fi

echo "开始安装 GTest..."

# 方法1: 使用包管理器安装（推荐，最简单）
if command -v apt-get &> /dev/null; then
    echo "使用 apt-get 安装..."
    sudo apt-get update
    sudo apt-get install -y libgtest-dev
    
    # 尝试编译 GTest 库（使用更新的 CMake 方法）
    echo "编译 GTest 库..."
    
    # 检查库文件是否已经存在
    if [ -f /usr/lib/x86_64-linux-gnu/libgtest.a ]; then
        echo "✅ GTest 库文件已存在于 /usr/lib/x86_64-linux-gnu/"
        echo "创建符号链接..."
        sudo ln -sf /usr/lib/x86_64-linux-gnu/libgtest.a /usr/lib/libgtest.a 2>/dev/null || true
        sudo ln -sf /usr/lib/x86_64-linux-gnu/libgtest_main.a /usr/lib/libgtest_main.a 2>/dev/null || true
        echo "✅ GTest 安装完成"
        exit 0
    fi
    
    # 如果不存在，尝试编译
    if [ -d /usr/src/gtest ]; then
        cd /usr/src/gtest
        # 使用更新的 CMake 方法
        sudo cmake -DCMAKE_BUILD_TYPE=Release .
        sudo make -j$(nproc)
        if [ -f lib/libgtest.a ]; then
            sudo cp lib/libgtest.a /usr/lib/ 2>/dev/null || sudo cp lib/*.a /usr/lib/
            sudo cp lib/libgtest_main.a /usr/lib/ 2>/dev/null || true
            echo "✅ GTest 安装完成"
            exit 0
        fi
    fi
fi

# 方法2: 从源码编译安装（使用更新的方法）
echo "从源码编译安装 GTest..."
cd /tmp
rm -rf googletest
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install

echo "✅ GTest 安装完成"
echo ""
echo "如果仍有问题，请检查："
echo "1. GTest 头文件: find /usr -name 'gtest.h'"
echo "2. GTest 库文件: find /usr -name 'libgtest.a'"
echo "3. 如果找到文件，请更新 Makefile 中的路径"

