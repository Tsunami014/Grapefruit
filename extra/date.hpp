#pragma once
#include <QDate>
#include <QWidget>

extern bool showingDate;
QDate getDate(const QDate& initial = {});

QString parseTime(float hours, bool brief = false);
QString parseDate(const QDate& date, bool brief = false);
