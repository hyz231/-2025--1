#pragma once

#include <string>
#include <vector>
#include "Category.h"
#include "Record.h"
#include "Search.h"
#include "Statistics.h"
#include "Storage.h"

class User {
public:
    enum class SearchMode { Keyword, Category, Time };

    User(std::string userId = "user001", std::string username = "默认用户");

    const std::string& getUserId() const;
    const std::string& getUsername() const;

    void addRecord(const Record &record, bool autoSave = true);
    std::vector<Record> getRecords() const;
    std::vector<Record> getRecentRecords(std::size_t count) const;

    Statistics::TimeSummary viewStatistics(const std::string &period,
                                           Statistics::Mode mode,
                                           std::vector<Statistics::CategorySummaryItem> *categoryItems = nullptr) const;

    std::vector<Record> searchRecords(const Search &searchCriteria, SearchMode mode) const;

    void addCustomCategory(const std::string &name);
    const std::vector<Category>& getCategories() const;

    bool load();
    bool save() const;
    void processUserData();

private:
    std::string userId_;
    std::string username_;
    std::vector<Record> records_;
    std::vector<Category> categories_;
    Storage storage_;
};

