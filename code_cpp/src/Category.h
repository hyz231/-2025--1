#pragma once

#include <string>
#include <vector>

class Category {
public:
    Category();
    explicit Category(std::string id, std::string name, bool isCustom=false);

    std::string getId() const;
    std::string getName() const;
    bool isCustom() const;

    static std::vector<Category> defaultCategories();
    static Category addCustomCategory(std::vector<Category> &categories, const std::string &name);
    static std::vector<std::string> getCategoryList(const std::vector<Category> &categories);
    static int calculateTotalCategoryCount(const std::vector<Category> &categories, int multiplier);

private:
    std::string id_;
    std::string name_;
    bool custom_;
};
