#pragma once
#include <QWidget>

class RenameOverlay : public QWidget {
public:
    explicit RenameOverlay(QString initial, std::function<void(QString)> onchange, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    QWidget* main;
};
