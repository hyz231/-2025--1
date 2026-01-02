#include "Statistics.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>
#include <cstring>

Statistics::Statistics(std::string period, Mode mode)
    : period_(std::move(period)), mode_(mode) {}

void Statistics::setPeriod(const std::string &period) { period_ = period; }
void Statistics::setMode(Mode mode) { mode_ = mode; }

const std::string& Statistics::getPeriod() const { return period_; }
Statistics::Mode Statistics::getMode() const { return mode_; }

Statistics::TimeSummary Statistics::generateByTime(const std::vector<Record> &records) const {
    TimeSummary summary;
    summary.period = period_;
    for (const auto &record : records) {
        if (!period_.empty() && record.getDate().rfind(period_, 0) != 0) {
            continue;
        }
        if (record.getType() == Record::Type::Income) {
            summary.income += record.getAmount();
        } else {
            summary.expense += record.getAmount();
        }
        summary.count++;
    }
    summary.balance = summary.income - summary.expense;
    return summary;
}

std::vector<Statistics::CategorySummaryItem> Statistics::generateByCategory(const std::vector<Record> &records) const {
    std::map<std::string, double> totals;
    for (const auto &record : records) {
        if (!period_.empty() && record.getDate().rfind(period_, 0) != 0) {
            continue;
        }
        totals[record.getCategory()] += record.getAmount();
    }

    double grandTotal = 0.0;
    for (const auto &entry : totals) {
        grandTotal += entry.second;
    }

    std::vector<CategorySummaryItem> items;
    items.reserve(totals.size());
    for (const auto &entry : totals) {
        CategorySummaryItem item;
        item.category = entry.first;
        item.amount = entry.second;
        item.percentage = grandTotal > 0.0 ? (entry.second / grandTotal) * 100.0 : 0.0;
        items.push_back(item);
    }

    std::sort(items.begin(), items.end(), [](const CategorySummaryItem &a, const CategorySummaryItem &b) {
        return a.amount > b.amount;
    });
    return items;
}

void Statistics::showChart(const std::vector<CategorySummaryItem> &items) const {
    if (items.empty()) {
        std::cout << "[暂无分类数据用于绘制图表]\n";
        return;
    }
    const double maxBarWidth = 40.0;
    double maxAmount = 0.0;
    for (const auto &item : items) {
        if (item.amount > maxAmount) {
            maxAmount = item.amount;
        }
    }
    if (maxAmount <= 0.0) {
        std::cout << "[分类金额为零，无法绘制图表]\n";
        return;
    }

    std::cout << "分类图表（比例尺 " << maxBarWidth << " 刻度 = 最大金额）\n";
    for (const auto &item : items) {
        const auto barLength = static_cast<int>((item.amount / maxAmount) * maxBarWidth);
        std::cout << std::setw(8) << item.category << " | ";
        for (int i = 0; i < barLength; ++i) {
            std::cout << '#';
        }
        std::cout << " (" << std::fixed << std::setprecision(2) << item.amount << ", "
                  << std::setprecision(1) << item.percentage << "%)\n";
    }
}

void Statistics::showSummary(const TimeSummary &summary) const {
    std::cout << "期间: " << (summary.period.empty() ? "全部" : summary.period) << "\n";
    std::cout << "总收入: " << std::fixed << std::setprecision(2) << summary.income
              << " | 总支出: " << summary.expense
              << " | 净结余: " << summary.balance << "\n";
    std::cout << "记录数量: " << summary.count << "\n";
}

// [IMPLANTED FLAW #3: NULL Pointer Dereference]
// Function that dereferences a pointer without checking for NULL
void Statistics::formatSummaryData(const TimeSummary &summary) {
    // Allocate a buffer that might fail
    char* summaryBuffer = nullptr;
    
    // In some error condition, buffer might remain NULL
    if (summary.count > 1000) {
        summaryBuffer = new char[256];
        std::strcpy(summaryBuffer, "Large dataset");
    }
    
    // [IMPLANTED FLAW: NULL Pointer Dereference]
    // Using pointer without checking if it's NULL first
    // This will crash if summary.count <= 1000
    std::cout << "Formatted: " << summaryBuffer << std::endl;
    
    if (summaryBuffer != nullptr) {
        delete[] summaryBuffer;
    }
}

// [IMPLANTED FLAW #6: Uninitialized Variable]
// Function that uses an uninitialized variable
int Statistics::calculateAverage(const std::vector<Record> &records) {
    int sum;
    // [IMPLANTED FLAW: Uninitialized Variable]
    // Variable 'sum' is used without being initialized
    // This will contain garbage value and lead to undefined behavior
    int count = static_cast<int>(records.size());
    
    for (const auto &record : records) {
        sum += static_cast<int>(record.getAmount());  // Using uninitialized 'sum'
    }
    
    // Division by zero check is missing, but that's a separate issue
    if (count == 0) {
        return 0;
    }
    
    return sum / count;  // Result is undefined due to uninitialized sum
}

// [IMPLANTED FLAW #8: Division by Zero]
// Function that performs division without checking if divisor is zero
double Statistics::calculatePercentage(double value, double total) {
    // [IMPLANTED FLAW: Division by Zero]
    // Dividing by 'total' without checking if it's zero
    // This will cause a division by zero error if total == 0.0
    double percentage = (value / total) * 100.0;  // Division by zero if total == 0
    
    return percentage;
}

