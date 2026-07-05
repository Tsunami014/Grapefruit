#include "txtedit.hpp"
#include "font.hpp"
#include "extra/drag.hpp"
#include <QScrollBar>
#include <QScroller>
#include <QKeyEvent>

TxtEdit::TxtEdit(QWidget* parent) : QTextEdit(parent) { init(); }
TxtEdit::TxtEdit(const QString& text, QWidget* parent) : QTextEdit(text, parent) { init(); }

void TxtEdit::init() {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewport()->setAutoFillBackground(false);
    setFrameStyle(QFrame::NoFrame);
    resizeFont(this, 1.2);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* drag = new DragScroll(viewport(), verticalScrollBar());
    drag->installOn(this);
}

void TxtEdit::focusInEvent(QFocusEvent* e) {
    QTextEdit::focusInEvent(e);
    emit focusChange(true);
}
void TxtEdit::focusOutEvent(QFocusEvent* e) {
    QTextEdit::focusOutEvent(e);
    emit focusChange(false);
}

void TxtEdit::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Up) {
        if (textCursor().blockNumber() == 0) {
            moveCursor(QTextCursor::Start);
            return;
        }
    } else if (e->key() == Qt::Key_Down) {
        if (textCursor().blockNumber() == document()->blockCount() - 1) {
            moveCursor(QTextCursor::End);
            return;
        }
    }
    QTextEdit::keyPressEvent(e);
}
