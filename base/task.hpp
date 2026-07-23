#pragma once
#include <QString>
#include <QDate>
#include <set>

struct progress {
    uint nextTasks;
    uint totTasks;
    float nextTime;
    float totTime;
    QDate nextDue;
    QDate lastDue;

    inline bool isEmpty() const { return totTasks == 0; }
};

class Task {
public:
    Task(const QString& name = "New Task",
        const QString& conts = "",
        int import = 1,
        std::set<QString> quals = {},
        const QString& reasons = "");
    bool operator<(const Task& other) const;

    QString name;
    QString items;
    int import;
    std::set<QString> quals;
    QString reasons;

    progress Progress();

    QString bottom();
    bool isNew() { return name == "New Task" && items.isEmpty() && quals.empty() && reasons.isEmpty(); }
};
