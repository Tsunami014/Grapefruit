#include "txtedit.hpp"
#include <QScrollBar>
#include <QScroller>

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

    // Enable scrolling!
    QScroller::grabGesture(viewport(), QScroller::TouchGesture);
    auto* scroller = QScroller::scroller(viewport());
    // Do not scroll outside the boundary
    QScrollerProperties props = scroller->scrollerProperties();
    props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(props);
}
