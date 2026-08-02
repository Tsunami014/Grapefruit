#include "saveesc.hpp"
#include <QRegularExpression>

QString escape(QString inp) {
    return inp
        .replace(QRegularExpression(R"([\x01-\x05])"), "")
        .replace("\r\n", "\1")
        .replace('\n', '\1')
        .replace(';', '\2')
        .replace('-', '\3')
    ;
}
QString deescape(QString inp) {
    return inp
        .replace('\1', '\n')
        .replace('\2', ';')
        .replace('\3', '-')
    ;
}
