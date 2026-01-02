# 如何修复 GitHub Actions 未运行的问题

## 问题原因

**最可能的原因**: `.github/workflows/ci.yml` 文件位置不正确

GitHub Actions **必须在仓库根目录**的 `.github/workflows/` 下才能被识别。

## 当前问题

如果你的配置文件在 `code_cpp/.github/workflows/ci.yml`，GitHub 无法识别它。

## 解决方案

### 方法1: 使用已创建的配置文件（最简单）

我已经在仓库根目录创建了正确的配置文件：`.github/workflows/ci.yml`

**下一步操作**:

```bash
# 1. 添加文件到Git
git add .github/workflows/ci.yml

# 2. 提交
git commit -m "Add GitHub Actions CI workflow"

# 3. 推送到 main 分支（确保推送到正确的分支）
git push origin main
# 或
git push origin master
```

### 方法2: 手动创建

如果方法1不工作，手动创建：

```bash
# 1. 在仓库根目录创建目录
mkdir -p .github/workflows

# 2. 创建配置文件
# 复制 code_cpp/.github/workflows/ci.yml 的内容
# 或使用我创建的 .github/workflows/ci.yml

# 3. 提交并推送
git add .github/workflows/ci.yml
git commit -m "Add GitHub Actions CI workflow"
git push origin main
```

## 验证步骤

### 1. 检查文件位置

```bash
# 在仓库根目录执行
ls -la .github/workflows/ci.yml

# 应该显示文件存在
```

### 2. 检查分支

```bash
# 查看当前分支
git branch

# 确保推送到 main 或 master 分支
git push origin main
```

### 3. 在GitHub上验证

1. 进入你的 GitHub 仓库
2. 点击 "Actions" 标签
3. 应该能看到工作流运行记录

## 常见问题

### Q1: 推送到其他分支（如 develop）怎么办？

**解决方法**: 修改配置文件中的分支名称

```yaml
on:
  push:
    branches: [ main, master, develop ]  # 添加你的分支名
```

### Q2: 仓库结构不同怎么办？

如果你的项目不在 `code_cpp` 目录下，需要修改工作流中的路径：

```yaml
# 修改这里
- name: Build project
  run: |
    cd 你的实际目录  # 修改为实际路径
    make clean
    make all
```

### Q3: GitHub Actions 被禁用？

**检查方法**:
1. 进入仓库 Settings
2. 点击 Actions → General
3. 检查 "Actions permissions" 是否启用

### Q4: 文件已存在但不运行？

**可能原因**:
- 文件在错误的目录
- 分支名称不匹配
- 文件格式错误（YAML语法错误）

**解决方法**:
- 检查文件位置
- 检查分支名称
- 使用 YAML 验证工具检查语法

## 正确的目录结构

```
你的仓库根目录/
├── .github/              ✅ 必须在根目录
│   └── workflows/
│       └── ci.yml        ✅ CI配置文件
├── code_cpp/             ✅ 项目代码
│   ├── src/
│   ├── tests/
│   └── Makefile
└── README.md (可选)
```

## 快速修复命令

```bash
# 在仓库根目录执行
mkdir -p .github/workflows
cp code_cpp/.github/workflows/ci.yml .github/workflows/ci.yml  # 如果存在
# 或直接使用我创建的 .github/workflows/ci.yml

git add .github/workflows/ci.yml
git commit -m "Add GitHub Actions CI workflow"
git push origin main
```

## 验证清单

- [ ] `.github/workflows/ci.yml` 在仓库根目录（不是 code_cpp 下）
- [ ] 文件已提交到 Git
- [ ] 推送到 `main` 或 `master` 分支
- [ ] GitHub Actions 已启用
- [ ] 在 GitHub 上能看到 Actions 标签

---

**现在按照上述步骤操作，GitHub Actions 应该就能正常运行了！**

