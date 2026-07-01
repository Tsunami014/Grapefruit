#pragma once
#include <QWidget>
#include <QBoxLayout>
#include <QMouseEvent>

class TaskBubble : public QWidget {
    Q_OBJECT
public:
    explicit TaskBubble(const QString& top, const QString& bot, QWidget* parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool pressed = false;

private:
    QVBoxLayout* main;
    void refreshStyle();
};
