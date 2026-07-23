#include "taskload.hpp"
#include "task.hpp"
#include "font.hpp"
#include "wids/confirm.hpp"
#include "wids/taskbbl.hpp"
#include <memory>
#include <QPushButton>
#include <QMessageBox>
#include <QStyle>
#include <QLabel>

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
    static std::vector<QPushButton*> btns;
    auto cur = getCurrent();

    uint idx = 0;
    for (const auto& [k, _] : alltasks) {
        if (btns.size() <= idx) {
            auto* btn = new QPushButton(k, parent);
            btn->setProperty("fancy", true);
            btn->setProperty("optbtn", true);
            btn->setProperty("current", k==cur);
            resizeFont(btn, 1.2);
            QObject::connect(btn, &QPushButton::clicked, lay, [btn, redo](){
                current = btn->text();
                redo();
            });
            lay->addWidget(btn);
            btns.push_back(btn);
        } else {
            auto* btn = btns.at(idx);
            btn->setText(k);
            btn->setProperty("current", k==cur);
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
        }
        idx++;
    }
    while (btns.size() > idx) {
        btns.back()->deleteLater();
        btns.pop_back();
    }
}
void setTasksLay(QLayout* lay, std::function<void(std::shared_ptr<Task>, bool)> press, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    {auto* labl = new QLabel("Tasks", parent);
    labl->setContentsMargins(4,8,4,4);
    labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    labl->setAlignment(Qt::AlignCenter);
    lay->addWidget(labl);}

    auto cur = getCurrent();
    if (cur.isNull()) return;
    sortTasks();

    for (auto& t : alltasks.at(cur)) {
        auto bub = new TaskBubble(t, parent);
        QObject::connect(bub, &TaskBubble::clicked, [=](){
            press(t, false);
        });
        lay->addWidget(bub);
    }

    QWidget* space = new QWidget(parent);
    space->setFixedSize(0, 8);
    lay->addWidget(space);

    auto btn = new QPushButton();
    btn->setProperty("fancy", true);
    btn->setIcon(QIcon(":/assets/UI/plus.svg"));
    btn->setIconSize(QSize(48, 44));
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    lay->connect(btn, &QPushButton::clicked, lay, [=](){
        press(newtask(), true);
    });
    lay->addWidget(btn);
}

void newCategory(QWidget* parent, QString name) {
    if (alltasks.find(name) == alltasks.end()) {
        alltasks[name] = {};
    } else {
        confirm(parent, "The category '" + name + "' already exists!", Conf_OK);
    }
    current = name;
}

bool renameCategory(QWidget* parent, QString newname) {
    auto cur = getCurrent();
    if (cur.isNull()) {
        confirm(parent, "No category is selected!\nCreate a category first!", Conf_OK);
        return false;
    }
    if (cur == newname) return false;

    if (auto it = alltasks.find(newname); it != alltasks.end()) {
        auto resp = confirm(parent,
            "The category name '" + newname + "' already exists! Do you want to overwrite it (yes) or use the next avaliable name (no)?",
            Conf_YESNOCANCEL);
        if (resp == QDialogButtonBox::RejectRole) return false;
        if (resp == QDialogButtonBox::YesRole) {
            alltasks.erase(it);
        } else {
            do {
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
            } while (alltasks.find(newname) != alltasks.end());
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
    if (confirm(parent, "Are you sure you want to delete the category '" + cur + "' and all its tasks?",
          Conf_YESNO) != QDialogButtonBox::YesRole) {
        return false;
    }
    alltasks.erase(alltasks.find(cur));
    current = {};
    return true;
}


void removeTask(std::shared_ptr<Task> task, bool trycurfirst) {
    // Most likely to be in the current category, so try there first
    QString cur;
    if (trycurfirst) {
        auto cur = getCurrent();
        if (!cur.isNull())
        if (auto it = alltasks.find(cur); it != alltasks.end()) {
            auto& list = it->second;
            auto oldsze = list.size();

            list.erase(
                std::remove(list.begin(), list.end(), task),
                list.end()
            );
            if (list.size() != oldsze) return;
            // Otherwise, was not found
        }
    }

    for (auto& [key, list] : alltasks) {
        if (key == cur) continue;

        auto oldsze = list.size();
        list.erase(
            std::remove(list.begin(), list.end(), task),
            list.end()
        );
        if (list.size() != oldsze) break;
    }
}

std::shared_ptr<Task> newtask() {
    auto cur = getCurrent();
    if (!cur.isNull()) {
        auto ntsk = std::make_shared<Task>();
        alltasks[cur].push_back(ntsk);
        return ntsk;
    }
    return nullptr;
}
std::shared_ptr<Task> newtask(QString cat) {
    auto it = alltasks.find(cat);
    if (it != alltasks.end()) {
        auto ntsk = std::make_shared<Task>();
        it->second.push_back(ntsk);
        return ntsk;
    }
    return nullptr;
}

void loadTasks() {
    tasklist test;
    test.emplace_back(std::make_shared<Task>("Sample task 1"));
    test.emplace_back(std::make_shared<Task>("Sample task 2"));
    alltasks["Test"] = test;

    tasklist hi;
    hi.emplace_back(std::make_shared<Task>("Hello, world!",
        "+1.5h Hello!\n"
        "✔ +8.5h Do something cool\n"
        "+1.5h I am awesome! @2026-07-25\n"
        "+0.5h NEXT\n"
        "+1.5h Hiness @2026-08-06\n"
    ));
    alltasks["Hello"] = hi;
    sortTasks(true);
}
void saveTasks() {}

bool sortT(const std::shared_ptr<Task>& lhs, const std::shared_ptr<Task>& rhs) {
    if (!lhs || !rhs) return !lhs && rhs;
    return *lhs < *rhs;
}
void sortTasks(bool all) {
    if (all) {
        for (auto& [_, list] : alltasks) {
            std::sort(list.begin(), list.end(), sortT);
        }
    } else {
        auto cur = getCurrent();
        if (!cur.isNull())
        if (auto it = alltasks.find(cur); it != alltasks.end()) {
            auto& list = it->second;
            std::sort(list.begin(), list.end(), sortT);
        }
    }
}
