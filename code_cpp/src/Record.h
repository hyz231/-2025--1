#pragma once

#include <string>

class Record {
public:
    enum class Type { Income, Expense };

    Record();
    Record(std::string id, std::string date, double amount, Type type, std::string category, std::string note);

    std::string getId() const;
    std::string getDate() const; // format YYYY-MM-DD
    double getAmount() const;
    Type getType() const;
    std::string getCategory() const;
    std::string getNote() const;

    std::string toTSV() const; // serialize for storage
    static Record fromTSV(const std::string &line);

    std::string getRecordInfo() const;

private:
    std::string id_;
    std::string date_;
    double amount_;
    Type type_;
    std::string category_;
    std::string note_;
};
