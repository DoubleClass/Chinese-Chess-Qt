#include "mydialog.h"
#include "ui_mydialog.h"
#include"ui_mainwindow.h"
#include"mainwindow.h"
#include"mainwindow2.h"
#include "ui_mainwindow2.h"
#include<QDebug>
MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    singleClicked = false;
}

MyDialog::~MyDialog()
{
    delete ui;
}

void MyDialog::on_pushButton_2_clicked()
{
    singleClicked = false;
    accept();

}

void MyDialog::on_pushButton_3_clicked()
{
    MainWindow *ww = new MainWindow();
    ww->loadgame();
    qDebug()<<ww->loadgame();
    ww->show();                                       //
    //exec();
    this->hide();
    reject();

}

void MyDialog::on_singleGame_clicked()
{
    singleClicked = true;
    MainWindow *xx =new MainWindow();
    xx->isSingleGame = true;
    emit sendData(singleClicked);
    xx->show();
    this->hide();
    reject();
}
