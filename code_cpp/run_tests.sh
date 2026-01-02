#!/bin/bash
# 一键运行单元测试脚本

cd "$(dirname "$0")"

echo "=== 运行单元测试 ==="
echo ""

# 检查 GTest
if [ ! -f /usr/include/gtest/gtest.h ] && [ ! -f /usr/local/include/gtest/gtest.h ]; then
    echo "❌ GTest 未安装，正在安装..."
    sudo apt-get install -y libgtest-dev
    if [ -f /usr/lib/x86_64-linux-gnu/libgtest.a ]; then
        echo "创建符号链接..."
        sudo ln -sf /usr/lib/x86_64-linux-gnu/libgtest.a /usr/lib/libgtest.a
        sudo ln -sf /usr/lib/x86_64-linux-gnu/libgtest_main.a /usr/lib/libgtest_main.a
    fi
fi

# 清理并编译
echo "清理旧文件..."
make clean

# 运行 Storage 测试
echo ""
echo "=== 运行 Storage 模块测试 ==="
if make test-storage 2>&1; then
    echo "✅ Storage 测试完成"
else
    echo "⚠️  Makefile 失败，尝试手动编译..."
    mkdir -p bin
    g++ -std=c++17 -O2 -I./src -I./tests -I/usr/include \
        -o bin/test_storage_gtest.exe \
        tests/test_storage_gtest.cpp src/*.cpp \
        -L/usr/lib/x86_64-linux-gnu -L/usr/lib -lgtest -lgtest_main -lpthread 2>&1
    if [ -f bin/test_storage_gtest.exe ]; then
        ./bin/test_storage_gtest.exe
    fi
fi

# 运行 Search 测试
echo ""
echo "=== 运行 Search 模块测试 ==="
if make test-search 2>&1; then
    echo "✅ Search 测试完成"
else
    echo "⚠️  Makefile 失败，尝试手动编译..."
    mkdir -p bin
    g++ -std=c++17 -O2 -I./src -I./tests -I/usr/include \
        -o bin/test_search_gtest.exe \
        tests/test_search_gtest.cpp src/*.cpp \
        -L/usr/lib/x86_64-linux-gnu -L/usr/lib -lgtest -lgtest_main -lpthread 2>&1
    if [ -f bin/test_search_gtest.exe ]; then
        ./bin/test_search_gtest.exe
    fi
fi

echo ""
echo "=== 测试完成 ==="

