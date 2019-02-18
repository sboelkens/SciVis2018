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

void MainWindow::on_selectColormapRho_currentIndexChanged(int index)
{
    ui->mainView->scalar_col = index;
}

void MainWindow::on_selectNColorsRho_valueChanged(int value)
{
    ui->mainView->levels_rho = value;
}

void MainWindow::on_selectNColorsV_valueChanged(int value)
{
    ui->mainView->levels_v = value;
}

void MainWindow::on_selectNColorsF_valueChanged(int value)
{
    ui->mainView->levels_f = value;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event->key();
    switch(event->key()) {
    case 'A':
        ui->mainView->frozen = !ui->mainView->frozen;
        break;
    }
}
