// Thanks in part to https://stackoverflow.com/a/37119983 !
#include <QVBoxLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>

class HeaderButton : public QToolButton {
    Q_OBJECT
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(QColor lineColour READ lineColour WRITE setLineColour)
public:
    explicit HeaderButton(QWidget* parent = nullptr);

    int lineWidth() const { return lwid; }
    void setLineWidth(int w) { if (lwid != w) { lwid = w; update(); } }
    QColor lineColour() const { return lcol; }
    void setLineColour(QColor c) { if (lcol != c) { lcol = c; update(); } }
protected:
    void paintEvent(QPaintEvent* event) override;

    int lwid = 2;
    QColor lcol = QColor();
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
