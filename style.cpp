#include "game.hpp"
#include <QFile>
#include <QStyleHints>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/qnativeinterface.h>
#endif

const QColor base(236, 161, 211);

void MainGame::setupStyle() {
    QObject::connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
              qApp, [this](Qt::ColorScheme scheme) {
        if (theme == -1) genStyle();
    });
    genStyle();
}


enum PalleteOpts {
    PRIMARY,
    SECONDARY,
    TERTIARY,
    NEUTRAL,
    NEUTRALVARIANT,
};

QColor getCol(float bhue, PalleteOpts palstyl, float tone) {
    switch (palstyl) {
        case PRIMARY:
            return QColor::fromHslF(bhue, 0.48, tone);
        case SECONDARY:
            return QColor::fromHslF(bhue, 0.18, tone);
        case TERTIARY:
            return QColor::fromHslF(
                std::fmod(bhue+(60.0f/360.0f), 1.0f), 0.24, tone);
        case NEUTRAL:
            return QColor::fromHslF(bhue, 0.04, tone);
        case NEUTRALVARIANT:
            return QColor::fromHslF(bhue, 0.08, tone);
    }
    return {};
}
inline QString getColName(float bhue, PalleteOpts palstyl, float tone) {
    return getCol(bhue, palstyl, tone).name();
}

void MainGame::genStyle() {
    bool light;
    if (theme == -1) {
        Qt::ColorScheme scheme = qApp->styleHints()->colorScheme();
        light = scheme != Qt::ColorScheme::Dark; // Includes unknown
    } else { light = theme == 1; }
    float bhue = base.hueF();

    styls = {
        getCol(bhue, NEUTRAL, light? 0.94:0.12), // cardbg
    };

    QFile file(":/style.qss");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    QString styl = QString::fromUtf8(file.readAll())
        .replace("$txt", getColName(bhue, PRIMARY, light? 0.1:0.9))
        .replace("$invtxt", getColName(bhue, PRIMARY, light? 0.9:0.1))
        .replace("$bg", getColName(bhue, NEUTRAL, light? 0.93:0.13))
        .replace("$cardbg", styls.cardbg.name())
        .replace("$fadecardbg", getColName(bhue, NEUTRAL, light? 0.80:0.20)) // Card BG but inline with background instead of overlay
        .replace("$ctbg", getColName(bhue, PRIMARY, light? 0.80:0.40))

        .replace("$primarydown", getColName(bhue, PRIMARY, light? 0.45:0.85))
        .replace("$primary", getColName(bhue, PRIMARY, light? 0.50:0.80))
        .replace("$seconddown", getColName(bhue, SECONDARY, light? 0.70:0.55))
        .replace("$secondary", getColName(bhue, SECONDARY, light? 0.75:0.50))
        .replace("$tertiarydown", getColName(bhue, TERTIARY, light? 0.70:0.55))
        .replace("$tertiary", getColName(bhue, TERTIARY, light? 0.75:0.50))
        .replace("$neutraldown", getColName(bhue, NEUTRALVARIANT, light? 0.70:0.55))
        .replace("$neutral", getColName(bhue, NEUTRALVARIANT, light? 0.75:0.50))
    ;
    setStyleSheet(styl);

    // If on Android, set the navigation icons to be dark/light with the theme
    // Must use a timer because of weird Qt quirks
#ifdef Q_OS_ANDROID
    if (!stylNavTimer) {
        stylNavTimer = new QTimer(this);
        stylNavTimer->setSingleShot(true);
    } else {
        stylNavTimer->disconnect();
    }

    auto updFn = [this, light]() {
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
    };

    connect(stylNavTimer, &QTimer::timeout, this, updFn);
    stylNavTimer->start(300);
    updFn();
#endif
}
