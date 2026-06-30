// Thanks in part to https://stackoverflow.com/a/37119983 !
#include <QVBoxLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>

class HeaderButton : public QToolButton {
    Q_OBJECT
public:
    explicit HeaderButton(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
};

class Spoiler : public QWidget {
    Q_OBJECT
public:
    explicit Spoiler(const QString& title = "", QWidget* parent = 0);
    void setContentLayout(QLayout& contentLayout);
private:
    QVBoxLayout mainLayout;
    HeaderButton toggleButton;
    QParallelAnimationGroup toggleAnimation;
    QScrollArea contentArea;
};
