#include "taskOverlay.hpp"
#include "base/taskload.hpp"
#include "extra/itemopts.hpp"
#include "extra/drag.hpp"
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
    QSignalBlocker block(edit);
    int pos = edit->textCursor().position();

    QString conts = edit->toPlainText().toHtmlEscaped();
    QStringList out;
    for (QString line : conts.split('\n')) {
        bool done = line.startsWith(donePref);
        auto m = timeRe.match(line);
        if (m.hasMatch()) {
            QString g = m.captured(1);
            QString timecol = done? "#C9C" : "#EAE";
            QString repl = "<span style='background:" + timecol + ";'>#" + g + "</span>";

            int start = m.capturedStart(0);
            line.replace(start, m.capturedEnd(0) - start, repl);
        }
        if (done) {
            line = "<span style='background:#AAA; color: #555;'><span style='font-weight: bold'>/</span>" + line.sliced(1) + "</span>";
        }
        out += line;
    }
    edit->setHtml("<span style='white-space: pre-wrap;'>"+out.join('\n')+"</span>");

    QTextCursor ncur = edit->textCursor();
    ncur.setPosition(pos);
    edit->setTextCursor(ncur);
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


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, std::function<void()> update, QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);

    auto* mlay = new QVBoxLayout();
    mlay->setContentsMargins(innerMarg + QMargins(16, 16, 16, 16));
    mlay->setSpacing(16);
        auto* sublay1 = new QHBoxLayout();
            auto titl = new QLineEdit(task->name, this);
            resizeFont(titl, 1.3);
            sublay1->addWidget(titl);

            auto bin = new QPushButton();
            bin->setProperty("fancy", true);
            bin->setIcon(QIcon(":/assets/UI/bin.svg"));
            int mx = titl->rect().height() + 8;
            bin->setIconSize(QSize(mx, mx-4));
            connect(bin, &QPushButton::clicked, this, [=](){
                removeTask(task);
                deleteLater();
                update();
            });
            sublay1->addWidget(bin);
        mlay->addLayout(sublay1);
        auto* sublay2 = new QHBoxLayout();
            auto* txt = new QLabel(this);
            sublay2->addWidget(txt);
            auto* slider = new Slidr(Qt::Horizontal, this);
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
        mlay->addLayout(sublay2);

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
        update();
        saveTasks();
    }); });
    connect(titl, &QLineEdit::textChanged, [=](){ QTimer::singleShot(0, this, [=]() {
            task->name = titl->text();
            update();
            saveTasks();
    }); });
    connect(slider, &QSlider::sliderMoved, [=](int val){ QTimer::singleShot(0, this, [=]() {
        task->import = val;
        update();
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
    } else {
        blay->addSpacing(32);
    }
    bbar->layout()->activate();
    QWidget::update();
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
