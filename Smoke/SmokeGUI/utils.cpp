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

QVector3D set_colormap(float vy, int scalar_col, int nlevels)
{
   vy *= nlevels; vy = (int)(vy); vy/= nlevels;

   if (scalar_col==COLOR_BLACKWHITE)
       return QVector3D(vy, vy, vy);
   else if (scalar_col==COLOR_RAINBOW)
       return rainbow(vy);
   else if (scalar_col==COLOR_HEATMAP)
       return getHeatMapColor(vy);
   return QVector3D(0.0, 0.0, 0.0);
}

QVector3D direction_to_color(float x, float y, int method)
{
    float r, g, b, f;
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
    {
        r = g = b = 1;
    }
    return QVector3D(r, g, b);
}

QVector3D getHeatMapColor(float value)
{
  const int NUM_COLORS = 3;
  static float color[NUM_COLORS][3] = { {0,0,0}, {1,0,0}, {1,1,1} };
    // A static array of 3 colors:  (black, red, white) using {r,g,b} for each.

  int idx1;        // |-- Our desired color will be between these two indexes in "color".
  int idx2;        // |
  float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

  if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
  else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
  else
  {
    value = value * (NUM_COLORS-1);        // Will multiply value by 3.
    idx1  = floor(value);                  // Our desired color will be after this index.
    idx2  = idx1+1;                        // ... and before this index (inclusive).
    fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
  }

  float red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
  float green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
  float blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];

  return QVector3D(red, green, blue);
}
