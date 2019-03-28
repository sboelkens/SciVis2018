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


void MainWindow::on_selectSmoke_currentIndexChanged(int index)
{
    ui->mainView->smoke_var = index;
    this->setSmokeColorLegend();
}
void MainWindow::on_selectNColorsSmoke_valueChanged(int value)
{
    ui->mainView->levels_smoke = value;
    this->setSmokeColorLegend();
}
void MainWindow::on_selectColormapSmoke_currentIndexChanged(int index)
{
    ui->mainView->smoke_col = index;
    this->setSmokeColorLegend();
}
void MainWindow::on_showSmoke_stateChanged(int state)
{
    ui->mainView->draw_smoke = state;
    this->setFocus();
}


void MainWindow::on_radioSmokeScale_clicked()
{
    ui->mainView->clamp_smoke_cmap = false;
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_radioSmokeClamp_clicked()
{
    ui->mainView->clamp_smoke_cmap = true;
    ui->mainView->clamp_smoke_max = static_cast<float>(ui->clampSmokeMaxValue->value());
    ui->mainView->clamp_smoke_min = static_cast<float>(ui->clampSmokeMinValue->value());
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_clampSmokeMinValue_valueChanged(double value)
{
    ui->mainView->clamp_smoke_min = static_cast<float>(value);
    ui->labelSmokeLegendMin->setText(QString::number(value));
    float mid = (ui->mainView->clamp_smoke_min+ui->mainView->clamp_smoke_max)/2;
    ui->labelSmokeLegendMid->setText(QString::number(static_cast<double>(mid)));
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_clampSmokeMaxValue_valueChanged(double value)
{
    ui->mainView->clamp_smoke_max = static_cast<float>(value);
    ui->labelSmokeLegendMax->setText(QString::number(value));
    float mid = (ui->mainView->clamp_smoke_min+ui->mainView->clamp_smoke_max)/2;
    ui->labelSmokeLegendMid->setText(QString::number(static_cast<double>(mid)));
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}


void MainWindow::on_selectGlyph_currentIndexChanged(int index)
{
    ui->mainView->glyph_var = index;
    this->setGlyphColorLegend();
}
void MainWindow::on_selectNColorsGlyph_valueChanged(int value)
{
    ui->mainView->levels_glyph = value;
    this->setGlyphColorLegend();
}
void MainWindow::on_selectColormapGlyph_currentIndexChanged(int index)
{
    ui->mainView->glyph_col = index;
    this->setGlyphColorLegend();
}
void MainWindow::on_showGlyph_stateChanged(int state)
{
    ui->mainView->draw_vecs = state;
    this->setFocus();
}


void MainWindow::on_radioGlyphScale_clicked()
{
    ui->mainView->clamp_glyph_cmap = false;
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_radioGlyphClamp_clicked()
{
    ui->mainView->clamp_glyph_cmap = true;
    ui->mainView->clamp_glyph_max = static_cast<float>(ui->clampGlyphMaxValue->value());
    ui->mainView->clamp_glyph_min = static_cast<float>(ui->clampGlyphMinValue->value());
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_clampGlyphMinValue_valueChanged(double value)
{
    ui->mainView->clamp_glyph_min = static_cast<float>(value);
    ui->labelGlyphLegendMin->setText(QString::number(value));
    float mid = (ui->mainView->clamp_glyph_min+ui->mainView->clamp_glyph_max)/2;
    ui->labelGlyphLegendMid->setText(QString::number(static_cast<double>(mid)));
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_clampGlyphMaxValue_valueChanged(double value)
{
    ui->mainView->clamp_glyph_max = static_cast<float>(value);
    ui->labelGlyphLegendMax->setText(QString::number(value));
    float mid = (ui->mainView->clamp_glyph_min+ui->mainView->clamp_glyph_max)/2;
    ui->labelGlyphLegendMid->setText(QString::number(static_cast<double>(mid)));
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}


void MainWindow::on_radioGlyphVectorV_clicked()
{
    ui->mainView->glyph_vector_var = V;
    this->setFocus();
}
void MainWindow::on_radioGlyphVectorF_clicked()
{
    ui->mainView->glyph_vector_var = F;
    this->setFocus();
}


void MainWindow::on_nrGlyphsX_valueChanged(int value)
{
    ui->mainView->nr_glyphs_x = value;
    ui->mainView->nr_glyphs_changed = true;
    this->setFocus();
}
void MainWindow::on_nrGlyphsY_valueChanged(int value)
{
    ui->mainView->nr_glyphs_y = value;
    ui->mainView->nr_glyphs_changed = true;
    this->setFocus();
}
void MainWindow::on_nrGlyphsP_valueChanged(int value)
{
    ui->mainView->nr_glyphs_p = value;
    ui->mainView->nr_glyphs_changed = true;
    this->setFocus();
}


void MainWindow::on_rhoIsolineValue_valueChanged(double value)
{
    ui->mainView->rho_isoline_value = static_cast<float>(value);
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}

void MainWindow::on_selectGlyphShape_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->mainView->glyphs3D = true;
        ui->mainView->nr_glyphs_x = 20;
        ui->mainView->nr_glyphs_y = 20;
        //ui->mainView->nr_glyphs_p = 4;
        ui->nrGlyphsP->setValue(1);
    }
    else if (index == 1)
    {
        ui->mainView->glyphs3D = false;
        ui->mainView->nr_glyphs_x = 50;
        ui->mainView->nr_glyphs_y = 50;
        //ui->mainView->nr_glyphs_p = 1;
        ui->nrGlyphsP->setValue(4);
    }
    ui->mainView->nr_glyphs_changed = true;
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}


void MainWindow::on_timeStepSlider_valueChanged(int value)
{
    ui->mainView->dt = (static_cast<double>(value) / 10);
    this->setFocus();
}
void MainWindow::on_viscositySlider_valueChanged(int value)
{
    ui->mainView->visc = (static_cast<float>(value) / 1000);
    this->setFocus();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case 'A':
    case Qt::Key_Space:
        ui->mainView->frozen = !ui->mainView->frozen;
        break;
    case Qt::Key_1:
        this->setPresetOne();
        break;
    case Qt::Key_2:
        this->setPresetTwo();
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
            pmp.drawRect(static_cast<int>(floor(x*color_width)),
                         0,
                         static_cast<int>(floor(color_width+10)),
                         height);
        } else {
            pmp.drawRect(static_cast<int>(ceil(x*color_width)),
                         0,
                         static_cast<int>(ceil(color_width+10)),
                         height);
        }
    }

    return QIcon(pm);
}


void MainWindow::setPresetOne()
{
    // Set presets Default values

    ui->selectSmoke->setCurrentIndex(RHO);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_BLACKWHITE);
    ui->selectNColorsSmoke->setValue(10);
    ui->showSmoke->setCheckState(Qt::Checked);

//    ui->radioSmokeScale->click();
    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(0.00);
    ui->clampSmokeMaxValue->setValue(1.00);

    ui->selectGlyph->setCurrentIndex(RHO);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsGlyph->setValue(10);
    ui->showGlyph->setCheckState(Qt::Checked);

//    ui->radioGlyphScale->click();
//    ui->radioGlyphClamp->click();
//    ui->clampGlyphMinValue->setValue(-0.001);
//    ui->clampGlyphMaxValue->setValue(0.001);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();

    ui->timeStepSlider->setValue(1);
    ui->viscositySlider->setValue(4);
}
void MainWindow::setPresetTwo()
{
    // Set presets for divergence V

    ui->selectSmoke->setCurrentIndex(DIVV);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsSmoke->setValue(10);
    ui->showSmoke->setCheckState(Qt::Checked);

//    ui->radioSmokeScale->click();
    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(-0.001);
    ui->clampSmokeMaxValue->setValue(0.001);

    ui->selectGlyph->setCurrentIndex(V);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_BLACKWHITE);
    ui->selectNColorsGlyph->setValue(10);
    ui->showGlyph->setCheckState(Qt::Unchecked);

//    ui->radioGlyphScale->click();
//    ui->radioGlyphClamp->click();
//    ui->clampGlyphMinValue->setValue(-0.001);
//    ui->clampGlyphMaxValue->setValue(0.001);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();

    ui->timeStepSlider->setValue(1);
    ui->viscositySlider->setValue(4);
}
