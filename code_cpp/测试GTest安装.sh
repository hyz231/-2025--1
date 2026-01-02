#!/bin/bash
# 测试 GTest 安装并显示路径

echo "=== 检查 GTest 安装 ==="
echo ""

# 查找头文件
echo "查找 GTest 头文件..."
GTEST_HEADERS=$(find /usr /usr/local -name "gtest.h" 2>/dev/null)
if [ -z "$GTEST_HEADERS" ]; then
    echo "❌ 未找到 gtest.h"
    echo "请运行: sudo apt-get install libgtest-dev"
else
    echo "✅ 找到 GTest 头文件:"
    echo "$GTEST_HEADERS" | head -3
fi

echo ""

# 查找库文件
echo "查找 GTest 库文件..."
GTEST_LIBS=$(find /usr /usr/local -name "libgtest.a" 2>/dev/null)
if [ -z "$GTEST_LIBS" ]; then
    echo "❌ 未找到 libgtest.a"
    echo "需要编译 GTest 库"
else
    echo "✅ 找到 GTest 库文件:"
    echo "$GTEST_LIBS" | head -3
fi

echo ""

# 测试编译
echo "测试编译..."
cat > /tmp/test_gtest_check.cpp << 'EOF'
#include <gtest/gtest.h>
TEST(Check, Install) { EXPECT_TRUE(true); }
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF

# 尝试多个可能的路径组合
COMPILE_SUCCESS=false

for INCLUDE_DIR in /usr/include /usr/local/include; do
    for LIB_DIR in /usr/lib/x86_64-linux-gnu /usr/lib /usr/local/lib; do
        if [ -f "$INCLUDE_DIR/gtest/gtest.h" ] && [ -f "$LIB_DIR/libgtest.a" ]; then
            echo "尝试: -I$INCLUDE_DIR -L$LIB_DIR"
            if g++ -std=c++17 -I$INCLUDE_DIR -o /tmp/test_gtest_check /tmp/test_gtest_check.cpp \
                -L$LIB_DIR -lgtest -lgtest_main -lpthread 2>/dev/null; then
                echo "✅ 编译成功！"
                echo "  头文件路径: $INCLUDE_DIR"
                echo "  库文件路径: $LIB_DIR"
                COMPILE_SUCCESS=true
                
                # 运行测试
                if /tmp/test_gtest_check > /dev/null 2>&1; then
                    echo "✅ 测试运行成功！"
                fi
                break 2
            fi
        fi
    done
done

if [ "$COMPILE_SUCCESS" = false ]; then
    echo "❌ 编译失败"
    echo ""
    echo "请运行以下命令安装 GTest:"
    echo "  sudo apt-get install libgtest-dev"
    echo "  cd /usr/src/gtest"
    echo "  sudo cmake -DCMAKE_BUILD_TYPE=Release ."
    echo "  sudo make -j\$(nproc)"
    echo "  sudo cp lib/*.a /usr/lib/"
fi

# 清理
rm -f /tmp/test_gtest_check.cpp /tmp/test_gtest_check

echo ""
echo "=== 检查完成 ==="

