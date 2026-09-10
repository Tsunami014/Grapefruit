#pragma once
#include <QWidget>
#include <QMouseEvent>
#include "base/task.hpp"

class TaskBubble : public QWidget {
    Q_OBJECT
public:
    explicit TaskBubble(std::shared_ptr<Task> t, QWidget* parent = nullptr);

signals:
    void clicked();
    void toggleStar();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool pressed = false;
    bool hovered = false;

private:
    void refreshStyle();
};
