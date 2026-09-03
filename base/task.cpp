#include "task.hpp"
#include "quals.hpp"
#include "saveesc.hpp"
#include "extra/itemopts.hpp"
#include "extra/date.hpp"
#include <QList>
#include <QRegularExpression>

#ifdef APP_DEBUG
#include "importance.hpp"
#endif

uint nxtid = 0;

Task::Task(const QString& nam, const QString& items, int import, std::set<QString> quals, const QString& reasons, bool star)
    : id(nxtid++), name(nam), items(items), import(import), quals(quals), reasons(reasons), star(star) {}
bool Task::operator==(const Task& oth) const { return id == oth.id; }
bool Task::operator<(const Task& oth) const {
    // If this is less than oth it will be higher in the list
#ifdef APP_DEBUG
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
        (star? "-":"")+
        escape(name)+';'+
        escape(items)+';'+
        QString::number(import)+';'+
        qualsOut.join('-')+';'+
        escape(reasons)
    ;
}
Task* Task::fromSaved(QString saved) {
    bool star = saved.startsWith('-');
    QStringList conts;
    if (star) conts = saved.mid(1).split(';');
    else conts = saved.split(';');
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
    return new Task(nam, items, impt, quals, reasons, star);
}

const QRegularExpression normlSpaces(R"(^\s*\n|\n\s*$|[ \t]+$|\s+(?=\n[ \t]*$))", QRegularExpression::MultilineOption);
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
            QDate date = QDate::fromString(m.captured(1).replace(sep, "-"), "yyyy-MM-dd");
            if (fst) {
                nxtdue = date;
                fst = false;
            } else { due = date; }
        }}
    }
    return {nxtamnt, amnt, nxttime, time, nxtdue, due};
}

QString Task::top() {
    QString out = name + "  "+QString("!").repeated(import);
#ifdef APP_DEBUG
    auto sco = basescore(*this);
    out += "  $"+QString::number(sco)+" - $"+QString::number(sco+baseextra());
#endif
    return out;
}
QString Task::bottom() {
    auto ps = Progress();
    if (ps.isEmpty()) return "No items";

    if (ps.nextDue.isNull()) {
        if (ps.totTime == 0) {
            return QString("%1 items").arg(ps.totTasks);
        }
        return QString("%1 over %2 items").arg(strTime(ps.totTime, true)).arg(ps.totTasks);
    }
    if (ps.nextTime == 0) {
        QString txt = QString("Due %2")
            .arg(parseDate(ps.nextDue, true));
        if (ps.lastDue.isNull()) return txt;
        return txt + QString(", Total due %2")
            .arg(parseDate(ps.lastDue, true));
    }
    QString txt = QString("%1 due %2")
        .arg(strTime(ps.nextTime, true)).arg(parseDate(ps.nextDue, true));

    if (ps.lastDue.isNull()) return txt;
    return txt + QString(", Total %1 due %2")
        .arg(strTime(ps.totTime, true)).arg(parseDate(ps.lastDue, true));
}
