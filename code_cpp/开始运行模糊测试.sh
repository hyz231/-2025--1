#!/bin/bash
# 开始运行模糊测试的脚本

cd "$(dirname "$0")"

echo "=== 开始运行模糊测试 ==="
echo ""

# 检查是否已设置
if [ ! -f fuzz_bin/fuzz_target ]; then
    echo "错误: 模糊测试目标程序未编译"
    echo "请先运行: ./afl_fuzz_setup.sh"
    exit 1
fi

# 检查输入目录
if [ ! -d fuzz_input ] || [ -z "$(ls -A fuzz_input)" ]; then
    echo "错误: 输入目录为空"
    echo "请先运行: ./afl_fuzz_setup.sh"
    exit 1
fi

echo "模糊测试目标程序: ./fuzz_bin/fuzz_target"
echo "输入目录: fuzz_input"
echo "输出目录: fuzz_output"
echo ""

# 设置core pattern
echo "设置core pattern..."
echo core | sudo tee /proc/sys/kernel/core_pattern > /dev/null 2>&1 || {
    echo "警告: 无法设置core_pattern（可能需要root权限）"
}

echo ""
echo "选择运行方式:"
echo "1. 前台运行（可以看到实时输出，按Ctrl+C停止）"
echo "2. 后台运行（使用nohup，输出到fuzz.log）"
echo "3. 使用screen运行（推荐，可以随时查看和分离）"
echo ""
read -p "请选择 (1/2/3): " choice

case $choice in
    1)
        echo ""
        echo "开始运行模糊测试（前台）..."
        echo "按 Ctrl+C 停止"
        echo ""
        afl-fuzz -i fuzz_input -o fuzz_output -- ./fuzz_bin/fuzz_target
        ;;
    2)
        echo ""
        echo "开始运行模糊测试（后台）..."
        nohup afl-fuzz -i fuzz_input -o fuzz_output -- ./fuzz_bin/fuzz_target > fuzz.log 2>&1 &
        PID=$!
        echo "模糊测试已在后台运行，进程ID: $PID"
        echo "查看日志: tail -f fuzz.log"
        echo "停止测试: kill $PID"
        echo "查看进程: ps aux | grep afl-fuzz"
        ;;
    3)
        echo ""
        echo "使用screen运行模糊测试..."
        echo "创建screen会话: fuzzing"
        screen -S fuzzing -dm bash -c "afl-fuzz -i fuzz_input -o fuzz_output -- ./fuzz_bin/fuzz_target; exec bash"
        echo "模糊测试已在screen会话中运行"
        echo "查看会话: screen -r fuzzing"
        echo "分离会话: 在screen中按 Ctrl+A 然后 D"
        echo "列出所有会话: screen -ls"
        ;;
    *)
        echo "无效选择，退出"
        exit 1
        ;;
esac

echo ""
echo "=== 运行说明 ==="
echo ""
echo "查看崩溃（如果有）:"
echo "  ls fuzz_output/default/crashes/"
echo ""
echo "查看统计信息:"
echo "  cat fuzz_output/default/fuzzer_stats"
echo ""
echo "复现崩溃（如果发现）:"
echo "  ./fuzz_bin/fuzz_target < fuzz_output/default/crashes/id:000000*"
echo ""
echo "根据实验要求，至少运行5小时！"

