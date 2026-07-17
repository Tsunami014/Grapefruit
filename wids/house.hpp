#include <QGraphicsView>
#include <QGraphicsScene>

class House : public QGraphicsView {
public:
    House(QWidget* parent = nullptr);
    void buildScene(QString scene);

private:
    QGraphicsScene scn;
};
