#include "mainview.h"
//#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";
}

MainView::~MainView() {
  clearArrays();

  glDeleteBuffers(1, &gridCoordsBO);
//  glDeleteBuffers(1, &gridColourBO);
  glDeleteBuffers(1, &gridValBO);
  glDeleteBuffers(1, &gridIndexBO);
  glDeleteVertexArrays(1, &gridVAO);
  glDeleteBuffers(1, &glyphCoordsBO);
  glDeleteBuffers(1, &glyphColourBO);
  glDeleteBuffers(1, &glyphIndexBO);
  glDeleteVertexArrays(1, &glyphsVAO);

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

  glGenBuffers(1, &gridValBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &gridIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);

  glBindVertexArray(0);

  glGenVertexArrays(1, &glyphsVAO);
  glBindVertexArray(glyphsVAO);

  glGenBuffers(1, &glyphCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, glyphCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &glyphColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, glyphColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &glyphIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyphIndexBO);

  glBindVertexArray(0);
}

void MainView::updateBuffers(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy) {

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;

  if (!is_initialized)
  {
      clearArrays();
      triaCoords.reserve(n_points);
      triaIndices.reserve(n_trias);
  }
  triaVals.clear();
  triaVals.squeeze();
  triaVals.reserve(n_points);

  updateAverages(rho, vx, vy, fx, fy);

  int idx0, idx1, idx2, idx3;
  double px, py;

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

          if (!is_initialized)
          {
              triaCoords.append(QVector2D(px, py));
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
  }

  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triaVals.size(), triaVals.data(), GL_DYNAMIC_DRAW);

  if (!is_initialized)
  {
      glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);
  }

  updateGlyphs(rho, vx, vy, fx, fy);
}

void MainView::updateGlyphs(fftw_real* rho, fftw_real* vx, fftw_real* vy, fftw_real* fx, fftw_real* fy)
{
    clearLineArrays();
    glyphCoords.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * 2);
    glyphColours.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * 2);
    glyphIndices.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * 2);
    if (nr_glyphs_changed)
    {
        glyphShifts.clear();
        glyphShifts.squeeze();
        glyphShifts.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * 2);
    }

    float px, py;
    fftw_real  w_glyphs = 2.0 / (fftw_real)(nr_glyphs_x + 1);   // Grid cell width
    fftw_real  h_glyphs = 2.0 / (fftw_real)(nr_glyphs_y + 1);  // Grid cell height

    int idx_glyphs;
    float x_pct, y_pct, x_inter, y_inter, col_inter;
    float x_shift, y_shift;

    for (int j = 0; j < nr_glyphs_y; j++)            //draw smoke
    {
        for (int i = 0; i < nr_glyphs_x; i++)
        {
            for (int k = 0; k < nr_glyphs_p; k++)
            {
                x_pct = (fftw_real)i / nr_glyphs_x;
                y_pct = (fftw_real)j / nr_glyphs_y;

                idx_glyphs = (j * nr_glyphs_x * nr_glyphs_p) + (i * nr_glyphs_p) + k;

                if (nr_glyphs_changed)
                {
                    x_shift = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/MAX_SAMPLE_DIST));
                    y_shift = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/MAX_SAMPLE_DIST));
                }
                else
                {
                    x_shift = glyphShifts[2 * idx_glyphs];
                    y_shift = glyphShifts[2 * idx_glyphs + 1];
                }

                x_pct += x_shift;
                y_pct += y_shift;
                x_pct = std::min(std::max(0.0f, x_pct),0.98f);
                y_pct = std::min(std::max(0.0f, y_pct),1.0f);

                px = w_glyphs + x_pct * (float)nr_glyphs_x * w_glyphs - 1.0;
                py = h_glyphs + y_pct * (float)nr_glyphs_x * h_glyphs - 1.0;

                glyphCoords.append(QVector2D(px, py));
                if(glyph_vector_var == V) {
                    x_inter = glyph_interpolation(x_pct, y_pct, vx);
                    y_inter = glyph_interpolation(x_pct, y_pct, vy);

                } else if(glyph_vector_var == F) {
                    x_inter = glyph_interpolation(x_pct, y_pct, fx);
                    y_inter = glyph_interpolation(x_pct, y_pct, fy);
                }
                glyphCoords.append(QVector2D(px + vec_scale * x_inter, py + vec_scale * y_inter));

                if(glyph_var == RHO) {
                    col_inter = glyph_interpolation(x_pct, y_pct, rho);
                } else if(glyph_var == V) {
                    x_inter = glyph_interpolation(x_pct, y_pct, vx);
                    y_inter = glyph_interpolation(x_pct, y_pct, vy);
                    col_inter = sqrt(x_inter*x_inter + y_inter*y_inter);
                } else if(glyph_var == F) {
                    x_inter = glyph_interpolation(x_pct, y_pct, fx);
                    y_inter = glyph_interpolation(x_pct, y_pct, fy);
                    col_inter = sqrt(x_inter*x_inter + y_inter*y_inter);
                }
                glyphColours.append(set_colormap(col_inter, glyph_col, levels_glyph));
                glyphColours.append(set_colormap(col_inter, glyph_col, levels_glyph));
                glyphIndices.append(2*idx_glyphs);
                glyphIndices.append(2*idx_glyphs+1);
                if (nr_glyphs_changed)
                {
                    glyphShifts.append(x_shift);
                    glyphShifts.append(y_shift);
                }
            }
        }
    }

    nr_glyphs_changed = false;

    glBindBuffer(GL_ARRAY_BUFFER, glyphCoordsBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D)*glyphCoords.size(), glyphCoords.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, glyphColourBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*glyphColours.size(), glyphColours.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyphIndexBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*glyphIndices.size(), glyphIndices.data(), GL_DYNAMIC_DRAW);
}

void MainView::updateAverages(fftw_real *rho, fftw_real *vx, fftw_real *vy, fftw_real *fx, fftw_real *fy)
{
    float rho_max = 0.0; float vnorm_max = 0.0; float fnorm_max = 0.0;
    float rho_min  = 9999.0; float vnorm_min = 9999.0; float fnorm_min = 9999.0;
    float vnorm, fnorm;

    int idx0;
    for (int j = 0; j < DIM; j++)            //draw smoke
    {
        for (int i = 0; i < DIM; i++)
        {
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
        }
    }

    scale_maxvals_rho[scale_cnt] = rho_max;
    scale_minvals_rho[scale_cnt] = rho_min;
    scale_maxvals_vnorm[scale_cnt] = vnorm_max;
    scale_minvals_vnorm[scale_cnt] = vnorm_min;
    scale_maxvals_fnorm[scale_cnt] = fnorm_max;
    scale_minvals_fnorm[scale_cnt] = fnorm_min;
}

void MainView::updateMatrices() {
  modelViewMatrix.setToIdentity();
  projectionMatrix.setToIdentity();

  updateUniformsRequired = true;
}

void MainView::updateUniforms() {

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
    triaVals.clear();
    triaVals.squeeze();
    triaIndices.clear();
    triaIndices.squeeze();
    // clear line displacements when clearing grid
    glyphShifts.clear();
    glyphShifts.squeeze();
}

void MainView::clearLineArrays()
{
    glyphCoords.clear();
    glyphCoords.squeeze();
    glyphColours.clear();
    glyphColours.squeeze();
    glyphIndices.clear();
    glyphIndices.squeeze();
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
  nr_glyphs_p = 4;
  nr_glyphs_changed = true;
  srand(time(NULL)); // initialize seed for rng

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

  do_one_simulation_step();
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
    if (is_initialized)
    {
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
          glBindVertexArray(glyphsVAO);
          glDrawElements(GL_LINES, glyphIndices.size(), GL_UNSIGNED_SHORT, nullptr);
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

float MainView::glyph_interpolation(float x_pct, float y_pct, fftw_real* mat)
{
    fftw_real  wn = 2.0 / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = 2.0 / (fftw_real)(DIM + 1);  // Grid cell height
    float x_pos_grid, y_pos_grid, x_under, y_under, x_over, y_over;
    float tx, ty, q11, q12, q21, q22, inter;

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
