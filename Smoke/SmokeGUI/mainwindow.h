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
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void setSmokeColorLegend();
  void setGlyphColorLegend();
  QIcon setColorLegend(int width, int height, int levels, int color);

private slots:
  void waitForInitialization();

  void on_selectSmoke_currentIndexChanged(int index);
  void on_selectNColorsSmoke_valueChanged(int value);
  void on_selectColormapSmoke_currentIndexChanged(int index);
  void on_showSmoke_stateChanged(int state);

  void on_radioSmokeScale_clicked();
  void on_radioSmokeClamp_clicked();
  void on_clampSmokeMinValue_valueChanged(double value);
  void on_clampSmokeMaxValue_valueChanged(double value);

  void on_selectGlyph_currentIndexChanged(int index);
  void on_selectNColorsGlyph_valueChanged(int value);
  void on_selectColormapGlyph_currentIndexChanged(int index);
  void on_showGlyph_stateChanged(int state);

  void on_radioGlyphScale_clicked();
  void on_radioGlyphClamp_clicked();
  void on_clampGlyphMinValue_valueChanged(double value);
  void on_clampGlyphMaxValue_valueChanged(double value);

  void on_radioGlyphVectorV_clicked();
  void on_radioGlyphVectorF_clicked();

  void on_nrGlyphsX_valueChanged(int value);
  void on_nrGlyphsY_valueChanged(int value);
  void on_nrGlyphsP_valueChanged(int value);

  void on_rhoIsolineValue_valueChanged(double value);

  void on_timeStepSlider_valueChanged(int value);
  void on_viscositySlider_valueChanged(int value);


  void keyPressEvent(QKeyEvent* event);

private:
  Ui::MainWindow *ui;

  void setPresetOne();
  void setPresetTwo();
};

#endif // MAINWINDOW_H
