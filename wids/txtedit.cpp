#include "txtedit.hpp"
#include <QScrollBar>
#include <QScroller>
#include <QKeyEvent>

TxtEdit::TxtEdit(QWidget* parent) : QTextEdit(parent) { init(); }
TxtEdit::TxtEdit(const QString& text, QWidget* parent) : QTextEdit(text, parent) { init(); }

void TxtEdit::init() {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewport()->setAutoFillBackground(false);
    setFrameStyle(QFrame::NoFrame);

    // Bigger font
    QFont f = font();
    f.setPointSize(f.pointSize() + 4);
    setFont(f);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Enable drag to scroll
    QScroller::grabGesture(viewport(), QScroller::TouchGesture);
    auto* scroller = QScroller::scroller(viewport());
    // Do not scroll outside the boundary
    QScrollerProperties props = scroller->scrollerProperties();
    props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(props);
}

void TxtEdit::focusInEvent(QFocusEvent *e) {
    QTextEdit::focusInEvent(e);
    emit focusChange(true);
}
void TxtEdit::focusOutEvent(QFocusEvent *e) {
    QTextEdit::focusOutEvent(e);
    emit focusChange(false);
}

void TxtEdit::keyPressEvent(QKeyEvent *e) {
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
