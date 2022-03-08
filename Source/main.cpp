#include "Graphics/Components/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Graphics::MainWindow w;
    QObject::connect(&w, &Graphics::MainWindow::anotherInstanceRequired, [=](const QString &projectFileToLoad)
    {
        auto *newWindow = new Graphics::MainWindow(projectFileToLoad);
        newWindow->setAttribute(Qt::WA_DeleteOnClose, true);
        newWindow->show();
    });
    w.showMaximized();
    return QApplication::exec();
}
