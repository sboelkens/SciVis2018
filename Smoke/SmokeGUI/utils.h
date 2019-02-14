#ifndef UTILS_H
#define UTILS_H

//#include <algorithm>
#include <QVector3D>

const int COLOR_BLACKWHITE = 0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW    = 1;
const int COLOR_BANDS      = 2;

QVector3D rainbow(double val);
QVector3D set_colormap(float val, int scalar_col);

#endif // UTILS_H
