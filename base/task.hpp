#pragma once
#include <QString>

class Task {
public:
    Task(const QString& name = "New Task", const QString& conts = "", int import = 1);

    QString name;
    QString items;
    int import;

    QString bottom();
};
