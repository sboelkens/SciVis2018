#ifndef MARCHINGSQUARE_H
#define MARCHINGSQUARE_H

#include <QString>
#include <QVector4D>


struct point {
    float x, y;
};
typedef struct point PStruct;

class MarchingSquare
{

public:
    const int SIDE_LEFT = 0;
    const int SIDE_UP    = 1;
    const int SIDE_RIGHT    = 2;
    const int SIDE_DOWN    = 2;

    MarchingSquare();
    QVector4D* calcIsoline(double *rho, int n, double rhoVal);
    QVector4D lineFromBinary(double l, double r, double dl, double dr, double rhoVal, QString code);
    point interpolateSide(double l, double r, double dl, double dr, double rhoVal, int side);
private:

};

#endif // MARCHINGSQUARE_H
