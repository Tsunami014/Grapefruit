#include "date.hpp"
#include <QDialog>
#include <QBoxLayout>
#include <QCalendarWidget>
#include <QDialogButtonBox>

QDate getDate(const QDate& initial) {
    QDialog dialog(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint);
    dialog.setWindowTitle("Select Date");

    auto* layout = new QVBoxLayout(&dialog);
    auto* calendar = new QCalendarWidget(&dialog);
    if (!initial.isNull()) calendar->setSelectedDate(initial);
    layout->addWidget(calendar);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QObject::connect(calendar, &QCalendarWidget::activated, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted) {
        return calendar->selectedDate();
    }
    return {};
}
