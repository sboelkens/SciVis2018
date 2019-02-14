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
}

void MainView::updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy) {
    qDebug() << " call me call me";

//  unsigned short k;
//  float length = 0.6;
//  float tau = 6.283185;

  clearArrays();

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;
  //qDebug() << DIM << " " << n_points << " " << n_trias;
  triaCoords.reserve(n_points);
  triaColours.reserve(n_points);
  triaIndices.reserve(n_trias);

  int idx0, idx1, idx2, idx3;
  double px, py;//px0, py0, px1, py1, px2, py2, px3, py3;

//  qDebug() << width() << " w " << height() << " h";

//  fftw_real  wn = (fftw_real)width() / (fftw_real)(DIM + 1);   // Grid cell width
//  fftw_real  hn = (fftw_real)height() / (fftw_real)(DIM + 1);  // Grid cell height
  fftw_real  wn = 2.0 / (fftw_real)(DIM + 1);   // Grid cell width
  fftw_real  hn = 2.0 / (fftw_real)(DIM + 1);  // Grid cell height

//  qDebug() << wn << " wn " << hn << " hn";
  //triaCoords.append(QVector2D(0.0, 0.0));
  //triaCoords.append(QVector2D(0.0, 1.0));

  for (int j = 0; j < DIM; j++)            //draw smoke
  {
      for (int i = 0; i < DIM; i++)
      {
          px = wn + (fftw_real)i * wn - 1.0;
          py = hn + (fftw_real)j * hn - 1.0;
          idx0 = (j * DIM) + i;
//          px1 = wn + (fftw_real)i * wn;
//          py1 = hn + (fftw_real)(j + 1) * hn;
          idx1 = ((j + 1) * DIM) + i;
//          px2 = wn + (fftw_real)(i + 1) * wn;
//          py2 = hn + (fftw_real)(j + 1) * hn;
          idx2 = ((j + 1) * DIM) + (i + 1);
//          px3 = wn + (fftw_real)(i + 1) * wn;
//          py3 = hn + (fftw_real)j * hn;
          idx3 = (j * DIM) + (i + 1);

          triaCoords.append(QVector2D(px, py));

          triaColours.append(set_colormap(rho[idx0], scalar_col));

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
      }
  }
  //qDebug() << "filled it up";
  //qDebug() << triaCoords.size();
  //qDebug() << triaIndices.size();
//  triaCoords.append( QVector2D(0.0, 0.0) );
//  triaColours.append( QVector3D(0.0, 1.0, 1.0) );
//  triaIndices.append(0);

//  for (k=0; k<n; k++) {
//    triaCoords.append( QVector2D(length*cos(tau * (float)k/n), length*sin(tau * (float)k/n)) );
//    triaColours.append( QVector3D( (float)k/n, 0.8, 0.5 ) );

//    triaIndices.append(k+1);
//  }

//  triaIndices.append(1);

  glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, gridColourBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaColours.size(), triaColours.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);
}

void MainView::updateMatrices() {
  modelViewMatrix.setToIdentity();
  //modelViewMatrix.translate(-width()/2, -height()/2, 0.0);
  //modelViewMatrix.scale(4, 4, 0.0);
  //modelViewMatrix.scale(width(), height(), 0.0);

  //(GLfloat)width(), (GLfloat)height());

  projectionMatrix.setToIdentity();
  //projectionMatrix.ortho(0.0, (GLdouble)width(), 0.0, (GLdouble)height(), -0.1, 0.1);
  //projectionMatrix.perspective(80.0, (float)width()/height(), 0.2, 2.0);

  //glViewport(-1.0f, -1.0f, 2.0, 2.0);

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

  // Parameter initialization
  simulation = Simulation(DIM);
  dt = 0.4;
  visc = 0.001;
  color_dir = 0;
  vec_scale = 1000;
  draw_smoke = 1;
  draw_vecs = 1;
  scalar_col = 0;
  frozen = 0;

  qDebug() << "lalala " << visc;
  do_one_simulation_step();
  //do_one_simulation_step();
//  timer = new QTimer(this);
//  timer->start(0);
  this->startTimer(0);
//  connect(&timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));
  updateMatrices();
}

void MainView::do_one_simulation_step(void)
{
    simulation.set_forces(DIM);
    Struct dir = simulation.solve(DIM, visc, dt);
    fftw_real* rho = simulation.diffuse_matter(DIM, dt);
    qDebug() <<" rho row: " << rho[0] << rho[1] << rho[50] << rho[51];
    updateBuffers(rho, dir.vx, dir.vy);
    qDebug() << "check";
}

void MainView::direction_to_color(float x, float y, int method)
{
    float r,g,b,f;
    if (method)
    {
      f = atan2(y,x) / 3.1415927 + 1;
      r = f;
      if(r > 1) r = 2 - r;
      g = f + .66667;
      if(g > 2) g -= 2;
      if(g > 1) g = 2 - g;
      b = f + 2 * .66667;
      if(b > 2) b -= 2;
      if(b > 1) b = 2 - b;
    }
    else
    { r = g = b = 1; }
//    glColor3f(r,g,b);
}

//visualize: This is the main visualization function
void MainView::visualize(fftw_real* vx, fftw_real* vy, fftw_real* rho)
{
    int        i, j, idx;
    fftw_real  wn = (fftw_real)width() / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)height() / (fftw_real)(DIM + 1);  // Grid cell heigh

    if (draw_smoke)
    {
        int idx0, idx1, idx2, idx3;
        double px0, py0, px1, py1, px2, py2, px3, py3;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        glBegin(GL_TRIANGLES);
        for (j = 0; j < DIM - 1; j++)            //draw smoke
        {
            for (i = 0; i < DIM - 1; i++)
            {
                px0 = wn + (fftw_real)i * wn;
                py0 = hn + (fftw_real)j * hn;
                idx0 = (j * DIM) + i;


                px1 = wn + (fftw_real)i * wn;
                py1 = hn + (fftw_real)(j + 1) * hn;
                idx1 = ((j + 1) * DIM) + i;


                px2 = wn + (fftw_real)(i + 1) * wn;
                py2 = hn + (fftw_real)(j + 1) * hn;
                idx2 = ((j + 1) * DIM) + (i + 1);


                px3 = wn + (fftw_real)(i + 1) * wn;
                py3 = hn + (fftw_real)j * hn;
                idx3 = (j * DIM) + (i + 1);


//                set_colormap(rho[idx0]);    glVertex2f(px0, py0);
//                set_colormap(rho[idx1]);    glVertex2f(px1, py1);
//                set_colormap(rho[idx2]);    glVertex2f(px2, py2);


//                set_colormap(rho[idx0]);    glVertex2f(px0, py0);
//                set_colormap(rho[idx2]);    glVertex2f(px2, py2);
//                set_colormap(rho[idx3]);    glVertex2f(px3, py3);
            }
        }
//        glEnd();
    }

    if (draw_vecs)
    {
//        glBegin(GL_LINES);				//draw velocities
        for (i = 0; i < DIM; i++)
            for (j = 0; j < DIM; j++)
            {
                idx = (j * DIM) + i;
                direction_to_color(vx[idx],vy[idx],color_dir);
//                glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
//                glVertex2f((wn + (fftw_real)i * wn) + vec_scale * vx[idx], (hn + (fftw_real)j * hn) + vec_scale * vy[idx]);
            }
//        glEnd();
    }
}

void MainView::paintGL() {

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glLoadIdentity();

  mainShaderProg->bind();

  if (updateUniformsRequired) {
    updateUniforms();
  }
  //visualize();
  glBindVertexArray(gridVAO);

  glDrawElements(GL_TRIANGLES, triaIndices.size(), GL_UNSIGNED_SHORT, nullptr);
  //glDrawElements(GL_POINTS, triaIndices.size()-2, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(GLushort)));

  glBindVertexArray(0);

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

void MainView::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event->key();
    qDebug() << "anything";
    switch(event->key()) {
    case 'A':
        qDebug() << "AAAAAAAAAAAAAAAAA";
        frozen = !frozen;
        break;
    }
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
