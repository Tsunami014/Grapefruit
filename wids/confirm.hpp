#include <QWidget>
#include <QString>
#include <QDialogButtonBox>

enum ConfirmOpts {
    Conf_YESNO,
    Conf_YESNOCANCEL,
    Conf_OK
};

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

QDialogButtonBox::ButtonRole confirm(QWidget* parent, const QString& text, ConfirmOpts opts);
