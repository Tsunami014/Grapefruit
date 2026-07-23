#include "task.hpp"
#include "dbug.hpp"
#include "extra/itemopts.hpp"
#include "extra/date.hpp"
#include <QList>

Task::Task(const QString& nam, const QString& conts, int import, std::set<QString> quals, const QString& reasons)
    : name(nam), items(conts), import(import), quals(quals), reasons(reasons) {}
bool Task::operator<(const Task& oth) const {
    // If this is less than oth it will be higher in the list
    // Swapping the values (like done for import) virtually reverses it
    return std::tie(oth.import, name) < std::tie(import, oth.name);
}

progress Task::Progress() {
    uint amnt = 0;
    float time = 0;
    QDate due = {};
    uint nxtamnt = 0;
    float nxttime = 0;
    QDate nxtdue = {};
    bool fst = true;
    for (const QString& line : items.split('\n')) {
        if (line.isEmpty() || line.startsWith(donePref)) continue;
        {auto m = timeRe.match(line);
        if (m.hasMatch()) {
            time += m.captured(1).toFloat();
            if (fst) nxttime = time;
        }}
        amnt++;
        if (fst) nxtamnt = amnt;
        {auto m = dateRe.match(line);
        if (m.hasMatch()) {
            QChar sep = m.captured(2)[0];
            due = QDate::fromString(m.captured(1).replace(sep, "-"), "yyyy-MM-dd");
            if (fst) {
                nxtdue = due;
                fst = false;
            }
        }}
    }
    return {nxtamnt, amnt, nxttime, time, nxtdue, due};
}

QString Task::bottom() {
#ifdef DEBUG
    QString pref = QString("!").repeated(import)+"  ";
#include "importance.hpp"
    pref += "$"+QString::number(basescore(std::make_shared<Task>(*this)))+"  ";
#endif
    auto ps = Progress();
    if (ps.isEmpty()) return pref+"No task items";

    if (ps.nextDue.isNull()) {
        return pref + QString("%1 tasks (%2)").arg(ps.totTasks).arg(parseTime(ps.totTime, true));
    }
    pref += QString("%1 due %2")
        .arg(parseTime(ps.nextTime, true)).arg(parseDate(ps.nextDue, true));

    if (ps.lastDue == ps.nextDue) return pref;
    return pref + QString(", %1 due %2")
        .arg(parseTime(ps.totTime, true)).arg(parseDate(ps.lastDue, true));
}
