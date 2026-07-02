#include "itemopts.hpp"
#include <QLayoutItem>
#include <QLabel>
#include <QPushButton>
#include <QTextCursor>
#include <QTextBlock>

void swapBlocks(QTextCursor& cur, QTextBlock block, QTextBlock next) {
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

void setBlockText(QTextCursor& cur, QTextBlock block, QString text, int coloffs = 0) {
    if (!block.isValid()) return;
    int column = cur.position() - cur.block().position() + coloffs;

    cur.beginEditBlock();
        cur.setPosition(block.position());
        cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cur.insertText(text);
    cur.endEditBlock();

    cur.setPosition(cur.block().position() + qBound(0, column, block.length() - 1));
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
        setBlockText(cur, block, "#A "+line, 3);
    }
    edit->setTextCursor(cur);
}

void GenerateOpts(QWidget* parent, QBoxLayout* lay, QTextEdit* edit, bool focus) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }
    if (!focus) return;

    QSize sze(40,40);

    auto mkbtn = [&](const QString& ico, auto slot){
        auto btn = new QPushButton(parent);
        btn->setProperty("fancy", true);
        btn->setProperty("tinybtn", true);
        btn->setIcon(QIcon(ico));
        btn->setIconSize(sze);
        btn->setFocusPolicy(Qt::NoFocus);
        parent->connect(btn, &QPushButton::clicked, slot);
        lay->addWidget(btn);
    };

    mkbtn(":/assets/up.svg", [=](){
        if (!edit) return;
        QTextCursor cur = edit->textCursor();
        QTextBlock block = cur.block();
        swapBlocks(cur, block, block.previous());
        edit->setTextCursor(cur);
    });
    mkbtn(":/assets/down.svg", [=](){
        QTextCursor cur = edit->textCursor();
        QTextBlock block = cur.block();
        swapBlocks(cur, block, block.next());
        edit->setTextCursor(cur);
    });
    lay->addSpacing(16);
    mkbtn(":/assets/addtime.svg", [=](){ addTime(edit, 1); });
    mkbtn(":/assets/subtime.svg", [=](){ addTime(edit, -1); });
    mkbtn(":/assets/clock.svg", [=](){
        QTextCursor cur = edit->textCursor();
        QTextBlock block = cur.block();
        QString line = block.text();

        auto m = timeRe.match(line);
        if (m.hasMatch()) {
            int start = m.capturedStart(0);
            line.replace(start, m.capturedEnd(0) - start, "#b");
            setBlockText(cur, block, line);
        } else {
            setBlockText(cur, block, "#b "+line, 3);
        }
        edit->setTextCursor(cur);
    });
    mkbtn(":/assets/calendar.svg", [=](){});

    lay->addSpacing(32);
    mkbtn(":/assets/erase.svg", [=](){
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
