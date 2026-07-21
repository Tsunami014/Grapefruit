#include "qualsEdit.hpp"
#include "font.hpp"
#include <QTextBlock>

QualityEdit::QualityEdit(std::set<QString> words, QWidget* parent)
    : TxtEdit(parent), words(words) {
    setFocusPolicy(Qt::NoFocus); // We manually focus ourselves
    setTextInteractionFlags(Qt::NoTextInteraction); // No selecting text
    setReadOnly(true);
    if (!words.empty()) {
        updtxt();
    }
    // Setup fonts
    QFont prev = font();
    resizeFont(this, 1.8);
    selfnt = font();
    setFont(prev);
    resizeFont(this, 1.2);
    regfnt = font();
}

void QualityEdit::addWord(QString word) {
    if (words.find(word) == words.end()) {
        words.insert(word);
        updtxt();
    }
}
void QualityEdit::rmWord(QString word) {
    if (auto it = words.find(word); it != words.end()) {
        words.erase(it);
        updtxt();
    }
}
void QualityEdit::toggleWord(QString word) {
    auto it = words.find(word);
    if (it != words.end()) {
        words.erase(it);
    } else {
        words.insert(word);
    }
    updtxt();
}

void QualityEdit::updtxt() {
    setText(QStringList(words.begin(), words.end()).join("  "));

    // Add highlighting!
    QList<QTextEdit::ExtraSelection> sels;
    QTextDocument* doc = document();
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QString line = block.text();
        for (int i = 0; i < line.size();) {
            while (i < line.size() && line[i].isSpace()) i++;
            if (i >= line.size()) break;

            int start = i;
            while (i < line.size() && !line[i].isSpace()) i++;

            QTextEdit::ExtraSelection sel;
            sel.cursor = QTextCursor(block);
            sel.cursor.setPosition(block.position() + start);
            sel.cursor.setPosition(block.position() + i, QTextCursor::KeepAnchor);
            sel.format.setBackground(QColor("#9C9"));
            sels << sel;
        }
    }
    setExtraSelections(sels);
}

void QualityEdit::mousePressEvent(QMouseEvent* event) {
    if (hasFocus()) {
        QTextCursor curs = cursorForPosition(event->pos());
        curs.select(QTextCursor::WordUnderCursor);
        QString word = curs.selectedText();
        if (!word.isEmpty()) rmWord(word);
    } else {
        setFocus(Qt::MouseFocusReason);
    }
    TxtEdit::mousePressEvent(event);
}

void QualityEdit::focusOutEvent(QFocusEvent* event) {
    TxtEdit::focusOutEvent(event);
    setFont(regfnt);
}
void QualityEdit::focusInEvent(QFocusEvent* event) {
    TxtEdit::focusInEvent(event);
    setFont(selfnt);
}
