#pragma once

#include <string>
#include <utility>
#include <vector>
#include "Record.h"

class Search {
public:
    Search();

    void setKeyword(const std::string &keyword);
    void setCategory(const std::string &category);
    void setTimeRange(const std::string &from, const std::string &to);

    const std::string& getKeyword() const;
    const std::string& getCategory() const;
    std::pair<std::string, std::string> getTimeRange() const;

    std::vector<Record> searchByKeyword(const std::vector<Record> &records) const;
    std::vector<Record> searchByCategory(const std::vector<Record> &records) const;
    std::vector<Record> searchByTime(const std::vector<Record> &records) const;
    void processSearchResults(const std::vector<Record> &records);
    void processRecordArray(const std::vector<Record> &records);

private:
    static bool between(const std::string &date, const std::string &from, const std::string &to);

    std::string keyword_;
    std::string category_;
    std::pair<std::string, std::string> timeRange_;
};

