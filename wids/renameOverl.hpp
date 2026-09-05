#pragma once
#include <QWidget>

class RenameOverlay : public QWidget {
    Q_OBJECT
public:
    explicit RenameOverlay(QString title, QString initial, std::function<void(QString)> onchange, QWidget* ref, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void setSze();

    QWidget* main;
    QWidget* ref;
};
