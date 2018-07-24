#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>

namespace Ui {
class MyDialog;
}

class MyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = 0);
    ~MyDialog();
    bool singleClicked;

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_singleGame_clicked();

private:
    Ui::MyDialog *ui;
signals:
    void sendData(bool);
};

#endif // MYDIALOG_H
