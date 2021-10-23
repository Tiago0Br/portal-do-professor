#include "portalaluno.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PortalAluno w;
    w.show();
    return a.exec();
}
