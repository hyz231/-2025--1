# GTest 安装检查和故障排除

## 快速检查

运行以下命令检查 GTest 是否已安装：

```bash
# 检查头文件
ls /usr/include/gtest/gtest.h
ls /usr/local/include/gtest/gtest.h

# 检查库文件
ls /usr/lib/libgtest.a
ls /usr/local/lib/libgtest.a

# 检查 pkg-config
pkg-config --modversion gtest
```

## 安装方法

### 方法1: 使用包管理器（Ubuntu/Debian）

```bash
sudo apt-get update
sudo apt-get install -y libgtest-dev

# 编译 GTest 库
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib
```

### 方法2: 使用安装脚本

```bash
cd code_cpp
chmod +x install_gtest.sh
./install_gtest.sh
```

### 方法3: 从源码编译

```bash
cd /tmp
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 如果 GTest 安装在非标准位置

如果 GTest 安装在其他位置，可以手动指定路径：

```bash
# 方法1: 修改 Makefile
# 编辑 Makefile，修改 GTEST_INCLUDE 和 GTEST_LIBS

# 方法2: 使用环境变量
export GTEST_DIR=/path/to/gtest
make test-storage

# 方法3: 直接编译（替换路径）
g++ -std=c++17 -O2 -I./src -I./tests \
    -I/path/to/gtest/include \
    -o bin/test_storage_gtest.exe \
    tests/test_storage_gtest.cpp src/*.cpp \
    -L/path/to/gtest/lib -lgtest -lgtest_main -lpthread
```

## 验证安装

安装后，运行以下命令验证：

```bash
# 检查头文件
find /usr -name "gtest.h" 2>/dev/null
find /usr/local -name "gtest.h" 2>/dev/null

# 检查库文件
find /usr -name "libgtest.a" 2>/dev/null
find /usr/local -name "libgtest.a" 2>/dev/null

# 尝试编译一个简单的测试
cat > /tmp/test_gtest.cpp << 'EOF'
#include <gtest/gtest.h>
TEST(Test, Basic) { EXPECT_EQ(1, 1); }
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF

g++ -std=c++17 -I/usr/local/include -o /tmp/test_gtest /tmp/test_gtest.cpp -lgtest -lgtest_main -lpthread
/tmp/test_gtest
```

如果测试通过，说明 GTest 安装成功。

## 常见问题

### Q: 找不到 gtest/gtest.h
**A**: GTest 未安装或路径不对
- 运行 `./install_gtest.sh` 安装
- 或检查 GTest 安装路径并更新 Makefile

### Q: 找不到 -lgtest
**A**: GTest 库文件未找到
- 确保已编译 GTest 库（`libgtest.a`）
- 检查库文件是否在 `/usr/lib` 或 `/usr/local/lib`

### Q: 链接错误
**A**: 可能需要指定库路径
```bash
g++ ... -L/usr/local/lib -lgtest -lgtest_main -lpthread
```

### Q: 在 WSL 中安装
**A**: WSL 中安装方法相同
```bash
sudo apt-get update
sudo apt-get install -y libgtest-dev
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib
```

