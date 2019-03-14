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

  void updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy);
  void updateGlyphs(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy);
  void updateAverages(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy);
  void clearArrays();
  void clearGridArrays();
  void clearLineArrays();

  double dt;                        //simulation time step
  float visc;                       //fluid viscosity
  int   color_dir;                  //use direction color-coding or not
  float vec_scale;                  //scaling of hedgehogs
  int   draw_smoke;                 //draw the smoke or not
  int   smoke_var;                  //variable encoded by the smoke (rho, ||v|| or ||f||)
  int   glyph_var;
  int   glyph_vector_var;
  int   draw_vecs;                  //draw the velocity field or not
  int   draw_force_field;           //draw the forcefield or not
  int   smoke_col;
  int   glyph_col;                 //method for scalar coloring
  int   frozen;                     //toggles on/off the animation
  int   levels_smoke;
  int   levels_glyph;
  bool updateUniformsRequired;
  // scaling/clamping
  bool clamp_cmap;                  // if false, scaling
  float clamp_min;
  float clamp_max;
  int scale_window;
  int scale_cnt;

  bool is_initialized = false;
  int nr_glyphs_x;
  int nr_glyphs_y;
  int nr_glyphs_p;
  bool nr_glyphs_changed;

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
  GLuint gridVAO, gridCoordsBO, gridValBO, gridIndexBO;
  GLuint glyphsVAO, glyphCoordsBO, glyphColourBO, glyphIndexBO;
  GLuint fLinesVAO, fLinesCoordsBO, fLinesColourBO, fLinesIndexBO;

  void createShaderPrograms();
  void createBuffers();
  void updateMatrices();
  void updateUniforms();

  float glyph_interpolation(float, float, fftw_real*);

  QVector<QVector2D> triaCoords;
  QVector<float> triaVals;
  QVector<unsigned short> triaIndices;
  QVector<float> glyphShifts;
  QVector<QVector2D> glyphCoords;
  QVector<QVector3D> glyphColours;
  QVector<unsigned short> glyphIndices;

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

  void do_one_simulation_step();

private slots:
  void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
