#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include "../src/Storage.h"
#include "../src/Record.h"
#include "../src/Category.h"

class StorageTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建临时测试目录
        testDir = "tmp_test_storage";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        storage = std::make_unique<Storage>(testDir);
    }

    void TearDown() override {
        // 清理临时目录
        std::filesystem::remove_all(testDir);
        storage.reset();
    }

    std::string testDir;
    std::unique_ptr<Storage> storage;
};

// 测试构造函数
TEST_F(StorageTest, ConstructorWithDefaultDir) {
    Storage s;
    EXPECT_TRUE(s.ensureDataDir());
}

TEST_F(StorageTest, ConstructorWithCustomDir) {
    Storage s("custom_data");
    EXPECT_TRUE(s.ensureDataDir());
    std::filesystem::remove_all("custom_data");
}

// 测试保存和加载记录
TEST_F(StorageTest, SaveAndLoadEmptyRecords) {
    std::vector<Record> records;
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    EXPECT_EQ(loaded.size(), 0);
}

TEST_F(StorageTest, SaveAndLoadSingleRecord) {
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "测试");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].getId(), "r1");
    EXPECT_EQ(loaded[0].getDate(), "2025-01-01");
    EXPECT_DOUBLE_EQ(loaded[0].getAmount(), 100.0);
    EXPECT_EQ(loaded[0].getType(), Record::Type::Income);
    EXPECT_EQ(loaded[0].getCategory(), "工资");
    EXPECT_EQ(loaded[0].getNote(), "测试");
}

TEST_F(StorageTest, SaveAndLoadMultipleRecords) {
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "测试1");
    records.emplace_back("r2", "2025-01-02", 50.5, Record::Type::Expense, "餐饮", "测试2");
    records.emplace_back("r3", "2025-01-03", 200.0, Record::Type::Income, "奖金", "测试3");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 3);
    EXPECT_EQ(loaded[1].getAmount(), 50.5);
    EXPECT_EQ(loaded[2].getType(), Record::Type::Income);
}

TEST_F(StorageTest, SaveAndLoadRecordsWithSpecialCharacters) {
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "测试\n\t");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].getNote(), "测试\n\t");
}

TEST_F(StorageTest, SaveAndLoadRecordsWithZeroAmount) {
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 0.0, Record::Type::Expense, "其他", "零金额");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_DOUBLE_EQ(loaded[0].getAmount(), 0.0);
}

TEST_F(StorageTest, SaveAndLoadRecordsWithNegativeAmount) {
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", -100.0, Record::Type::Expense, "退款", "负数");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_DOUBLE_EQ(loaded[0].getAmount(), -100.0);
}

TEST_F(StorageTest, SaveAndLoadRecordsWithLargeAmount) {
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 999999.99, Record::Type::Income, "工资", "大金额");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_DOUBLE_EQ(loaded[0].getAmount(), 999999.99);
}

TEST_F(StorageTest, LoadRecordsFromNonExistentFile) {
    // 不保存任何记录，直接加载
    auto loaded = storage->loadRecords();
    EXPECT_EQ(loaded.size(), 0);
}

TEST_F(StorageTest, SaveRecordsOverwritesExistingFile) {
    std::vector<Record> records1;
    records1.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "第一次");
    EXPECT_TRUE(storage->saveRecords(records1));
    
    std::vector<Record> records2;
    records2.emplace_back("r2", "2025-01-02", 200.0, Record::Type::Expense, "餐饮", "第二次");
    EXPECT_TRUE(storage->saveRecords(records2));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].getId(), "r2");
}

// 测试保存和加载类别
TEST_F(StorageTest, SaveAndLoadEmptyCategories) {
    std::vector<Category> categories;
    EXPECT_TRUE(storage->saveCategories(categories));
    
    auto loaded = storage->loadCategories();
    EXPECT_EQ(loaded.size(), 0);
}

TEST_F(StorageTest, SaveAndLoadCustomCategories) {
    std::vector<Category> categories;
    categories.emplace_back("c1", "自定义1", true);
    categories.emplace_back("c2", "自定义2", true);
    
    EXPECT_TRUE(storage->saveCategories(categories));
    
    auto loaded = storage->loadCategories();
    ASSERT_EQ(loaded.size(), 2);
    EXPECT_EQ(loaded[0].getId(), "c1");
    EXPECT_EQ(loaded[0].getName(), "自定义1");
    EXPECT_TRUE(loaded[0].isCustom());
}

TEST_F(StorageTest, SaveCategoriesIgnoresNonCustom) {
    std::vector<Category> categories;
    categories.emplace_back("c1", "默认类别", false);
    categories.emplace_back("c2", "自定义类别", true);
    
    EXPECT_TRUE(storage->saveCategories(categories));
    
    auto loaded = storage->loadCategories();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].getId(), "c2");
    EXPECT_TRUE(loaded[0].isCustom());
}

TEST_F(StorageTest, LoadCategoriesFromNonExistentFile) {
    auto loaded = storage->loadCategories();
    EXPECT_EQ(loaded.size(), 0);
}

// 测试ensureDataDir
TEST_F(StorageTest, EnsureDataDirCreatesDirectory) {
    std::string newDir = "tmp_new_dir";
    std::filesystem::remove_all(newDir);
    
    Storage s(newDir);
    EXPECT_TRUE(s.ensureDataDir());
    EXPECT_TRUE(std::filesystem::exists(newDir));
    
    std::filesystem::remove_all(newDir);
}

TEST_F(StorageTest, EnsureDataDirHandlesExistingDirectory) {
    EXPECT_TRUE(storage->ensureDataDir());
    EXPECT_TRUE(storage->ensureDataDir()); // 再次调用应该也成功
}

// 边界测试
TEST_F(StorageTest, SaveRecordsWithEmptyFields) {
    std::vector<Record> records;
    records.emplace_back("", "", 0.0, Record::Type::Expense, "", "");
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_TRUE(loaded[0].getId().empty());
}

TEST_F(StorageTest, SaveRecordsWithVeryLongStrings) {
    std::vector<Record> records;
    std::string longString(1000, 'A');
    records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, longString, longString);
    
    EXPECT_TRUE(storage->saveRecords(records));
    
    auto loaded = storage->loadRecords();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].getCategory().length(), 1000);
}

