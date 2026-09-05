#include "taskload.hpp"
#include "task.hpp"
#include "importance.hpp"
#include "font.hpp"
#include "saveesc.hpp"
#include "tasklist.hpp"
#include "wids/confirm.hpp"
#include "wids/taskbbl.hpp"
#include <memory>
#include <QPushButton>
#include <QMessageBox>
#include <QStyle>
#include <QLabel>
#include <QStandardPaths>
#include <QDir>

QString current; // \3 is the 'Star' category
QString getCurrent() {
    if (current == "\3") return "\3";
    if (alltasks.empty()) return {};
    if (current.isNull() || alltasks.find(current) == alltasks.end()) {
        current = alltasks.begin()->first;
    }
    return current;
}
QString curCatName() {
    QString cur = getCurrent();
    if (cur.isNull()) return {};
    if (cur == "\3") return "Starred";
    return cur;
}
void showNoCat() { current = {}; }
void showStar() { current = "\3"; }
bool isStarCat() { return current == "\3"; }

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
        lay->removeWidget(btns.back());
        btns.back()->deleteLater();
        btns.pop_back();
    }
}
void setTasksLay(QLayout* lay, std::function<void(std::shared_ptr<Task>)> press, std::function<void()> reload, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    auto cur = getCurrent();
    if (cur.isNull()) return;
    sortTasks();

    if (cur == "\3") {
        for (const auto& [key, tasks] : alltasks) {
            for (const auto& t : tasks) {
                if (!t || !t->star) continue;
                auto bub = new TaskBubble(t, parent);
                QObject::connect(bub, &TaskBubble::toggleStar, [=](){
                    t->star = !t->star;
                    saveTasks();
                    reload();
                });
                QObject::connect(bub, &TaskBubble::clicked, [=](){
                    press(t);
                });
                lay->addWidget(bub);
            }
        }
    } else {
        for (const auto& t : alltasks.at(cur)) {
            auto bub = new TaskBubble(t, parent);
            QObject::connect(bub, &TaskBubble::toggleStar, [=](){
                t->star = !t->star;
                saveTasks();
                reload();
            });
            QObject::connect(bub, &TaskBubble::clicked, [=](){
                press(t);
            });
            lay->addWidget(bub);
        }
    }
}

void newCategory(QWidget* parent, QString name) {
    name = name.replace('\3', "");
    if (alltasks.find(name) == alltasks.end()) {
        alltasks[name] = {};
        saveTasks();
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
    if (cur == '\3') {
        confirm(parent, "Cannot rename the star category!", Conf_OK);
        return false;
    }
    newname = newname.replace('\3', "");
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
    saveTasks();
    return true;
}

bool deleteCategory(QWidget* parent) {
    auto cur = getCurrent();
    if (cur.isNull()) return false;
    if (cur == '\3') {
        confirm(parent, "Cannot delete the star category!", Conf_OK);
        return false;
    }
    if (confirm(parent, "Are you sure you want to delete the category '" + cur + "' AND ALL ITS TASKS?",
          Conf_YESNO) != QDialogButtonBox::YesRole) {
        return false;
    }
    alltasks.erase(alltasks.find(cur));
    current = {};
    saveTasks();
    return true;
}


QStringList allTaskCats() {
    QStringList out;
    for (auto& [cat, _] : alltasks) {
        out << cat;
    }
    return out;
}

QString taskCategory(std::shared_ptr<Task> task) {
    for (auto& [key, list] : alltasks) {
        if (std::find(list.begin(), list.end(), task) != list.end()) {
            return key;
        }
    }
    return {};
}

void changeCat(std::shared_ptr<Task> task, QString newcat, QString fromcat) {
    if (fromcat == '\3') fromcat = {};
    newcat = newcat.replace('\3', "");
    if (newcat == fromcat) return;

    // Try the stated category first
    bool done = false;
    if (!fromcat.isNull()) {
        if (auto it = alltasks.find(fromcat); it != alltasks.end()) {
            auto& list = it->second;
            auto oldsze = list.size();

            list.erase(
                std::remove(list.begin(), list.end(), task),
                list.end()
            );
            done = list.size() != oldsze;
        }
    }
    if (!done) {
        for (auto& [key, list] : alltasks) {
            if (key == fromcat) continue;

            auto oldsze = list.size();
            list.erase(
                std::remove(list.begin(), list.end(), task),
                list.end()
            );
            if (list.size() != oldsze) break;
        }
    }

    if (auto it = alltasks.find(newcat); it != alltasks.end()) {
        it->second.push_back(task);
        current = newcat;
    }
    saveTasks();
}

void removeTask(std::shared_ptr<Task> task, bool trycurfirst) {
    // Most likely to be in the current category, so try there first
    QString cur;
    if (trycurfirst) {
        auto cur = getCurrent();
        if (!cur.isNull() && cur != "\3")
        if (auto it = alltasks.find(cur); it != alltasks.end()) {
            auto& list = it->second;
            auto oldsze = list.size();

            list.erase(
                std::remove(list.begin(), list.end(), task),
                list.end()
            );
            if (list.size() != oldsze) {
                saveTasks();
                return;
            }
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
        if (list.size() != oldsze) {
            saveTasks();
            return;
        }
    }
}

std::shared_ptr<Task> newtask() {
    if (auto cur = getCurrent(); !cur.isNull() && cur != "\3") {
        auto ntsk = std::make_shared<Task>();
        alltasks[cur].push_back(ntsk);
        saveTasks();
        return ntsk;
    }
    return nullptr;
}
std::shared_ptr<Task> newtask(QString cat) {
    cat = cat.replace('\3', "");
    if (auto it = alltasks.find(cat); it != alltasks.end()) {
        auto ntsk = std::make_shared<Task>();
        it->second.push_back(ntsk);
        saveTasks();
        return ntsk;
    }
    return nullptr;
}

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


QString datapth() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir d(path);
    if (!d.exists()) d.mkpath(path);
    return path + "/data.sav";
}
void loadTasks() {
    for (auto& [_, tasks] : alltasks) { tasks.clear(); }
    alltasks.clear();
#ifdef APP_DEBUG
    qDebug() << "Loading data from" << datapth();
#endif

    QFile file(datapth());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // File does not exist, so MAKE IT EXIST
        defTasks();
        sortTasks(true);
        return;
    }
    QTextStream in(&file);
    QString line = in.readLine();
    QString title;
    tasklist tl;
    while (!line.isNull()) {
        if (line == "") {
        } else if (line[0] == '\5') {
            auto parts = line.mid(1).split(';');
            qWarning() << "Unknown setting key:" << parts[0];
        } else if (line[0] == '\4') {
            if (!title.isNull()) {
                alltasks[title] = tl;
                tl = {};
            }
            title = deescape(line.mid(1));
        } else {
            tl.push_back(std::shared_ptr<Task>(Task::fromSaved(line)));
        }
        line = in.readLine();
    }
    if (!title.isNull()) {
        alltasks[title] = tl;
    }
    file.close();
}
void saveTasks() {
    QFile file(datapth());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // This *should* never fail but it's still good to check
        qFatal() << "Failed writing to data file!";
        return;
    }
    QTextStream out(&file);
    for (const auto& [key, tasks] : alltasks) {
        out << "\4" << escape(key) << "\n";
        for (const auto& tsk : tasks) {
            if (!tsk) continue;
            out << tsk->toSave() << "\n";
        }
    }
    file.close();
}

void delAllTasks() {
    for (auto& [_, tasks] : alltasks) { tasks.clear(); }
    alltasks.clear();
    saveTasks();
}
void resetTasks() {
    for (auto& [_, tasks] : alltasks) { tasks.clear(); }
    alltasks.clear();
    defTasks();
    sortTasks(true);
    saveTasks();
}

QString getAllTasksDebugInfo() {
    QString out = "=== Tasks ===";
    const auto extra = baseextra();
    for (const auto& [key, tasks] : alltasks) {
        out += "\n--- " + key + " ---";
        std::vector<std::pair<int, std::shared_ptr<Task>>> scored;
        for (const auto& tsk : tasks) {
            if (tsk) scored.emplace_back(basescore(*tsk), tsk);
        }
        std::sort(scored.begin(), scored.end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });
        for (const auto& [score, tsk] : scored) {
            out += "\n  " + tsk->name + "  " + QString::number(score) + " - " + QString::number(score+extra);
        }
    }
    return out;
}

std::shared_ptr<Task> best = nullptr;
bool init = false;
std::shared_ptr<Task> getBestTask(bool ignoreinit) {
    if ((init || ignoreinit) && !best) {
        best = nullptr;
        int highsco = std::numeric_limits<int>::min();

        for (const auto& [key, tasks] : alltasks) {
            for (const auto& tsk : tasks) {
                if (!tsk) continue;
                int sco = score(tsk);
                if (sco > highsco) {
                    highsco = sco;
                    best = tsk;
                }
            }
        }
        if (best != nullptr) justSuggested(best);
    }
    return best;
}
void resetBest() { best = nullptr; init = true; }
void removeBest() { best = nullptr; init = false; }
