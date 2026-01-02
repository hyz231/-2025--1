// 缺陷检测测试用例
// 这些测试用例专门用于发现代码中的缺陷
#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "../src/Storage.h"
#include "../src/Search.h"
#include "../src/Statistics.h"
#include "../src/Record.h"

// 测试缺陷1: 内存泄漏检测
// 注意: 这个测试需要使用内存检测工具（如Valgrind）来发现内存泄漏
TEST(DefectDetection, MemoryLeakInLoadRecords) {
    std::string testDir = "tmp_test_defect1";
    std::filesystem::remove_all(testDir);
    std::filesystem::create_directories(testDir);
    
    Storage storage(testDir);
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "测试");
    storage.saveRecords(records);
    
    // 多次调用loadRecords，内存泄漏会累积
    for (int i = 0; i < 10; ++i) {
        auto loaded = storage.loadRecords();
        EXPECT_EQ(loaded.size(), 1);
    }
    
    // 使用Valgrind运行此测试可以发现内存泄漏:
    // valgrind --leak-check=full ./bin/test_defects.exe --gtest_filter=DefectDetection.MemoryLeakInLoadRecords
    
    std::filesystem::remove_all(testDir);
}

// 测试缺陷2: 缓冲区溢出检测
TEST(DefectDetection, BufferOverflowInCopyCategoryName) {
    Storage storage;
    
    // 测试正常长度的类别名（应该不会崩溃）
    storage.copyCategoryName("正常类别名");
    
    // 测试超长类别名（可能导致缓冲区溢出）
    // 注意: 这个测试在某些环境下可能会崩溃或产生未定义行为
    std::string longCategoryName(100, 'A');  // 100个'A'字符，超过64字节缓冲区
    EXPECT_NO_THROW({
        storage.copyCategoryName(longCategoryName);
    });
    
    // 使用AddressSanitizer编译可以发现缓冲区溢出:
    // 编译时添加 -fsanitize=address 选项
}

// 测试缺陷3: 双重释放检测
TEST(DefectDetection, DoubleFreeInProcessSearchResults) {
    Search search;
    std::vector<Record> records;
    records.emplace_back("r1", "2025-01-01", 100.0, Record::Type::Income, "工资", "测试");
    
    // 这个测试可能会崩溃或产生未定义行为
    // 使用AddressSanitizer可以发现双重释放:
    // 编译时添加 -fsanitize=address 选项
    EXPECT_DEATH({
        search.processSearchResults(records);
    }, ".*");
    
    // 如果上面的EXPECT_DEATH失败，说明程序没有崩溃，但缺陷仍然存在
    // 可以使用Valgrind检测:
    // valgrind --leak-check=full ./bin/test_defects.exe --gtest_filter=DefectDetection.DoubleFreeInProcessSearchResults
}

// 测试缺陷4: NULL指针解引用检测
TEST(DefectDetection, NullPointerDereferenceInFormatSummaryData) {
    Statistics stats;
    Statistics::TimeSummary summary;
    summary.period = "2025-01";
    summary.count = 100;  // 小于1000，所以summaryBuffer会是NULL
    
    // 这个测试应该会崩溃（如果缺陷存在）
    // 使用AddressSanitizer可以发现NULL指针解引用:
    // 编译时添加 -fsanitize=address 选项
    EXPECT_DEATH({
        stats.formatSummaryData(summary);
    }, ".*");
    
    // 如果上面的EXPECT_DEATH失败，说明程序没有崩溃，但缺陷仍然存在
}

// 测试缺陷4的另一个场景: 不会崩溃的情况
TEST(DefectDetection, NullPointerDereferenceNoCrash) {
    Statistics stats;
    Statistics::TimeSummary summary;
    summary.period = "2025-01";
    summary.count = 2000;  // 大于1000，所以summaryBuffer会被分配
    
    // 这种情况下不应该崩溃
    EXPECT_NO_THROW({
        stats.formatSummaryData(summary);
    });
}

