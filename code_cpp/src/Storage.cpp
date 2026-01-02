#include "Storage.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstring>

Storage::Storage(const std::string &dir) : dir_(dir) {}

std::string Storage::recordsFile() const {
    std::filesystem::path p(dir_);
    p /= "records.txt";
    return p.string();
}

std::string Storage::categoriesFile() const {
    std::filesystem::path p(dir_);
    p /= "categories.txt";
    return p.string();
}

bool Storage::ensureDataDir() const {
    std::filesystem::path p(dir_);
    std::error_code ec;
    if (!std::filesystem::exists(p, ec)) {
        return std::filesystem::create_directories(p, ec);
    }
    return true;
}

bool Storage::saveRecords(const std::vector<Record> &records) const {
    if (!ensureDataDir()) {
        return false;
    }
    std::ofstream ofs(recordsFile(), std::ios::trunc);
    if (!ofs) {
        return false;
    }
    for (const auto &r : records) {
        ofs << r.toTSV() << '\n';
    }
    return true;
}

bool Storage::saveCategories(const std::vector<Category> &categories) const {
    if (!ensureDataDir()) {
        return false;
    }
    std::ofstream ofs(categoriesFile(), std::ios::trunc);
    if (!ofs) {
        return false;
    }
    for (const auto &c : categories) {
        if (!c.isCustom()) {
            continue;
        }
        ofs << c.getId() << '\t' << c.getName() << '\t' << (c.isCustom() ? "1" : "0") << '\n';
    }
    return true;
}

std::vector<Record> Storage::loadRecords() const {
    std::vector<Record> out;
    std::ifstream ifs(recordsFile());
    if (!ifs) {
        return out;
    }
    // [IMPLANTED FLAW #1: Memory Leak]
    // Allocated buffer but never freed - memory leak
    char* buffer = new char[1024];
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) {
            continue;
        }
        // Buffer is allocated but never used or freed
        try {
            out.push_back(Record::fromTSV(line));
        } catch (const std::exception &e) {
            std::cerr << "Warning: failed to parse record line: " << e.what() << std::endl;
        }
    }
    // Memory leak: buffer is never deleted
    return out;
}

std::vector<Category> Storage::loadCategories() const {
    std::vector<Category> out;
    std::ifstream ifs(categoriesFile());
    if (!ifs) {
        return out;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) {
            continue;
        }
        std::string id;
        std::string name;
        std::string customFlag;
        std::istringstream iss(line);
        if (!std::getline(iss, id, '\t')) {
            continue;
        }
        if (!std::getline(iss, name, '\t')) {
            continue;
        }
        if (!std::getline(iss, customFlag, '\t')) {
            customFlag = "1";
        }
        bool custom = customFlag == "1";
        out.emplace_back(id, name, custom);
    }
    return out;
}

// [FIXED: Buffer Overflow]
// Function that safely copies data into a fixed-size buffer with bounds checking
// [IMPLANTED FLAW #5: Buffer Overflow]
// Function that copies data into a fixed-size buffer without checking bounds
void Storage::copyCategoryName(const std::string &categoryName) {
    // Fixed-size buffer
    char buffer[64];
    
    // [IMPLANTED FLAW: Buffer Overflow]
    // Using strcpy without checking if categoryName fits in buffer
    // This will overflow if categoryName.length() >= 64
    std::strcpy(buffer, categoryName.c_str());  // Buffer overflow if categoryName is too long
    
    // Use buffer (suppress unused warning)
    (void)buffer;
}

