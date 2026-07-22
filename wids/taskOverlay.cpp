#include "taskOverlay.hpp"
#include "base/taskload.hpp"
#include "base/quals.hpp"
#include "extra/itemopts.hpp"
#include "extra/drag.hpp"
#include "wids/confirm.hpp"
#include "wids/slider.hpp"
#include "wids/flow.hpp"
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
#include <QAbstractTextDocumentLayout>

const QMargins innerMarg{36, 16, 64, 36};

void HlTxtEdit::paintEvent(QPaintEvent* event) {
    // Paint done backgrounds manually
    QPainter painter(viewport());
    QPointF offset(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
    QTextDocument* doc = document();

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        if (!block.isVisible()) continue;
        if (!block.text().startsWith(donePref)) continue;

        QRectF r = doc->documentLayout()->blockBoundingRect(block).translated(offset);
        r.setLeft(0);
        r.setWidth(viewport()->width());
        painter.fillRect(r, QColor("#AAA"));
    }

    QTextEdit::paintEvent(event);
}

void HlTxtEdit::highlight() {
    QList<QTextEdit::ExtraSelection> sels;
    QTextDocument* doc = document();
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QString line = block.text();
        bool done = line.startsWith(donePref);
        if (done) {
            QTextEdit::ExtraSelection sel;
            sel.cursor = QTextCursor(block);
            sel.cursor.setPosition(block.position());
            sel.cursor.setPosition(block.position() + block.length() - 1, QTextCursor::KeepAnchor);
            sel.format.setForeground(QColor("#555"));
            sel.format.setFontStrikeOut(true);
            sel.format.setProperty(QTextFormat::FullWidthSelection, true);
            sels << sel;
        }
        auto m = timeRe.match(line);
        if (m.hasMatch()) {
            QTextEdit::ExtraSelection sel;
            sel.cursor = QTextCursor(block);
            sel.cursor.setPosition(block.position() + m.capturedStart(0));
            sel.cursor.setPosition(block.position() + m.capturedEnd(0), QTextCursor::KeepAnchor);
            sel.format.setBackground(QColor(done ? "#C9C" : "#EAE"));
            sels << sel;
        }
    }
    setExtraSelections(sels);
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
        float time = m.captured(1).toFloat();
        QString nxt = QString(done? "spent" : "will spend") + " ~";
        if (time <= 0.5) nxt += "30 mins";
        else if (time == 1) nxt += "1 hr";
        else nxt += QString::number(time) + " hrs";
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
    mlay->setSpacing(8);
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
        auto* sublay2 = new QVBoxLayout(sl2wid);
            {auto* txt = new QLabel("Importance:", sl2wid);
            sublay2->addWidget(txt);}

            auto* subsublay2 = new QHBoxLayout();
            sublay2->addLayout(subsublay2);
            {auto* txt = new QLabel("!", sl2wid);
            subsublay2->addWidget(txt);}
            auto* slider = new Slidr(Qt::Horizontal, sl2wid);
            slider->setTickInterval(1);
            slider->setTickPosition(QSlider::TicksBelow);
            slider->setMaximum(5);
            slider->setMinimum(1);
            slider->setValue(task->import);
            subsublay2->addWidget(slider);
            {auto* txt = new QLabel("!!!!!", sl2wid);
            subsublay2->addWidget(txt);}

        midwid = new QWidget(this);
        midwid->setContentsMargins(0,0,0,0);
        mlay->addWidget(midwid, 2);
        auto* midlay = new QHBoxLayout(midwid);
        midlay->setContentsMargins(0,0,0,0);
            reasonsWid = new QWidget(this);
            reasonsWid->setContentsMargins(0,0,0,0);
            midlay->addWidget(reasonsWid);
            auto* reasonsLay = new QVBoxLayout(reasonsWid);
                {auto* labl = new QLabel("Reasoning:", reasonsWid);
                resizeFont(labl, 1.2);
                reasonsLay->addWidget(labl);}

                reasons = new TxtEdit(reasonsWid);
                reasons->setPlainText("Hello!");
                reasonsLay->addWidget(reasons);
            qualsWid = new QWidget(this);
            qualsWid->setContentsMargins(0,0,0,0);
            midlay->addWidget(qualsWid);
            auto* qualsLay = new QVBoxLayout(qualsWid);
                {auto* labl = new QLabel("Qualities:", qualsWid);
                resizeFont(labl, 1.2);
                qualsLay->addWidget(labl);}

                quals = new QualityEdit({}, qualsWid);
                qualsLay->addWidget(quals);

        editWid = new QWidget(this);
        editWid->setContentsMargins(0,0,0,0);
        mlay->addWidget(editWid, 3);
        auto* editLay = new QVBoxLayout(editWid);
            {auto* labl = new QLabel("Task items:", editWid);
            resizeFont(labl, 1.2);
            editLay->addWidget(labl);}

            edit = new HlTxtEdit(editWid);
            edit->setPlainText(task->items);
            edit->highlight();
            editLay->addWidget(edit);
    lay->addLayout(mlay);

    bbar = new QWidget(this);
    bbar->setProperty("bg", true);
    lay->addWidget(bbar);
    new QVBoxLayout(bbar);
    generateBot();

    connect(edit, &TxtEdit::focusChange, [=](bool focus){ generateBot(); });
    connect(reasons, &TxtEdit::focusChange, [=](bool focus){ generateBot(); });
    connect(quals, &TxtEdit::focusChange, [=](bool focus){ generateBot(); });
    connect(edit, &QTextEdit::textChanged, [=](){ QTimer::singleShot(0, this, [=]() {
        task->items = edit->toPlainText();
        edit->highlight();
        saveTasks();
    }); });
    connect(titl, &QLineEdit::textChanged, [=](){ QTimer::singleShot(0, this, [=]() {
        task->name = titl->text();
        saveTasks();
    }); });
    connect(titl, &QLineEdit::returnPressed, [=](){ titl->clearFocus(); });
    connect(slider, &QSlider::sliderMoved, [=](int val){ QTimer::singleShot(0, this, [=]() {
        task->import = val;
        saveTasks();
    }); });
}

void TaskOverlay::generateBot() {
    auto* blay = qobject_cast<QBoxLayout*>(bbar->layout());
    QLayoutItem* item;
    while ((item = blay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        if (auto* lay = item->layout()) lay->deleteLater();
        delete item;
    }

    if (bool isedit = edit->hasFocus(); isedit || reasons->hasFocus() || quals->hasFocus()) {
        if (isedit) {
            auto labl = new QLabel(bbar);
            labl->setFocusPolicy(Qt::NoFocus);
            labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            labl->setAlignment(Qt::AlignCenter);
            auto setLablTxt = [=](){ labl->setText(labelTxt(edit)); };
            labl->connect(edit, &QTextEdit::cursorPositionChanged, labl, setLablTxt);
            QTimer::singleShot(0, labl, setLablTxt);
            blay->addWidget(labl);
        }

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

        if (quals->hasFocus()) {
            scrl->setWidgetResizable(true);
            auto* bflow = new FlowLayout(bitsWid, -1, 16, 16);
            bflow->vertical(3);

            // Create the buttons!
            for (const auto& k : qualkeys()) {
                auto btn = new QPushButton(k, bbar);
                resizeFont(btn, 1.3);
                btn->setProperty("fancy", true);
                btn->setProperty("tinybtn", true);
                btn->setFocusPolicy(Qt::NoFocus);
                connect(btn, &QPushButton::clicked, quals, [=](){ quals->toggleWord(k); });
                bflow->addWidget(btn);
            }
        } else {
            auto* bits = new QHBoxLayout(bitsWid);
            bits->setContentsMargins(12,6,12,6);
            bits->setSpacing(12);

            bits->setSizeConstraint(QLayout::SetMinimumSize);
            GenerateOpts(bitsWid, bits, isedit? edit:reasons, isedit);
            bitsWid->adjustSize();

            int sb = scrl->horizontalScrollBar()->sizeHint().height();
            scrl->setFixedHeight(bitsWid->rect().height() + sb);
        }
        auto* drag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());
        drag->installOn(bitsWid);

        for (auto* w : parts) w->hide();
        if (isedit) {
            editWid->show(); midwid->hide();
        } else {
            editWid->hide(); midwid->show();
            if (reasons->hasFocus()) {
                qualsWid->hide(); reasonsWid->show();
            } else {
                qualsWid->show(); reasonsWid->hide();
            }
        }
    } else {
        blay->addSpacing(32);
        for (auto* w : parts) w->show();
        editWid->show(); midwid->show();
        qualsWid->show(); reasonsWid->show();
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
