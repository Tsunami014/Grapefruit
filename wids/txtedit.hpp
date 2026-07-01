#pragma once
#include <QTextEdit>

class TxtEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit TxtEdit(QWidget* parent = nullptr);
    explicit TxtEdit(const QString& text, QWidget* parent = nullptr);

private:
    void init();
};
