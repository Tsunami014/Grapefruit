#include <QGraphicsView>
#include <QGraphicsScene>

class House : public QGraphicsView {
public:
    House(QWidget* parent = nullptr);

private:
    QGraphicsScene scn;
};
