// 模糊测试目标程序
// 用于afl++模糊测试
// 注意：不要捕获所有异常，让AFL++能够检测到崩溃
#include <iostream>
#include <fstream>
#include <string>
#include "src/Record.h"
#include "src/Storage.h"

// 从标准输入读取数据并解析为Record
int main(int argc, char* argv[]) {
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return 0;
    }
    
    // 尝试将输入解析为Record
    // 不捕获异常，让AFL++能够检测到崩溃
    Record record = Record::fromTSV(input);
    
    // 测试Storage功能
    Storage storage("fuzz_test_data");
    std::vector<Record> records;
    records.push_back(record);
    
    // 保存和加载
    storage.saveRecords(records);
    auto loaded = storage.loadRecords();
    
    // 测试copyCategoryName（包含缓冲区溢出缺陷）
    // 这个函数在输入超长类别名时会导致缓冲区溢出
    if (!loaded.empty()) {
        storage.copyCategoryName(loaded[0].getCategory());
    }
    
    return 0;
}

