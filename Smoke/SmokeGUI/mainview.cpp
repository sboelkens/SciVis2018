#include "mainview.h"
//#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";
}

MainView::~MainView() {
  clearArrays();

  glDeleteBuffers(1, &gridCoordsBO);
  glDeleteBuffers(1, &gridColourBO);
  glDeleteBuffers(1, &gridIndexBO);
  glDeleteVertexArrays(1, &gridVAO);
  glDeleteBuffers(1, &linesCoordsBO);
  glDeleteBuffers(1, &linesColourBO);
  glDeleteBuffers(1, &linesIndexBO);
  glDeleteVertexArrays(1, &linesVAO);

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

  // Qt wrappers
  // uniModelViewMatrix = mainShaderProg->uniformLocation("modelviewmatrix");
  // uniProjectionMatrix = mainShaderProg->uniformLocation("projectionmatrix");

  // Pure OpenGL
  uniModelViewMatrix = glGetUniformLocation(mainShaderProg->programId(), "modelviewmatrix");
  uniProjectionMatrix = glGetUniformLocation(mainShaderProg->programId(), "projectionmatrix");
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
}

void MainView::updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy) {
//    qDebug() << " call me call me";

  clearArrays();

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;

  triaCoords.reserve(n_points);
  triaColours.reserve(n_points);
  triaIndices.reserve(n_trias);

  lineCoords.reserve(n_points * 2);
  lineColours.reserve(n_points * 2);
  lineIndices.reserve(n_points * 2);

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
      }
  }

  glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, gridColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaColours.size(), triaColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, linesCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*lineCoords.size(), lineCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, linesColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*lineColours.size(), lineColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*lineIndices.size(), lineIndices.data(), GL_DYNAMIC_DRAW);

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
  glUniformMatrix4fv(uniModelViewMatrix, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjectionMatrix, 1, false, projectionMatrix.data());

  updateUniformsRequired = false;
}

void MainView::clearArrays() {
  triaCoords.clear();
  triaCoords.squeeze();
  triaColours.clear();
  triaColours.squeeze();
  triaIndices.clear();
  triaIndices.squeeze();
  lineCoords.clear();
  lineCoords.squeeze();
  lineColours.clear();
  lineColours.squeeze();
  lineIndices.clear();
  lineIndices.squeeze();
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
  scalar_col = 0;
  frozen = 0;
  levels_rho = 10;
  levels_v = 10;
  levels_f = 10;

  do_one_simulation_step();
  this->startTimer(0);
  updateMatrices();
}

void MainView::do_one_simulation_step(void)
{
    simulation.set_forces(DIM);
    Struct dir = simulation.solve(DIM, visc, dt);
    fftw_real* rho = simulation.diffuse_matter(DIM, dt);
//    qDebug() <<" rho row: " << rho[0] << rho[1] << rho[50] << rho[51];
    updateBuffers(rho, dir.vx, dir.vy);
//    qDebug() << "check";
}

void MainView::paintGL() {

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glLoadIdentity();

  mainShaderProg->bind();

  if (updateUniformsRequired) {
    updateUniforms();
  }
  if (draw_smoke)
  {
      glBindVertexArray(gridVAO);
      glDrawElements(GL_TRIANGLES, triaIndices.size(), GL_UNSIGNED_SHORT, nullptr);
      glBindVertexArray(0);
  }
  if (draw_vecs)
  {
      glBindVertexArray(linesVAO);
      glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_SHORT, nullptr);
      glBindVertexArray(0);
  }

  mainShaderProg->release();
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
