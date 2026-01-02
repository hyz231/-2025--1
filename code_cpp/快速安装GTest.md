# 快速安装 GTest - 故障排除指南

## 问题：CMake 版本兼容性错误

如果遇到 CMake 版本错误，使用以下方法：

## 方法1: 使用系统包管理器（最简单，推荐）

```bash
# 安装 GTest 开发包
sudo apt-get update
sudo apt-get install -y libgtest-dev

# 检查库文件是否已存在（某些系统会自动编译）
ls /usr/lib/x86_64-linux-gnu/libgtest.a
ls /usr/lib/libgtest.a

# 如果库文件在 x86_64-linux-gnu 目录下，创建符号链接
sudo ln -sf /usr/lib/x86_64-linux-gnu/libgtest.a /usr/lib/libgtest.a
sudo ln -sf /usr/lib/x86_64-linux-gnu/libgtest_main.a /usr/lib/libgtest_main.a
```

## 方法2: 手动编译（修复 CMake 问题）

```bash
# 清理旧的编译
cd /usr/src/gtest
sudo rm -rf CMakeCache.txt CMakeFiles

# 使用更新的 CMake 配置
sudo cmake -DCMAKE_BUILD_TYPE=Release .
sudo make -j$(nproc)

# 复制库文件
if [ -f lib/libgtest.a ]; then
    sudo cp lib/libgtest.a /usr/lib/
    sudo cp lib/libgtest_main.a /usr/lib/
fi
```

## 方法3: 从源码编译（完全控制）

```bash
cd /tmp
rm -rf googletest
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

## 方法4: 使用修复后的安装脚本

```bash
cd ~/软工/code_cpp
chmod +x install_gtest_fixed.sh
./install_gtest_fixed.sh
```

## 验证安装

安装后验证：

```bash
# 检查头文件
find /usr -name "gtest.h" 2>/dev/null
find /usr/local -name "gtest.h" 2>/dev/null

# 检查库文件
find /usr -name "libgtest.a" 2>/dev/null
find /usr/local -name "libgtest.a" 2>/dev/null

# 如果找到文件，记录路径
GTEST_HEADER=$(find /usr /usr/local -name "gtest.h" 2>/dev/null | head -1)
GTEST_LIB=$(find /usr /usr/local -name "libgtest.a" 2>/dev/null | head -1)

echo "GTest 头文件: $GTEST_HEADER"
echo "GTest 库文件: $GTEST_LIB"
```

## 如果 GTest 在非标准位置

如果 GTest 安装在非标准位置，更新 Makefile 或直接编译：

```bash
# 假设 GTest 在 /usr/lib/x86_64-linux-gnu/
cd ~/软工/code_cpp

g++ -std=c++17 -O2 -I./src -I./tests \
    -I/usr/include \
    -o bin/test_storage_gtest.exe \
    tests/test_storage_gtest.cpp src/*.cpp \
    -L/usr/lib/x86_64-linux-gnu -lgtest -lgtest_main -lpthread
```

## 快速测试编译

创建一个简单的测试来验证 GTest 是否可用：

```bash
cat > /tmp/test_gtest_simple.cpp << 'EOF'
#include <gtest/gtest.h>
TEST(SimpleTest, Basic) { 
    EXPECT_EQ(1, 1); 
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF

# 尝试编译（根据你的系统调整路径）
g++ -std=c++17 -I/usr/include -o /tmp/test_gtest /tmp/test_gtest_simple.cpp \
    -L/usr/lib/x86_64-linux-gnu -lgtest -lgtest_main -lpthread 2>&1 || \
g++ -std=c++17 -I/usr/include -o /tmp/test_gtest /tmp/test_gtest_simple.cpp \
    -L/usr/lib -lgtest -lgtest_main -lpthread 2>&1 || \
g++ -std=c++17 -I/usr/local/include -o /tmp/test_gtest /tmp/test_gtest_simple.cpp \
    -L/usr/local/lib -lgtest -lgtest_main -lpthread

# 运行测试
/tmp/test_gtest
```

如果测试通过，说明 GTest 安装成功，然后可以更新 Makefile 使用正确的路径。

