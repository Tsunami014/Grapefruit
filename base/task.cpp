#include "task.hpp"

Task::Task(const QString& nam, const QString& conts, int import)
    : name(nam), items(conts), import(import) {}

QString Task::bottom() {
    return QString("!").repeated(import);
}
