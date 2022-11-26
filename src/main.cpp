#include <QApplication>
#include <iostream>
#include "MainWindow.h"
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w(QDir::homePath().replace("/", QDir::separator()).append(QString(QDir::separator()) + ".SimpleDJ").toStdString());
    w.show();
    return QCoreApplication::exec();
}
