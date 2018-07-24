#include "mainwindow.h"
#include"Item.h"
#include <QApplication>
#include"mydialog.h"
#include"mainwindow2.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyDialog dialog;

    if(dialog.exec() == QDialog::Accepted){
    MainWindow w;
    w.show();
    return a.exec();
    }

    else return a.exec();
    return 0;
}



