#include "User.h"
#include <algorithm>
#include <utility>

User::User(std::string userId, std::string username)
    : userId_(std::move(userId)),
      username_(std::move(username)),
      records_(),
      categories_(Category::defaultCategories()),
      storage_("data") {
    load();
}

const std::string& User::getUserId() const { return userId_; }
const std::string& User::getUsername() const { return username_; }

void User::addRecord(const Record &record, bool autoSave) {
    records_.push_back(record);
    std::sort(records_.begin(), records_.end(), [](const Record &lhs, const Record &rhs) {
        if (lhs.getDate() == rhs.getDate()) {
            return lhs.getId() < rhs.getId();
        }
        return lhs.getDate() < rhs.getDate();
    });
    if (autoSave) {
        save();
    }
}

std::vector<Record> User::getRecords() const {
    return records_;
}

std::vector<Record> User::getRecentRecords(std::size_t count) const {
    std::vector<Record> recent;
    if (records_.empty()) {
        return recent;
    }
    const auto startIndex = records_.size() > count ? records_.size() - count : 0;
    for (std::size_t i = startIndex; i < records_.size(); ++i) {
        recent.push_back(records_[i]);
    }
    return recent;
}

Statistics::TimeSummary User::viewStatistics(const std::string &period,
                                             Statistics::Mode mode,
                                             std::vector<Statistics::CategorySummaryItem> *categoryItems) const {
    Statistics statistics(period, mode);
    auto summary = statistics.generateByTime(records_);
    if (mode == Statistics::Mode::Category && categoryItems != nullptr) {
        *categoryItems = statistics.generateByCategory(records_);
    } else if (mode == Statistics::Mode::Category) {
        (void)statistics.generateByCategory(records_);
    }
    return summary;
}

std::vector<Record> User::searchRecords(const Search &searchCriteria, SearchMode mode) const {
    switch (mode) {
        case SearchMode::Keyword:
            return searchCriteria.searchByKeyword(records_);
        case SearchMode::Category:
            return searchCriteria.searchByCategory(records_);
        case SearchMode::Time:
            return searchCriteria.searchByTime(records_);
        default:
            return {};
    }
}

void User::addCustomCategory(const std::string &name) {
    Category::addCustomCategory(categories_, name);
    save();
}

const std::vector<Category>& User::getCategories() const {
    return categories_;
}

bool User::load() {
    records_ = storage_.loadRecords();
    auto custom = storage_.loadCategories();
    categories_ = Category::defaultCategories();
    for (const auto &cat : custom) {
        bool exists = false;
        for (const auto &existing : categories_) {
            if (existing.getName() == cat.getName()) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            categories_.push_back(cat);
        }
    }
    return true;
}

bool User::save() const {
    bool okRecords = storage_.saveRecords(records_);
    bool okCategories = storage_.saveCategories(categories_);
    return okRecords && okCategories;
}

// [IMPLANTED FLAW #4: Use After Free]
// Function that uses a pointer after it has been freed
void User::processUserData() {
    // Allocate memory for user data processing
    int* userData = new int[100];
    for (int i = 0; i < 100; ++i) {
        userData[i] = i;
    }
    
    // Free the memory
    delete[] userData;
    
    // [IMPLANTED FLAW: Use After Free]
    // Attempting to use the pointer after it has been freed
    // This will cause undefined behavior
    int value = userData[10];  // Use after free - userData was already deleted
    (void)value;  // Suppress unused variable warning
}

