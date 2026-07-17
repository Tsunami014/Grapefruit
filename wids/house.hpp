#include <QGraphicsView>
#include <QGraphicsScene>

#define BUILD

class House : public QGraphicsView {
public:
    House(QWidget* parent = nullptr);
    void buildScene(QString scene);

protected:
#ifdef BUILD
    void keyPressEvent(QKeyEvent* event) override;
#endif

private:
    QGraphicsScene scn;
};
