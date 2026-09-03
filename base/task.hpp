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
        const QString& items = "",
        int import = 1,
        std::set<QString> quals = {},
        const QString& reasons = "",
        bool star = false);
    bool operator<(const Task& other) const;
    bool operator==(const Task& other) const;

    const uint id;

    QString name;
    int import;
    bool star;
    std::set<QString> quals;

    inline QString getReasons() const { return reasons; }
    void setReasons(QString newrs);
    inline QString getItems() const { return items; }
    void setItems(QString newits);

    progress Progress();

    QString top();
    QString bottom();
    bool isNew() { return name == "New Task" && items.isEmpty() && quals.empty() && reasons.isEmpty(); }

    QString toSave();
    static Task* fromSaved(QString saved);

protected:
    QString reasons;
    QString items;
};
