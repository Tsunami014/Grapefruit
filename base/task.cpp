#include "task.hpp"

Task::Task(const QString& nam, const QString& conts, int import, std::set<QString> quals, const QString& reasons)
    : name(nam), items(conts), import(import), quals(quals), reasons(reasons) {}
bool Task::operator<(const Task& oth) const {
    // If this is less than oth it will be higher in the list
    // Swapping the values (like done for import) virtually reverses it
    return std::tie(oth.import, name) < std::tie(import, oth.name);
}

QString Task::bottom() {
    return QString("!").repeated(import);
}
