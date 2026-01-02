#pragma once

#include <string>
#include <vector>
#include "Record.h"
#include "Category.h"

class Storage {
public:
    Storage(const std::string &dir="data");

    bool saveRecords(const std::vector<Record> &records) const;
    std::vector<Record> loadRecords() const;

    bool saveCategories(const std::vector<Category> &categories) const;
    std::vector<Category> loadCategories() const;

    bool ensureDataDir() const;
    void copyCategoryName(const std::string &categoryName);

private:
    std::string dir_;
    std::string recordsFile() const;
    std::string categoriesFile() const;
};
