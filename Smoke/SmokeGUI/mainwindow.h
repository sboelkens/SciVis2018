#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPainter>
#include <QLinearGradient>

#include "mainview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void setColorLegend();

private slots:
  void on_selectNColorsSmoke_valueChanged(int value);
  void on_selectColormapSmoke_currentIndexChanged(int index);
  void on_showSmoke_stateChanged(int state);

  void on_selectNColorsGlyph_valueChanged(int value);
  void on_selectColormapGlyph_currentIndexChanged(int index);
  void on_showGlyph_stateChanged(int state);

  void keyPressEvent(QKeyEvent* event);


private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
