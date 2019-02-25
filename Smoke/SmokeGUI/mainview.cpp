#include "mainview.h"
//#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";
}

MainView::~MainView() {
  clearArrays();

  glDeleteBuffers(1, &gridCoordsBO);
  glDeleteBuffers(1, &gridColourBO);
  glDeleteBuffers(1, &gridValBO);
  glDeleteBuffers(1, &gridIndexBO);
  glDeleteVertexArrays(1, &gridVAO);
  glDeleteBuffers(1, &linesCoordsBO);
  glDeleteBuffers(1, &linesColourBO);
  glDeleteBuffers(1, &linesIndexBO);
  glDeleteVertexArrays(1, &linesVAO);
  glDeleteBuffers(1, &fLinesCoordsBO);
  glDeleteBuffers(1, &fLinesColourBO);
  glDeleteBuffers(1, &fLinesIndexBO);
  glDeleteVertexArrays(1, &fLinesVAO);

  delete mainShaderProg;

  debugLogger->stopLogging();
  qDebug() << "✗✗ MainView destructor";
}

// ---

void MainView::createShaderPrograms() {

  // Qt approach
  mainShaderProg = new QOpenGLShaderProgram();
  mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
  mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
  mainShaderProg->link();

  cMapShaderProg = new QOpenGLShaderProgram();
  cMapShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cmapvertshader.glsl");
  cMapShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/cmapfragshader.glsl");
  cMapShaderProg->link();


  // Qt wrappers
  // uniModelViewMatrix = mainShaderProg->uniformLocation("modelviewmatrix");
  // uniProjectionMatrix = mainShaderProg->uniformLocation("projectionmatrix");

  // Pure OpenGL
  uniModelViewMatrix = glGetUniformLocation(mainShaderProg->programId(), "modelviewmatrix");
  uniProjectionMatrix = glGetUniformLocation(mainShaderProg->programId(), "projectionmatrix");

  uniMVMat_cMap = glGetUniformLocation(cMapShaderProg->programId(), "modelviewmatrix");
  uniProjMat_cMap = glGetUniformLocation(cMapShaderProg->programId(), "projectionmatrix");
  uniNLevels_cMap = glGetUniformLocation(cMapShaderProg->programId(), "levels");
  uniColorMap_cMap = glGetUniformLocation(cMapShaderProg->programId(), "mode");
  uniClamping = glGetUniformLocation(cMapShaderProg->programId(), "clamp");
  uniClampMax = glGetUniformLocation(cMapShaderProg->programId(), "maxval");
  uniClampMin = glGetUniformLocation(cMapShaderProg->programId(), "minval");
}

void MainView::createBuffers() {

  // Pure OpenGL
  glGenVertexArrays(1, &gridVAO);
  glBindVertexArray(gridVAO);

  glGenBuffers(1, &gridCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &gridColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &gridValBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &gridIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);

  glBindVertexArray(0);

  glGenVertexArrays(1, &linesVAO);
  glBindVertexArray(linesVAO);

  glGenBuffers(1, &linesCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, linesCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &linesColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, linesColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &linesIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBO);

  glBindVertexArray(0);

  glGenVertexArrays(1, &fLinesVAO);
  glBindVertexArray(fLinesVAO);

  glGenBuffers(1, &fLinesCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, fLinesCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &fLinesColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, fLinesColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &fLinesIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fLinesIndexBO);

  glBindVertexArray(0);
}

void MainView::updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy) {
//    qDebug() << " call me call me";

  clearArrays();

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;

  triaCoords.reserve(n_points);
  triaColours.reserve(n_points);
  triaVals.reserve(n_points);
  triaIndices.reserve(n_trias);

  lineCoords.reserve(n_points * 2);
  lineColours.reserve(n_points * 2);
  lineIndices.reserve(n_points * 2);

  fLineCoords.reserve(n_points * 2);
  fLineColours.reserve(n_points * 2);
  fLineIndices.reserve(n_points * 2);

  int idx0, idx1, idx2, idx3;
  double px, py;//px0, py0, px1, py1, px2, py2, px3, py3;

  fftw_real  wn = 2.0 / (fftw_real)(DIM + 1);   // Grid cell width
  fftw_real  hn = 2.0 / (fftw_real)(DIM + 1);  // Grid cell height

  for (int j = 0; j < DIM; j++)            //draw smoke
  {
      for (int i = 0; i < DIM; i++)
      {
          px = wn + (fftw_real)i * wn - 1.0;
          py = hn + (fftw_real)j * hn - 1.0;

          idx0 = (j * DIM) + i;
          idx1 = ((j + 1) * DIM) + i;
          idx2 = ((j + 1) * DIM) + (i + 1);
          idx3 = (j * DIM) + (i + 1);

          triaCoords.append(QVector2D(px, py));
          triaColours.append(set_colormap(rho[idx0], scalar_col, levels_rho));
          triaVals.append(rho[idx0]);


          if (j + 1 < DIM && i + 1 < DIM)
          {
              //first tria
              triaIndices.append(idx0);
              triaIndices.append(idx1);
              triaIndices.append(idx2);
              // second tria
              triaIndices.append(idx0);
              triaIndices.append(idx2);
              triaIndices.append(idx3);
          }

          lineCoords.append(QVector2D(px, py));
          lineCoords.append(QVector2D(px + vec_scale * vx[idx0], py + vec_scale * vy[idx0]));
          lineColours.append(direction_to_color(vx[idx0], vy[idx0], color_dir));
          lineColours.append(direction_to_color(vx[idx0], vy[idx0], color_dir));
          lineIndices.append(2*idx0);
          lineIndices.append(2*idx0+1);

          fLineCoords.append(QVector2D(px, py));
          fLineCoords.append(QVector2D(px + vec_scale * fx[idx0], py + vec_scale * fy[idx0]));
          fLineColours.append(direction_to_color(fx[idx0], fy[idx0], color_dir));
          fLineColours.append(direction_to_color(fx[idx0], fy[idx0], color_dir));
          fLineIndices.append(2*idx0);
          fLineIndices.append(2*idx0+1);
      }
  }

  glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, gridColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaColours.size(), triaColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triaVals.size(), triaVals.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, linesCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*lineCoords.size(), lineCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, linesColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*lineColours.size(), lineColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*lineIndices.size(), lineIndices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, fLinesCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*fLineCoords.size(), fLineCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, fLinesColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*fLineColours.size(), fLineColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fLinesIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*fLineIndices.size(), fLineIndices.data(), GL_DYNAMIC_DRAW);
}

void MainView::updateMatrices() {
  modelViewMatrix.setToIdentity();
  projectionMatrix.setToIdentity();

  updateUniformsRequired = true;
}

void MainView::updateUniforms() {

  // Qt wrappers
  // mainShaderProg->setUniformValue(uniModelViewMatrix, modelViewMatrix);
  // mainShaderProg->setUniformValue(uniProjectionMatrix, projectionMatrix);

  // Pure OpenGL
  mainShaderProg->bind();
  glUniformMatrix4fv(uniModelViewMatrix, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjectionMatrix, 1, false, projectionMatrix.data());
  mainShaderProg->release();
  cMapShaderProg->bind();
  glUniformMatrix4fv(uniMVMat_cMap, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjMat_cMap, 1, false, projectionMatrix.data());
  glUniform1i(uniNLevels_cMap, levels_rho);
  glUniform1i(uniColorMap_cMap, scalar_col);
  glUniform1i(uniClamping, clamp_cmap);
  glUniform1f(uniClampMin, clamp_min);
  glUniform1f(uniClampMax, clamp_max);
  cMapShaderProg->release();
  updateUniformsRequired = false;
}

void MainView::clearArrays() {
  triaCoords.clear();
  triaCoords.squeeze();
  triaColours.clear();
  triaColours.squeeze();
  triaVals.clear();
  triaVals.squeeze();
  triaIndices.clear();
  triaIndices.squeeze();
  lineCoords.clear();
  lineCoords.squeeze();
  lineColours.clear();
  lineColours.squeeze();
  lineIndices.clear();
  lineIndices.squeeze();
  fLineCoords.clear();
  fLineCoords.squeeze();
  fLineColours.clear();
  fLineColours.squeeze();
  fLineIndices.clear();
  fLineIndices.squeeze();
}

// ---

void MainView::initializeGL() {

  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  debugLogger = new QOpenGLDebugLogger();
  connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

  if ( debugLogger->initialize() ) {
    qDebug() << ":: Logging initialized";
    debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
    debugLogger->enableMessages();
  }

  QString glVersion;
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  glPointSize(16);

  // ---

  createShaderPrograms();
  createBuffers();

  // Parameter initialization
  simulation = Simulation(DIM);
  dt = 0.4;
  visc = 0.001;
  color_dir = 0;
  vec_scale = 1;
  draw_smoke = 1;
  draw_vecs = 1;
  draw_force_field = 0;
  scalar_col = 0;
  frozen = 0;
  levels_rho = 10;
  levels_v = 10;
  levels_f = 10;

  clamp_cmap = true;
  clamp_min = 0.0;
  clamp_max = 1.0;

  do_one_simulation_step();
  this->startTimer(0);
  updateMatrices();
}

void MainView::do_one_simulation_step(void)
{
//    try
//    {
    simulation.set_forces(DIM);
    Struct vdir = simulation.solve(DIM, visc, dt);
    fftw_real* rho = simulation.diffuse_matter(DIM, dt);
    Struct fdir = simulation.get_force();
//    }
//    catch (std::exception e)
//    {
//        qDebug() << "simulation step failed";
//        qDebug() << e.what();
//    }
    try
    {
//    qDebug() <<" rho row: " << rho[0] << rho[1] << rho[50] << rho[51];
    updateBuffers(rho, vdir.x, vdir.y, fdir.x, fdir.y);
//    qDebug() << "check";
    }
    catch (std::exception e)
    {
        qDebug() << "updating buffers failed";
        qDebug() << e.what();
    }
}

void MainView::paintGL() {
  try
    {
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //glLoadIdentity();

      if (updateUniformsRequired) {
        updateUniforms();
      }
      cMapShaderProg->bind();
      if (draw_smoke)
      {
          glBindVertexArray(gridVAO);
          glDrawElements(GL_TRIANGLES, triaIndices.size(), GL_UNSIGNED_SHORT, nullptr);
          glBindVertexArray(0);
      }
      cMapShaderProg->release();
      mainShaderProg->bind();
      if (draw_vecs)
      {
          glBindVertexArray(linesVAO);
          glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_SHORT, nullptr);
          glBindVertexArray(0);
      }
      if (draw_force_field)
      {
          glBindVertexArray(fLinesVAO);
          glDrawElements(GL_LINES, fLineIndices.size(), GL_UNSIGNED_SHORT, nullptr);
          glBindVertexArray(0);
      }

      mainShaderProg->release();
    }
    catch (std::exception e)
    {
        qDebug() << "update failed";
        qDebug() << e.what();
    }
  //glFlush();
}

void MainView::resizeGL(int newWidth, int newHeight) {
  updateMatrices();
}

void MainView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        simulation.drag(DIM, width(), height(), event->pos().x(), event->pos().y());
    }
    lastpos = event->pos();

}

void MainView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        simulation.drag(DIM, width(), height(), event->pos().x(), event->pos().y());
    }
    lastpos = event->pos();
}

void MainView::timerEvent(QTimerEvent *e)
{
    if (!frozen)
    {
        do_one_simulation_step();
        this->update();
    }
}

// ---

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
  qDebug() << " → Log:" << Message;
}
