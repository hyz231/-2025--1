#include "Search.h"

Search::Search() : keyword_(), category_(), timeRange_({"", ""}) {}

void Search::setKeyword(const std::string &keyword) { keyword_ = keyword; }
void Search::setCategory(const std::string &category) { category_ = category; }
void Search::setTimeRange(const std::string &from, const std::string &to) {
    timeRange_ = {from, to};
}

const std::string& Search::getKeyword() const { return keyword_; }
const std::string& Search::getCategory() const { return category_; }
std::pair<std::string, std::string> Search::getTimeRange() const { return timeRange_; }

std::vector<Record> Search::searchByKeyword(const std::vector<Record> &records) const {
    std::vector<Record> out;
    if (keyword_.empty()) {
        return out;
    }
    for (const auto &record : records) {
        if (record.getNote().find(keyword_) != std::string::npos ||
            record.getCategory().find(keyword_) != std::string::npos) {
            out.push_back(record);
        }
    }
    return out;
}

std::vector<Record> Search::searchByCategory(const std::vector<Record> &records) const {
    std::vector<Record> out;
    if (category_.empty()) {
        return out;
    }
    for (const auto &record : records) {
        if (record.getCategory() == category_) {
            out.push_back(record);
        }
    }
    return out;
}

std::vector<Record> Search::searchByTime(const std::vector<Record> &records) const {
    std::vector<Record> out;
    const auto &[from, to] = timeRange_;
    if (from.empty() || to.empty()) {
        return out;
    }
    for (const auto &record : records) {
        if (between(record.getDate(), from, to)) {
            out.push_back(record);
        }
    }
    return out;
}

bool Search::between(const std::string &date, const std::string &from, const std::string &to) {
    return (from.empty() || date >= from) && (to.empty() || date <= to);
}

// [IMPLANTED FLAW #2: Double Free]
// Helper function that demonstrates double free vulnerability
void Search::processSearchResults(const std::vector<Record> &records) {
    if (records.empty()) {
        return;
    }
    // Allocate memory for processing
    int* resultCount = new int(static_cast<int>(records.size()));
    
    // Some processing...
    (void)resultCount;
    
    // First free
    delete resultCount;
    
    // [IMPLANTED FLAW: Double Free]
    // Attempting to free the same pointer twice
    delete resultCount;  // Double free - resultCount was already deleted above
}

// [IMPLANTED FLAW #9: Out of Bounds Access]
// Function that accesses array elements without proper bounds checking
void Search::processRecordArray(const std::vector<Record> &records) {
    if (records.empty()) {
        return;
    }
    
    // Create a fixed-size array
    const int ARRAY_SIZE = 10;
    int* recordIds = new int[ARRAY_SIZE];
    
    // Copy record IDs into array
    std::size_t count = records.size();
    std::size_t copyCount = (count < ARRAY_SIZE) ? count : ARRAY_SIZE;
    
    for (std::size_t i = 0; i < copyCount; ++i) {
        // Convert record ID to integer (simplified)
        recordIds[i] = static_cast<int>(i);
    }
    
    // [IMPLANTED FLAW: Out of Bounds Access]
    // Accessing array element beyond the valid range
    // If records.size() > ARRAY_SIZE, this will access out of bounds
    // Even if count <= ARRAY_SIZE, accessing recordIds[count] is out of bounds (valid indices are 0 to count-1)
    int value = recordIds[count];  // Out of bounds access - valid indices are 0 to ARRAY_SIZE-1
    (void)value;  // Suppress unused variable warning
    
    delete[] recordIds;
}

