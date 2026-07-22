#include "date.hpp"
#include "style.hpp"
#include <QDialog>
#include <QBoxLayout>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QHeaderView>
#include <QTableView>
#include <QTextCharFormat>

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


QDate getDate(const QDate& initial) {
    QDialog dialog(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint);
    dialog.setWindowTitle("Select Date");
    StyleWidget(&dialog);

    auto* layout = new QVBoxLayout(&dialog);
    auto* calendar = new QCalendarWidget(&dialog);
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

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        return calendar->selectedDate();
    }
    return {};
}
