#pragma once

#include <string>
#include <vector>
#include "Record.h"

class Statistics {
public:
    enum class Mode { Time, Category };

    struct TimeSummary {
        std::string period;
        double income {0.0};
        double expense {0.0};
        double balance {0.0};
        std::size_t count {0};
    };

    struct CategorySummaryItem {
        std::string category;
        double amount {0.0};
        double percentage {0.0};
    };

    explicit Statistics(std::string period = "", Mode mode = Mode::Time);

    void setPeriod(const std::string &period);
    void setMode(Mode mode);

    const std::string& getPeriod() const;
    Mode getMode() const;

    TimeSummary generateByTime(const std::vector<Record> &records) const;
    std::vector<CategorySummaryItem> generateByCategory(const std::vector<Record> &records) const;

    void showChart(const std::vector<CategorySummaryItem> &items) const;
    void showSummary(const TimeSummary &summary) const;
    void formatSummaryData(const TimeSummary &summary);
    int calculateAverage(const std::vector<Record> &records);
    double calculatePercentage(double value, double total);

private:
    std::string period_;
    Mode mode_;
};

