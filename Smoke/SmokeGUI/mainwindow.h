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

  void on_selectNColorsSmoke_valueChanged(int value);
  void on_selectColormapSmoke_currentIndexChanged(int index);
  void on_showSmoke_stateChanged(int state);
  void on_radioSmokeRho_clicked();
  void on_radioSmokeV_clicked();
  void on_radioSmokeF_clicked();
  void on_radioGlyphRho_clicked();
  void on_radioGlyphV_clicked();
  void on_radioGlyphF_clicked();
  void on_radioGlyphVectorV_clicked();
  void on_radioGlyphVectorF_clicked();
  void on_clampMinValue_valueChanged(double value);
  void on_clampMaxValue_valueChanged(double value);
  void on_radioScale_clicked();
  void on_radioClamp_clicked();

  void on_selectNColorsGlyph_valueChanged(int value);
  void on_selectColormapGlyph_currentIndexChanged(int index);
  void on_showGlyph_stateChanged(int state);

  void on_timeStepSlider_valueChanged(int value);
  void on_viscositySlider_valueChanged(int value);

  void on_nrGlyphsX_valueChanged(int value);
  void on_nrGlyphsY_valueChanged(int value);
  void on_nrGlyphsP_valueChanged(int value);


  void keyPressEvent(QKeyEvent* event);

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
