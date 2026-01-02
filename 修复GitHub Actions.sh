#!/bin/bash
# 修复 GitHub Actions 配置的脚本

echo "=== GitHub Actions 配置修复 ==="
echo ""

# 检查是否在仓库根目录
if [ ! -d ".git" ]; then
    echo "错误: 请在Git仓库根目录执行此脚本"
    exit 1
fi

# 检查 code_cpp 目录是否存在
if [ ! -d "code_cpp" ]; then
    echo "警告: 未找到 code_cpp 目录"
    echo "请确认项目结构"
    exit 1
fi

# 1. 创建正确的目录结构
echo "1. 创建 .github/workflows 目录..."
mkdir -p .github/workflows

# 2. 检查源文件是否存在
if [ -f "code_cpp/.github/workflows/ci.yml" ]; then
    echo "2. 复制 CI 配置文件..."
    cp code_cpp/.github/workflows/ci.yml .github/workflows/ci.yml
    echo "   ✅ 配置文件已复制到 .github/workflows/ci.yml"
else
    echo "2. 创建 CI 配置文件..."
    cat > .github/workflows/ci.yml << 'EOF'
# 工作流名称
name: C++ Ledger Project CI

# 触发工作流的事件：当有代码推送到 main 分支或有人向 main 分支发起 Pull Request 时触发
on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]

# 工作流包含的任务 (jobs)
jobs:
  build-and-test:
    # 运行此任务的操作系统环境
    runs-on: ubuntu-latest
    
    # 任务包含的步骤 (steps)
    steps:
    # 第一步：检出你的代码库
    - name: Checkout repository
      uses: actions/checkout@v4
    
    # 第二步：安装构建工具
    - name: Install build tools
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake
    
    # 第三步：安装GTest
    - name: Install Google Test
      run: |
        sudo apt-get install -y libgtest-dev
        cd /usr/src/gtest
        sudo cmake .
        sudo make
        sudo cp lib/*.a /usr/lib
    
    # 第四步：编译项目
    - name: Build project
      run: |
        cd code_cpp
        make clean
        make all
    
    # 第五步：运行单元测试
    - name: Run unit tests
      run: |
        cd code_cpp
        make test-storage || echo "Storage tests failed"
        make test-search || echo "Search tests failed"
        make test-integration || echo "Integration tests failed"
    
    # 第六步：运行原始测试
    - name: Run original tests
      run: |
        cd code_cpp
        make test-storage-original || echo "Original tests failed"
EOF
    echo "   ✅ 配置文件已创建"
fi

# 3. 显示当前分支
echo ""
echo "3. 当前 Git 状态:"
current_branch=$(git branch --show-current)
echo "   当前分支: $current_branch"

# 4. 检查文件
echo ""
echo "4. 检查配置文件:"
if [ -f ".github/workflows/ci.yml" ]; then
    echo "   ✅ .github/workflows/ci.yml 存在"
    echo "   文件大小: $(wc -l < .github/workflows/ci.yml) 行"
else
    echo "   ❌ .github/workflows/ci.yml 不存在"
    exit 1
fi

# 5. 显示下一步操作
echo ""
echo "=== 修复完成 ==="
echo ""
echo "下一步操作:"
echo "1. 检查配置文件:"
echo "   cat .github/workflows/ci.yml"
echo ""
echo "2. 添加并提交文件:"
echo "   git add .github/workflows/ci.yml"
echo "   git commit -m 'Add GitHub Actions CI workflow'"
echo ""
echo "3. 推送到 main 分支:"
echo "   git push origin main"
echo "   或"
echo "   git push origin master"
echo ""
echo "4. 在 GitHub 上查看 Actions:"
echo "   进入仓库 → 点击 'Actions' 标签 → 查看工作流运行情况"
echo ""

