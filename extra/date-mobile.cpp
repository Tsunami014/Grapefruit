#include "date.hpp"
#include <QJniObject>
#include <QJniEnvironment>
#include <QEventLoop>
#include <QCoreApplication>
#include <QtCore/qnativeinterface.h>

QEventLoop* loop = nullptr;
static constexpr auto bridgeCls = "org/extra/AndrdQtBridge/DatePick";
QDate result;

void JNICALL onDateSet(JNIEnv*, jobject, jint year, jint month, jint day) {
    QMetaObject::invokeMethod(qApp, [year, month, day]() {
        result = QDate(year, month + 1, day); // Android months are 0-based
        if (loop) loop->quit();
    }, Qt::QueuedConnection);
}

void JNICALL onCancel(JNIEnv*, jobject) {
    QMetaObject::invokeMethod(qApp, []() {
        if (loop) loop->quit();
    }, Qt::QueuedConnection);
}

bool registerNatives() {
    static bool registered = false;
    if (registered) return true;
    QJniEnvironment env;
    const JNINativeMethod methods[] = {
        { "nativeOnDateSet", "(III)V", reinterpret_cast<void*>(onDateSet) },
        { "nativeOnCancel", "()V", reinterpret_cast<void*>(onCancel)  }
    };
    registered = env.registerNativeMethods(bridgeCls, methods, 2);
    return registered;
}

QDate getDate(const QDate& initial) {
    if (!registerNatives()) return {};

    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([initial]() {
        QJniObject context = QNativeInterface::QAndroidApplication::context();
        QJniObject::callStaticMethod<void>(
            bridgeCls, "show",
            "(Landroid/content/Context;III)V",
            context.object(), initial.year(), initial.month() - 1, initial.day());
    });

    QEventLoop evloop;
    loop = &evloop;
    evloop.exec();
    loop = nullptr;

    return result;
}
