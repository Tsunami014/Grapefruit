#include "dbug.hpp"
#include <QGraphicsView>
#include <QGraphicsScene>

class House : public QGraphicsView {
public:
    House(QWidget* parent = nullptr);
    void newScene();

protected:
#ifdef DEBUG
    void keyPressEvent(QKeyEvent* event) override;
#endif
    void buildScene(QString scene);

    QGraphicsScene scn;
};
