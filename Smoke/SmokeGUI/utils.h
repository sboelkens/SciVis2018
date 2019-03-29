#ifndef UTILS_H
#define UTILS_H

#include <QVector>
#include <QVector3D>

const int COLOR_BLACKWHITE = 0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW    = 1;
const int COLOR_HEATMAP    = 2;
const int COLOR_BLUE_RED   = 3;

const int RHO  = 0;
const int V    = 1;
const int F    = 2;
const int DIVV = 3;
const int DIVF = 4;

QVector3D rainbow(double val);
QVector3D set_colormap(float val, int scalar_col, int nlevels = 7);
QVector3D direction_to_color(float x, float y, int method);
QVector3D getBlueRedColor(float value);
QVector3D getHeatMapColor(float value);


float findMean(QVector<float>);

#endif // UTILS_H
