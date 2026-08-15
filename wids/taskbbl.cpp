#include "taskbbl.hpp"
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QTimer>

class CutoffLabel : public QLabel {
public:
    CutoffLabel(const QString& text, QWidget* parent) : QLabel(parent), full(text) {
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
    void setFullText(const QString& text) {
        full = text;
        updateClip();
    }
protected:
    void resizeEvent(QResizeEvent* e) override {
        QLabel::resizeEvent(e);
        updateClip();
    }
    void updateClip() {
        QFontMetrics fm(font());
        auto margs = contentsMargins();
        int realw = width() - margs.left() - margs.right();
        QLabel::setText(fm.elidedText(full, Qt::ElideRight, realw));
    }
    QString full;
};

TaskBubble::TaskBubble(std::shared_ptr<Task> t, QWidget* parent) : QWidget(parent) {
    auto* main = new QHBoxLayout(this);
    main->setContentsMargins(0, 0, 0, 0);
    main->setSpacing(0);

    auto* sub = new QVBoxLayout();
    sub->setSpacing(0);
        {auto labl = new CutoffLabel(t->top(), this);
        labl->setAlignment(Qt::AlignCenter);
        labl->setProperty("bubble", "top");
        sub->addWidget(labl);}
        {auto labl = new CutoffLabel(t->bottom(), this);
        labl->setAlignment(Qt::AlignCenter);
        labl->setProperty("bubble", "bot");
        sub->addWidget(labl);}

    main->addLayout(sub, 1);
    {auto btn = new QPushButton(this);
    btn->setProperty("bubblebtn", true);
    btn->setIcon(QIcon(t->today? ":/assets/UI/cal-heart.svg" : ":/assets/UI/cal-empty.svg"));
    btn->setIconSize(QSize(48, 48));
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    QObject::connect(btn, &QPushButton::clicked, this, &TaskBubble::clickedCalendar);
    main->addWidget(btn);}
}

void TaskBubble::mousePressEvent(QMouseEvent* event) {
    event->accept();
    pressed = true;
    refreshStyle();
    QWidget::mousePressEvent(event);
}
void TaskBubble::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    if (!pressed) return;
    event->accept();
    pressed = false;
    refreshStyle();
    emit clicked();
}
void TaskBubble::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    if (!pressed) return;
    event->accept();
    pressed = false;
    refreshStyle();
}
void TaskBubble::refreshStyle() {
    setProperty("pressed", pressed);
    style()->unpolish(this);
    style()->polish(this);
    for (QLabel* labl : findChildren<QLabel*>()) {
        style()->unpolish(labl);
        style()->polish(labl);
        labl->update();
    }
    update();
}
