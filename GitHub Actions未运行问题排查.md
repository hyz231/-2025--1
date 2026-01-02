# GitHub Actions 未运行问题排查指南

## 一、常见原因

### 1. ❌ 配置文件位置错误（最可能的原因）

**问题**: `.github/workflows/ci.yml` 文件在 `code_cpp` 目录下

**GitHub Actions要求**: 配置文件必须在**仓库根目录**的 `.github/workflows/` 下

**当前结构**:
```
仓库根目录/
└── code_cpp/
    └── .github/
        └── workflows/
            └── ci.yml  ❌ 错误位置
```

**正确结构**:
```
仓库根目录/
├── code_cpp/
│   ├── src/
│   ├── tests/
│   └── ...
└── .github/          ✅ 必须在根目录
    └── workflows/
        └── ci.yml    ✅ 正确位置
```

### 2. ❌ 分支名称不匹配

**问题**: 配置文件触发的是 `main` 或 `master` 分支

**检查**:
- 你的默认分支是什么？
- 你推送到哪个分支？

**解决方法**: 
- 确保推送到 `main` 或 `master` 分支
- 或者修改配置文件中的分支名称

### 3. ❌ GitHub Actions 被禁用

**检查方法**:
1. 进入 GitHub 仓库
2. 点击 "Settings"（设置）
3. 左侧菜单找到 "Actions" → "General"
4. 检查 "Actions permissions" 是否启用

### 4. ❌ 文件路径问题

**问题**: 工作流中使用了 `cd code_cpp`，但实际仓库结构可能不同

**检查**: 确认仓库根目录下是否有 `code_cpp` 文件夹

---

## 二、解决方案

### 方案1: 移动配置文件到正确位置（推荐）

#### 步骤1: 在仓库根目录创建 `.github/workflows/` 目录

```bash
# 在仓库根目录执行
mkdir -p .github/workflows
```

#### 步骤2: 复制或移动 ci.yml 文件

```bash
# 从 code_cpp/.github/workflows/ci.yml 复制到根目录
cp code_cpp/.github/workflows/ci.yml .github/workflows/ci.yml
```

#### 步骤3: 提交并推送

```bash
git add .github/workflows/ci.yml
git commit -m "Add GitHub Actions CI workflow"
git push origin main  # 或 master
```

### 方案2: 修改工作流路径（如果仓库结构不同）

如果 `code_cpp` 不在根目录，需要修改工作流中的路径。

---

## 三、验证 GitHub Actions 是否运行

### 检查步骤

1. **进入 GitHub 仓库**
   - 打开你的 GitHub 仓库页面

2. **查看 Actions 标签**
   - 点击仓库顶部的 "Actions" 标签
   - 应该能看到工作流运行历史

3. **检查工作流状态**
   - 绿色 ✓ = 成功
   - 黄色 ⚠ = 运行中
   - 红色 ✗ = 失败

4. **查看工作流详情**
   - 点击工作流运行记录
   - 查看每个步骤的执行情况

---

## 四、快速修复命令

### 在仓库根目录执行

```bash
# 1. 创建正确的目录结构
mkdir -p .github/workflows

# 2. 复制配置文件
cp code_cpp/.github/workflows/ci.yml .github/workflows/ci.yml

# 3. 提交更改
git add .github/workflows/ci.yml
git commit -m "Fix: Move GitHub Actions workflow to root directory"

# 4. 推送到 main 分支
git push origin main
```

---

## 五、修改后的正确结构

```
你的仓库根目录/
├── .github/                    ✅ 在根目录
│   └── workflows/
│       └── ci.yml             ✅ CI配置文件
├── code_cpp/                  ✅ 项目代码
│   ├── src/
│   ├── tests/
│   ├── Makefile
│   └── ...
└── README.md (可选)
```

---

## 六、配置文件路径修正

如果 `code_cpp` 在根目录，当前配置是正确的。如果不在，需要修改：

```yaml
# 如果 code_cpp 在根目录，保持：
cd code_cpp
make clean
make all

# 如果 code_cpp 不在根目录，需要修改路径：
cd 你的实际路径
make clean
make all
```

---

## 七、常见错误信息

### 错误1: "No workflow file found"
**原因**: `.github/workflows/` 不在仓库根目录
**解决**: 移动配置文件到根目录

### 错误2: "Workflow not triggered"
**原因**: 分支名称不匹配或推送到错误分支
**解决**: 推送到 `main` 或 `master` 分支

### 错误3: "Path not found"
**原因**: 工作流中的路径不正确
**解决**: 检查并修改工作流中的路径

---

## 八、验证清单

- [ ] `.github/workflows/ci.yml` 在仓库根目录
- [ ] 推送到 `main` 或 `master` 分支
- [ ] GitHub Actions 已启用
- [ ] 工作流中的路径正确
- [ ] 文件已提交并推送

---

现在按照上述步骤修复，GitHub Actions 应该就能正常运行了！

