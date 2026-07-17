#include <QString>
#include <QPointF>

struct SceneItem {
    QString name;
    QPointF pos;
};
std::vector<SceneItem> getSceneItems(QString scene);
