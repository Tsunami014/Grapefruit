// Thanks in part to https://stackoverflow.com/a/37119983 !
#include "secret.hpp"
#include <QPropertyAnimation>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QStyle>

const int animationDuration = 200;

HeaderButton::HeaderButton(QWidget* parent) : QToolButton(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}
void HeaderButton::paintEvent(QPaintEvent* event) {
    QToolButton::paintEvent(event);
    QFontMetrics fm(font());

    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    int x = fm.horizontalAdvance(opt.text) + 28;
    if (!opt.icon.isNull()) {
        x += style()->pixelMetric(QStyle::PM_SmallIconSize, &opt, this) + 4;
    }

    QPainter p(this);
    int from = x + 8;
    int to = width() - 12;
    if (to > from) {
        QColor col = lcol.isValid()? lcol : palette().mid().color();
        QPen pen(col, lwid, Qt::SolidLine, Qt::FlatCap);
        p.setPen(pen);

        int y = height() / 2;
        p.drawLine(from, y, to, y);
    }
}

Spoiler::Spoiler(const QString& title, QWidget* parent) : QWidget(parent) {
    toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton.setArrowType(Qt::ArrowType::RightArrow);
    toggleButton.setText(title);
    toggleButton.setCheckable(true);
    toggleButton.setChecked(false);

    contentArea.setStyleSheet("QScrollArea { background-color: white; border: none; }");
    contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // start out collapsed
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);
    toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));
    // don't waste space
    mainLayout.setContentsMargins(0, 0, 0, 0);
    mainLayout.setSpacing(0);
    int row = 0;
    mainLayout.addWidget(&toggleButton);
    mainLayout.addWidget(&contentArea);
    setLayout(&mainLayout);
    QObject::connect(&toggleButton, &QToolButton::clicked, [this](const bool checked) {
        toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        toggleAnimation.start();
    });
}

void Spoiler::setContentLayout(QLayout& contentLayout) {
    delete contentArea.layout();
    contentArea.setLayout(&contentLayout);
    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();
    for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i) {
        QPropertyAnimation* spoilerAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}
