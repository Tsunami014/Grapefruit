#include <QString>
#include <QPointF>

struct SceneItem {
    QString name;
    QPointF pos;
    bool flip;
};
std::vector<SceneItem> getSceneItems(QString scene);
std::pair<QString, QString> randomScene();
