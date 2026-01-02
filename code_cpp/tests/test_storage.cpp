#include <iostream>
#include <cassert>
#include <filesystem>
#include "../src/Category.h"
#include "../src/Record.h"
#include "../src/Storage.h"

int main() {
    std::filesystem::path d = "tmp_test_data";
    std::filesystem::remove_all(d);
    std::filesystem::create_directories(d);

    Storage s(d.string());
    std::vector<Record> recs;
    recs.emplace_back("t1","2025-11-12",100.5,Record::Type::Income,"工资","test");
    recs.emplace_back("t2","2025-11-12",20.0,Record::Type::Expense,"餐饮","lunch");

    bool ok = s.saveRecords(recs);
    if (!ok) { std::cerr<<"save failed"<<std::endl; return 2; }

    auto loaded = s.loadRecords();
    assert(loaded.size() == 2);
    assert(loaded[0].getCategory() == "工资" || loaded[0].getCategory() == "餐饮");

    auto categories = Category::defaultCategories();
    auto custom = Category::addCustomCategory(categories, "测试类别");
    assert(custom.isCustom());
    bool savedCats = s.saveCategories(categories);
    if (!savedCats) { std::cerr << "save categories failed" << std::endl; return 3; }
    auto loadedCats = s.loadCategories();
    bool foundCustom = false;
    for (const auto &cat : loadedCats) {
        if (cat.getName() == "测试类别") {
            foundCustom = true;
            break;
        }
    }
    assert(foundCustom);

    std::cout << "test_storage OK\n";
    std::filesystem::remove_all(d);
    return 0;
}
