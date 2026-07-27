#include "dbug.hpp"
#include <QGraphicsView>
#include <QGraphicsScene>

class House : public QGraphicsView {
public:
    House(QWidget* parent = nullptr);
    void newScene();
    QString curScnName() { return curScnNam; }

protected:
#ifdef DEBUG
    void keyPressEvent(QKeyEvent* event) override;
#endif
    void buildScene(QString scene);

    QGraphicsScene scn;
    QString curScnNam;
};
