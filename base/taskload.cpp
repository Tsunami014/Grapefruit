#include "taskload.hpp"
#include "tasks.hpp"
#include "font.hpp"
#include <memory>
#include <QPushButton>

using tasklist = std::vector<std::shared_ptr<Task>>;
std::unordered_map<QString, tasklist> alltasks;
QString current;
QString getCurrent() {
    if (alltasks.empty()) return {};
    if (current.isNull() || alltasks.find(current) == alltasks.end()) {
        current = alltasks.begin()->first;
    }
    return current;
}

void setTasksCatsLay(QLayout* lay, std::function<void()> redo, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    auto cur = getCurrent();
    for (const auto& [k, _] : alltasks) {
        auto* btn = new QPushButton(k, parent);
        btn->setProperty("fancy", true);
        btn->setProperty("optbtn", true);
        btn->setProperty("current", k==cur);
        resizeFont(btn, 1.25);
        lay->connect(btn, &QPushButton::clicked, lay, [key=k, redo](){
            current = key;
            redo();
        });
        lay->addWidget(btn);
    }
}
void setTasksLay(QLayout* lay, std::function<void(TaskBubble*, std::shared_ptr<Task>)> connect, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    auto cur = getCurrent();
    if (cur.isNull()) return;

    for (auto& t : alltasks.at(cur)) {
        auto bub = new TaskBubble(t, parent);
        connect(bub, t);
        lay->addWidget(bub);
    }

    auto btn = new QPushButton();
    btn->setProperty("fancy", true);
    btn->setIcon(QIcon(":/assets/UI/plus.svg"));
    btn->setIconSize(QSize(48, 44));
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //lay->connect(btn, &QPushButton::clicked, lay, );
    lay->addWidget(btn);
}

void loadTasks() {
    tasklist test = {};
    test.emplace_back(std::make_shared<Task>("Sample task 1"));
    test.emplace_back(std::make_shared<Task>("Sample task 2"));
    alltasks["Test"] = test;

    tasklist hi = {};
    hi.emplace_back(std::make_shared<Task>("Hello, world!"));
    alltasks["Hello"] = hi;
}
void saveTasks() {}
