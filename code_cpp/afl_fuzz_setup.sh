#!/bin/bash
# AFL++ 模糊测试设置脚本

echo "=== AFL++ 模糊测试设置 ==="

# 检查afl++是否安装
if ! command -v afl-fuzz &> /dev/null; then
    echo "错误: 未找到afl-fuzz命令"
    echo "请先安装AFL++:"
    echo "  Ubuntu/Debian: sudo apt-get install afl++"
    echo "  或从源码编译: https://github.com/AFLplusplus/AFLplusplus"
    exit 1
fi

echo "1. 编译模糊测试目标程序（使用afl-g++）"
mkdir -p fuzz_bin fuzz_input fuzz_output

# 使用afl-g++编译，启用代码插桩
# 排除main.cpp，因为fuzz_test_target有自己的main函数
afl-g++ -std=c++17 -O2 -I./src -o fuzz_bin/fuzz_target \
    fuzz_test_target.cpp \
    src/Category.cpp src/MainUI.cpp src/Record.cpp src/Search.cpp \
    src/Statistics.cpp src/Storage.cpp src/User.cpp

if [ $? -ne 0 ]; then
    echo "编译失败！"
    exit 1
fi

echo "2. 创建测试输入目录"
mkdir -p fuzz_input

# 创建一些基本的测试用例
echo "r1	2025-01-01	100.0	I	工资	测试" > fuzz_input/test1.txt
echo "r2	2025-01-02	50.5	E	餐饮	午餐" > fuzz_input/test2.txt
echo "r3	2025-01-03	200.0	I	奖金	年终奖" > fuzz_input/test3.txt

echo "3. 设置模糊测试环境"
echo "core" | sudo tee /proc/sys/kernel/core_pattern > /dev/null 2>&1 || echo "警告: 无法设置core_pattern（可能需要root权限）"

echo ""
echo "=== 设置完成 ==="
echo ""
echo "运行模糊测试:"
echo "  afl-fuzz -i fuzz_input -o fuzz_output -- ./fuzz_bin/fuzz_target"
echo ""
echo "查看结果:"
echo "  ls fuzz_output/default/crashes/"
echo "  复现崩溃: ./fuzz_bin/fuzz_target < fuzz_output/default/crashes/id:000000*"

