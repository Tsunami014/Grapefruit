#include "game.hpp"
#include <QFile>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/qnativeinterface.h>
#endif

const QColor base(236, 161, 211);


enum PalleteOpts {
    PRIMARY,
    SECONDARY,
    TERTIARY,
    NEUTRAL,
    NEUTRALVARIANT,
};

QString getCol(float bhue, PalleteOpts palstyl, float tone) {
    switch (palstyl) {
        case PRIMARY:
            return QColor::fromHslF(bhue, 0.48, tone).name();
        case SECONDARY:
            return QColor::fromHslF(bhue, 0.16, tone).name();
        case TERTIARY:
            return QColor::fromHslF(std::fmod(bhue+60.0, 360.0), 0.24, tone).name();
        case NEUTRAL:
            return QColor::fromHslF(bhue, 0.4, tone).name();
        case NEUTRALVARIANT:
            return QColor::fromHslF(bhue, 0.8, tone).name();
    }
    return {};
}

void MainGame::genStyle() {
    const bool light = true;
    float bhue = base.hueF();

    QFile file(":/style.qss");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    QString styl = QString::fromUtf8(file.readAll())
        .replace("$txt", getCol(bhue, PRIMARY, light? 0.10:0.90))
        .replace("$invtxt", getCol(bhue, PRIMARY, light? 1.0:0.20))
        .replace("$bg", getCol(bhue, NEUTRAL, light? 0.98:0.06))
        .replace("$cardbg", getCol(bhue, NEUTRAL, light? 0.94:0.12))
    ;
    setStyleSheet(styl);

    // If on Android, set the navigation icons to be dark/light with the theme
    // Must use a timer because of weird Qt quirks
#ifdef Q_OS_ANDROID
    if (!stylNavTimer) {
        stylNavTimer = new QTimer(this);
        stylNavTimer->setSingleShot(true);
    }

    // Disconnect any previous lambda so we don't fire a stale `light` value
    // from an earlier call to genStyle().
    stylNavTimer->disconnect();
    connect(stylNavTimer, &QTimer::timeout, this, [this, light]() {
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([light]() {
            QJniObject activity = QNativeInterface::QAndroidApplication::context();
            QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

            if (QNativeInterface::QAndroidApplication::sdkVersion() >= 30) {
                QJniObject controller = window.callObjectMethod(
                    "getInsetsController", "()Landroid/view/WindowInsetsController;");
                if (controller.isValid()) {
                    const int APPEARANCE_LIGHT_STATUS_BARS = 8;
                    const int APPEARANCE_LIGHT_NAVIGATION_BARS = 16;
                    int mask = APPEARANCE_LIGHT_STATUS_BARS | APPEARANCE_LIGHT_NAVIGATION_BARS;
                    int value = light ? mask : 0;   // set flag => dark icons on light bg
                    controller.callMethod<void>("setSystemBarsAppearance", "(II)V", value, mask);
                }
            } else {
                QJniObject decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
                jint visibility = decorView.callMethod<jint>("getSystemUiVisibility", "()I");

                const int flags = 0x2000 | 0x10; // LIGHT_STATUS_BAR | LIGHT_NAVIGATION_BAR
                visibility = light ? (visibility | flags) : (visibility & ~flags);

                decorView.callMethod<void>("setSystemUiVisibility", "(I)V", visibility);
            }
        });
    });
    stylNavTimer->start(300);
#endif
}
