#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "../src/Storage.h"
#include "../src/Search.h"
#include "../src/Record.h"
#include "../src/Category.h"
#include "../src/Statistics.h"

// 集成测试1: Storage + Search 集成
class StorageSearchIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "tmp_test_integration1";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        storage = std::make_unique<Storage>(testDir);
        search = std::make_unique<Search>();
        
        // 创建测试数据
        records.clear();
        records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "一月工资");
        records.emplace_back("r2", "2025-01-15", 50.0, Record::Type::Expense, "餐饮", "午餐");
        records.emplace_back("r3", "2025-02-01", 200.0, Record::Type::Income, "奖金", "年终奖");
        records.emplace_back("r4", "2025-02-10", 30.0, Record::Type::Expense, "餐饮", "晚餐");
        records.emplace_back("r5", "2025-03-01", 150.0, Record::Type::Income, "工资", "三月工资");
    }

    void TearDown() override {
        std::filesystem::remove_all(testDir);
    }

    std::string testDir;
    std::unique_ptr<Storage> storage;
    std::unique_ptr<Search> search;
    std::vector<Record> records;
};

TEST_F(StorageSearchIntegrationTest, SaveLoadAndSearchByKeyword) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 5);
    
    // 搜索关键词
    search->setKeyword("工资");
    auto results = search->searchByKeyword(loaded);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.getCategory(), "工资");
    }
}

TEST_F(StorageSearchIntegrationTest, SaveLoadAndSearchByCategory) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    
    // 按类别搜索
    search->setCategory("餐饮");
    auto results = search->searchByCategory(loaded);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.getCategory(), "餐饮");
    }
}

TEST_F(StorageSearchIntegrationTest, SaveLoadAndSearchByTime) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    
    // 按时间范围搜索
    search->setTimeRange("2025-01-01", "2025-01-31");
    auto results = search->searchByTime(loaded);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_GE(r.getDate(), "2025-01-01");
        EXPECT_LE(r.getDate(), "2025-01-31");
    }
}

TEST_F(StorageSearchIntegrationTest, SaveLoadAndMultipleSearchCriteria) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    
    // 组合搜索：先按关键词，再按时间
    search->setKeyword("工资");
    auto keywordResults = search->searchByKeyword(loaded);
    
    search->setTimeRange("2025-01-01", "2025-01-31");
    auto timeResults = search->searchByTime(keywordResults);
    
    ASSERT_EQ(timeResults.size(), 1);
    EXPECT_EQ(timeResults[0].getDate(), "2025-01-01");
    EXPECT_EQ(timeResults[0].getCategory(), "工资");
}

// 集成测试2: Storage + Statistics 集成
class StorageStatisticsIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "tmp_test_integration2";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        storage = std::make_unique<Storage>(testDir);
        
        // 创建测试数据
        records.clear();
        records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "一月工资");
        records.emplace_back("r2", "2025-01-15", 50.0, Record::Type::Expense, "餐饮", "午餐");
        records.emplace_back("r3", "2025-01-20", 30.0, Record::Type::Expense, "餐饮", "晚餐");
        records.emplace_back("r4", "2025-02-01", 200.0, Record::Type::Income, "奖金", "年终奖");
        records.emplace_back("r5", "2025-02-10", 80.0, Record::Type::Expense, "购物", "买衣服");
    }

    void TearDown() override {
        std::filesystem::remove_all(testDir);
    }

    std::string testDir;
    std::unique_ptr<Storage> storage;
    std::vector<Record> records;
};

TEST_F(StorageStatisticsIntegrationTest, SaveLoadAndGenerateTimeSummary) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 5);
    
    // 生成时间统计
    Statistics stats("2025-01", Statistics::Mode::Time);
    auto summary = stats.generateByTime(loaded);
    
    EXPECT_DOUBLE_EQ(summary.income, 100.0);
    EXPECT_DOUBLE_EQ(summary.expense, 80.0);
    EXPECT_DOUBLE_EQ(summary.balance, 20.0);
    EXPECT_EQ(summary.count, 3);
}

TEST_F(StorageStatisticsIntegrationTest, SaveLoadAndGenerateCategorySummary) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    
    // 生成类别统计
    Statistics stats("", Statistics::Mode::Category);
    auto categorySummary = stats.generateByCategory(loaded);
    
    ASSERT_GT(categorySummary.size(), 0);
    
    // 检查餐饮类别
    bool foundFood = false;
    for (const auto& item : categorySummary) {
        if (item.category == "餐饮") {
            foundFood = true;
            EXPECT_DOUBLE_EQ(item.amount, 80.0);
            break;
        }
    }
    EXPECT_TRUE(foundFood);
}

TEST_F(StorageStatisticsIntegrationTest, SaveLoadAndGenerateFullPeriodSummary) {
    // 保存记录
    ASSERT_TRUE(storage->saveRecords(records));
    
    // 加载记录
    auto loaded = storage->loadRecords();
    
    // 生成全部期间统计
    Statistics stats("", Statistics::Mode::Time);
    auto summary = stats.generateByTime(loaded);
    
    EXPECT_DOUBLE_EQ(summary.income, 300.0);
    EXPECT_DOUBLE_EQ(summary.expense, 160.0);
    EXPECT_DOUBLE_EQ(summary.balance, 140.0);
    EXPECT_EQ(summary.count, 5);
}

TEST_F(StorageStatisticsIntegrationTest, SaveLoadCategoriesAndUseInStatistics) {
    // 创建并保存类别
    auto categories = Category::defaultCategories();
    auto custom = Category::addCustomCategory(categories, "自定义类别");
    ASSERT_TRUE(storage->saveCategories(categories));
    
    // 加载类别
    auto loadedCategories = storage->loadCategories();
    ASSERT_GT(loadedCategories.size(), 0);
    
    // 验证自定义类别被保存
    bool foundCustom = false;
    for (const auto& cat : loadedCategories) {
        if (cat.getName() == "自定义类别") {
            foundCustom = true;
            EXPECT_TRUE(cat.isCustom());
            break;
        }
    }
    EXPECT_TRUE(foundCustom);
}

