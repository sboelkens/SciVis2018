#include "utils.h"

QVector3D rainbow(double value) {
    const double dx=0.8;
    if (value<0) value=0; if (value>1) value=1;
    value = (6-2*dx)*value+dx;
    float R = std::max(0.0,(3-fabs(value-4)-fabs(value-5))/2);
    float G = std::max(0.0,(4-fabs(value-2)-fabs(value-4))/2);
    float B = std::max(0.0,(3-fabs(value-1)-fabs(value-2))/2);

    return QVector3D(R, G, B);
}

QVector3D set_colormap(float vy, int scalar_col)
{
   if (scalar_col==COLOR_BLACKWHITE)
       return QVector3D(vy, vy, vy);
   else if (scalar_col==COLOR_RAINBOW)
       return rainbow(vy);
   else if (scalar_col==COLOR_BANDS)
       {
          const int NLEVELS = 7;
          vy *= NLEVELS; vy = (int)(vy); vy/= NLEVELS;
          return rainbow(vy);
       }
   return QVector3D(0.0, 0.0, 0.0);
}
