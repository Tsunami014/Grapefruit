#include "taskload.hpp"
#include "tasks.hpp"
#include "font.hpp"
#include <memory>
#include <QPushButton>
#include <QMessageBox>

using tasklist = std::vector<std::shared_ptr<Task>>;
std::map<QString, tasklist> alltasks;
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

void newCategory(QString name) {
    if (alltasks.find(name) == alltasks.end()) {
        alltasks[name] = {};
    }
    current = name;
}

bool renameCategory(QString newname) {
    auto cur = getCurrent();
    if (cur.isNull() || cur == newname) return false;

    while (alltasks.find(newname) != alltasks.end()) {
        if (newname.isEmpty()) {
            newname = "1";
        } else {
            int digi = newname.at(newname.length()-1).digitValue();
            if (digi == -1) {
                newname += "2";
            } else if (digi == 9) {
                newname = newname.sliced(0, -1) + "10";
            } else {
                newname = newname.sliced(0, -1) + QString::number(digi+1);
            }
        }
    }

    if (auto node = alltasks.extract(cur); !node.empty()) {
        node.key() = newname;
        alltasks.insert(std::move(node));
    }
    current = newname;
    return true;
}

bool deleteCategory(QWidget* parent) {
    auto cur = getCurrent();
    if (cur.isNull()) return false;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "Confirmation",
          "Are you sure you want to delete the category '" + cur + "' and all its tasks?",
          QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return false;
    alltasks.erase(alltasks.find(cur));
    current = {};
    return true;
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
