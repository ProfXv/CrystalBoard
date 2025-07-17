#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("CrystalBoard");
    QCoreApplication::setApplicationName("CrystalBoard");

    MainWindow w;
    w.show();

    return a.exec();
}
