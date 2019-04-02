#include "mainview.h"
//#include "math.h"

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";
}

MainView::~MainView() {
  clearArrays();

  glDeleteBuffers(1, &gridCoordsBO);
  glDeleteBuffers(1, &gridNormalsBO);
  glDeleteBuffers(1, &gridValBO);
  glDeleteBuffers(1, &gridIndexBO);
  glDeleteVertexArrays(1, &gridVAO);
  glDeleteBuffers(1, &glyphCoordsBO);
  glDeleteBuffers(1, &glyphColourBO);
  glDeleteBuffers(1, &glyphNormalsBO);
  glDeleteBuffers(1, &glyphIndexBO);
  glDeleteVertexArrays(1, &glyphsVAO);
  glDeleteBuffers(1, &isolinesCoordsBO);
  glDeleteBuffers(1, &isolinesColourBO);
  glDeleteBuffers(1, &isolinesIndexBO);
  glDeleteVertexArrays(1, &isolinesVAO);

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
  uniNormalMatrix = glGetUniformLocation(mainShaderProg->programId(), "normalmatrix");
  uniPhong = glGetUniformLocation(mainShaderProg->programId(), "phong");

  uniMVMat_cMap = glGetUniformLocation(cMapShaderProg->programId(), "modelviewmatrix");
  uniProjMat_cMap = glGetUniformLocation(cMapShaderProg->programId(), "projectionmatrix");
  uniNormMat_cMap = glGetUniformLocation(cMapShaderProg->programId(), "normalmatrix");
  uniNLevels_cMap = glGetUniformLocation(cMapShaderProg->programId(), "levels");
  uniColorMap_cMap = glGetUniformLocation(cMapShaderProg->programId(), "mode");
  uniGlyphClamping = glGetUniformLocation(cMapShaderProg->programId(), "glyphclamp");
  uniGlyphClampMax = glGetUniformLocation(cMapShaderProg->programId(), "glyphmaxval");
  uniGlyphClampMin = glGetUniformLocation(cMapShaderProg->programId(), "glyphminval");
  uniSmokeClamping = glGetUniformLocation(cMapShaderProg->programId(), "clamp");
  uniSmokeClampMax = glGetUniformLocation(cMapShaderProg->programId(), "maxval");
  uniSmokeClampMin = glGetUniformLocation(cMapShaderProg->programId(), "minval");
  uniPhongHeightPlot = glGetUniformLocation(cMapShaderProg->programId(), "phong");
}

void MainView::createBuffers() {

  // Pure OpenGL
  glGenVertexArrays(1, &gridVAO);
  glBindVertexArray(gridVAO);

  glGenBuffers(1, &gridCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &gridValBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &gridNormalsBO);
  glBindBuffer(GL_ARRAY_BUFFER, gridNormalsBO);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &gridIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);

  glBindVertexArray(0);

  glGenVertexArrays(1, &glyphsVAO);
  glBindVertexArray(glyphsVAO);

  glGenBuffers(1, &glyphCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, glyphCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &glyphColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, glyphColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &glyphNormalsBO);
  glBindBuffer(GL_ARRAY_BUFFER, glyphNormalsBO);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &glyphIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyphIndexBO);

  glBindVertexArray(0);

  glGenVertexArrays(1, &isolinesVAO);
  glBindVertexArray(isolinesVAO);

  glGenBuffers(1, &isolinesCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, isolinesCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &isolinesColourBO);
  glBindBuffer(GL_ARRAY_BUFFER, isolinesColourBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &isolinesIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, isolinesIndexBO);

  glBindVertexArray(0);
}

void MainView::updateBuffers() {

  int n_points = (DIM) * (DIM); //= DIM*DIM
  int n_trias = (DIM-1) * (DIM-1) * 6;

  fftw_real* rho = simulation.getRho();
  fftw_real* vx = simulation.getVx();
  fftw_real* vy = simulation.getVy();
  fftw_real* fx = simulation.getFx();
  fftw_real* fy = simulation.getFy();

  if (!is_initialized)
  {
      clearArrays();
      triaIndices.reserve(n_trias);
      triaVertFaces.reserve(n_points);
      for (int i = 0; i < n_points; i++)
      {
          QVector<unsigned short> vertFaces;
          vertFaces.reserve(6);
          triaVertFaces.append(vertFaces);
      }
  }
  triaCoords.clear();
  triaCoords.squeeze();
  triaCoords.reserve(n_points);
  triaNormals.clear();
  triaNormals.squeeze();
  triaNormals.reserve(n_points);
  triaVals.clear();
  triaVals.squeeze();
  triaVals.reserve(n_points);

  updateAverages(rho, vx, vy, fx, fy);

  int idx0, idx1, idx2, idx3;
  double px, py;
  int idxT = 0;

  fftw_real var;

  fftw_real  wn = 2.0 / static_cast<double>(DIM + 1);   // Grid cell width
  fftw_real  hn = 2.0 / static_cast<double>(DIM + 1);  // Grid cell height

  for (int j = 0; j < DIM; j++)            //draw smoke
  {
      for (int i = 0; i < DIM; i++)
      {
          px = wn + static_cast<double>(i) * wn - 1.0;
          py = hn + static_cast<double>(j) * hn - 1.0;


          idx0 = (j * DIM) + i;
          idx1 = ((j + 1) * DIM) + i;
          idx2 = ((j + 1) * DIM) + (i + 1);
          idx3 = (j * DIM) + (i + 1);

          if (smoke_var == RHO)
          {
              triaVals.append(static_cast<float>(rho[idx0]));
          }
          else if (smoke_var == V)
          {
              triaVals.append(static_cast<float>(sqrt(vx[idx0]*vx[idx0] + vy[idx0]*vy[idx0])));
          }
          else if (smoke_var == F)
          {
              triaVals.append(static_cast<float>(sqrt(fx[idx0]*fx[idx0] + fy[idx0]*fy[idx0])));
          }
          else if (smoke_var == DIVV)
          {
              triaVals.append(static_cast<float>(simulation.getDivV()[idx0]));
          }
          else if (smoke_var == DIVF)
          {
              triaVals.append(static_cast<float>(simulation.getDivF()[idx0]));
          }
          if (heightplot)
          {
              if (height_var == RHO)
              {
                  triaCoords.append(QVector3D(static_cast<float>(px), static_cast<float>(py),
                                              static_cast<float>(std::min(0.0+rho[idx0]/height_scaler, 1.0))));
              }
              if (height_var == V)
              {
                  var = sqrt(vx[idx0]*vx[idx0] + vy[idx0]*vy[idx0]);
                  triaCoords.append(QVector3D(static_cast<float>(px), static_cast<float>(py),
                                              static_cast<float>(std::min(0.0+var/height_scaler, 1.0))));
              }
              if (height_var == F)
              {
                  var = sqrt(fx[idx0]*fx[idx0] + fy[idx0]*fy[idx0]);
                  triaCoords.append(QVector3D(static_cast<float>(px), static_cast<float>(py),
                                              static_cast<float>(std::min(0.0+var/height_scaler, 1.0))));
              }
          }
          else
          {
              triaCoords.append(QVector3D(static_cast<float>(px), static_cast<float>(py), -1.0f));
              triaNormals.append(QVector3D(0.0, 0.0, 1.0));
          }

          if (!is_initialized)
          {
              if (j + 1 < DIM && i + 1 < DIM)
              {
                  //first tria
                  triaIndices.append(static_cast<unsigned short>(idx0));
                  triaIndices.append(static_cast<unsigned short>(idx1));
                  triaIndices.append(static_cast<unsigned short>(idx2));
                  // second tria
                  triaIndices.append(static_cast<unsigned short>(idx0));
                  triaIndices.append(static_cast<unsigned short>(idx2));
                  triaIndices.append(static_cast<unsigned short>(idx3));

                  triaVertFaces[idx0].append(static_cast<unsigned short>(idxT));
                  triaVertFaces[idx1].append(static_cast<unsigned short>(idxT));
                  triaVertFaces[idx2].append(static_cast<unsigned short>(idxT));

                  triaVertFaces[idx0].append(static_cast<unsigned short>(idxT+1));
                  triaVertFaces[idx2].append(static_cast<unsigned short>(idxT+1));
                  triaVertFaces[idx3].append(static_cast<unsigned short>(idxT+1));

                  idxT += 2;
              }
          }
      }
  }
  if (heightplot)
  {
      QVector<QVector3D> faceNormals;
      QVector3D coord1, coord2, coord3, normal;
      faceNormals.reserve(n_trias/3);
      for (int i = 0; i < triaIndices.length(); i += 3)
      {
          coord1 = triaCoords[triaIndices[i]];
          coord2 = triaCoords[triaIndices[i+1]];
          coord3 = triaCoords[triaIndices[i+2]];
          normal = QVector3D::crossProduct(coord1 - coord2, coord1 - coord3);
          normal = normal / normal.length();
          faceNormals.append(normal);
      }
      QVector3D vertNormal;
      for (int i = 0; i < triaVertFaces.length(); i++)
      {
          vertNormal = QVector3D(0.0, 0.0, 0.0);
          for (int j = 0; j < triaVertFaces[i].length(); j++)
          {
              vertNormal += faceNormals[triaVertFaces[i][j]];
          }
          vertNormal = vertNormal / vertNormal.length();
          triaNormals.append(vertNormal);
      }
  }

  glBindBuffer(GL_ARRAY_BUFFER, gridValBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triaVals.size(), triaVals.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, gridCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaCoords.size(), triaCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, gridNormalsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*triaNormals.size(), triaNormals.data(), GL_DYNAMIC_DRAW);

  if (!is_initialized)
  {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIndexBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*triaIndices.size(), triaIndices.data(), GL_DYNAMIC_DRAW);
  }
}

void MainView::updateGlyphs()
{
    //glyphs3D = 0;

    int glyphV, glyphT;
    if (glyphs3D)
    {
        glyphV = cone.vertexCoords.length();
        glyphT = cone.faceCoordInd.length();
    }
    else
    {
        glyphT = glyphV = 2;
    }

    clearLineArrays();
    glyphCoords.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * glyphV);
    glyphColours.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * glyphV);
    glyphNormals.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * glyphV);
    glyphIndices.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * glyphT);
    if (nr_glyphs_changed)
    {
        glyphShifts.clear();
        glyphShifts.squeeze();
        glyphShifts.reserve(nr_glyphs_x*nr_glyphs_y*nr_glyphs_p * 2);
    }

    fftw_real* rho = simulation.getRho();
    fftw_real* vx = simulation.getVx();
    fftw_real* vy = simulation.getVy();
    fftw_real* fx = simulation.getFx();
    fftw_real* fy = simulation.getFy();

    float px, py;
    fftw_real  w_glyphs = 2.0 / static_cast<double>(nr_glyphs_x + 1);   // Grid cell width
    fftw_real  h_glyphs = 2.0 / static_cast<double>(nr_glyphs_y + 1);  // Grid cell height

    int idx_glyphs;
    float x_pct, y_pct, x_inter, y_inter, col_inter;
    float scale_factor = 1.0f;
    float x_shift, y_shift;

    QMatrix4x4 modelview;
    QMatrix4x4 normalmat;
    float d, angle;
    QVector2D dir;
    QVector3D vec3;
    QVector4D vec4;

    for (int j = 0; j < nr_glyphs_y; j++)            //draw smoke
    {
        for (int i = 0; i < nr_glyphs_x; i++)
        {
            for (int k = 0; k < nr_glyphs_p; k++)
            {

                px = w_glyphs + (fftw_real)i * w_glyphs - 1.0;
                py = h_glyphs + (fftw_real)j * h_glyphs - 1.0;
                if (nr_glyphs_x == 1)
                {
                    x_pct = 0.5;
                }
                else
                {
                    x_pct = (px - (w_glyphs - 1.0))/(w_glyphs*(nr_glyphs_x-1));
                }
                if (nr_glyphs_y == 1)
                {
                    y_pct = 0.5;
                }
                else
                {
                    y_pct = (py - (h_glyphs - 1.0))/(h_glyphs*(nr_glyphs_y-1));
                }

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
                if (x_pct > 1.0f)
                {
                    x_pct -= 2*x_shift;
                }
                y_pct = std::min(std::max(0.0f, y_pct),1.0f);

                px = -1.0 + w_glyphs + (w_glyphs*(nr_glyphs_x-1))*x_pct;
                py = -1.0 + h_glyphs + (h_glyphs*(nr_glyphs_y-1))*y_pct;
                if(glyph_vector_var == V) {
                    x_inter = glyph_interpolation(x_pct, y_pct, vx);
                    y_inter = glyph_interpolation(x_pct, y_pct, vy);
                    scale_factor = std::min((float)sqrt(x_inter*x_inter + y_inter*y_inter), 0.1f)*10.0f;

                } else if(glyph_vector_var == F) {
                    x_inter = glyph_interpolation(x_pct, y_pct, fx);
                    y_inter = glyph_interpolation(x_pct, y_pct, fy);
                    scale_factor = std::min((float)sqrt(x_inter*x_inter + y_inter*y_inter), 0.1f)*10.0f;
                } else {
                    x_inter = 0;
                    y_inter = 0;
                    scale_factor = 1;
                }

                if (glyphs3D)
                {
                    dir = QVector2D(x_inter, y_inter);
                    angle = acos(dir.dotProduct(dir, QVector2D(0.0, 1.0)) / sqrt(x_inter*x_inter + y_inter*y_inter))*360/(2*M_PI);
                    d = glyphs3D_size*scale_factor;
                    if (x_inter > 0.0f)
                    {
                        angle = -angle;
                    }
                    for (int n = 0; n < glyphV; n++)
                    {
                        modelview.setToIdentity();
                        modelview.translate(QVector3D(px, py, 0.0));
                        modelview.scale(QVector3D(d, d*width()/height(), d));
                        modelview.rotate(angle, QVector3D(0.0, 0.0, 1.0));

                        vec3 = cone.vertexCoords[n];
                        //vec3.setX(vec3.x() * height()/width());
                        vec4 = modelview * QVector4D(vec3.x(), vec3.y(), vec3.z(), 1.0);

                        glyphCoords.append(QVector3D(vec4.x(), vec4.y(), vec4.z()));
                        //glyphCoords.append(cone.vertexCoords[n]);
                        normalmat = QMatrix4x4(modelview.normalMatrix());
                        vec3 = -cone.vertexNormals[n];
                        vec4 = normalmat * QVector4D(vec3.x(), vec3.y(), vec3.z(), 1.0);

                        glyphNormals.append(QVector3D(vec4.x(), vec4.y(), vec4.z()));
                    }
                }
                else
                {
                    glyphCoords.append(QVector3D(px, py, -1.0));
                    glyphCoords.append(QVector3D(px + vec_scale * x_inter, py + vec_scale * y_inter, -1.0));
                    glyphNormals.append(QVector3D(0.0, 0.0, 1.0));
                    glyphNormals.append(QVector3D(0.0, 0.0, 1.0));
                }
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
                } else if(glyph_var == DIVV) {
                    col_inter = glyph_interpolation(x_pct, y_pct, simulation.getDivV());
                } else if(glyph_var == DIVF) {
                    col_inter = glyph_interpolation(x_pct, y_pct, simulation.getDivF());
                } else {
                    col_inter = 0.0;
                }
                for (int n = 0; n < glyphV; n++)
                {
                    if (glyphs3D)
                    {
                        //glyphColours.append(QVector3D((float)idx_glyphs,1.0,(float)n/(float)glyphV));
                        glyphColours.append(set_colormap(col_inter, glyph_col, levels_glyph, clamp_glyph_min, clamp_glyph_max));
                    }
                    else
                    {
                        glyphColours.append(set_colormap(col_inter, glyph_col, levels_glyph, clamp_glyph_min, clamp_glyph_max));
                    }
                    //glyphColours.append(set_colormap(col_inter, glyph_col, levels_glyph, clamp_glyph_min, clamp_glyph_max));
                }
                if (glyphs3D)
                {
                    for (int n = 0; n < glyphT; n++)
                    {
                        glyphIndices.append(static_cast<unsigned short>(cone.faceCoordInd[n] + static_cast<unsigned int>(glyphV*idx_glyphs)));
                    }
                }
                else
                {
                    glyphIndices.append(static_cast<unsigned short>(2*idx_glyphs));
                    glyphIndices.append(static_cast<unsigned short>(2*idx_glyphs+1));
                }
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*glyphCoords.size(), glyphCoords.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, glyphColourBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*glyphColours.size(), glyphColours.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, glyphNormalsBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*glyphNormals.size(), glyphNormals.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyphIndexBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*glyphIndices.size(), glyphIndices.data(), GL_DYNAMIC_DRAW);
}

void MainView::updateIsolines()
{
    int n_points = (DIM) * (DIM) * 2 * nr_isolines; //= DIM*DIM

    clearIsolineArrays();
    isolineCoords.reserve(n_points);
    isolineColours.reserve(n_points);
    isolineIndices.reserve(n_points);

    double isoline_min = static_cast<double>((isoline_min_value < isoline_max_value) ? isoline_min_value : isoline_max_value);
    double isoline_diff = static_cast<double>((abs(isoline_max_value - isoline_min_value)) / (nr_isolines + 1));

    double isolineVal = isoline_min;
    unsigned short idx = 0;

    for(int k = 0; k < nr_isolines; k++)
    {
        isolineVal += isoline_diff;
        QVector<QVector2D> isolines = marchingSquare.calcIsoline(simulation.getRho(), DIM, isolineVal);

        for (int i = 0; i < isolines.size(); i++)
        {
            isolineCoords.append(QVector3D(static_cast<float>(isolines[i].x()), static_cast<float>(isolines[i].y()), -1));
            isolineColours.append(set_colormap(static_cast<float>(isolineVal), isoline_col, levels_isoline));
            isolineIndices.append(idx);
            idx++;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, isolinesCoordsBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*isolineCoords.size(), isolineCoords.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, isolinesColourBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*isolineColours.size(), isolineColours.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, isolinesIndexBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*isolineIndices.size(), isolineIndices.data(), GL_DYNAMIC_DRAW);
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
            if (rho[idx0] > static_cast<double>(rho_max))
            {
                rho_max = static_cast<float>(rho[idx0]);
            }
            if (rho[idx0] < static_cast<double>(rho_min))
            {
                rho_min = static_cast<float>(rho[idx0]);
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

    scale_maxvals_rho[scale_smoke_cnt] = rho_max;
    scale_minvals_rho[scale_smoke_cnt] = rho_min;
    scale_maxvals_vnorm[scale_smoke_cnt] = vnorm_max;
    scale_minvals_vnorm[scale_smoke_cnt] = vnorm_min;
    scale_maxvals_fnorm[scale_smoke_cnt] = fnorm_max;
    scale_minvals_fnorm[scale_smoke_cnt] = fnorm_min;
}

void MainView::updateMatrices() {
  modelViewMatrix.setToIdentity();
  if (heightplot)
  {
      //modelViewMatrix.translate(0.0, -0.5, 0.0);
      modelViewMatrix.scale(hPlot_zoom, hPlot_zoom, hPlot_zoom);
      modelViewMatrix.rotate(-hPlot_xAngle, QVector3D(1.0,0.0,0.0));
      modelViewMatrix.rotate(hPlot_zAngle, QVector3D(0.0,0.0,1.0));
      //modelViewMatrix.translate(-0.75, -0.75, 0.0);

      //modelViewMatrix.rotate(45.0, QVector3D(1.0,0.0,0.0));
  }
  projectionMatrix.setToIdentity();
  float dispRatio = 1.0;//width()/height();
  projectionMatrix.ortho(-1.0*dispRatio, 1.0*dispRatio, -1.0, 1.0, -1.0, 1.0);
  //projectionMatrix.perspective(60.0, dispRatio, 0.2, 4.0);//
  normalMatrix.setToIdentity();
  normalMatrix = modelViewMatrix.normalMatrix();
  updateUniformsRequired = true;
  updateMatricesRequired = false;
}

void MainView::updateUniforms() {

  mainShaderProg->bind();
  glUniformMatrix4fv(uniModelViewMatrix, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjectionMatrix, 1, false, projectionMatrix.data());
  glUniformMatrix3fv(uniNormalMatrix, 1, false, normalMatrix.data());
  glUniform1i(uniPhong, glyphs3D);
  mainShaderProg->release();
  cMapShaderProg->bind();
  glUniformMatrix4fv(uniMVMat_cMap, 1, false, modelViewMatrix.data());
  glUniformMatrix4fv(uniProjMat_cMap, 1, false, projectionMatrix.data());
  glUniformMatrix3fv(uniNormMat_cMap, 1, false, normalMatrix.data());
  glUniform1i(uniNLevels_cMap, levels_smoke);
  glUniform1i(uniColorMap_cMap, smoke_col);
  glUniform1i(uniGlyphClamping, clamp_glyph_cmap);
  glUniform1f(uniGlyphClampMin, clamp_glyph_min);
  glUniform1f(uniGlyphClampMax, clamp_glyph_max);
  glUniform1i(uniSmokeClamping, clamp_smoke_cmap);
  glUniform1f(uniSmokeClampMin, clamp_smoke_min);
  glUniform1f(uniSmokeClampMax, clamp_smoke_max);
  glUniform1i(uniPhongHeightPlot, heightplot);
  cMapShaderProg->release();
  updateUniformsRequired = false;
}

void MainView::clearArrays()
{
    clearGridArrays();
    clearLineArrays();
    clearIsolineArrays();
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
    glyphNormals.clear();
    glyphNormals.squeeze();
    glyphIndices.clear();
    glyphIndices.squeeze();
}

void MainView::clearIsolineArrays()
{
    isolineCoords.clear();
    isolineCoords.squeeze();
    isolineColours.clear();
    isolineColours.squeeze();
    isolineIndices.clear();
    isolineIndices.squeeze();
}

void MainView::initializeGL() {

  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  QString glVersion;
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);
  glPointSize(16);

  createShaderPrograms();
  createBuffers();

  // Parameter initialization
  simulation = Simulation(DIM);
  marchingSquare = MarchingSquare();

  srand(static_cast<unsigned int>(time(nullptr))); // initialize seed for rng

  scale_maxvals_rho.reserve(scale_smoke_window);
  scale_minvals_rho.reserve(scale_smoke_window);
  scale_maxvals_vnorm.reserve(scale_smoke_window);
  scale_minvals_vnorm.reserve(scale_smoke_window);
  scale_maxvals_fnorm.reserve(scale_smoke_window);
  scale_minvals_fnorm.reserve(scale_smoke_window);
  scale_maxvals_rho.resize(scale_smoke_window);
  scale_minvals_rho.resize(scale_smoke_window);
  scale_maxvals_vnorm.resize(scale_smoke_window);
  scale_minvals_vnorm.resize(scale_smoke_window);
  scale_maxvals_fnorm.resize(scale_smoke_window);
  scale_minvals_fnorm.resize(scale_smoke_window);

  do_one_simulation_step();
  this->startTimer(0);
  updateMatrices();
  is_initialized = true;
}

void MainView::do_one_simulation_step(void)
{
    simulation.set_forces(DIM);
    simulation.solve(DIM, static_cast<double>(visc), dt);
    simulation.diffuse_matter(DIM, dt);
    simulation.divergenceV(DIM);
    simulation.divergenceF(DIM);
    try
    {
        if(draw_smoke) {
            updateBuffers();
        }
        if(draw_vecs) {
            updateGlyphs();
        }
        if(draw_isolines) {
            updateIsolines();
        }
    }
    catch (std::exception e)
    {
        qDebug() << "updating buffers failed";
        qDebug() << e.what();
    }
    if (is_initialized)
    {
        if (!clamp_smoke_cmap)
        {
            if (smoke_var == RHO)
            {
                clamp_smoke_max = findMean(scale_maxvals_rho);
                clamp_smoke_min = findMean(scale_minvals_rho);
            }
            else if (smoke_var == V)
            {
                clamp_smoke_max = findMean(scale_maxvals_vnorm);
                clamp_smoke_min = findMean(scale_minvals_vnorm);
            }
            else if (smoke_var == F)
            {
                clamp_smoke_max = findMean(scale_maxvals_fnorm);
                clamp_smoke_min = findMean(scale_minvals_fnorm);
            }
            updateUniformsRequired = true;
        }
        if (scale_smoke_cnt < scale_smoke_window - 1)
        {
            scale_smoke_cnt++;
        }
        else
        {
            scale_smoke_cnt = 0;
        }

        if (!clamp_glyph_cmap)
        {
            if (glyph_var == RHO)
            {
                clamp_glyph_max = findMean(scale_maxvals_rho);
                clamp_glyph_min = findMean(scale_minvals_rho);
            }
            else if (glyph_var == V)
            {
                clamp_glyph_max = findMean(scale_maxvals_vnorm);
                clamp_glyph_min = findMean(scale_minvals_vnorm);
            }
            else if (glyph_var == F)
            {
                clamp_glyph_max = findMean(scale_maxvals_fnorm);
                clamp_glyph_min = findMean(scale_minvals_fnorm);
            }
            updateUniformsRequired = true;
        }
    }
}

void MainView::paintGL() {
  try
    {
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (updateMatricesRequired)
      {
          updateMatrices();
      }
      if (updateUniformsRequired)
      {
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
          if (glyphs3D)
          {
              glUniform1i(uniPhong, true);
              glDrawElements(GL_TRIANGLES, glyphIndices.size(), GL_UNSIGNED_SHORT, nullptr);
          }
          else
          {
              glDrawElements(GL_LINES, glyphIndices.size(), GL_UNSIGNED_SHORT, nullptr);
          }
          glBindVertexArray(0);
      }
      if (draw_isolines)
      {
          glUniform1i(uniPhong, false);
          glBindVertexArray(isolinesVAO);
          glDrawElements(GL_LINES, isolineIndices.size(), GL_UNSIGNED_SHORT, nullptr);
          glBindVertexArray(0);
      }
      mainShaderProg->release();
    }
    catch (std::exception e)
    {
        qDebug() << "update failed";
        qDebug() << e.what();
    }
}

void MainView::resizeGL(int newWidth, int newHeight) {
  updateMatrices();
}

void MainView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QMatrix4x4 tfMatrix;
        if (heightplot)
        {
            float x_n = (2.0 * event->pos().x()) / (float)width() - 1.0;
            float y_n = (2.0 * event->pos().y()) / (float)height() - 1.0;
            tfMatrix.setToIdentity();
            tfMatrix.scale(1.0/hPlot_zoom,1.0/hPlot_zoom,1.0/hPlot_zoom);
            //tfMatrix.rotate(-45.0, QVector3D(1.0,0.0,0.0));
            tfMatrix.rotate(hPlot_zAngle, QVector3D(0.0,0.0,1.0));
            QVector4D tf = tfMatrix * QVector4D(x_n, y_n, 0.0, 1.0);
            float x_s = (tf.x() + 1.0)/2.0 * width();
            float y_s = (tf.y() + 1.0)/2.0 * height();
            simulation.drag(DIM, width(), height(), x_s, y_s);
        }
        else
        {
            simulation.drag(DIM, width(), height(), event->pos().x(), event->pos().y());
        }
    }
    if (event->buttons() & Qt::RightButton)
    {
        if (heightplot)
        {
            QVector3D v = getArcBallVector(lastpos.x(), lastpos.y());
            QVector3D u = getArcBallVector(event->x(), event->y());

            float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(u,v)));

            QVector3D rotAxis = QVector3D::crossProduct(v,u);
            if (QVector3D::dotProduct(rotAxis, QVector3D(0.0, 0.0, 1.0)) > 0)
            {
                angle = -angle;
            }

            hPlot_zAngle = hPlot_zAngle - 180*angle/M_PI;
            updateMatricesRequired = true;
        }
    }
    lastpos = event->pos();

}

QVector3D MainView::getArcBallVector(int x, int y)
{
   QVector3D pt = QVector3D(2.0 * x / width() - 1.0, 2.0 * y / height() - 1.0 , 0);
   pt.setY(pt.y() * -1);

   // compute z-coordinates

   float xySquared = pt.x() * pt.x() + pt.y() * pt.y();

   if(xySquared <= 1.0)

       pt.setZ(std::sqrt(1.0 - xySquared));
   else
       pt.normalize();

   return pt;

}

void MainView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QMatrix4x4 tfMatrix;
        if (heightplot)
        {
            float x_n = (2.0 * event->pos().x()) / (float)width() - 1.0;
            float y_n = (2.0 * event->pos().y()) / (float)height() - 1.0;
            tfMatrix.setToIdentity();
            tfMatrix.scale(1.0/hPlot_zoom,1.0/hPlot_zoom,1.0/hPlot_zoom);
            //tfMatrix.rotate(-45.0, QVector3D(1.0,0.0,0.0));
            tfMatrix.rotate(hPlot_zAngle, QVector3D(0.0,0.0,1.0));
            QVector4D tf = tfMatrix * QVector4D(x_n, y_n, 0.0, 1.0);
            float x_s = (tf.x() + 1.0)/2.0 * width();
            float y_s = (tf.y() + 1.0)/2.0 * height();
            simulation.drag(DIM, width(), height(), x_s, y_s);
        }
        else
        {
            simulation.drag(DIM, width(), height(), event->pos().x(), event->pos().y());
        }
    }
    if (event->buttons() & Qt::RightButton)
    {
        if (heightplot)
        {
            QVector3D v = getArcBallVector(lastpos.x(), lastpos.y());
            QVector3D u = getArcBallVector(event->x(), event->y());

            float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(u,v)));

            QVector3D rotAxis = QVector3D::crossProduct(v,u);
            if (QVector3D::dotProduct(rotAxis, QVector3D(0.0, 0.0, 1.0)) > 0)
            {
                angle = -angle;
            }

            hPlot_zAngle = hPlot_zAngle - 180*angle/M_PI;
            updateMatricesRequired = true;
        }
    }
    lastpos = event->pos();
}

void MainView::wheelEvent(QWheelEvent *e)
{
    if (heightplot)
    {
        float d = e->delta() / 1200.0;
        hPlot_zoom = std::min(0.8, std::max(0.25, (double)(hPlot_zoom+d)));
        updateMatrices();
    }
}

void MainView::timerEvent(QTimerEvent *e)
{
    if (!frozen)
    {
        try
        {
            do_one_simulation_step();
            this->update();
        }
        catch (std::exception e)
        {
            qDebug() << "tick failed";
            qDebug() << e.what();
        }

    }
}

float MainView::glyph_interpolation(float x_pct, float y_pct, fftw_real* mat)
{
    float x_pos_grid, y_pos_grid, x_under, y_under, x_over, y_over;
    float tx, ty, q11, q12, q21, q22, inter;

    x_pos_grid = x_pct * (DIM-1);
    y_pos_grid = y_pct * (DIM-1);

    x_under = floorf(x_pos_grid);
    y_under = floorf(y_pos_grid);

    x_over = ceilf(x_pos_grid);
    y_over = ceilf(y_pos_grid);

    tx = (x_pos_grid - x_under);
    ty = (y_pos_grid - y_under);

    q11 = static_cast<float>(mat[(static_cast<int>(y_under) - 0) * DIM + (static_cast<int>(x_under) - 0)]);
    q12 = static_cast<float>(mat[(static_cast<int>(y_under) - 0) * DIM + (static_cast<int>(x_over) - 0)]);
    q21 = static_cast<float>(mat[(static_cast<int>(y_over) - 0) * DIM + (static_cast<int>(x_under) - 0)]);
    q22 = static_cast<float>(mat[(static_cast<int>(y_over) - 0) * DIM + (static_cast<int>(x_over) - 0)]);
    inter = (1 - tx) * (1 - ty) * q11 + tx * (1 - ty) * q12 +
            (1 - tx) * ty * q21 + tx * ty * q22;

    return inter;
}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
  qDebug() << " → Log:" << Message;
}
