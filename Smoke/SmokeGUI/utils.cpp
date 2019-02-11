#include "utils.h"

QVector3D rainbow(float value) {
    const float dx=0.8;
    if (value<0) value=0; if (value>1) value=1;
    value = (6-2*dx)*value+dx;
    float R = std::max(0.0,(3-fabs(value-4)-fabs(value-5))/2);
    float G = std::max(0.0,(4-fabs(value-2)-fabs(value-4))/2);
    float B = std::max(0.0,(3-fabs(value-1)-fabs(value-2))/2);

    return QVector3D(R, G, B);
}
