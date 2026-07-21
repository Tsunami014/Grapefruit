#include "itemopts.hpp"
#include <QLayoutItem>
#include <QPushButton>
#include <QTextCursor>
#include <QTextBlock>
#include <QTimer>

const QRegularExpression timeRe(R"((?<=^|[ \n<>])#([a-zA-Z])(?=[ \n<>]|$))");
const QString donePref = "/ ";

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

void addTime(QTextEdit* edit, int diff) {
    QTextCursor cur = edit->textCursor();
    QTextBlock block = cur.block();
    QString line = block.text();

    auto m = timeRe.match(line);
    if (m.hasMatch()) {
        char nc = m.captured(1)[0].unicode() + diff;
        if (!(nc >= 'a' && nc <= 'z') && !(nc >= 'A' && nc <= 'Z')) return;
        QString repl = QString("#") + QChar::fromLatin1(nc);

        int start = m.capturedStart(0);
        line.replace(start, m.capturedEnd(0) - start, repl);
        setBlockText(cur, block, line);
    } else {
        if (line.startsWith(donePref)) {
            setBlockText(cur, block, donePref+"#A "+line.sliced(donePref.length()), 3, donePref.length());
        } else {
            setBlockText(cur, block, "#A "+line, 3);
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
        parent->connect(btn, &QPushButton::clicked, slot);
        lay->addWidget(btn);
    };

    mkbtn(":/assets/UI/up.svg", [=](){
        if (!edit) return;
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
    if (full) {
        lay->addSpacing(16);
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
        mkbtn(":/assets/UI/addtime.svg", [=](){ addTime(edit, 1); });
        mkbtn(":/assets/UI/subtime.svg", [=](){ addTime(edit, -1); });
        mkbtn(":/assets/UI/calendar.svg", [=](){});
    }

    lay->addSpacing(32);
    if (full) {
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
