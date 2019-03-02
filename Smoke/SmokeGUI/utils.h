#ifndef UTILS_H
#define UTILS_H

//#include <algorithm>
#include <QVector3D>

const int COLOR_BLACKWHITE = 0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW    = 1;
const int COLOR_HEATMAP    = 2;

const int SMOKE_RHO        = 0;
const int SMOKE_MAG_V      = 1;
const int SMOKE_MAG_F      = 2;

QVector3D rainbow(double val);
QVector3D set_colormap(float val, int scalar_col, int nlevels = 7);
QVector3D direction_to_color(float x, float y, int method);
QVector3D getHeatMapColor(float value);

#endif // UTILS_H
