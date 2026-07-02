#include "taskOverlay.hpp"
#include "extra/itemopts.hpp"
#include "extra/drag.hpp"
#include "wids/txtedit.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QGuiApplication>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>

const QMargins innerMarg{64, 72, 64, 56};


void highlight(QTextEdit* edit) {
    QSignalBlocker block(edit);
    int pos = edit->textCursor().position();

    QString conts = edit->toPlainText().toHtmlEscaped();
    QStringList out;
    for (QString line : conts.split('\n')) {
        auto m = timeRe.match(line);
        if (m.hasMatch()) {
            QString g = m.captured(1);
            QString repl = "<span style='background:#EAE; color:#222;'>#" + g + "</span>";

            int start = m.capturedStart(0);
            line.replace(start, m.capturedEnd(0) - start, repl);
        }
        out += line;
    }
    edit->setHtml("<span style='white-space: pre-wrap;'>"+out.join('\n')+"</span>");

    QTextCursor ncur = edit->textCursor();
    ncur.setPosition(pos);
    edit->setTextCursor(ncur);
}


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);

    auto* mlay = new QVBoxLayout();
    mlay->setContentsMargins(innerMarg + QMargins(16, 16, 16, 16));
        auto titl = new QLabel(task->name, this);
        mlay->addWidget(titl);
        auto* edit = new TxtEdit(task->items, this);
        highlight(edit);
        mlay->addWidget(edit);
    lay->addLayout(mlay);

    auto* scrl = new QScrollArea(this);
    scrl->setFrameShape(QFrame::NoFrame);
    scrl->setFocusPolicy(Qt::NoFocus);
    scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    scrl->setProperty("bg", true);

    scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrl->horizontalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    scrl->horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);

    lay->addWidget(scrl);
    bbar = new QWidget;
    bbar->setProperty("bg", true);
    scrl->setWidget(bbar);

    auto* blay = new QHBoxLayout(bbar);
    blay->setContentsMargins(12,6,12,6);
    blay->setSpacing(12);
    blay->setSizeConstraint(QLayout::SetMinimumSize);

    GenerateOpts(bbar, blay, edit, true);
    bbar->adjustSize();
    auto* drag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());

    int sb = scrl->horizontalScrollBar()->sizeHint().height();
    scrl->setFixedHeight(bbar->rect().height() + sb);
    GenerateOpts(bbar, blay, edit, false);

    connect(edit, &TxtEdit::focusChange, bbar, [=](bool focus){
        GenerateOpts(bbar, blay, edit, focus);
        drag->installOn(bbar);
    });
    connect(edit, &QTextEdit::textChanged, [=](){
        task->items = edit->toPlainText();
        highlight(edit);
    });
}

inline QMargins TaskOverlay::totMargin() {
    return innerMarg + QMargins(0,0,0,bbar->rect().height());
}

void TaskOverlay::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Back) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
        } else { deleteLater(); }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
void TaskOverlay::mousePressEvent(QMouseEvent* event) {
    auto point = mapToGlobal(event->position().toPoint());
    auto r = rect();
    if (point.y() > r.bottom() - bbar->rect().height()) { event->ignore(); return; }

    if (!r.marginsRemoved(totMargin()).contains(point)) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
            if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
        } else {
            deleteLater();
        }
        event->accept();
        return;
    }
    QWidget* wid = QApplication::widgetAt(point);
    if (qobject_cast<QTextEdit*>(wid)) { event->ignore(); }
    else if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
}

void TaskOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    auto r = rect();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(r, QColor(125, 125, 125, 125));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRoundedRect(r.marginsRemoved(totMargin()), 16, 16);
}
