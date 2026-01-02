#pragma once

#include <string>
#include <vector>
#include "User.h"

class RecordUI {
public:
    explicit RecordUI(User &user);

    void showRecordForm();
    void selectCategory();
    void saveRecord();
    void colorCodeType() const;

private:
    User &user_;
    Record::Type selectedType_;
    std::string selectedDate_;
    double amount_;
    std::string note_;
    std::string selectedCategory_;

    void inputAmount();
    void inputDate();
    void inputNote();
    void applyCategorySelection(std::size_t index);
};

class StatisticsUI {
public:
    explicit StatisticsUI(User &user);

    void showTimeView();
    void showCategoryView();
    void toggleChartSummary();
    void displayResults(const Statistics::TimeSummary &summary,
                        const std::vector<Statistics::CategorySummaryItem> &items);

private:
    User &user_;
    bool showChart_;
};

class SearchUI {
public:
    explicit SearchUI(User &user);

    void showSearchForm();
    void filterByTime();
    void filterByKeyword();
    void filterByCategory();
    void displayResults(const std::vector<Record> &records) const;

private:
    User &user_;
};

class MainUI {
public:
    explicit MainUI(User &user);

    void showHomeScreen();
    void setupBottomNavigation();
    void navigateToRecord();
    void navigateToStatistics();
    void navigateToSearch();
    void navigateToProfile();
    void run();

private:
    User &user_;
    RecordUI recordUI_;
    StatisticsUI statisticsUI_;
    SearchUI searchUI_;

    void pause() const;
};

