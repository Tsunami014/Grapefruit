#include "taskload.hpp"
#include "tasks.hpp"
#include "font.hpp"
#include "wids/taskbbl.hpp"
#include "wids/taskOverlay.hpp"
#include <memory>
#include <QPushButton>

std::vector<std::shared_ptr<Task>> taskslist;

void setTasksCatsLay(QLayout* lay, std::function<void()> redo, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    //for (auto& o : taskslist) {
        auto* btn = new QPushButton("Hello!", parent);
        btn->setProperty("fancy", true);
        btn->setProperty("optbtn", true);
        resizeFont(btn, 1.25);
        lay->connect(btn, &QPushButton::clicked, lay, redo);
        lay->addWidget(btn);
    //}
}
void setTasksLay(QLayout* lay, QGridLayout* tlay, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    for (auto& t : taskslist) {
        auto bub = new TaskBubble(t, parent);
        QObject::connect(bub, &TaskBubble::clicked, [=](){
            tlay->addWidget(new TaskOverlay(t, parent), 0, 0);
        });
        lay->addWidget(bub);
    }
}

void loadTasks() {
    taskslist.emplace_back(std::make_shared<Task>("Sample task 1"));
    taskslist.emplace_back(std::make_shared<Task>("Sample task 2"));
}
void saveTasks() {}
