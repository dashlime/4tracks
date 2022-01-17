#include "Graphics/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.anotherInstanceRequired = [=](const QString& projectFileToLoad)
    {
        auto *newWindow = new MainWindow(projectFileToLoad);
        newWindow->show();
    };
    w.show();
    return QApplication::exec();
}
