#version 410
// Fragment shader

layout (location = 1) in vec3 vertcolour_in;
layout (location = 2) in float vertattrib_in;

uniform int levels;
uniform int mode;
uniform int clamp;
uniform float minval;
uniform float maxval;

out vec4 fColor;

void main() {

  float value = vertattrib_in;
  value *= levels;
  value = int(value);
  value/= levels;

  if (clamp == 1)
  {
      if (value<minval) value=minval; if (value>maxval) value=maxval;
      value = value/(maxval-minval);
  }

  if (mode == 0) //grayscale
  {
      fColor = vec4(value, value, value, 1.0);
  }
  else if (mode == 1) //rainbow
  {
      float dx=0.8;
      value = (6-2*dx)*value+dx;
      float R = max(0.0,(3-abs(value-4)-abs(value-5))/2);
      float G = max(0.0,(4-abs(value-2)-abs(value-4))/2);
      float B = max(0.0,(3-abs(value-1)-abs(value-2))/2);

      fColor = vec4(R, G, B, 1.0);
  }
  else if (mode == 2) //heat map
  {
      int NUM_COLORS = 3;
      mat3 color;
      color[0] = vec3(0.0,0.0,0.0);
      color[1] = vec3(1.0,0.0,0.0);
      color[2] = vec3(1.0,1.0,1.0);
        // A static array of 3 colors:  (black, red, white) using {r,g,b} for each.

      int idx1;        // |-- Our desired color will be between these two indexes in "color".
      int idx2;        // |
      float fractBetween = 0.0;  // Fraction between "idx1" and "idx2" where our value is.

      if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
      else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
      else
      {
        value = value * (NUM_COLORS-1.0);        // Will multiply value by 3.
        idx1  = int(floor(value));                  // Our desired color will be after this index.
        idx2  = idx1+1;                        // ... and before this index (inclusive).
        fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
      }

      float red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
      float green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
      float blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];

      fColor = vec4(red, green, blue, 1.0);
  }
  else //unknown colormap
  {
      fColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
