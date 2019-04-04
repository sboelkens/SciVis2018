#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPainter>
#include <QLinearGradient>
#include <QPushButton>

#include "mainview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void setSmokeColorLegend();
  void setGlyphColorLegend();
  void setIsolineColorLegend();
  QIcon setColorLegend(int levels, int color_scheme);

private slots:
  void waitForInitialization();

  void updateLegendLabels();
  void updateSmokeLegendLabels();
  void updateGlyphLegendLabels();
  void updateIsolineLegendLabels();

  void on_showSmoke_stateChanged(int state);
  void on_selectSmoke_currentIndexChanged(int index);
  void on_selectNColorsSmoke_valueChanged(int value);
  void on_selectColormapSmoke_currentIndexChanged(int index);

  void on_radioSmokeScale_clicked();
  void on_radioSmokeClamp_clicked();
  void on_clampSmokeMinValue_valueChanged(double value);
  void on_clampSmokeMaxValue_valueChanged(double value);

  void on_showGlyph_stateChanged(int state);
  void on_selectGlyph_currentIndexChanged(int index);
  void on_selectNColorsGlyph_valueChanged(int value);
  void on_selectColormapGlyph_currentIndexChanged(int index);

  void on_radioGlyphScale_clicked();
  void on_radioGlyphClamp_clicked();
  void on_clampGlyphMinValue_valueChanged(double value);
  void on_clampGlyphMaxValue_valueChanged(double value);

  void on_radioGlyphVectorV_clicked();
  void on_radioGlyphVectorF_clicked();

  void on_nrGlyphsX_valueChanged(int value);
  void on_nrGlyphsY_valueChanged(int value);
  void on_nrGlyphsP_valueChanged(int value);
  void on_selectGlyphShape_currentIndexChanged(int index);

  void on_showIsoline_stateChanged(int state);
  void on_isolineMinValue_valueChanged(double value);
  void on_isolineMaxValue_valueChanged(double value);
  void on_nrIsolines_valueChanged(int value);
  void on_selectNColorsIsoline_valueChanged(int value);
  void on_selectColormapIsoline_currentIndexChanged(int index);

  void on_heightplotBox_stateChanged(int state);
  void on_clampHeightMaxValue_valueChanged(double value);
  void on_hPlot_xAngle_valueChanged(int value);
  void on_selectHeightVar_currentIndexChanged(int index);

  void on_streamtubesBox_stateChanged(int state);

  void on_timeStepSlider_valueChanged(int value);
  void on_viscositySlider_valueChanged(int value);


  void keyPressEvent(QKeyEvent* event);

private:
  Ui::MainWindow *ui;

  void setPresetDefault();
  void setPresetDivergence();
  void setPresetHeightplot();
  void setPresetIsolines();
  void setPreset3D();
  void setPresetScalingClamping();

  int legend_width = 255;
  int legend_height = 30;
};

#endif // MAINWINDOW_H
