/********************************************************************************
** Form generated from reading UI file 'mydialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYDIALOG_H
#define UI_MYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyDialog
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QLabel *label;

    void setupUi(QDialog *MyDialog)
    {
        if (MyDialog->objectName().isEmpty())
            MyDialog->setObjectName(QStringLiteral("MyDialog"));
        MyDialog->resize(500, 400);
        QIcon icon;
        icon.addFile(QStringLiteral(":/image/image/timg.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        MyDialog->setWindowIcon(icon);
        verticalLayoutWidget = new QWidget(MyDialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(380, 100, 95, 161));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(228, 238, 240);\n"
"font: 11pt \"\346\245\267\344\275\223\";"));

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(verticalLayoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setStyleSheet(QString::fromUtf8("background-color: rgb(228, 238, 240);\n"
"font: 11pt \"\346\245\267\344\275\223\";"));

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(verticalLayoutWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setStyleSheet(QString::fromUtf8("background-color: rgb(226, 236, 238);\n"
"font: 11pt \"\346\245\267\344\275\223\";"));

        verticalLayout->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(verticalLayoutWidget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setStyleSheet(QString::fromUtf8("background-color: rgb(225, 235, 237);\n"
"font: 11pt \"\346\245\267\344\275\223\";"));

        verticalLayout->addWidget(pushButton_4);

        label = new QLabel(MyDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 0, 500, 400));
        label->setStyleSheet(QStringLiteral("border-image: url(:/image/image/n.jpg);"));
        label->raise();
        verticalLayoutWidget->raise();

        retranslateUi(MyDialog);
        QObject::connect(pushButton_4, SIGNAL(clicked()), MyDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(MyDialog);
    } // setupUi

    void retranslateUi(QDialog *MyDialog)
    {
        MyDialog->setWindowTitle(QApplication::translate("MyDialog", "Dialog", 0));
        pushButton->setText(QApplication::translate("MyDialog", "\345\215\225\344\272\272\346\270\270\346\210\217", 0));
        pushButton_2->setText(QApplication::translate("MyDialog", "\345\217\214\344\272\272\346\270\270\346\210\217", 0));
        pushButton_3->setText(QApplication::translate("MyDialog", "\347\273\247\347\273\255\346\270\270\346\210\217", 0));
        pushButton_4->setText(QApplication::translate("MyDialog", "\351\200\200\345\207\272\346\270\270\346\210\217", 0));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MyDialog: public Ui_MyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYDIALOG_H
