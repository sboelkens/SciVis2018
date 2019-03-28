#ifndef MARCHINGSQUARE_H
#define MARCHINGSQUARE_H

#include <QString>
#include <QVector2D>
#include <QVector4D>
#include <QDebug>


const int SIDE_LEFT = 0;
const int SIDE_UP    = 1;
const int SIDE_RIGHT    = 2;
const int SIDE_DOWN    = 3;


struct point {
    float x, y;
};
typedef struct point PStruct;

class MarchingSquare
{

public:
    MarchingSquare();

    QVector<QVector2D> calcIsoline(double *rho, int n, double isoline_value);
    void lineFromBinary(double l, double r, double dl, double dr, QString code, double px, double py);
    QVector2D interpolateSide(double l, double r, double dl, double dr, int side, double px, double py);
private:
    QVector<QVector2D> isolines;
    double wn, hn, rhoVal;
};

#endif // MARCHINGSQUARE_H
