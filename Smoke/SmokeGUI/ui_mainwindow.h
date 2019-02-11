/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "mainview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QGroupBox *settingsGB;
    QSlider *ngonSlider;
    QLabel *ngonLabel;
    QLabel *ngonValue;
    MainView *mainView;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1366, 768);
        MainWindow->setStyleSheet(QLatin1String("/* Groupbox */\n"
"\n"
"QGroupBox {\n"
"    border: 1px solid #DDD;\n"
"    border-radius: 9px;\n"
"    margin-top: 9px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 3px 0 3px;\n"
"}"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(4, 4, 4, 4);
        settingsGB = new QGroupBox(centralWidget);
        settingsGB->setObjectName(QStringLiteral("settingsGB"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(settingsGB->sizePolicy().hasHeightForWidth());
        settingsGB->setSizePolicy(sizePolicy);
        settingsGB->setMinimumSize(QSize(240, 0));
        ngonSlider = new QSlider(settingsGB);
        ngonSlider->setObjectName(QStringLiteral("ngonSlider"));
        ngonSlider->setGeometry(QRect(20, 70, 151, 16));
        ngonSlider->setMinimum(3);
        ngonSlider->setMaximum(24);
        ngonSlider->setPageStep(2);
        ngonSlider->setValue(6);
        ngonSlider->setOrientation(Qt::Horizontal);
        ngonLabel = new QLabel(settingsGB);
        ngonLabel->setObjectName(QStringLiteral("ngonLabel"));
        ngonLabel->setGeometry(QRect(20, 40, 201, 20));
        ngonValue = new QLabel(settingsGB);
        ngonValue->setObjectName(QStringLiteral("ngonValue"));
        ngonValue->setGeometry(QRect(180, 68, 31, 20));

        horizontalLayout->addWidget(settingsGB);

        mainView = new MainView(centralWidget);
        mainView->setObjectName(QStringLiteral("mainView"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mainView->sizePolicy().hasHeightForWidth());
        mainView->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(mainView);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Basic OpenGL framework for ACG", 0));
        settingsGB->setTitle(QApplication::translate("MainWindow", "Settings", 0));
        ngonLabel->setText(QApplication::translate("MainWindow", "The n in n-gon", 0));
        ngonValue->setText(QApplication::translate("MainWindow", "6", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
