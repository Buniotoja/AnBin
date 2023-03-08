#include "mainwindow.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QString scanpath=w.getpath();
    QString to_show=w.scanning(scanpath);

        if(!to_show.isEmpty())
        {
            w.show();
            w.show_on_window(to_show);
        }
        else
        {
            w.show();
            w.show_on_window("empty return of scanning");
        }

    return a.exec();
}
