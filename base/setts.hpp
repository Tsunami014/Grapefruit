#pragma once
#include <QStringList>

void loadSetting(QString name, QString conts);
std::unordered_map<QString, QString> saveSettings();
