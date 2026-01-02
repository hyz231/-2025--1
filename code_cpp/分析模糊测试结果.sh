#!/bin/bash
# 分析模糊测试结果的详细脚本

cd "$(dirname "$0")"

echo "=========================================="
echo "    模糊测试结果详细分析"
echo "=========================================="
echo ""

# 1. 检查统计信息
echo "【1. 运行统计信息】"
echo "----------------------------------------"
if [ -f fuzz_output/default/fuzzer_stats ]; then
    echo "运行时间:"
    run_time=$(grep "run_time" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    hours=$((run_time / 3600))
    minutes=$(((run_time % 3600) / 60))
    seconds=$((run_time % 60))
    echo "  $hours 小时 $minutes 分钟 $seconds 秒"
    
    echo ""
    echo "执行统计:"
    execs_done=$(grep "execs_done" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    execs_per_sec=$(grep "execs_per_sec" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    echo "  总执行次数: $execs_done"
    echo "  执行速度: $execs_per_sec 次/秒"
    
    echo ""
    echo "崩溃统计:"
    unique_crashes=$(grep "unique_crashes" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    saved_crashes=$(grep "saved_crashes" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    echo "  唯一崩溃数: $unique_crashes"
    echo "  保存的崩溃数: $saved_crashes"
    
    echo ""
    echo "路径统计:"
    paths_total=$(grep "paths_total" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    paths_found=$(grep "paths_found" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    echo "  总路径数: $paths_total"
    echo "  发现的路径数: $paths_found"
else
    echo "未找到 fuzzer_stats 文件"
fi

echo ""
echo "【2. 崩溃文件分析】"
echo "----------------------------------------"
if [ -d fuzz_output/default/crashes ]; then
    crash_count=$(ls fuzz_output/default/crashes/ 2>/dev/null | wc -l)
    echo "崩溃文件总数: $crash_count"
    
    if [ $crash_count -gt 0 ]; then
        echo ""
        echo "崩溃文件列表（前10个）:"
        ls -lh fuzz_output/default/crashes/ | head -11 | tail -10
        
        echo ""
        echo "分析前3个崩溃文件:"
        crash_files=($(ls fuzz_output/default/crashes/ | head -3))
        for i in "${!crash_files[@]}"; do
            crash_file="${crash_files[$i]}"
            echo ""
            echo "--- 崩溃文件 $((i+1)): $crash_file ---"
            echo "文件大小: $(wc -c < fuzz_output/default/crashes/$crash_file) 字节"
            echo "文件内容:"
            cat "fuzz_output/default/crashes/$crash_file" | head -3
            echo ""
            
            # 分析类别字段长度
            if grep -q $'\t' "fuzz_output/default/crashes/$crash_file"; then
                category=$(cat "fuzz_output/default/crashes/$crash_file" | cut -f5)
                echo "类别字段长度: ${#category} 字符"
                if [ ${#category} -ge 64 ]; then
                    echo "⚠️  类别名长度 >= 64，可能触发缓冲区溢出！"
                fi
            fi
        done
    else
        echo "未发现崩溃文件"
    fi
else
    echo "未找到崩溃目录"
fi

echo ""
echo "【3. 测试结果评估】"
echo "----------------------------------------"
if [ -f fuzz_output/default/fuzzer_stats ]; then
    saved_crashes=$(grep "saved_crashes" fuzz_output/default/fuzzer_stats | awk '{print $2}')
    if [ "$saved_crashes" -gt 0 ]; then
        echo "✅ 测试成功: 发现了 $saved_crashes 个崩溃"
        echo "✅ 结果正常: 模糊测试有效发现了程序缺陷"
        echo "✅ 符合要求: 已发现崩溃，不需要运行5小时"
    else
        echo "⚠️  未发现崩溃"
        echo "   建议: 继续运行至少5小时"
    fi
fi

echo ""
echo "=========================================="
echo "分析完成！"
echo ""
echo "下一步:"
echo "1. 查看崩溃文件: cat fuzz_output/default/crashes/id:000000*"
echo "2. 复现崩溃: ./fuzz_bin/fuzz_target < fuzz_output/default/crashes/id:000000*"
echo "3. 使用GDB分析: gdb ./fuzz_bin/fuzz_target"
echo "=========================================="

