#pragma once
#include <QString>

class Task {
public:
    Task(const QString& name = "New Task", const QString& conts = "", int import = 1);
    bool operator<(const Task& other) const;

    QString name;
    QString items;
    int import;

    QString bottom();

    bool isNew() { return name == "New Task" && items.isEmpty(); }
};
