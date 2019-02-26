#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setFocus();
//  setColorLegend();

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
    QIcon pm = setColorLegend(260, 30, ui->mainView->levels_rho, index);
    ui->legendSmoke->setIcon(pm);
    ui->legendSmoke->setIconSize(QSize(260, 30));
    this->setFocus();
}

void MainWindow::on_selectNColorsSmoke_valueChanged(int value)
{
    ui->mainView->levels_rho = value;
    ui->mainView->updateUniformsRequired = true;
    QIcon pm = setColorLegend(260, 30, value, ui->mainView->scalar_col);
    ui->legendSmoke->setIcon(pm);
    ui->legendSmoke->setIconSize(QSize(260, 30));
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

QIcon MainWindow::setColorLegend(int width, int height, int levels, int color)
{
    qDebug() << "setColorLegend";

    QPixmap pm(width, height);
    QPainter pmp(&pm);
    double color_width = static_cast<double>(width)/(static_cast<double>(levels)+1);

    for(int x = 0; x <= levels;x++) {
        float fx = static_cast<float>(x);\
        float flevels = static_cast<float>(levels);

        QVector3D rgb = set_colormap(fx/flevels,color, levels);
        QColor color = QColor(static_cast<int>(rgb.x()*255), static_cast<int>(rgb.y()*255), static_cast<int>(rgb.z()*255));
        pmp.setBrush(QBrush(color));
        pmp.setPen(Qt::NoPen);
        if ( x % 2 == 0) {
            pmp.drawRect(floor(x*color_width), 0, floor(color_width+10), height);
        } else {
            pmp.drawRect(ceil(x*color_width), 0, ceil(color_width+10), height);
        }
    }

    return QIcon(pm);
}
