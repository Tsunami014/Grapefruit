#pragma once
#include <QString>

class Task {
public:
    Task(const QString& name = "New Task", const QString& conts = "");

    QString name;
    void setItems(QString nits);
    QString getItems();

    QString bottom();

private:
    QString items;
};
