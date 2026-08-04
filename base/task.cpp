#include "task.hpp"
#include "quals.hpp"
#include "saveesc.hpp"
#include "dbug.hpp"
#include "extra/itemopts.hpp"
#include "extra/date.hpp"
#include <QList>
#include <QRegularExpression>

#ifdef DEBUG
#include "importance.hpp"
#endif

uint nxtid = 0;

Task::Task(const QString& nam, const QString& items, int import, std::set<QString> quals, const QString& reasons)
    : id(nxtid++), name(nam), items(items), import(import), quals(quals), reasons(reasons) {}
bool Task::operator==(const Task& oth) const { return id == oth.id; }
bool Task::operator<(const Task& oth) const {
    // If this is less than oth it will be higher in the list
#ifdef DEBUG
    return basescore(oth) < basescore(*this);
#else
    // Swapping the values (like done for import) virtually reverses it
    return std::tie(oth.import, name) < std::tie(import, oth.name);
#endif
}

QString Task::toSave() {
    QStringList qualsOut;
    for (const auto& q : quals) {
        qualsOut << escape(q);
    }
    return
        escape(name)+';'+
        escape(items)+';'+
        QString::number(import)+';'+
        qualsOut.join('-')+';'+
        escape(reasons)
    ;
}
Task* Task::fromSaved(QString saved) {
    auto conts = saved.split(';');
    QString nam = deescape(conts.at(0));
    QString items = deescape(conts.at(1));
    int impt = conts.at(2).toInt();
    std::set<QString> quals;
    const auto& qks = qualkeys();
    for (const auto& q : conts.at(3).split('-')) {
        if (!q.isEmpty())
        if (QString txt = deescape(q); qks.find(txt) != qks.end())
            quals.insert(txt);
    }
    QString reasons = deescape(conts.at(4));
    return new Task(nam, items, impt, quals, reasons);
}

const QRegularExpression normlSpaces(R"(^\s*\n|\n\s*$|[ \t]+(?=\n)|\s+(?=\n[ \t]*\n))");
QString normaliseSpaces(QString txt) {
    auto it = normlSpaces.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        txt.remove(start, end - start);
        offs -= (end - start);
    }
    return txt;
}
void Task::setReasons(QString newrs) {
    reasons = normaliseSpaces(newrs);
}
void Task::setItems(QString newits) {
    items = normaliseSpaces(newits);
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
            time += parseStrTime(m.captured(1));
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
    QString suff = "  "+QString("!").repeated(import);
#ifdef DEBUG
    suff += "  $"+QString::number(basescore(*this));
#endif
    auto ps = Progress();
    if (ps.isEmpty()) return "No task items"+suff;

    if (ps.nextDue.isNull()) {
        return QString("%1 tasks (%2)").arg(ps.totTasks).arg(strTime(ps.totTime, true)) + suff;
    }
    QString txt = QString("%1 due %2")
        .arg(strTime(ps.nextTime, true)).arg(parseDate(ps.nextDue, true));

    if (ps.lastDue == ps.nextDue) return txt + suff;
    return txt + QString(", Total %1 due %2")
        .arg(strTime(ps.totTime, true)).arg(parseDate(ps.lastDue, true))
        + suff;
}
