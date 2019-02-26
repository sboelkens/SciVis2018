#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setFocus();
  setColorLegend();

  qDebug() << "✓✓ MainWindow constructor";
}

MainWindow::~MainWindow() {
  delete ui;
  qDebug() <<    "✗✗ MainWindow destructor";
}

void MainWindow::on_showSmoke_stateChanged(int state)
{
    ui->mainView->draw_smoke = state;
    this->setFocus();
}

void MainWindow::on_showGlyph_stateChanged(int state)
{
    ui->mainView->draw_vecs = state;
    this->setFocus();
}

//void MainWindow::on_showF_stateChanged(int state)
//{
//    ui->mainView->draw_force_field = state;
//    this->setFocus();
//}

void MainWindow::on_selectColormapSmoke_currentIndexChanged(int index)
{
    ui->mainView->scalar_col = index;
    ui->mainView->updateUniformsRequired = true;
    setColorLegend();
    this->setFocus();
}

void MainWindow::on_selectNColorsSmoke_valueChanged(int value)
{
    ui->mainView->levels_rho = value;
    ui->mainView->updateUniformsRequired = true;
    setColorLegend();
    this->setFocus();
}

void MainWindow::on_selectColormapGlyph_currentIndexChanged(int index)
{
//    ui->mainView->scalar_col = index;
//    ui->mainView->updateUniformsRequired = true;
//    setColorLegend();
//    this->setFocus();
}

void MainWindow::on_selectNColorsGlyph_valueChanged(int value)
{
    ui->mainView->levels_v = value;
    this->setFocus();
}

void MainWindow::on_timeStepSlider_valueChanged(int value)
{
    ui->mainView->dt = (static_cast<double>(value) / 10);
}

void MainWindow::on_viscositySlider_valueChanged(int value)
{
    ui->mainView->visc = (static_cast<float>(value) / 1000);
}

//void MainWindow::on_selectNColorsF_valueChanged(int value)
//{
//    ui->mainView->levels_f = value;
//    this->setFocus();
//}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event->key();
    switch(event->key()) {
    case 'A':
        ui->mainView->frozen = !ui->mainView->frozen;
        break;
    }
}

void MainWindow::setColorLegend(int width, int height)
{
    qDebug() << "setColorLegend";
    QLinearGradient grBtoY(0, 0, width, 0);
    for(int x = 0; x <= ui->mainView->levels_rho;x++) {
        float fx = static_cast<float>(x);
        float flevels = static_cast<float>(ui->mainView->levels_rho);

        QVector3D rgb = set_colormap(fx/flevels,ui->mainView->scalar_col, ui->mainView->levels_rho);
        QColor color = QColor(static_cast<int>(rgb.x()*255), static_cast<int>(rgb.y()*255), static_cast<int>(rgb.z()*255));
        grBtoY.setColorAt(static_cast<double>(fx/flevels), color);
    }
    QPixmap pm(width, height);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, width, height);
    ui->legendSmoke->setIcon(QIcon(pm));
    ui->legendSmoke->setIconSize(QSize(width, height));
}
