#pragma once
#include <QTextEdit>

class TxtEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit TxtEdit(QWidget* parent = nullptr);
    explicit TxtEdit(const QString& text, QWidget* parent = nullptr);

signals:
    void focusChange(bool focus);
protected:
    void focusInEvent(QFocusEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    void init();
};
