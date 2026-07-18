#include <QGraphicsView>
#include <QGraphicsScene>

#define BUILD

class House : public QGraphicsView {
public:
    House(QWidget* parent = nullptr);
    void newScene();

protected:
#ifdef BUILD
    void keyPressEvent(QKeyEvent* event) override;
#endif
    void buildScene(QString scene);

    QGraphicsScene scn;
};
