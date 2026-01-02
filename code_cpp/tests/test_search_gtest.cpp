#include <gtest/gtest.h>
#include <memory>
#include "../src/Search.h"
#include "../src/Record.h"

class SearchTest : public ::testing::Test {
protected:
    void SetUp() override {
        search = std::make_unique<Search>();
        
        // 创建测试数据
        records.clear();
        records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "一月工资");
        records.emplace_back("r2", "2025-01-15", 50.0, Record::Type::Expense, "餐饮", "午餐");
        records.emplace_back("r3", "2025-02-01", 200.0, Record::Type::Income, "奖金", "年终奖");
        records.emplace_back("r4", "2025-02-10", 30.0, Record::Type::Expense, "餐饮", "晚餐");
        records.emplace_back("r5", "2025-03-01", 150.0, Record::Type::Income, "工资", "三月工资");
        records.emplace_back("r6", "2025-03-20", 80.0, Record::Type::Expense, "购物", "买衣服");
    }

    std::unique_ptr<Search> search;
    std::vector<Record> records;
};

// 测试构造函数
TEST_F(SearchTest, ConstructorInitializesEmpty) {
    Search s;
    EXPECT_TRUE(s.getKeyword().empty());
    EXPECT_TRUE(s.getCategory().empty());
    auto range = s.getTimeRange();
    EXPECT_TRUE(range.first.empty());
    EXPECT_TRUE(range.second.empty());
}

// 测试setKeyword和getKeyword
TEST_F(SearchTest, SetAndGetKeyword) {
    search->setKeyword("工资");
    EXPECT_EQ(search->getKeyword(), "工资");
    
    search->setKeyword("");
    EXPECT_TRUE(search->getKeyword().empty());
    
    search->setKeyword("测试关键词");
    EXPECT_EQ(search->getKeyword(), "测试关键词");
}

// 测试setCategory和getCategory
TEST_F(SearchTest, SetAndGetCategory) {
    search->setCategory("餐饮");
    EXPECT_EQ(search->getCategory(), "餐饮");
    
    search->setCategory("");
    EXPECT_TRUE(search->getCategory().empty());
}

// 测试setTimeRange和getTimeRange
TEST_F(SearchTest, SetAndGetTimeRange) {
    search->setTimeRange("2025-01-01", "2025-01-31");
    auto range = search->getTimeRange();
    EXPECT_EQ(range.first, "2025-01-01");
    EXPECT_EQ(range.second, "2025-01-31");
    
    search->setTimeRange("", "");
    range = search->getTimeRange();
    EXPECT_TRUE(range.first.empty());
    EXPECT_TRUE(range.second.empty());
}

// 测试searchByKeyword
TEST_F(SearchTest, SearchByKeywordFindsMatchingRecords) {
    search->setKeyword("工资");
    auto results = search->searchByKeyword(records);
    
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].getCategory(), "工资");
    EXPECT_EQ(results[1].getCategory(), "工资");
}

TEST_F(SearchTest, SearchByKeywordFindsInNote) {
    search->setKeyword("午餐");
    auto results = search->searchByKeyword(records);
    
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].getNote(), "午餐");
}

TEST_F(SearchTest, SearchByKeywordFindsInCategory) {
    search->setKeyword("餐饮");
    auto results = search->searchByKeyword(records);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.getCategory(), "餐饮");
    }
}

TEST_F(SearchTest, SearchByKeywordWithEmptyKeyword) {
    search->setKeyword("");
    auto results = search->searchByKeyword(records);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByKeywordWithNoMatches) {
    search->setKeyword("不存在的关键词");
    auto results = search->searchByKeyword(records);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByKeywordCaseSensitive) {
    search->setKeyword("工资");
    auto results1 = search->searchByKeyword(records);
    
    search->setKeyword("工");
    auto results2 = search->searchByKeyword(records);
    
    EXPECT_GT(results1.size(), 0);
    EXPECT_GT(results2.size(), 0);
}

TEST_F(SearchTest, SearchByKeywordWithPartialMatch) {
    search->setKeyword("餐");
    auto results = search->searchByKeyword(records);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_TRUE(r.getCategory().find("餐") != std::string::npos);
    }
}

// 测试searchByCategory
TEST_F(SearchTest, SearchByCategoryFindsMatchingRecords) {
    search->setCategory("餐饮");
    auto results = search->searchByCategory(records);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.getCategory(), "餐饮");
    }
}

TEST_F(SearchTest, SearchByCategoryWithEmptyCategory) {
    search->setCategory("");
    auto results = search->searchByCategory(records);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByCategoryWithNoMatches) {
    search->setCategory("不存在的类别");
    auto results = search->searchByCategory(records);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByCategoryExactMatch) {
    search->setCategory("工资");
    auto results = search->searchByCategory(records);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_EQ(r.getCategory(), "工资");
    }
}

// 测试searchByTime
TEST_F(SearchTest, SearchByTimeFindsRecordsInRange) {
    search->setTimeRange("2025-01-01", "2025-01-31");
    auto results = search->searchByTime(records);
    
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_GE(r.getDate(), "2025-01-01");
        EXPECT_LE(r.getDate(), "2025-01-31");
    }
}

TEST_F(SearchTest, SearchByTimeWithEmptyRange) {
    search->setTimeRange("", "");
    auto results = search->searchByTime(records);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByTimeWithOnlyFromDate) {
    search->setTimeRange("2025-02-01", "");
    auto results = search->searchByTime(records);
    EXPECT_EQ(results.size(), 0); // 因为to为空，应该返回空
}

TEST_F(SearchTest, SearchByTimeWithOnlyToDate) {
    search->setTimeRange("", "2025-01-31");
    auto results = search->searchByTime(records);
    EXPECT_EQ(results.size(), 0); // 因为from为空，应该返回空
}

TEST_F(SearchTest, SearchByTimeSingleDay) {
    search->setTimeRange("2025-01-01", "2025-01-01");
    auto results = search->searchByTime(records);
    
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].getDate(), "2025-01-01");
}

TEST_F(SearchTest, SearchByTimeCrossMonthBoundary) {
    search->setTimeRange("2025-01-15", "2025-02-15");
    auto results = search->searchByTime(records);
    
    ASSERT_GE(results.size(), 2);
    for (const auto& r : results) {
        EXPECT_GE(r.getDate(), "2025-01-15");
        EXPECT_LE(r.getDate(), "2025-02-15");
    }
}

TEST_F(SearchTest, SearchByTimeNoMatches) {
    search->setTimeRange("2025-12-01", "2025-12-31");
    auto results = search->searchByTime(records);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByTimeAllRecords) {
    search->setTimeRange("2025-01-01", "2025-12-31");
    auto results = search->searchByTime(records);
    EXPECT_EQ(results.size(), 6);
}

// 边界测试
TEST_F(SearchTest, SearchByKeywordWithEmptyRecords) {
    std::vector<Record> emptyRecords;
    search->setKeyword("工资");
    auto results = search->searchByKeyword(emptyRecords);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByCategoryWithEmptyRecords) {
    std::vector<Record> emptyRecords;
    search->setCategory("餐饮");
    auto results = search->searchByCategory(emptyRecords);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByTimeWithEmptyRecords) {
    std::vector<Record> emptyRecords;
    search->setTimeRange("2025-01-01", "2025-01-31");
    auto results = search->searchByTime(emptyRecords);
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SearchTest, SearchByKeywordWithVeryLongKeyword) {
    std::string longKeyword(1000, 'A');
    search->setKeyword(longKeyword);
    auto results = search->searchByKeyword(records);
    EXPECT_EQ(results.size(), 0);
}

