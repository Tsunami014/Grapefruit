#pragma once
#include <QString>
#include <set>

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

    QString bottom();
    bool isNew() { return name == "New Task" && items.isEmpty() && quals.empty() && reasons.isEmpty(); }
};
