#!/bin/bash

# ESBMC 分析脚本
# 分析包含植入缺陷的C++文件

ESBMC_OUTPUT_DIR="flaw_report/ESBMC"
ESBMC_RAW_OUTPUT="$ESBMC_OUTPUT_DIR/esbmc_raw_output.txt"
ESBMC_RESULTS="$ESBMC_OUTPUT_DIR/esbmc_results.json"

# 创建输出目录
mkdir -p "$ESBMC_OUTPUT_DIR"

# 清空之前的输出
> "$ESBMC_RAW_OUTPUT"
> "$ESBMC_RESULTS"

echo "开始ESBMC分析..."
echo "=========================================="

# 定义要分析的文件和对应的缺陷
declare -A FILES=(
    ["code_cpp/src/Storage.cpp"]="Memory Leak, Buffer Overflow"
    ["code_cpp/src/Search.cpp"]="Double Free, Out of Bounds Access"
    ["code_cpp/src/Statistics.cpp"]="NULL Pointer Dereference, Uninitialized Variable, Division by Zero"
    ["code_cpp/src/User.cpp"]="Use After Free"
    ["code_cpp/src/Category.cpp"]="Integer Overflow"
)

# ESBMC 分析函数
analyze_file() {
    local file=$1
    local defects=$2
    
    echo "" >> "$ESBMC_RAW_OUTPUT"
    echo "==========================================" >> "$ESBMC_RAW_OUTPUT"
    echo "分析文件: $file" >> "$ESBMC_RAW_OUTPUT"
    echo "预期缺陷: $defects" >> "$ESBMC_RAW_OUTPUT"
    echo "==========================================" >> "$ESBMC_RAW_OUTPUT"
    
    # 运行ESBMC，使用多种检查选项
    # --unwind: 设置循环展开次数
    # --no-bounds-check: 禁用边界检查（我们想检测边界问题）
    # --memory-leak-check: 启用内存泄漏检查
    # --overflow-check: 启用溢出检查
    # --div-by-zero-check: 启用除零检查
    
    esbmc "$file" \
        --unwind 10 \
        --memory-leak-check \
        --overflow-check \
        --div-by-zero-check \
        --pointer-check \
        --bounds-check \
        --deadlock-check \
        --race-check \
        --k-induction \
        --incremental-bmc \
        2>&1 | tee -a "$ESBMC_RAW_OUTPUT"
    
    echo "" >> "$ESBMC_RAW_OUTPUT"
}

# 分析每个文件
for file in "${!FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "正在分析: $file"
        analyze_file "$file" "${FILES[$file]}"
    else
        echo "警告: 文件不存在: $file"
    fi
done

echo ""
echo "ESBMC分析完成！"
echo "原始输出保存在: $ESBMC_RAW_OUTPUT"
echo "=========================================="

