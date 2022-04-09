#include "mainwindow.h"
#include <QApplication>
#include "canvas.h"
#include "openingwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Model *m = nullptr;
    OpeningWindow o(m);
    o.show();
    return a.exec();
}
