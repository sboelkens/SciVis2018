#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTime"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  qDebug() << "✓✓ MainWindow constructor";

  ui->setupUi(this);
  this->setFocus();

  this->waitForInitialization();

  QTimer *timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(updateLegendLabels()));
  timer->start(200);

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
        this->setIsolineColorLegend();
    } else {
        QTimer *timer = new QTimer;
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(waitForInitialization()));
        timer->start(100);
    }
}


void MainWindow::updateLegendLabels()
{
    updateSmokeLegendLabels();
    updateGlyphLegendLabels();
}
void MainWindow::updateSmokeLegendLabels()
{
    double min = static_cast<double>(ui->mainView->clamp_smoke_min);
    if(min > -0.001 && min < 0.001) { min = 0.000; }
    double max = static_cast<double>(ui->mainView->clamp_smoke_max);
    if(max > -0.001 && max < 0.001) { max = 0.000; }
    ui->labelSmokeLegendMin->setText(QString::number(min, 'f', 3));
    ui->labelSmokeLegendMax->setText(QString::number(max, 'f', 3));
    ui->labelSmokeLegendMid->setText(QString::number(((min+max)/2), 'f', 3));
}
void MainWindow::updateGlyphLegendLabels()
{
    if(ui->mainView->glyph_col == COLOR_DIRECTIONAL) {
        ui->labelGlyphLegendMin->setText("0°");
        ui->labelGlyphLegendMid->setText("180°");
        ui->labelGlyphLegendMax->setText("360°");
    } else {
        double min = static_cast<double>(ui->mainView->clamp_glyph_min);
        if(min > -0.001 && min < 0.001) { min = 0.000; }
        double max = static_cast<double>(ui->mainView->clamp_glyph_max);
        if(max > -0.001 && max < 0.001) { max = 0.000; }
        ui->labelGlyphLegendMin->setText(QString::number(min, 'f', 3));
        ui->labelGlyphLegendMax->setText(QString::number(max, 'f', 3));
        ui->labelGlyphLegendMid->setText(QString::number(((min+max)/2), 'f', 3));
    }
}
void MainWindow::updateIsolineLegendLabels()
{
    ui->labelIsolineLegendMin->setText(QString::number(static_cast<double>(ui->mainView->isoline_min_value)));
    ui->labelIsolineLegendMax->setText(QString::number(static_cast<double>(ui->mainView->isoline_max_value)));
    float mid = (ui->mainView->isoline_min_value+ui->mainView->isoline_max_value)/2;
    ui->labelIsolineLegendMid->setText(QString::number(static_cast<double>(mid)));
}

void MainWindow::on_showSmoke_stateChanged(int state)
{
    ui->mainView->draw_smoke = state;
    this->setFocus();
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
    updateSmokeLegendLabels();

    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_clampSmokeMinValue_valueChanged(double value)
{
    if(ui->mainView->clamp_smoke_cmap) {
        if(value > static_cast<double>(ui->mainView->clamp_smoke_max)){
            value = static_cast<double>(ui->mainView->clamp_smoke_max);
            ui->clampSmokeMinValue->setValue(value);
        }
        ui->mainView->clamp_smoke_min = static_cast<float>(value);
        updateSmokeLegendLabels();
        ui->mainView->updateUniformsRequired = true;
    }
    this->setFocus();
}
void MainWindow::on_clampSmokeMaxValue_valueChanged(double value)
{
    if(ui->mainView->clamp_smoke_cmap) {
        if(value < static_cast<double>(ui->mainView->clamp_smoke_min)){
            value = static_cast<double>(ui->mainView->clamp_smoke_min);
            ui->clampSmokeMaxValue->setValue(value);
        }
        ui->mainView->clamp_smoke_max = static_cast<float>(value);
        updateSmokeLegendLabels();
        ui->mainView->updateUniformsRequired = true;
    }
    this->setFocus();
}

void MainWindow::on_showGlyph_stateChanged(int state)
{
    ui->mainView->draw_vecs = state;
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
    updateGlyphLegendLabels();
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_clampGlyphMinValue_valueChanged(double value)
{
    if(ui->mainView->clamp_glyph_cmap) {
        if(value > static_cast<double>(ui->mainView->clamp_glyph_max)){
            value = static_cast<double>(ui->mainView->clamp_glyph_max);
            ui->clampGlyphMinValue->setValue(value);
        }
        ui->mainView->clamp_glyph_min = static_cast<float>(value);
        updateGlyphLegendLabels();
        ui->mainView->updateUniformsRequired = true;
    }

    this->setFocus();
}
void MainWindow::on_clampGlyphMaxValue_valueChanged(double value)
{
    if(ui->mainView->clamp_glyph_cmap) {
        if(value < static_cast<double>(ui->mainView->clamp_glyph_min)){
            value = static_cast<double>(ui->mainView->clamp_glyph_min);
            ui->clampGlyphMaxValue->setValue(value);
        }
        ui->mainView->clamp_glyph_max = static_cast<float>(value);
        updateGlyphLegendLabels();
        ui->mainView->updateUniformsRequired = true;
    }
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
void MainWindow::on_selectGlyphShape_currentIndexChanged(int index)
{
    ui->mainView->glyph_type = index;
    if (index == GLYPH_3D_CONE)
    {
        ui->mainView->glyphs3D = true;
        ui->mainView->nr_glyphs_x = 20;
        ui->mainView->nr_glyphs_y = 20;
        //ui->mainView->nr_glyphs_p = 4;
        ui->nrGlyphsP->setValue(1);
    }
    else if (index == GLYPH_3D_ARROW)
    {
        ui->mainView->glyphs3D = true;
        ui->mainView->nr_glyphs_x = 20;
        ui->mainView->nr_glyphs_y = 20;
        //ui->mainView->nr_glyphs_p = 1;
        ui->nrGlyphsP->setValue(1);
    }
    else if (index == GLYPH_2D_LINE)
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

void MainWindow::on_heightplotBox_stateChanged(int state)
{
    ui->mainView->heightplot = (bool)state;
    ui->mainView->updateMatricesRequired = true;
    this->setFocus();
}

void MainWindow::on_clampHeightMaxValue_valueChanged(double value)
{
    ui->mainView->height_scaler = value;
    this->setFocus();
}

void MainWindow::on_hPlot_xAngle_valueChanged(int value)
{
    ui->mainView->hPlot_xAngle = value;
    ui->mainView->updateMatricesRequired = true;
    this->setFocus();
}

void MainWindow::on_selectHeightVar_currentIndexChanged(int index)
{
    ui->mainView->height_var = index;
    if (index == 0)
    {
        ui->mainView->height_scaler = 10.0;
    }
    else
    {
        ui->mainView->height_scaler = 0.1;
    }
    this->setFocus();
}

void MainWindow::on_showIsoline_stateChanged(int state)
{
    ui->mainView->draw_isolines = state;
    this->setFocus();
}
void MainWindow::on_isolineMinValue_valueChanged(double value)
{
    ui->mainView->isoline_min_value = static_cast<float>(value);
    if(value > static_cast<double>(ui->mainView->isoline_max_value)){
        value = static_cast<double>(ui->mainView->isoline_max_value);
        ui->isolineMinValue->setValue(value);
    }
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_isolineMaxValue_valueChanged(double value)
{
    ui->mainView->isoline_max_value = static_cast<float>(value);
    if(value < static_cast<double>(ui->mainView->isoline_min_value)){
        value = static_cast<double>(ui->mainView->isoline_min_value);
        ui->isolineMaxValue->setValue(value);
    }
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_nrIsolines_valueChanged(int value)
{
    ui->mainView->nr_isolines = value;
    ui->mainView->updateUniformsRequired = true;
    this->setFocus();
}
void MainWindow::on_selectNColorsIsoline_valueChanged(int value)
{
    ui->mainView->levels_isoline = value;
    this->setIsolineColorLegend();
    this->setFocus();
}
void MainWindow::on_selectColormapIsoline_currentIndexChanged(int index)
{
    ui->mainView->isoline_col = index;
    this->setIsolineColorLegend();
    this->setFocus();
}

void MainWindow::on_streamtubesBox_stateChanged(int value)
{
    ui->mainView->streamtubes = (bool)value;
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
    case Qt::Key_Q:
        this->setPresetDefault();
        break;
    case Qt::Key_W:
        this->setPresetScalingClamping();
        break;
    case Qt::Key_E:
        this->setPreset3D();
        break;
    case Qt::Key_R:
        this->setPresetDivergence();
        break;
    case Qt::Key_T:
        this->setPresetIsolines();
        break;
    case Qt::Key_Y:
        this->setPresetHeightplot();
        break;
    }
}


void MainWindow::setSmokeColorLegend()
{
    QIcon pm = setColorLegend(ui->mainView->levels_smoke, ui->mainView->smoke_col);
    ui->legendSmoke->setIcon(pm);
    ui->legendSmoke->setIconSize(QSize(legend_width, legend_height));
    this->setFocus();
}
void MainWindow::setGlyphColorLegend()
{
    QIcon pm = setColorLegend(ui->mainView->levels_glyph, ui->mainView->glyph_col);
    ui->legendGlyph->setIcon(pm);
    ui->legendGlyph->setIconSize(QSize(legend_width, legend_height));
    if(ui->mainView->glyph_col == COLOR_DIRECTIONAL) {
        ui->selectNColorsGlyph->setEnabled(false);
        ui->clampGlyphMinValue->setEnabled(false);
        ui->clampGlyphMaxValue->setEnabled(false);
        ui->radioGlyphClamp->setEnabled(false);
        ui->radioGlyphScale->setEnabled(false);
    } else {
        ui->selectNColorsGlyph->setEnabled(true);
        ui->clampGlyphMinValue->setEnabled(true);
        ui->clampGlyphMaxValue->setEnabled(true);
        ui->radioGlyphClamp->setEnabled(true);
        ui->radioGlyphScale->setEnabled(true);
    }
    this->setFocus();
}
void MainWindow::setIsolineColorLegend()
{
    QIcon pm = setColorLegend(ui->mainView->levels_isoline, ui->mainView->isoline_col);
    ui->legendIsoline->setIcon(pm);
    ui->legendIsoline->setIconSize(QSize(legend_width, legend_height));
    this->setFocus();
}
QIcon MainWindow::setColorLegend(int levels, int color_scheme)
{
    ui->mainView->updateUniformsRequired = true;

    QPixmap pm(legend_width, legend_height);
    QPainter pmp(&pm);
    QVector3D rgb;

    for(int x = 0; x <= 256; x++) {
        if(color_scheme == COLOR_DIRECTIONAL) {
            rgb = getColorLegendDirectionValue(x, 256);
        } else {
            rgb = set_colormap(static_cast<float>(x)/256, color_scheme, levels);
        }

        QColor color = QColor(static_cast<int>(rgb.x()*255), static_cast<int>(rgb.y()*255), static_cast<int>(rgb.z()*255));
        pmp.setBrush(QBrush(color));
        pmp.setPen(Qt::NoPen);
        pmp.drawRect(x, 0, 1, legend_height);
    }
    return QIcon(pm);
}

QVector3D MainWindow::getColorLegendDirectionValue(double i, double num_values)
{
    double num_values_side = num_values / 8;
    double x,y;
    if(i < (num_values_side*1)) {
        x = i/num_values_side;
        y = 1.00;
    } else if(i < (num_values_side*2)) {
        x = 1.00;
        y = 1.00-((i-(num_values_side*1))/num_values_side);
    } else if(i < (num_values_side*3)) {
        x = 1.00;
        y = 0-((i-(num_values_side*2))/num_values_side);
    } else if(i < (num_values_side*4)) {
        x = 1.00-((i-(num_values_side*3))/num_values_side);
        y = -1.00;
    } else if(i < (num_values_side*5)) {
        x = 0.00-((i-(num_values_side*4))/num_values_side);
        y = -1.00;
    } else if(i < (num_values_side*6)) {
        x = -1.00;
        y = -1.00+((i-(num_values_side*5))/num_values_side);
    } else if(i < (num_values_side*7)) {
        x = -1.00;
        y = (i-(num_values_side*6))/num_values_side;
    } else {
        x = -1.00+((i-(num_values_side*7))/num_values_side);
        y = 1.00;
    }
    return direction_to_color(static_cast<float>(x), static_cast<float>(y));
}


void MainWindow::setPresetDefault()
{
    // Set presets Default values
    ui->settingsGB->setCurrentIndex(0);

    ui->showSmoke->setCheckState(Qt::Checked);
    ui->selectSmoke->setCurrentIndex(RHO);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_BLACKWHITE);
    ui->selectNColorsSmoke->setValue(10);

//    ui->radioSmokeScale->click();
    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(0.000);
    ui->clampSmokeMaxValue->setValue(1.000);

    ui->showGlyph->setCheckState(Qt::Checked);
    ui->selectGlyph->setCurrentIndex(RHO);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsGlyph->setValue(10);

//    ui->radioGlyphScale->click();
    ui->radioGlyphClamp->click();
    ui->clampGlyphMinValue->setValue(0.000);
    ui->clampGlyphMaxValue->setValue(1.000);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();
    ui->selectGlyphShape->setCurrentIndex(GLYPH_2D_LINE);

    ui->showIsoline->setCheckState(Qt::Checked);
    ui->isolineMinValue->setValue(0.000);
    ui->isolineMaxValue->setValue(1.000);
    ui->nrIsolines->setValue(1);
    ui->selectColormapIsoline->setCurrentIndex(COLOR_HEATMAP);
    ui->selectNColorsIsoline->setValue(10);

    ui->heightplotBox->setCheckState(Qt::Unchecked);
    ui->hPlot_xAngle->setValue(45);

    ui->streamtubesBox->setCheckState(Qt::Unchecked);

    ui->timeStepSlider->setValue(4);
    ui->viscositySlider->setValue(1);
}
void MainWindow::setPresetDivergence()
{
    // Set presets for divergence V
    ui->settingsGB->setCurrentIndex(0);

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
    ui->radioGlyphClamp->click();
    ui->clampGlyphMinValue->setValue(0.000);
    ui->clampGlyphMaxValue->setValue(1.000);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();
    ui->selectGlyphShape->setCurrentIndex(GLYPH_2D_LINE);

    ui->showIsoline->setCheckState(Qt::Unchecked);
    ui->isolineMinValue->setValue(0.000);
    ui->isolineMaxValue->setValue(1.000);
    ui->nrIsolines->setValue(1);
    ui->selectColormapIsoline->setCurrentIndex(COLOR_HEATMAP);
    ui->selectNColorsIsoline->setValue(10);

    ui->heightplotBox->setCheckState(Qt::Unchecked);
    ui->hPlot_xAngle->setValue(45);

    ui->streamtubesBox->setCheckState(Qt::Unchecked);

    ui->timeStepSlider->setValue(1);
    ui->viscositySlider->setValue(1);
}
void MainWindow::setPresetHeightplot()
{
    // Set presets for Heightplot
    ui->settingsGB->setCurrentIndex(3);

    ui->showSmoke->setCheckState(Qt::Checked);
    ui->selectSmoke->setCurrentIndex(RHO);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_BLUE_RED);
    ui->selectNColorsSmoke->setValue(10);

    ui->radioSmokeScale->click();
//    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(0.000);
    ui->clampSmokeMaxValue->setValue(1.000);

    ui->showGlyph->setCheckState(Qt::Unchecked);
    ui->selectGlyph->setCurrentIndex(RHO);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsGlyph->setValue(10);

//    ui->radioGlyphScale->click();
    ui->radioGlyphClamp->click();
    ui->clampGlyphMinValue->setValue(0.000);
    ui->clampGlyphMaxValue->setValue(1.000);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();
    ui->selectGlyphShape->setCurrentIndex(GLYPH_2D_LINE);

    ui->showIsoline->setCheckState(Qt::Unchecked);
    ui->isolineMinValue->setValue(0.000);
    ui->isolineMaxValue->setValue(1.000);
    ui->nrIsolines->setValue(1);
    ui->selectColormapIsoline->setCurrentIndex(COLOR_HEATMAP);
    ui->selectNColorsIsoline->setValue(10);

    ui->heightplotBox->setCheckState(Qt::Checked);
    ui->hPlot_xAngle->setValue(60);

    ui->streamtubesBox->setCheckState(Qt::Unchecked);

    ui->timeStepSlider->setValue(4);
    ui->viscositySlider->setValue(1);
}
void MainWindow::setPresetIsolines()
{
    // Set presets for Isolines
    ui->settingsGB->setCurrentIndex(2);

    ui->showSmoke->setCheckState(Qt::Checked);
    ui->selectSmoke->setCurrentIndex(RHO);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_BLACKWHITE);
    ui->selectNColorsSmoke->setValue(10);

//    ui->radioSmokeScale->click();
    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(0.000);
    ui->clampSmokeMaxValue->setValue(1.000);

    ui->showGlyph->setCheckState(Qt::Unchecked);
    ui->selectGlyph->setCurrentIndex(RHO);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsGlyph->setValue(10);

//    ui->radioGlyphScale->click();
    ui->radioGlyphClamp->click();
    ui->clampGlyphMinValue->setValue(0.000);
    ui->clampGlyphMaxValue->setValue(1.000);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();
    ui->selectGlyphShape->setCurrentIndex(GLYPH_2D_LINE);

    ui->showIsoline->setCheckState(Qt::Checked);
    ui->isolineMinValue->setValue(0.000);
    ui->isolineMaxValue->setValue(1.000);
    ui->nrIsolines->setValue(4);
    ui->selectColormapIsoline->setCurrentIndex(COLOR_BLUE_RED);
    ui->selectNColorsIsoline->setValue(4);

    ui->heightplotBox->setCheckState(Qt::Unchecked);
    ui->hPlot_xAngle->setValue(45);

    ui->streamtubesBox->setCheckState(Qt::Unchecked);

    ui->timeStepSlider->setValue(4);
    ui->viscositySlider->setValue(1);
}
void MainWindow::setPreset3D()
{
    // Set presets for 3D cones
    ui->settingsGB->setCurrentIndex(1);

    ui->showSmoke->setCheckState(Qt::Checked);
    ui->selectSmoke->setCurrentIndex(RHO);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_BLACKWHITE);
    ui->selectNColorsSmoke->setValue(10);

//    ui->radioSmokeScale->click();
    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(0.000);
    ui->clampSmokeMaxValue->setValue(1.000);

    ui->showGlyph->setCheckState(Qt::Checked);
    ui->selectGlyph->setCurrentIndex(RHO);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsGlyph->setValue(10);

//    ui->radioGlyphScale->click();
    ui->radioGlyphClamp->click();
    ui->clampGlyphMinValue->setValue(0.000);
    ui->clampGlyphMaxValue->setValue(1.000);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();
    ui->selectGlyphShape->setCurrentIndex(GLYPH_3D_CONE);

    ui->showIsoline->setCheckState(Qt::Unchecked);
    ui->isolineMinValue->setValue(0.000);
    ui->isolineMaxValue->setValue(1.000);
    ui->nrIsolines->setValue(1);
    ui->selectColormapIsoline->setCurrentIndex(COLOR_HEATMAP);
    ui->selectNColorsIsoline->setValue(10);

    ui->heightplotBox->setCheckState(Qt::Unchecked);
    ui->hPlot_xAngle->setValue(45);

    ui->streamtubesBox->setCheckState(Qt::Unchecked);

    ui->timeStepSlider->setValue(4);
    ui->viscositySlider->setValue(1);
}
void MainWindow::setPresetScalingClamping()
{
    // Set presets for Scaling and clamping
    ui->settingsGB->setCurrentIndex(0);

    ui->showSmoke->setCheckState(Qt::Checked);
    ui->selectSmoke->setCurrentIndex(RHO);
    ui->selectColormapSmoke->setCurrentIndex(COLOR_BLACKWHITE);
    ui->selectNColorsSmoke->setValue(10);

    ui->radioSmokeScale->click();
//    ui->radioSmokeClamp->click();
    ui->clampSmokeMinValue->setValue(0.000);
    ui->clampSmokeMaxValue->setValue(1.000);

    ui->showGlyph->setCheckState(Qt::Checked);
    ui->selectGlyph->setCurrentIndex(RHO);
    ui->selectColormapGlyph->setCurrentIndex(COLOR_RAINBOW);
    ui->selectNColorsGlyph->setValue(10);

//    ui->radioGlyphScale->click();
    ui->radioGlyphClamp->click();
    ui->clampGlyphMinValue->setValue(0.000);
    ui->clampGlyphMaxValue->setValue(1.000);

    ui->radioGlyphVectorV->click();
//    ui->radioGlyphVectorF->click();
    ui->selectGlyphShape->setCurrentIndex(GLYPH_2D_LINE);

    ui->showIsoline->setCheckState(Qt::Checked);
    ui->isolineMinValue->setValue(0.000);
    ui->isolineMaxValue->setValue(1.000);
    ui->nrIsolines->setValue(1);
    ui->selectColormapIsoline->setCurrentIndex(COLOR_HEATMAP);
    ui->selectNColorsIsoline->setValue(10);

    ui->heightplotBox->setCheckState(Qt::Unchecked);
    ui->hPlot_xAngle->setValue(45);

    ui->streamtubesBox->setCheckState(Qt::Unchecked);

    ui->timeStepSlider->setValue(4);
    ui->viscositySlider->setValue(1);
}
