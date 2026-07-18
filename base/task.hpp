#pragma once
#include <QString>

class Task {
public:
    Task(const QString& name = "New Task");

    QString name;
    QString items;
};
