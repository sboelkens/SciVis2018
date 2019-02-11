#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>

#include <QVector3D>

#include "simulation.h"
#include "utils.h"

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

  Q_OBJECT

public:
  MainView(QWidget *parent = 0);
  ~MainView();

  void updateBuffers(unsigned short n);
  void clearArrays();

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

private:
  QOpenGLDebugLogger* debugLogger;

  QOpenGLShaderProgram* mainShaderProg;
  GLuint fanVAO, fanCoordsBO, fanColourBO, fanIndexBO;

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
  int   color_dir;            //use direction color-coding or not
  float vec_scale;			//scaling of hedgehogs
  int   draw_smoke;           //draw the smoke or not
  int   draw_vecs;            //draw the vector field or not
  const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
  const int COLOR_RAINBOW=1;
  const int COLOR_BANDS=2;
  int   scalar_col;           //method for scalar coloring
  int   frozen;               //toggles on/off the animation

  void visualize(fftw_real* rho, fftw_real* vx, fftw_real* vy);
  void set_colormap(float val);
  void direction_to_color(float x, float y, int method);


private slots:
  void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
