#include <QWidget>
#include <QString>

class ConfirmOverlay : public QWidget {
    Q_OBJECT
public:
    ConfirmOverlay(QWidget* parent) : QWidget(parent) {}
signals:
    void pressed();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
};

bool confirm(QWidget* parent, const QString& title, const QString& text);
