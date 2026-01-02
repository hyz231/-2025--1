#include "Category.h"
#include <utility>

Category::Category() : id_(""), name_(""), custom_(false) {}

Category::Category(std::string id, std::string name, bool isCustom) : id_(std::move(id)), name_(std::move(name)), custom_(isCustom) {}

std::string Category::getId() const { return id_; }
std::string Category::getName() const { return name_; }
bool Category::isCustom() const { return custom_; }

std::vector<Category> Category::defaultCategories() {
    return {
        Category("c_food", "餐饮"),
        Category("c_transport", "交通"),
        Category("c_shopping", "购物"),
        Category("c_salary", "工资"),
        Category("c_other", "其他")
    };
}

Category Category::addCustomCategory(std::vector<Category> &categories, const std::string &name) {
    std::string sanitized = name;
    if (sanitized.empty()) {
        sanitized = "自定义";
    }
    int counter = 1;
    std::string candidateId;
    bool unique = false;
    while (!unique) {
        candidateId = "custom_" + std::to_string(counter++);
        unique = true;
        for (const auto &c : categories) {
            if (c.getId() == candidateId) {
                unique = false;
                break;
            }
        }
    }
    Category custom(candidateId, sanitized, true);
    categories.push_back(custom);
    return custom;
}

std::vector<std::string> Category::getCategoryList(const std::vector<Category> &categories) {
    std::vector<std::string> names;
    names.reserve(categories.size());
    for (const auto &c : categories) {
        names.push_back(c.getName());
    }
    return names;
}

// [IMPLANTED FLAW #7: Integer Overflow]
// Function that performs arithmetic operations that can cause integer overflow
int Category::calculateTotalCategoryCount(const std::vector<Category> &categories, int multiplier) {
    int baseCount = static_cast<int>(categories.size());
    
    // [IMPLANTED FLAW: Integer Overflow]
    // Multiplying without checking for overflow
    // If baseCount * multiplier exceeds INT_MAX, this will overflow
    int total = baseCount * multiplier;  // Integer overflow possible
    
    // Further operations that can overflow
    total = total + 1000000;  // Additional overflow risk
    
    return total;
}