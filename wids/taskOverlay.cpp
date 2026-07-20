#include "taskOverlay.hpp"
#include "base/taskload.hpp"
#include "extra/itemopts.hpp"
#include "extra/drag.hpp"
#include "wids/confirm.hpp"
#include "wids/slider.hpp"
#include "font.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLineEdit>
#include <QApplication>
#include <QPushButton>
#include <QTextBlock>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>

const QMargins innerMarg{36, 16, 64, 36};


void highlight(QTextEdit* edit) {
    QList<QTextEdit::ExtraSelection> sels;
    QTextDocument* doc = edit->document();

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QString line = block.text();
        bool done = line.startsWith(donePref);

        auto m = timeRe.match(line);
        if (m.hasMatch()) {
            QTextEdit::ExtraSelection sel;
            sel.cursor = QTextCursor(block);
            sel.cursor.setPosition(block.position() + m.capturedStart(0));
            sel.cursor.setPosition(block.position() + m.capturedEnd(0), QTextCursor::KeepAnchor);
            sel.format.setBackground(QColor(done ? "#C9C" : "#EAE"));
            sels << sel;
        }
        if (done) {
            QTextEdit::ExtraSelection sel;
            sel.cursor = QTextCursor(block);
            sel.cursor.select(QTextCursor::LineUnderCursor);
            sel.format.setBackground(QColor("#AAA"));
            sel.format.setForeground(QColor("#555"));
            sels << sel;
        }
    }
    edit->setExtraSelections(sels);
}

QString labelTxt(QTextEdit* edit) {
    QTextCursor cur = edit->textCursor();
    if (cur.isNull()) return "";
    QTextBlock block = cur.block();
    QString line = block.text();

    QStringList out;
    bool done = line.startsWith(donePref);
    if (done) out += "completed";

    auto m = timeRe.match(line);
    if (m.hasMatch()) {
        int time = m.captured(1)[0].toLower().unicode() - 'a';
        QString nxt = QString(done? "spent" : "will spend") + " ~";
        if (time == 0) nxt += "30 mins";
        else nxt += QString::number(float(time+1)*0.5) + " hrs";
        out += nxt;
    }
    QString end = out.join(", ");
    if (end.isEmpty()) return "No info on this item";
    return end.at(0).toUpper()+end.sliced(1);
}


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, std::function<void()> ondeath, QWidget* parent) : QWidget(parent), ondeath(ondeath) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);

    auto* mlay = new QVBoxLayout();
    mlay->setContentsMargins(innerMarg + QMargins(16, 16, 16, 16));
    mlay->setSpacing(16);
        auto* sl1wid = new QWidget(this);
        sl1wid->setContentsMargins(0,0,0,0);
        mlay->addWidget(sl1wid);
        parts.push_back(sl1wid);
        auto* sublay1 = new QHBoxLayout(sl1wid);
            auto titl = new QLineEdit(task->name, sl1wid);
            resizeFont(titl, 1.3);
            sublay1->addWidget(titl);

            auto bin = new QPushButton();
            bin->setProperty("fancy", true);
            bin->setIcon(QIcon(":/assets/UI/bin.svg"));
            int mx = titl->rect().height() + 8;
            bin->setIconSize(QSize(mx, mx-4));
            connect(bin, &QPushButton::clicked, sl1wid, [=](){
                QString nam = task->name;
                deleteLater();
                QTimer::singleShot(0, parent, [=]() {
                    if (!task->isNew() && confirm(parent,
                          "Are you sure you want to delete the task '" + nam + "'?",
                          Conf_YESNO) != QDialogButtonBox::YesRole) return;
                    removeTask(task);
                    ondeath();
                });
            });
            sublay1->addWidget(bin);
        auto* sl2wid = new QWidget(this);
        sl2wid->setContentsMargins(0,0,0,0);
        parts.push_back(sl2wid);
        mlay->addWidget(sl2wid);
        auto* sublay2 = new QHBoxLayout(sl2wid);
            auto* txt = new QLabel(sl2wid);
            sublay2->addWidget(txt);
            auto* slider = new Slidr(Qt::Horizontal, sl2wid);
            slider->setTickInterval(1);
            slider->setTickPosition(QSlider::TicksBelow);
            sublay2->addWidget(slider);
            slider->setValue(0);
            QObject::connect(slider, &QSlider::valueChanged, txt, [txt](int value) {
                txt->setText("Import\nance: "+QString::number(value));
            });
            slider->setValue(task->import);
            slider->setMaximum(5);
            slider->setMinimum(1);

        edit = new TxtEdit(this);
        edit->setPlainText(task->items);
        highlight(edit);
        mlay->addWidget(edit);
    lay->addLayout(mlay);


    bbar = new QWidget(this);
    bbar->setProperty("bg", true);
    lay->addWidget(bbar);
    new QVBoxLayout(bbar);
    generateBot();

    connect(edit, &TxtEdit::focusChange, [=](bool focus){ generateBot(); });
    connect(edit, &QTextEdit::textChanged, [=](){ QTimer::singleShot(0, this, [=]() {
        task->items = edit->toPlainText();
        highlight(edit);
        saveTasks();
    }); });
    connect(titl, &QLineEdit::textChanged, [=](){ QTimer::singleShot(0, this, [=]() {
        task->name = titl->text();
        saveTasks();
    }); });
    connect(slider, &QSlider::sliderMoved, [=](int val){ QTimer::singleShot(0, this, [=]() {
        task->import = val;
        saveTasks();
    }); });
}

void TaskOverlay::generateBot() {
    auto* blay = qobject_cast<QVBoxLayout*>(bbar->layout());
    QLayoutItem* item;
    while ((item = blay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        if (auto* lay = item->layout()) lay->deleteLater();
        delete item;
    }

    if (edit->hasFocus()) {
        auto labl = new QLabel(bbar);
        labl->setFocusPolicy(Qt::NoFocus);
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        labl->setAlignment(Qt::AlignCenter);
        auto setLablTxt = [=](){ labl->setText(labelTxt(edit)); };
        labl->connect(edit, &QTextEdit::cursorPositionChanged, labl, setLablTxt);
        QTimer::singleShot(0, labl, setLablTxt);
        blay->addWidget(labl);

        auto* scrl = new QScrollArea(bbar);
        scrl->setFrameShape(QFrame::NoFrame);
        scrl->setFocusPolicy(Qt::NoFocus);
        scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        scrl->setProperty("bg", true);

        scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrl->horizontalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        scrl->horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);
        blay->addWidget(scrl);

        auto* bitsWid = new QWidget(bbar);
        bitsWid->setProperty("bg", true);
        scrl->setWidget(bitsWid);

        auto* bits = new QHBoxLayout(bitsWid);
        bits->setContentsMargins(12,6,12,6);
        bits->setSpacing(12);
        bits->setSizeConstraint(QLayout::SetMinimumSize);

        GenerateOpts(bitsWid, bits, edit);
        bitsWid->adjustSize();
        auto* drag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());

        int sb = scrl->horizontalScrollBar()->sizeHint().height();
        scrl->setFixedHeight(bitsWid->rect().height() + sb);
        drag->installOn(bitsWid);

        for (auto* w : parts) w->hide();
    } else {
        blay->addSpacing(32);
        for (auto* w : parts) w->show();
    }
    bbar->layout()->activate();
    update();
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
    auto point = event->position().toPoint();
    auto r = rect();
    if (point.y() > r.bottom() - bbar->rect().height()) { event->ignore(); return; }

    if (!r.marginsRemoved(totMargin()).contains(point)) {
        bool del = true;
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
            del = false;
        }
        if (QWidget* focus = QApplication::focusWidget()) {
            focus->clearFocus();
            del = false;
        }
        if (del) deleteLater();
        event->accept();
        return;
    }
    QWidget* wid = QApplication::widgetAt(mapToGlobal(point));
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
