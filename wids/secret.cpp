// Thanks in part to https://stackoverflow.com/a/37119983 !
#include "secret.hpp"
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QStyle>
#include <QTimer>

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

    contentArea.setObjectName("contentArea");
    QSizePolicy p(QSizePolicy::Expanding, QSizePolicy::Fixed);
    p.setHeightForWidth(true);
    contentArea.setSizePolicy(p);
    // start out collapsed
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);
    setProperty("open", false);

    // don't waste space
    mainLayout.setContentsMargins(0, 0, 0, 0);
    mainLayout.setSpacing(0);
    mainLayout.addWidget(&toggleButton);
    mainLayout.addWidget(&contentArea);
    setLayout(&mainLayout);
    QObject::connect(&toggleButton, &QToolButton::clicked, [this](const bool checked) {
        toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        setProperty("open", checked);
        toggleButton.style()->unpolish(&toggleButton);
        toggleButton.style()->polish(&toggleButton);

        // Ensure everything is in place to prevent flickers
        QWidget* top = window();
        top->setUpdatesEnabled(false);

        contentArea.setMaximumHeight(checked ? QWIDGETSIZE_MAX : 0);

        for (QWidget* w = this; w; w = w->parentWidget()) {
            if (QLayout* l = w->layout()) {
                l->invalidate();
                l->activate();
            }
        }

        top->setUpdatesEnabled(true);
        top->update();
    });
}

void Spoiler::setContentLayout(QLayout* ncont) {
    delete contentArea.layout();
    contentArea.setLayout(ncont);
}
