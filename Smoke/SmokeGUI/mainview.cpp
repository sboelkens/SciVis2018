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
}

void MainView::initBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy) {

  clearArrays();

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;

  triaCoords.reserve(n_points);
  triaColours.reserve(n_points);
  triaVals.reserve(n_points);
  triaIndices.reserve(n_trias);

  lineCoords.reserve(nr_glyphs_x * nr_glyphs_y * 2);
  lineColours.reserve(nr_glyphs_x * nr_glyphs_y * 2);
  lineIndices.reserve(nr_glyphs_x * nr_glyphs_y * 2);

  int idx0, idx1, idx2, idx3;
  double px, py;//px0, py0, px1, py1, px2, py2, px3, py3;

  fftw_real  wn = 2.0 / (fftw_real)(DIM + 1);   // Grid cell width
  fftw_real  hn = 2.0 / (fftw_real)(DIM + 1);  // Grid cell height

  float rho_max = 0.0; float vnorm_max = 0.0; float fnorm_max = 0.0;
  float rho_min = 9999.0; float vnorm_min = 9999.0; float fnorm_min = 9999.0;
  float vnorm, fnorm;

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
          triaColours.append(set_colormap(rho[idx0], smoke_col, levels_smoke));
          triaVals.append(rho[idx0]);

          if (rho[idx0] > rho_max)
          {
              rho_max = rho[idx0];
          }
          if (rho[idx0] < rho_min)
          {
              rho_min = rho[idx0];
          }
          vnorm = sqrt(vx[idx0]*vx[idx0] + vy[idx0]*vy[idx0]);
          fnorm = sqrt(fx[idx0]*fx[idx0] + fy[idx0]*fy[idx0]);
          if (vnorm > vnorm_max)
          {
              vnorm_max = vnorm;
          }
          if (vnorm < vnorm_min)
          {
              vnorm_min = vnorm;
          }
          if (fnorm > fnorm_max)
          {
              fnorm_max = fnorm;
          }
          if (fnorm > fnorm_min)
          {
              fnorm_min = fnorm;
          }

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

  fftw_real  w_glyphs = 2.0 / (fftw_real)(nr_glyphs_x + 1);   // Grid cell width
  fftw_real  h_glyphs = 2.0 / (fftw_real)(nr_glyphs_y + 1);  // Grid cell height

  int idx_glyphs;
  float tx, ty, x_inter, y_inter, col_inter;


  for (int j = 0; j < nr_glyphs_y; j++)            //draw smoke
  {
      for (int i = 0; i < nr_glyphs_x; i++)
      {
          px = w_glyphs + (fftw_real)i * w_glyphs - 1.0;
          py = h_glyphs + (fftw_real)j * h_glyphs - 1.0;

          x_inter = glyph_interpolation(i, j, vx);
          y_inter = glyph_interpolation(i, j, vy);
          col_inter = glyph_interpolation(i, j, rho);

          lineCoords.append(QVector2D(px, py));
          lineCoords.append(QVector2D(px + vec_scale * x_inter, py + vec_scale * y_inter));
          lineColours.append(set_colormap(col_inter, glyph_col, levels_glyph));
          lineColours.append(set_colormap(col_inter, glyph_col, levels_glyph));
          lineIndices.append(2*idx_glyphs);
          lineIndices.append(2*idx_glyphs+1);
      }
  }

  scale_maxvals_rho[scale_cnt] = rho_max;
  scale_minvals_rho[scale_cnt] = rho_min;
  scale_maxvals_vnorm[scale_cnt] = vnorm_max;
  scale_minvals_vnorm[scale_cnt] = vnorm_min;
  scale_maxvals_fnorm[scale_cnt] = fnorm_max;
  scale_minvals_fnorm[scale_cnt] = fnorm_min;

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
}

void MainView::updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy) {

  //clearArrays();

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;

  triaVals.clear();
  triaVals.squeeze();
  triaVals.reserve(n_points);

  clearLineArrays();
  lineCoords.reserve(nr_glyphs_x*nr_glyphs_y * 2);
  lineColours.reserve(nr_glyphs_x*nr_glyphs_y * 2);
  lineIndices.reserve(nr_glyphs_x*nr_glyphs_y * 2);

  int idx0;
  double px, py;//px0, py0, px1, py1, px2, py2, px3, py3;

  float rho_max = 0.0; float vnorm_max = 0.0; float fnorm_max = 0.0;
  float rho_min  = 9999.0; float vnorm_min = 9999.0; float fnorm_min = 9999.0;
  float vnorm, fnorm;

  fftw_real  wn = 2.0 / (fftw_real)(DIM + 1);   // Grid cell width
  fftw_real  hn = 2.0 / (fftw_real)(DIM + 1);  // Grid cell height

  for (int j = 0; j < DIM; j++)            //draw smoke
  {
      for (int i = 0; i < DIM; i++)
      {
          px = wn + (fftw_real)i * wn - 1.0;
          py = hn + (fftw_real)j * hn - 1.0;
          idx0 = (j * DIM) + i;

          if (rho[idx0] > rho_max)
          {
              rho_max = rho[idx0];
          }
          if (rho[idx0] < rho_min)
          {
              rho_min = rho[idx0];
          }
          vnorm = sqrt(vx[idx0]*vx[idx0] + vy[idx0]*vy[idx0]);
          fnorm = sqrt(fx[idx0]*fx[idx0] + fy[idx0]*fy[idx0]);
          if (vnorm > vnorm_max)
          {
              vnorm_max = vnorm;
          }
          if (vnorm < vnorm_min)
          {
              vnorm_min = vnorm;
          }
          if (fnorm > fnorm_max)
          {
              fnorm_max = fnorm;
          }
          if (fnorm > fnorm_min)
          {
              fnorm_min = fnorm;
          }

          if (smoke_var == RHO)
          {
              triaVals.append(rho[idx0]);
          }
          else if (smoke_var == V)
          {
              triaVals.append(sqrt(vx[idx0]*vx[idx0] + vy[idx0]*vy[idx0]));
          }
          else if (smoke_var == F)
          {
              triaVals.append(sqrt(fx[idx0]*fx[idx0] + fy[idx0]*fy[idx0]));
          }

//          if(glyph_vector_var == V) {
//              lineCoords.append(QVector2D(px, py));
//              lineCoords.append(QVector2D(px + vec_scale * vx[idx0], py + vec_scale * vy[idx0]));
//          } else if(glyph_vector_var == F) {
//              lineCoords.append(QVector2D(px, py));
//              lineCoords.append(QVector2D(px + vec_scale * fx[idx0], py + vec_scale * fy[idx0]));
//          }

//          float val;
//          if(glyph_var == RHO) {
//            val = rho[idx0];
//          } else if(glyph_var == V) {
//              val = sqrt(vx[idx0]*vx[idx0] + vy[idx0]*vy[idx0]);
//          } else if(glyph_var == F) {
//              val = sqrt(fx[idx0]*fx[idx0] + fy[idx0]*fy[idx0]);
//          }
//          lineColours.append(set_colormap(val, glyph_col, levels_glyph));
//          lineColours.append(set_colormap(val, glyph_col, levels_glyph));
//          lineIndices.append(2*idx0);
//          lineIndices.append(2*idx0+1);
      }
  }

  scale_maxvals_rho[scale_cnt] = rho_max;
  scale_minvals_rho[scale_cnt] = rho_min;
  scale_maxvals_vnorm[scale_cnt] = vnorm_max;
  scale_minvals_vnorm[scale_cnt] = vnorm_min;
  scale_maxvals_fnorm[scale_cnt] = fnorm_max;
  scale_minvals_fnorm[scale_cnt] = fnorm_min;

  fftw_real  w_glyphs = 2.0 / (fftw_real)(nr_glyphs_x + 1);   // Grid cell width
  fftw_real  h_glyphs = 2.0 / (fftw_real)(nr_glyphs_y + 1);  // Grid cell height

  int idx_glyphs;
  float x_inter, y_inter, col_inter;

  for (int j = 0; j < nr_glyphs_y; j++)            //draw smoke
  {
      for (int i = 0; i < nr_glyphs_x; i++)
      {
          px = w_glyphs + (fftw_real)i * w_glyphs - 1.0;
          py = h_glyphs + (fftw_real)j * h_glyphs - 1.0;

          idx_glyphs = (j * nr_glyphs_x) + i;

          lineCoords.append(QVector2D(px, py));
          if(glyph_vector_var == V) {
              x_inter = glyph_interpolation(i, j, vx);
              y_inter = glyph_interpolation(i, j, vy);

          } else if(glyph_vector_var == F) {
              x_inter = glyph_interpolation(i, j, fx);
              y_inter = glyph_interpolation(i, j, fy);
          }
          lineCoords.append(QVector2D(px + vec_scale * x_inter, py + vec_scale * y_inter));

          if(glyph_var == RHO) {
              col_inter = glyph_interpolation(i, j, rho);
          } else if(glyph_var == V) {
              x_inter = glyph_interpolation(i, j, vx);
              y_inter = glyph_interpolation(i, j, vy);
              col_inter = sqrt(x_inter*x_inter + y_inter*y_inter);
          } else if(glyph_var == F) {
              x_inter = glyph_interpolation(i, j, fx);
              y_inter = glyph_interpolation(i, j, fy);
              col_inter = sqrt(x_inter*x_inter + y_inter*y_inter);
          }
          lineColours.append(set_colormap(col_inter, glyph_col, levels_glyph));
          lineColours.append(set_colormap(col_inter, glyph_col, levels_glyph));
          lineIndices.append(2*idx_glyphs);
          lineIndices.append(2*idx_glyphs+1);
      }
  }



  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triaVals.size(), triaVals.data(), GL_DYNAMIC_DRAW);

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
  mainShaderProg->bind();
  glUniformMatrix4fv(uniModelViewMatrix, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjectionMatrix, 1, false, projectionMatrix.data());
  mainShaderProg->release();
  cMapShaderProg->bind();
  glUniformMatrix4fv(uniMVMat_cMap, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjMat_cMap, 1, false, projectionMatrix.data());
  glUniform1i(uniNLevels_cMap, levels_smoke);
  glUniform1i(uniColorMap_cMap, smoke_col);
  glUniform1i(uniClamping, clamp_cmap);
  glUniform1f(uniClampMin, clamp_min);
  glUniform1f(uniClampMax, clamp_max);
  cMapShaderProg->release();
  updateUniformsRequired = false;
}

void MainView::clearArrays()
{
    clearGridArrays();
    clearLineArrays();
}

void MainView::clearGridArrays()
{
    triaCoords.clear();
    triaCoords.squeeze();
    triaColours.clear();
    triaColours.squeeze();
    triaVals.clear();
    triaVals.squeeze();
    triaIndices.clear();
    triaIndices.squeeze();
}

void MainView::clearLineArrays()
{
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

  //debugLogger = new QOpenGLDebugLogger();
  //connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

  //if ( debugLogger->initialize() ) {
  //  qDebug() << ":: Logging initialized";
  //   debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
  //  debugLogger->enableMessages();
  //}

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
  color_dir = 1;
  vec_scale = 1;
  draw_smoke = 1;
  smoke_var = 0;
  glyph_var = 0;
  glyph_vector_var = 1;
  draw_vecs = 1;
  draw_force_field = 0;
  smoke_col = 0;
  glyph_col = 1;
  frozen = 0;
  levels_smoke = 10;
  levels_glyph = 10;

  nr_glyphs_x = DIM;
  nr_glyphs_y = DIM;

  clamp_cmap = true;
  clamp_min = 0.0;
  clamp_max = 1.0;
  scale_window = 1000;
  scale_cnt = 0;
  scale_maxvals_rho.reserve(scale_window);
  scale_minvals_rho.reserve(scale_window);
  scale_maxvals_vnorm.reserve(scale_window);
  scale_minvals_vnorm.reserve(scale_window);
  scale_maxvals_fnorm.reserve(scale_window);
  scale_minvals_fnorm.reserve(scale_window);
  scale_maxvals_rho.resize(scale_window);
  scale_minvals_rho.resize(scale_window);
  scale_maxvals_vnorm.resize(scale_window);
  scale_minvals_vnorm.resize(scale_window);
  scale_maxvals_fnorm.resize(scale_window);
  scale_minvals_fnorm.resize(scale_window);

  first_simulation_step();
  this->startTimer(0);
  updateMatrices();
  is_initialized = true;
}

void MainView::do_one_simulation_step(void)
{
    simulation.set_forces(DIM);
    Struct vdir = simulation.solve(DIM, visc, dt);
    fftw_real* rho = simulation.diffuse_matter(DIM, dt);
    Struct fdir = simulation.get_force();
    try
    {
        updateBuffers(rho, vdir.x, vdir.y, fdir.x, fdir.y);
    }
    catch (std::exception e)
    {
        qDebug() << "updating buffers failed";
        qDebug() << e.what();
    }
    if (!clamp_cmap)
    {
        if (smoke_var == RHO)
        {
            clamp_max = findMean(scale_maxvals_rho);
            clamp_min = findMean(scale_minvals_rho);
        }
        else if (smoke_var == V)
        {
            clamp_max = findMean(scale_maxvals_vnorm);
            clamp_min = findMean(scale_minvals_vnorm);
        }
        else if (smoke_var == F)
        {
            clamp_max = findMean(scale_maxvals_fnorm);
            clamp_min = findMean(scale_minvals_fnorm);
        }
        updateUniformsRequired = true;
    }
    if (scale_cnt < scale_window - 1)
    {
        scale_cnt++;
    }
    else
    {
        scale_cnt = 0;
    }
}

void MainView::first_simulation_step(void)
{
    simulation.set_forces(DIM);
    Struct vdir = simulation.solve(DIM, visc, dt);
    fftw_real* rho = simulation.diffuse_matter(DIM, dt);
    Struct fdir = simulation.get_force();
    try
    {
        initBuffers(rho, vdir.x, vdir.y, fdir.x, fdir.y);
    }
    catch (std::exception e)
    {
        qDebug() << "initializing buffers failed";
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

float MainView::glyph_interpolation(int i, int j, fftw_real* mat)
{
    fftw_real  wn = 2.0 / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = 2.0 / (fftw_real)(DIM + 1);  // Grid cell height
    float x_pct, y_pct, x_pos_grid, y_pos_grid, x_under, y_under, x_over, y_over;
    float tx, ty, q11, q12, q21, q22, inter;

    x_pct = (fftw_real)i / nr_glyphs_x;
    y_pct = (fftw_real)j / nr_glyphs_y;
    x_pos_grid = x_pct * DIM;
    y_pos_grid = y_pct * DIM;
    x_under = floorf(x_pos_grid);
    y_under = floorf(y_pos_grid);
    x_over = ceilf(x_pos_grid);
    y_over = ceilf(y_pos_grid);

    tx = (x_pos_grid - x_under);
    ty = (y_pos_grid - y_under);

    q11 = mat[(int)y_under * DIM + (int)x_under];
    q12 = mat[(int)y_under * DIM + (int)x_over];
    q21 = mat[(int)y_over * DIM + (int)x_under];
    q22 = mat[(int)y_over * DIM + (int)x_over];
    inter = (1 - tx) * (1 - ty) * q11 + tx * (1 - ty) * q12 +
            (1 - tx) * ty * q21 + tx * ty * q22;
    return inter;
}

// ---

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
  qDebug() << " → Log:" << Message;
}
