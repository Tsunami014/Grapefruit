#include "task.hpp"

Task::Task(const QString& nam, const QString& conts) : name(nam) { setItems(conts); }

QString Task::getItems() { return items; }
void Task::setItems(QString nits) {
    items = nits;
}

QString Task::bottom() {
    return "Bottom text!";
}
