#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.anotherInstanceRequired = [=](QString projectFileToLoad) {
        MainWindow *newWindow = new MainWindow(projectFileToLoad);
        newWindow->show();
    };
    w.show();
    return a.exec();
}
