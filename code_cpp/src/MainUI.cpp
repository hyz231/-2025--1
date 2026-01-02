#include "MainUI.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

namespace {

std::string nowDate() {
    auto current = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(current);
    std::tm tm {};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string nowMonth() {
    const auto today = nowDate();
    if (today.size() >= 7) {
        return today.substr(0, 7);
    }
    return today;
}

std::string generateRecordId() {
    static int counter = 0;
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    if (counter >= 1000) {
        counter = 0;
    }
    std::ostringstream oss;
    oss << "REC" << millis << std::setw(3) << std::setfill('0') << counter++;
    return oss.str();
}

double parseAmount(const std::string &input) {
    try {
        return std::stod(input);
    } catch (...) {
        return 0.0;
    }
}

Record::Type parseTypeInput(const std::string &input) {
    if (input == "1" || input == "I" || input == "i") {
        return Record::Type::Income;
    }
    return Record::Type::Expense;
}

} // namespace

RecordUI::RecordUI(User &user)
    : user_(user),
      selectedType_(Record::Type::Expense),
      selectedCategory_(""),
      selectedDate_(nowDate()),
      amount_(0.0),
      note_() {}

void RecordUI::showRecordForm() {
    std::cout << "\n=== 记账页面 ===\n";
    selectedType_ = Record::Type::Expense;
    selectedCategory_ = user_.getCategories().empty()
                            ? std::string("其他")
                            : user_.getCategories().front().getName();
    selectedDate_ = nowDate();
    amount_ = 0.0;
    note_.clear();

    std::cout << "选择类型: [1] 收入  [2] 支出 (默认支出) > ";
    std::string typeInput;
    std::getline(std::cin, typeInput);
    if (!typeInput.empty()) {
        selectedType_ = parseTypeInput(typeInput);
    }
    colorCodeType();

    inputAmount();
    selectCategory();
    inputDate();
    inputNote();
    saveRecord();
}

void RecordUI::selectCategory() {
    const auto categories = user_.getCategories();
    std::cout << "\n类别选择:\n";
    for (std::size_t i = 0; i < categories.size(); ++i) {
        const bool isActive = categories[i].getName() == selectedCategory_;
        std::cout << "  [" << (i + 1) << "] " << categories[i].getName();
        if (isActive) {
            std::cout << " *";
        }
        std::cout << "\n";
    }
    std::cout << "  [0] + 自定义类别\n";
    std::cout << "请选择类别编号 (默认当前选中): ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
        return;
    }
    std::size_t index = 0;
    try {
        index = static_cast<std::size_t>(std::stoul(input));
    } catch (...) {
        std::cout << "输入无效，保持当前分类。\n";
        return;
    }
    if (index == 0) {
        std::cout << "输入自定义类别名称: ";
        std::string name;
        std::getline(std::cin, name);
        if (!name.empty()) {
            user_.addCustomCategory(name);
            selectedCategory_ = name;
            std::cout << "新类别已添加并选中: " << name << "\n";
        }
        return;
    }
    if (index >= 1 && index <= categories.size()) {
        applyCategorySelection(index - 1);
    }
}

void RecordUI::applyCategorySelection(std::size_t index) {
    const auto categories = user_.getCategories();
    if (index < categories.size()) {
        selectedCategory_ = categories[index].getName();
    }
}

void RecordUI::inputAmount() {
    std::cout << "金额 (¥): ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
        amount_ = 0.0;
        return;
    }
    amount_ = parseAmount(input);
}

void RecordUI::inputDate() {
    std::cout << "日期 (YYYY-MM-DD, 回车为今天 " << nowDate() << "): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        selectedDate_ = input;
    }
}

void RecordUI::inputNote() {
    std::cout << "备注: ";
    std::getline(std::cin, note_);
}

void RecordUI::saveRecord() {
    const std::string recordId = generateRecordId();
    const Record record(recordId, selectedDate_, amount_, selectedType_, selectedCategory_, note_);
    user_.addRecord(record, true);
    std::cout << "记录已保存 (" << record.getRecordInfo() << ")\n";
}

void RecordUI::colorCodeType() const {
    std::cout << "当前类型: " << (selectedType_ == Record::Type::Income ? "[收入]" : "[支出]") << "\n";
}

StatisticsUI::StatisticsUI(User &user)
    : user_(user),
      showChart_(true) {}

void StatisticsUI::showTimeView() {
    std::cout << "\n=== 统计 - 时间视图 ===\n";
    std::cout << "输入年月 (YYYY-MM), 默认 " << nowMonth() << ": ";
    std::string period;
    std::getline(std::cin, period);
    if (period.empty()) {
        period = nowMonth();
    }
    auto summary = user_.viewStatistics(period, Statistics::Mode::Time);
    displayResults(summary, {});
}

void StatisticsUI::showCategoryView() {
    std::cout << "\n=== 统计 - 分类视图 ===\n";
    std::cout << "输入年月 (YYYY-MM), 默认 " << nowMonth() << ": ";
    std::string period;
    std::getline(std::cin, period);
    if (period.empty()) {
        period = nowMonth();
    }
    std::vector<Statistics::CategorySummaryItem> items;
    auto summary = user_.viewStatistics(period, Statistics::Mode::Category, &items);
    displayResults(summary, items);
}

void StatisticsUI::toggleChartSummary() {
    showChart_ = !showChart_;
    std::cout << "图表展示已" << (showChart_ ? "开启" : "关闭") << "\n";
}

void StatisticsUI::displayResults(const Statistics::TimeSummary &summary,
                                  const std::vector<Statistics::CategorySummaryItem> &items) {
    Statistics statistics(summary.period, Statistics::Mode::Time);
    statistics.showSummary(summary);
    if (!items.empty()) {
        std::cout << "\n分类占比:\n";
        if (showChart_) {
            statistics.showChart(items);
        } else {
            for (const auto &item : items) {
                std::cout << "  - " << item.category << " "
                          << std::fixed << std::setprecision(2) << item.amount
                          << " (" << std::setprecision(1) << item.percentage << "%)\n";
            }
        }
    }
}

SearchUI::SearchUI(User &user)
    : user_(user) {}

void SearchUI::showSearchForm() {
    std::cout << "\n=== 搜索 ===\n";
    std::cout << "1. 关键字\n2. 类别\n3. 时间范围\n选择操作 (其他返回): ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "1") {
        filterByKeyword();
    } else if (input == "2") {
        filterByCategory();
    } else if (input == "3") {
        filterByTime();
    }
}

void SearchUI::filterByTime() {
    std::cout << "起始日期 (YYYY-MM-DD): ";
    std::string from;
    std::getline(std::cin, from);
    std::cout << "结束日期 (YYYY-MM-DD): ";
    std::string to;
    std::getline(std::cin, to);
    Search search;
    search.setTimeRange(from, to);
    auto results = user_.searchRecords(search, User::SearchMode::Time);
    displayResults(results);
}

void SearchUI::filterByKeyword() {
    std::cout << "输入关键字: ";
    std::string keyword;
    std::getline(std::cin, keyword);
    Search search;
    search.setKeyword(keyword);
    auto results = user_.searchRecords(search, User::SearchMode::Keyword);
    displayResults(results);
}

void SearchUI::filterByCategory() {
    std::cout << "选择类别:\n";
    const auto categories = user_.getCategories();
    for (std::size_t i = 0; i < categories.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << categories[i].getName() << "\n";
    }
    std::cout << "输入编号: ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
        return;
    }
    std::size_t index = 0;
    try {
        index = static_cast<std::size_t>(std::stoul(input));
    } catch (...) {
        std::cout << "输入无效。\n";
        return;
    }
    if (index < 1 || index > categories.size()) {
        return;
    }
    Search search;
    search.setCategory(categories[index - 1].getName());
    auto results = user_.searchRecords(search, User::SearchMode::Category);
    displayResults(results);
}

void SearchUI::displayResults(const std::vector<Record> &records) const {
    if (records.empty()) {
        std::cout << "没有找到匹配的记录。\n";
        return;
    }
    std::cout << "共找到 " << records.size() << " 条记录:\n";
    for (const auto &record : records) {
        std::cout << "  - " << record.getRecordInfo() << "\n";
    }
}

MainUI::MainUI(User &user)
    : user_(user),
      recordUI_(user_),
      statisticsUI_(user_),
      searchUI_(user_) {}

void MainUI::showHomeScreen() {
    std::cout << "\n=== 首页 ===\n";
    const auto month = nowMonth();
    auto summary = user_.viewStatistics(month, Statistics::Mode::Time);
    std::cout << "本月概览（" << month << "）\n";
    std::cout << "  总收入: ¥" << std::fixed << std::setprecision(2) << summary.income << "\n";
    std::cout << "  总支出: ¥" << summary.expense << "\n";
    std::cout << "  净余额: ¥" << summary.balance << "\n";

    std::cout << "\n最近记录:\n";
    const auto recent = user_.getRecentRecords(10);
    if (recent.empty()) {
        std::cout << "  暂无记录，快去记一笔吧！\n";
    } else {
        for (const auto &record : recent) {
            std::cout << "  - " << record.getRecordInfo() << "\n";
        }
    }
}

void MainUI::setupBottomNavigation() {
    std::cout << "\n==============================\n";
    std::cout << "[1] 首页 🏠\n";
    std::cout << "[2] 统计 📊\n";
    std::cout << "[3] 记账 ➕\n";
    std::cout << "[4] 搜索 🔍\n";
    std::cout << "[5] 我的 👤\n";
    std::cout << "[0] 退出\n";
    std::cout << "请选择功能: ";
}

void MainUI::navigateToRecord() {
    recordUI_.showRecordForm();
    pause();
}

void MainUI::navigateToStatistics() {
    std::cout << "统计选项: [1] 时间视图 [2] 分类视图 [3] 切换图表显示 [其他返回]\n选择: ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "1") {
        statisticsUI_.showTimeView();
    } else if (input == "2") {
        statisticsUI_.showCategoryView();
    } else if (input == "3") {
        statisticsUI_.toggleChartSummary();
    }
    pause();
}

void MainUI::navigateToSearch() {
    searchUI_.showSearchForm();
    pause();
}

void MainUI::navigateToProfile() {
    std::cout << "\n=== 我的 ===\n";
    std::cout << "用户: " << user_.getUsername() << " (" << user_.getUserId() << ")\n";
    std::cout << "分类数量: " << user_.getCategories().size() << "\n";
    std::cout << "记录总数: " << user_.getRecords().size() << "\n";
    pause();
}

void MainUI::run() {
    while (true) {
        showHomeScreen();
        setupBottomNavigation();
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            continue;
        }
        if (choice == "2") {
            navigateToStatistics();
        } else if (choice == "3") {
            navigateToRecord();
        } else if (choice == "4") {
            navigateToSearch();
        } else if (choice == "5") {
            navigateToProfile();
        } else if (choice == "0") {
            user_.save();
            std::cout << "谢谢使用，已退出。\n";
            break;
        } else {
            std::cout << "无效选项，请重新选择。\n";
        }
    }
}

void MainUI::pause() const {
    std::cout << "\n按回车返回导航...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

