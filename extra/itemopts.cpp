#include "itemopts.hpp"
#include "date.hpp"
#include <QLayoutItem>
#include <QPushButton>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>

const QRegularExpression timeRe(R"((?<=^|[ \n<>])\+([0-9]+(?:(?:\.[0-9])?h|m))(?=[ \n<>]|$))");
const QString timeFmt = "+%1";
const QString donePref = "✔ ";
const QRegularExpression dateRe(R"((?<=^|[ \n<>])@(\d{4}([.\/\-])\d{2}\2\d{2})(?=[ \n<>]|$))");

int parseStrTime(QString time) {
    if (time.endsWith('h')) {
        auto parts = time.left(time.length()-1).split('.');
        if (parts.length() == 1 || parts[1] == "0") return parts[0].toInt()*60;
        auto dec = parts[1];
        return parts[0].toInt()*60 + (
            dec=="1"? 5 :
            dec=="2"? 10 :
            dec=="3"? 20 :
            dec=="4"? 25 :
            dec=="5"? 30 :
            dec=="6"? 35 :
            dec=="7"? 40 :
            dec=="8"? 50 :
            55
        );
    }
    return time.left(time.length()-1).toInt();
}

const QStringList strtimes{
    "2m", "5m", "10m", "15m", "20m", "30m", "45m",
    "1h", "1.3h", "1.7h", "2h", "2.3h", "2.7h",
    "3h", "3.5h", "4h"
};
const QList<int> _getTimes() {
    QList<int> ts;
    for (const auto& t : strtimes) {
        ts.push_back(parseStrTime(t));
    }
    return ts;
}
const QList<int> times = _getTimes();

void swapBlocks(QTextCursor& cur, const QTextBlock& block, const QTextBlock& next) {
    if (!next.isValid()) return;
    int column = cur.position() - cur.block().position();

    QString block_txt = block.text();
    QString next_txt = next.text();

    cur.beginEditBlock();
        cur.setPosition(block.position());
        cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cur.insertText(next_txt);

        cur.setPosition(next.position());
        cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cur.insertText(block_txt);
    cur.endEditBlock();

    cur.setPosition(next.position() + qBound(0, column, next.length() - 1));
}

void addBlock(QTextCursor& cur, bool after) {
    cur.beginEditBlock();
    cur.setPosition(cur.block().position());
    if (after) {
        cur.movePosition(QTextCursor::EndOfBlock);
    }
    cur.insertBlock();
    if (!after) {
        cur.movePosition(QTextCursor::PreviousBlock);
    }
    cur.endEditBlock();
}

void setBlockText(QTextCursor& cur, const QTextBlock& block, const QString& text, int coloffs = 0, int minoffs = 0) {
    if (!block.isValid()) return;

    const int blockPos = block.position();
    int column = cur.position() - cur.block().position();
    if (column >= minoffs) column += coloffs;

    cur.beginEditBlock();
        cur.setPosition(blockPos);
        cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cur.insertText(text);
    cur.endEditBlock();

    cur.setPosition(blockPos + qBound(0, column, text.size()));
}

void addTime(QTextEdit* edit, int add) {
    QTextCursor cur = edit->textCursor();
    QTextBlock block = cur.block();
    QString line = block.text();

    auto m = timeRe.match(line);
    if (m.hasMatch()) {
        int curamnt = parseStrTime(m.captured(1));
        int idx = 0;
        for (const auto& t : times) {
            if (t >= curamnt) break;
            idx++;
        }
        if (idx >= times.length()) return;
        idx += add;
        if (idx >= times.length() || idx < 0) return;
        QString repl = timeFmt.arg(strtimes.at(idx));

        int start = m.capturedStart(0);
        line.replace(start, m.capturedEnd(0) - start, repl);
        int origln = m.captured(0).length();
        setBlockText(cur, block, line, repl.length()-origln, start);
    } else {
        QString bt = timeFmt.arg(strtimes.at(0))+" ";
        if (line.startsWith(donePref)) {
            setBlockText(cur, block, donePref+bt+line.sliced(donePref.length()), bt.length(), donePref.length());
        } else {
            setBlockText(cur, block, bt+line, bt.length());
        }
    }
    edit->setTextCursor(cur);
}
void setDate(QTextEdit* edit) {
    QTextCursor cur = edit->textCursor();
    QTextBlock block = cur.block();
    QString line = block.text();

    auto m = dateRe.match(line);
    if (m.hasMatch()) {
        QChar sep = m.captured(2)[0];
        auto oldD = QDate::fromString(m.captured(1).replace(sep, "-"), "yyyy-MM-dd");
        auto newD = getDate(oldD);
        if (!newD.isValid()) return;
        QString repl = "@"+newD.toString("yyyy-MM-dd").replace('-', sep);

        int start = m.capturedStart(0);
        line.replace(start, m.capturedEnd(0) - start, repl);
        int origln = m.captured(0).length();
        setBlockText(cur, block, line, repl.length()-origln, start);
    } else {
        if (auto newD = getDate(); newD.isValid()) {
            setBlockText(cur, block, line+" @"+newD.toString("yyyy-MM-dd"));
        }
    }
    edit->setTextCursor(cur);
}

void GenerateOpts(QWidget* parent, QBoxLayout* lay, QTextEdit* edit, bool full) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    auto mkbtn = [&](const QString& ico, auto slot){
        auto btn = new QPushButton(parent);
        btn->setProperty("fancy", true);
        btn->setProperty("tinybtn", true);
        btn->setIcon(QIcon(ico));
        btn->setIconSize(QSize(40, 40));
        btn->setFocusPolicy(Qt::NoFocus);
        parent->connect(btn, &QPushButton::clicked, edit, slot);
        lay->addWidget(btn);
    };

    mkbtn(":/assets/UI/up.svg", [=](){
        QTextCursor cur = edit->textCursor();
        QTextBlock block = cur.block();
        swapBlocks(cur, block, block.previous());
        edit->setTextCursor(cur);
    });
    mkbtn(":/assets/UI/down.svg", [=](){
        QTextCursor cur = edit->textCursor();
        QTextBlock block = cur.block();
        swapBlocks(cur, block, block.next());
        edit->setTextCursor(cur);
    });
    mkbtn(":/assets/UI/above.svg", [=](){
        QTextCursor cur = edit->textCursor();
        addBlock(cur, false);
        edit->setTextCursor(cur);
    });
    mkbtn(":/assets/UI/below.svg", [=](){
        QTextCursor cur = edit->textCursor();
        addBlock(cur, true);
        edit->setTextCursor(cur);
    });
    lay->addSpacing(16);
    if (full) {
        mkbtn(":/assets/UI/addtime.svg", [=](){ addTime(edit, 1); });
        mkbtn(":/assets/UI/subtime.svg", [=](){ addTime(edit, -1); });
        mkbtn(":/assets/UI/checkbox.svg", [=](){
            QTextCursor cur = edit->textCursor();
            QTextBlock block = cur.block();
            QString line = block.text();
            if (line.startsWith(donePref)) {
                setBlockText(cur, block, line.sliced(donePref.length()), -donePref.length());
            } else {
                setBlockText(cur, block, donePref+line, donePref.length());
            }
            edit->setTextCursor(cur);
        });
        mkbtn(":/assets/UI/calendar.svg", [=](){ setDate(edit); });

        lay->addSpacing(32);
        mkbtn(":/assets/UI/checkall.svg", [=](){
            QTextCursor cur = edit->textCursor();
            QTextBlock curblk = cur.block();

            for (QTextBlock block = edit->document()->begin();
                    block.isValid(); block = block.next()) {
                QString line = block.text();
                if (line.startsWith(donePref)) {
                    if (block == curblk) break;
                    continue;
                }
                if (block == curblk) {
                    setBlockText(cur, block, donePref+line, donePref.length());
                    break;
                } else {
                    setBlockText(cur, block, donePref+line, 0);
                }
            }
            edit->setTextCursor(cur);
        });
    }
    mkbtn(":/assets/UI/erase.svg", [=](){
        QTextCursor cur = edit->textCursor();
        if (cur.block().text().isEmpty()) {
            cur.deleteChar();
        } else {
            cur.select(QTextCursor::BlockUnderCursor);
            cur.removeSelectedText();
        }
    });

    lay->addStretch();
}
