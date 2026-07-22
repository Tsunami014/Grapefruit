#include "date.hpp"
#include "game.hpp"
#include "font.hpp"
#include <QDialog>
#include <QBoxLayout>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QHeaderView>
#include <QTableView>
#include <QTextCharFormat>
#include <QMouseEvent>
#include <QPushButton>
#include <QApplication>

class CalendarItemDelegate : public QStyledItemDelegate {
public:
    explicit CalendarItemDelegate(QCalendarWidget* calendar, QObject* parent = nullptr)
        : QStyledItemDelegate(parent), calendar(calendar) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        const QString text = index.data(Qt::DisplayRole).toString();
        bool ok = false;
        const int day = text.toInt(&ok);
        // Not a day cell -> fall back to default paint
        if (!ok || day <= 0) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillRect(option.rect, QColor("#ffffff"));

        const bool isSelected = option.state & QStyle::State_Selected;
        const bool isHovered = option.state & QStyle::State_MouseOver;
        const bool isEnabled = option.state & QStyle::State_Enabled;

        // Find the real date of this cell
        const QDate topleft(calendar->yearShown(), calendar->monthShown(), 1);
        const int offs = (topleft.dayOfWeek() - int(calendar->firstDayOfWeek()) + 7) % 7;
        const QDate cellDate = topleft.addDays((index.row()-1) * 7 + index.column() - offs);
        const bool isOtherMonth = cellDate.month() != calendar->monthShown();

        const int diam = qMin(option.rect.width(), option.rect.height()) - 8;
        QRect rect(0, 0, diam, diam);
        rect.moveCenter(option.rect.center());

        QColor textColour;
        if (!isEnabled) {  // Out of range
            textColour = QColor("#E2E4E9");
        } else if (isOtherMonth) {  // Other month
            textColour = QColor("#C2C7D0");
        } else {  // Normal
            textColour = QColor("#1F2937");
        }

        if (isSelected) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor("#3B82F6"));
            painter->drawEllipse(rect);
            textColour = Qt::white;
        } else if (isEnabled && !isOtherMonth && cellDate == QDate::currentDate()) {
            painter->setPen(QPen(QColor("#3B82F6"), 1.5));
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(rect);
            textColour = QColor("#3B82F6");
        } else if (isHovered && isEnabled) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor("#E8F0FE"));
            painter->drawEllipse(rect);
        }

        painter->setPen(textColour);
        painter->drawText(option.rect, Qt::AlignCenter, text);
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        return QSize(32, 32);  // For smolness
    }

private:
    QCalendarWidget* calendar;
};

constexpr float Hmaxratio = 1.5;
constexpr float Wmaxratio = 1;
constexpr int reg = 8;
const QMargins regMargs(reg,reg,reg,reg);

class DateOverlay : public QWidget {
public:
    explicit DateOverlay(QWidget* parent, std::function<void()> click) : QWidget(parent), click(click) {
        parent->installEventFilter(this);
        setGeometry(parent->contentsRect());
        constrain();
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
            if (auto* w = qobject_cast<QWidget*>(watched)) {
                setGeometry(w->contentsRect());
                constrain();
            }
        }
        return QWidget::eventFilter(watched, event);
    }
    void constrain() {
        const auto margs = contentsMargins();
        const int W = width()-(2*reg);
        const int H = height()-(2*reg);
        if (W <= 0 || H <= 0) {
            setContentsMargins(regMargs);
            return;
        }
        if (float(H)/W < Hmaxratio) {
            if (float(W)/H < Wmaxratio) {
                setContentsMargins(regMargs);
                return;
            }
            const int xtra = qRound((W - H*Wmaxratio) / 2.0);
            setContentsMargins(reg+xtra,reg,reg+xtra,reg);
            return;
        }
        const int xtra = qRound((H - W*Hmaxratio) / 2.0);
        setContentsMargins(reg,reg+xtra,reg,reg+xtra);
    }

    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(125, 125, 125, 125));
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (QApplication::widgetAt(mapToGlobal(event->position().toPoint())) == this) {
            click();
            event->accept();
            return;
        }
        QWidget::mousePressEvent(event);
    }
    std::function<void()> click;
};


QDate getDate(const QDate& initial) {
    QWidget* prevFocus = QApplication::focusWidget();
    if (prevFocus) prevFocus->clearFocus();
    if (QGuiApplication::inputMethod()->isVisible()) {
        QGuiApplication::inputMethod()->hide();
    }
    bool accepted = false;
    QEventLoop loop;
    DateOverlay overlay(MG, [&]() {
        accepted = true;
        loop.quit();
    });
    auto* outer = new QVBoxLayout(&overlay);
    outer->setAlignment(Qt::AlignCenter);

    auto* card = new QWidget(&overlay);
    card->setObjectName("card");
    card->setAttribute(Qt::WA_StyledBackground, true);
    outer->addWidget(card);
    auto* layout = new QVBoxLayout(card);

    auto* calendar = new QCalendarWidget(card);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);  // Remove week numbers
    calendar->setGridVisible(false);
    calendar->setNavigationBarVisible(true);
    if (QTableView* view = calendar->findChild<QTableView*>("qt_calendar_calendarview")) {
        view->setItemDelegate(new CalendarItemDelegate(calendar, view));
        view->horizontalHeader()->setDefaultSectionSize(32);
        view->verticalHeader()->setDefaultSectionSize(32);
        view->setShowGrid(false);
        view->setMouseTracking(true);
    }
    auto fmt = QTextCharFormat();
    fmt.setForeground(QBrush(Qt::black));
    calendar->setWeekdayTextFormat(Qt::Saturday, fmt);
    calendar->setWeekdayTextFormat(Qt::Sunday, fmt);

    calendar->setSelectedDate(initial.isNull()? QDate::currentDate():initial);
    layout->addWidget(calendar);

    auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, card);
    layout->addWidget(btns);
    for (QPushButton* b : btns->findChildren<QPushButton*>()) {
        resizeFont(b, 1.5);
        b->setIcon(QIcon());
    }

    QObject::connect(btns, &QDialogButtonBox::accepted, [&]() {
        accepted = true;
        loop.quit();
    });
    QObject::connect(btns, &QDialogButtonBox::rejected, [&]() {
        loop.quit();
    });

    overlay.show();
    overlay.raise();
    loop.exec();
    overlay.hide();
    MG->removeEventFilter(&overlay);
    if (prevFocus) prevFocus->setFocus();
    if (accepted) return calendar->selectedDate();
    return {};
}
