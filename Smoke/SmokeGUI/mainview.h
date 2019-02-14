#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>

#include <QVector3D>
#include <QMouseEvent>
#include <QTimer>
#include <QTimerEvent>
#include <QKeyEvent>

#include "simulation.h"
#include "utils.h"

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

  Q_OBJECT

public:
  MainView(QWidget *parent = 0);
  ~MainView();

  void updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy);
  void clearArrays();

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void timerEvent(QTimerEvent *event);
  void keyPressEvent(QKeyEvent* event);

private:
  QOpenGLDebugLogger* debugLogger;

  QOpenGLShaderProgram* mainShaderProg;
  GLuint gridVAO, gridCoordsBO, gridColourBO, gridIndexBO;

  void createShaderPrograms();
  void createBuffers();
  void updateMatrices();
  void updateUniforms();

  bool updateUniformsRequired;

  QVector<QVector2D> triaCoords;
  QVector<QVector3D> triaColours;
  QVector<unsigned short> triaIndices;

  GLint uniModelViewMatrix, uniProjectionMatrix;
  QMatrix4x4 modelViewMatrix, projectionMatrix;

  //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
  const int DIM = 50;               //size of simulation grid
  double dt;                        //simulation time step
  float visc;                       //fluid viscosity
  int   color_dir;                  //use direction color-coding or not
  float vec_scale;                  //scaling of hedgehogs
  int   draw_smoke;                 //draw the smoke or not
  int   draw_vecs;                  //draw the vector field or not
  int   scalar_col;                 //method for scalar coloring
  int   frozen;                     //toggles on/off the animation

  QPoint lastpos;
  Simulation simulation;
  QTimer* timer;

  void do_one_simulation_step();
  void visualize(fftw_real* rho, fftw_real* vx, fftw_real* vy);
  void direction_to_color(float x, float y, int method);


private slots:
  void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
