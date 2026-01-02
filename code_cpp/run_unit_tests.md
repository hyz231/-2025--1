# 单元测试执行指南

## Windows环境

### 前提条件
1. 安装GTest（推荐使用vcpkg）
```powershell
# 安装vcpkg（如果还没有）
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装GTest
.\vcpkg install gtest:x64-windows
```

2. 或者从源码编译GTest
   - 下载: https://github.com/google/googletest
   - 按照官方文档编译安装

### 运行测试

#### 方法1: 使用Makefile（需要安装make工具）
```powershell
cd code_cpp

# 编译Storage模块测试
make test-storage

# 运行Storage模块测试
.\bin\test_storage_gtest.exe

# 编译Search模块测试
make test-search

# 运行Search模块测试
.\bin\test_search_gtest.exe
```

#### 方法2: 直接编译（如果Makefile不工作）
```powershell
cd code_cpp

# 设置GTest路径（根据你的安装位置调整）
$GTEST_DIR = "C:\path\to\gtest"

# 编译Storage测试
g++ -std=c++17 -I./src -I./tests -I"$GTEST_DIR\include" `
    -o bin/test_storage_gtest.exe `
    tests/test_storage_gtest.cpp src/*.cpp `
    "$GTEST_DIR\lib\gtest.lib" "$GTEST_DIR\lib\gtest_main.lib"

# 运行测试
.\bin\test_storage_gtest.exe

# 编译Search测试
g++ -std=c++17 -I./src -I./tests -I"$GTEST_DIR\include" `
    -o bin/test_search_gtest.exe `
    tests/test_search_gtest.cpp src/*.cpp `
    "$GTEST_DIR\lib\gtest.lib" "$GTEST_DIR\lib\gtest_main.lib"

# 运行测试
.\bin\test_search_gtest.exe
```

## Linux环境

### 安装GTest
```bash
sudo apt-get update
sudo apt-get install libgtest-dev

# 编译GTest库
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib
```

### 运行测试
```bash
cd code_cpp

# 编译并运行Storage测试
make test-storage
./bin/test_storage_gtest.exe

# 编译并运行Search测试
make test-search
./bin/test_search_gtest.exe

# 或者运行所有测试
make test-all
```

## 测试输出示例

### 成功输出示例
```
[==========] Running 18 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 18 tests from StorageTest
[ RUN      ] StorageTest.ConstructorWithDefaultDir
[       OK ] StorageTest.ConstructorWithDefaultDir (1 ms)
[ RUN      ] StorageTest.ConstructorWithCustomDir
[       OK ] StorageTest.ConstructorWithCustomDir (0 ms)
[ RUN      ] StorageTest.SaveAndLoadEmptyRecords
[       OK ] StorageTest.SaveAndLoadEmptyRecords (2 ms)
...
[----------] 18 tests from StorageTest (15 ms total)

[----------] Global test environment tear-down
[==========] 18 tests from 1 test suite ran. (20 ms total)
[  PASSED  ] 18 tests.
```

### 失败输出示例（如果有失败）
```
[ RUN      ] StorageTest.SaveAndLoadSingleRecord
test_storage_gtest.cpp:58: Failure
Expected equality of these values:
  loaded[0].getId()
    Which is: "r1"
  "r2"
    Which is: "r2"
[  FAILED  ] StorageTest.SaveAndLoadSingleRecord (1 ms)
```

## 生成测试覆盖率报告（可选）

### 使用gcov（Linux）
```bash
# 编译时添加覆盖率选项
g++ -std=c++17 -fprofile-arcs -ftest-coverage -I./src -I./tests \
    -I/usr/local/include -o bin/test_storage_gtest.exe \
    tests/test_storage_gtest.cpp src/*.cpp \
    -lgtest -lgtest_main -lpthread

# 运行测试
./bin/test_storage_gtest.exe

# 生成覆盖率报告
gcov src/Storage.cpp
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## 截图建议

为了实验报告，建议截图以下内容：

1. **测试代码截图**
   - 打开 `tests/test_storage_gtest.cpp` 和 `tests/test_search_gtest.cpp`
   - 截图显示测试用例代码

2. **测试执行命令截图**
   - 显示运行测试的命令
   - 显示编译过程

3. **测试结果截图**
   - 显示所有测试通过的状态
   - 显示测试统计信息（通过数、失败数、运行时间等）

4. **测试覆盖率截图**（如果生成了）
   - 显示覆盖率报告
   - 显示覆盖率百分比

## 常见问题

### Q: 找不到gtest头文件
A: 检查GTest安装路径，确保在编译时正确指定include路径

### Q: 链接错误
A: 确保链接了gtest和gtest_main库，并指定正确的库路径

### Q: Windows上make命令不存在
A: 可以安装MinGW或使用Visual Studio的nmake，或者直接使用g++命令编译

### Q: 测试运行但找不到测试用例
A: 确保测试文件正确包含了gtest/gtest.h，并且使用了TEST或TEST_F宏

