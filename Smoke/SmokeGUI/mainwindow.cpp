#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->mainView->setFocus();
  setColorLegend();

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
    ui->mainView->draw_force_field = state;
}

void MainWindow::on_selectColormapRho_currentIndexChanged(int index)
{
    ui->mainView->scalar_col = index;
    setColorLegend();
}

void MainWindow::on_selectNColorsRho_valueChanged(int value)
{
    ui->mainView->levels_rho = value;
    setColorLegend();
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

void MainWindow::setColorLegend()
{
    qDebug() << "setColorLegend";
    QLinearGradient grBtoY(0, 0, 240, 0);
    for(int x = 0; x <= ui->mainView->levels_rho;x++) {
        float fx = static_cast<float>(x);
        float flevels = static_cast<float>(ui->mainView->levels_rho);

        QVector3D rgb = set_colormap(fx/flevels,ui->mainView->scalar_col, ui->mainView->levels_rho);
        QColor color = QColor(static_cast<int>(rgb.x()*255), static_cast<int>(rgb.y()*255), static_cast<int>(rgb.z()*255));
        grBtoY.setColorAt(static_cast<double>(fx/flevels), color);
    }
    QPixmap pm(240, 30);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 240, 30);
    ui->legendView->setIcon(QIcon(pm));
    ui->legendView->setIconSize(QSize(240, 30));
}
