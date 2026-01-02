#include "Record.h"
#include <sstream>
#include <utility>
#include <vector>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif
static std::string ensureUtf8(const std::string &text) {
#ifdef _WIN32
    if (text.empty()) return text;

    int testLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, nullptr, 0);
    if (testLen > 0) {
        return text; // already valid UTF-8
    }

    int wideLen = MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, nullptr, 0);
    if (wideLen <= 0) {
        return text;
    }
    std::wstring wide(static_cast<size_t>(wideLen - 1), L'\0');
    MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, wide.data(), wideLen);

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Len <= 0) {
        return text;
    }
    std::string utf8(static_cast<size_t>(utf8Len - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, utf8.data(), utf8Len, nullptr, nullptr);
    return utf8;
#else
    return text;
#endif
}

static std::vector<std::string> splitTSV(const std::string &s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == '\t') {
            out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    out.push_back(cur);
    return out;
}

Record::Record() : amount_(0.0), type_(Type::Expense) {}

Record::Record(std::string id, std::string date, double amount, Type type, std::string category, std::string note)
    : id_(std::move(id)), date_(std::move(date)), amount_(amount), type_(type), category_(std::move(category)), note_(std::move(note)) {}

std::string Record::getId() const { return id_; }
std::string Record::getDate() const { return date_; }
double Record::getAmount() const { return amount_; }
Record::Type Record::getType() const { return type_; }
std::string Record::getCategory() const { return category_; }
std::string Record::getNote() const { return note_; }

std::string Record::toTSV() const {
    std::ostringstream os;
    os << id_ << '\t' << date_ << '\t' << amount_ << '\t' << (type_ == Type::Income ? "I" : "E") << '\t' << category_ << '\t' << note_;
    return os.str();
}

Record Record::fromTSV(const std::string &line) {
    auto parts = splitTSV(line);
    if (parts.size() < 6) throw std::runtime_error("bad record line");
    std::string id = parts[0];
    std::string date = parts[1];
    double amount = 0.0;
    try {
        amount = std::stod(parts[2]);
    } catch (...) { amount = 0.0; }
    Type t = (parts[3] == "I") ? Type::Income : Type::Expense;
    std::string category = ensureUtf8(parts[4]);
    std::string note = ensureUtf8(parts[5]);
    return Record(id, date, amount, t, category, note);
}

std::string Record::getRecordInfo() const {
    std::ostringstream os;
    os << date_ << " | " << (type_ == Type::Income ? "+" : "-") << amount_ << " | " << category_ << " | " << note_;
    return os.str();
}
