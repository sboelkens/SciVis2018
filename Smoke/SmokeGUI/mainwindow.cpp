#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTime"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  qDebug() << "✓✓ MainWindow constructor";

  ui->setupUi(this);
  this->setFocus();

  this->waitForInitialization();

}

MainWindow::~MainWindow() {
  delete ui;
  qDebug() <<    "✗✗ MainWindow destructor";
}

void MainWindow::waitForInitialization()
{
    if(ui->mainView->is_initialized) {
        this->setSmokeColorLegend();
        this->setGlyphColorLegend();
    } else {
        QTimer *timer = new QTimer;
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(waitForInitialization()));
        timer->start(100);
    }

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

void MainWindow::on_selectColormapSmoke_currentIndexChanged(int index)
{
    ui->mainView->smoke_col = index;
    this->setSmokeColorLegend();
}

void MainWindow::on_selectNColorsSmoke_valueChanged(int value)
{
    ui->mainView->levels_smoke = value;
    this->setSmokeColorLegend();
}

void MainWindow::on_selectColormapGlyph_currentIndexChanged(int index)
{
    ui->mainView->glyph_col = index;
    this->setGlyphColorLegend();
}

void MainWindow::on_selectNColorsGlyph_valueChanged(int value)
{
    ui->mainView->levels_glyph = value;
    this->setGlyphColorLegend();
}

void MainWindow::on_timeStepSlider_valueChanged(int value)
{
    ui->mainView->dt = (static_cast<double>(value) / 10);
}

void MainWindow::on_viscositySlider_valueChanged(int value)
{
    ui->mainView->visc = (static_cast<float>(value) / 1000);
}

void MainWindow::on_radioSmokeRho_clicked()
{
    ui->mainView->smoke_var = RHO;
}

void MainWindow::on_radioSmokeV_clicked()
{
    ui->mainView->smoke_var = V;
}

void MainWindow::on_radioSmokeF_clicked()
{
    ui->mainView->smoke_var = F;
}

void MainWindow::on_radioGlyphRho_clicked()
{
    ui->mainView->glyph_var = RHO;
}

void MainWindow::on_radioGlyphV_clicked()
{
    ui->mainView->glyph_var = V;
}

void MainWindow::on_radioGlyphF_clicked()
{
    ui->mainView->glyph_var = F;
}

void MainWindow::on_radioGlyphVectorV_clicked()
{
    ui->mainView->glyph_vector_var = V;
}

void MainWindow::on_radioGlyphVectorF_clicked()
{
    ui->mainView->glyph_vector_var = F;
}

void MainWindow::on_clampMinValue_valueChanged(double value)
{
    qDebug() << "on_clampMinValue_valueChanged" << value;
    ui->mainView->clamp_min = value;
}

void MainWindow::on_clampMaxValue_valueChanged(double value)
{
    qDebug() << "on_clampMaxValue_valueChanged" << value;
    ui->mainView->clamp_max = value;
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

void MainWindow::setSmokeColorLegend()
{
    QIcon pm = setColorLegend(260, 30, ui->mainView->levels_smoke, ui->mainView->smoke_col);
    ui->legendSmoke->setIcon(pm);
    ui->legendSmoke->setIconSize(QSize(260, 30));
    this->setFocus();
}

void MainWindow::setGlyphColorLegend()
{
    QIcon pm = setColorLegend(260, 30, ui->mainView->levels_glyph, ui->mainView->glyph_col);
    ui->legendGlyph->setIcon(pm);
    ui->legendGlyph->setIconSize(QSize(260, 30));
    this->setFocus();
}

QIcon MainWindow::setColorLegend(int width, int height, int levels, int color)
{
    qDebug() << "setColorLegend";
    ui->mainView->updateUniformsRequired = true;

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
