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

#include <stdlib.h>
#include <time.h>

#include "simulation.h"
#include "utils.h"

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

  Q_OBJECT

public:
  MainView(QWidget *parent = 0);
  ~MainView();

  void initBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy);
  void updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy);
  void clearArrays();
  void clearGridArrays();
  void clearLineArrays();

  double dt = 0.4;                        //simulation time step
  float visc = 0.001;                       //fluid viscosity
  int   color_dir = 1;                  //use direction color-coding or not
  float vec_scale = 1;                  //scaling of hedgehogs
  int   draw_smoke = 1;                 //draw the smoke or not
  int   smoke_var = 0;                  //variable encoded by the smoke (rho, ||v|| or ||f||)
  int   glyph_var = 0;
  int   glyph_vector_var = 1;
  int   draw_vecs = 1;                  //draw the velocity field or not
  int   draw_force_field = 0;           //draw the forcefield or not
  int   smoke_col = 0;
  int   glyph_col = 1;                 //method for scalar coloring
  int   frozen = 0;                     //toggles on/off the animation
  int   levels_smoke = 10;
  int   levels_glyph = 10;
  bool updateUniformsRequired;
  // scaling/clamping
  bool clamp_cmap = true;                  // if false, scaling
  float clamp_min = 0.0;
  float clamp_max = 1.0;
  int scale_window = 1000;
  int scale_cnt = 0;

  bool is_initialized = false;
  int nr_glyphs_x = 50;
  int nr_glyphs_y = 50;
  int nr_glyphs_p = 4;
  bool nr_glyphs_changed = false;

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void timerEvent(QTimerEvent *event);

private:
  QOpenGLDebugLogger* debugLogger;

  QOpenGLShaderProgram* mainShaderProg;
  QOpenGLShaderProgram* cMapShaderProg;
  GLuint gridVAO, gridCoordsBO, gridColourBO, gridValBO, gridIndexBO;
  GLuint linesVAO, linesCoordsBO, linesColourBO, linesIndexBO;
  GLuint fLinesVAO, fLinesCoordsBO, fLinesColourBO, fLinesIndexBO;

  void createShaderPrograms();
  void createBuffers();
  void updateMatrices();
  void updateUniforms();

  float glyph_interpolation(float, float, fftw_real*);

  QVector<QVector2D> triaCoords;
  QVector<QVector3D> triaColours;
  QVector<float> triaVals;
  QVector<unsigned short> triaIndices;
  QVector<float> lineShifts;
  QVector<QVector2D> lineCoords;
  QVector<QVector3D> lineColours;
  QVector<unsigned short> lineIndices;

  GLint uniModelViewMatrix, uniProjectionMatrix;
  GLint uniMVMat_cMap, uniProjMat_cMap, uniNLevels_cMap, uniColorMap_cMap;
  GLint uniClamping, uniClampMin, uniClampMax; // scaling/clamping uniforms
  QMatrix4x4 modelViewMatrix, projectionMatrix;

  //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
  const int DIM = 50;               //size of simulation grid
  const float MAX_SAMPLE_DIST = 0.02;

  QPoint lastpos;
  Simulation simulation;
  QTimer* timer;

  QVector<float> scale_maxvals_rho;
  QVector<float> scale_minvals_rho;
  QVector<float> scale_maxvals_vnorm;
  QVector<float> scale_minvals_vnorm;
  QVector<float> scale_maxvals_fnorm;
  QVector<float> scale_minvals_fnorm;

  void first_simulation_step();
  void do_one_simulation_step();

private slots:
  void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
