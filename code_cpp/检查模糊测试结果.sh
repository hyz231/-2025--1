#!/bin/bash
# 检查模糊测试结果的脚本

cd "$(dirname "$0")"

echo "=== 模糊测试结果分析 ==="
echo ""

# 检查输出目录
if [ ! -d fuzz_output/default ]; then
    echo "错误: 未找到模糊测试输出目录"
    echo "请先运行模糊测试: afl-fuzz -i fuzz_input -o fuzz_output -- ./fuzz_bin/fuzz_target"
    exit 1
fi

echo "1. 查看统计信息"
echo "=================="
if [ -f fuzz_output/default/fuzzer_stats ]; then
    echo "运行时间:"
    grep "run_time" fuzz_output/default/fuzzer_stats | awk '{print "  " $1 ": " $2 " 秒"}'
    
    echo ""
    echo "崩溃统计:"
    grep "unique_crashes" fuzz_output/default/fuzzer_stats | awk '{print "  唯一崩溃数: " $2}'
    grep "saved_crashes" fuzz_output/default/fuzzer_stats | awk '{print "  保存的崩溃数: " $2}'
    
    echo ""
    echo "执行统计:"
    grep "execs_done" fuzz_output/default/fuzzer_stats | awk '{print "  总执行次数: " $2}'
    grep "execs_per_sec" fuzz_output/default/fuzzer_stats | awk '{print "  执行速度: " $2 " 次/秒"}'
    
    echo ""
    echo "路径统计:"
    grep "paths_total" fuzz_output/default/fuzzer_stats | awk '{print "  总路径数: " $2}'
    grep "paths_found" fuzz_output/default/fuzzer_stats | awk '{print "  发现的路径数: " $2}'
else
    echo "未找到 fuzzer_stats 文件"
fi

echo ""
echo "2. 检查崩溃文件"
echo "=================="
if [ -d fuzz_output/default/crashes ]; then
    crash_count=$(ls fuzz_output/default/crashes/ 2>/dev/null | wc -l)
    echo "崩溃文件数量: $crash_count"
    
    if [ $crash_count -gt 0 ]; then
        echo ""
        echo "崩溃文件列表:"
        ls -lh fuzz_output/default/crashes/ | head -10
        
        echo ""
        echo "第一个崩溃文件内容:"
        first_crash=$(ls fuzz_output/default/crashes/ | head -1)
        if [ -n "$first_crash" ]; then
            echo "--- $first_crash ---"
            cat "fuzz_output/default/crashes/$first_crash" | head -5
            echo ""
            echo "文件大小: $(wc -c < fuzz_output/default/crashes/$first_crash) 字节"
        fi
    else
        echo "未发现崩溃文件"
    fi
else
    echo "未找到崩溃目录"
fi

echo ""
echo "3. 检查队列文件"
echo "=================="
if [ -d fuzz_output/default/queue ]; then
    queue_count=$(ls fuzz_output/default/queue/ 2>/dev/null | wc -l)
    echo "队列文件数量: $queue_count"
    
    if [ $queue_count -gt 0 ]; then
        echo "前5个队列文件:"
        ls fuzz_output/default/queue/ | head -5
    fi
else
    echo "未找到队列目录"
fi

echo ""
echo "=== 分析完成 ==="

