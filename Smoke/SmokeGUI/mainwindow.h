#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_selectNColorsRho_valueChanged(int value);
  void on_selectNColorsF_valueChanged(int value);
  void on_selectNColorsV_valueChanged(int value);
  void on_selectColormapRho_currentIndexChanged(int index);
//  void on_selectColormapV_currentIndexChanged(int index);
//  void on_selectColormapF_currentIndexChanged(int index);
  void on_showRho_stateChanged(int state);
  void on_showV_stateChanged(int state);
  void on_showF_stateChanged(int state);
  void keyPressEvent(QKeyEvent* event);


private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
