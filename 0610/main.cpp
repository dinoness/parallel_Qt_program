#include "mainwindow.h"

#include <QApplication>
#include "app/AppContext.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppContext appContext;

    MainWindow w(&appContext);
    w.show();
    return a.exec();
}
