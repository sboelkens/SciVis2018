#include "mainview.h"
#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";
}

MainView::~MainView() {

  clearArrays();

  glDeleteBuffers(1, &fanCoordsBO);
  glDeleteBuffers(1, &fanColourBO);
  glDeleteBuffers(1, &fanIndexBO);
  glDeleteVertexArrays(1, &fanVAO);

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
  glGenVertexArrays(1, &fanVAO);
  glBindVertexArray(fanVAO);

  glGenBuffers(1, &fanCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, fanCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &fanColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, fanColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &fanIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fanIndexBO);

  glBindVertexArray(0);
}

void MainView::updateBuffers(unsigned short n) {

  unsigned short k;
  float length = 0.6;
  float tau = 6.283185;

  clearArrays();

  triaCoords.reserve(n+1);
  triaColours.reserve(n+1);
  triaIndices.reserve(n+2);

  triaCoords.append( QVector2D(0.0, 0.0) );
  triaColours.append( QVector3D(0.0, 1.0, 1.0) );
  triaIndices.append(0);

  for (k=0; k<n; k++) {
    triaCoords.append( QVector2D(length*cos(tau * (float)k/n), length*sin(tau * (float)k/n)) );
    triaColours.append( QVector3D( (float)k/n, 0.8, 0.5 ) );

    triaIndices.append(k+1);
  }

  triaIndices.append(1);

  glBindBuffer(GL_ARRAY_BUFFER, fanCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, fanColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaColours.size(), triaColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fanIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);
}

void MainView::updateMatrices() {
  modelViewMatrix.setToIdentity();

  projectionMatrix.setToIdentity();
  projectionMatrix.perspective(80.0, (float)width()/height(), 0.2, 2.0);

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
  updateBuffers(6);
  updateMatrices();
}

void MainView::resizeGL(int newWidth, int newHeight) {
  updateMatrices();
}

//visualize: This is the main visualization function
void MainView::visualize(void)
{
}

void MainView::paintGL() {

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mainShaderProg->bind();

  if (updateUniformsRequired) {
    updateUniforms();
  }
  //visualize();
  glBindVertexArray(fanVAO);

  glDrawElements(GL_TRIANGLE_FAN, triaIndices.size(), GL_UNSIGNED_SHORT, nullptr);
  glDrawElements(GL_POINTS, triaIndices.size()-2, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(GLushort)));

  glBindVertexArray(0);

  mainShaderProg->release();
}

// ---

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
  qDebug() << " → Log:" << Message;
}
