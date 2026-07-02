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

void GenerateOpts(QWidget* parent, QBoxLayout* lay, QTextEdit* edit, bool focus) {
    QLayoutItem *item;
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
