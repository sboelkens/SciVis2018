#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->mainView->setFocus();
  qDebug() << "✓✓ MainWindow constructor";
}

MainWindow::~MainWindow() {
  delete ui;
  qDebug() << "✗✗ MainWindow destructor";
}

void MainWindow::on_showRho_stateChanged(int state)
{
    ui->mainView->draw_smoke = state;
}

void MainWindow::on_showV_stateChanged(int state)
{
    ui->mainView->draw_vecs = state;
}

void MainWindow::on_showF_stateChanged(int state)
{
}
