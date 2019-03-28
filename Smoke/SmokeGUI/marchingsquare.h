#ifndef MARCHINGSQUARE_H
#define MARCHINGSQUARE_H

#include <QString>
#include <QVector4D>
#include <QDebug>


const int SIDE_LEFT = 0;
const int SIDE_UP    = 1;
const int SIDE_RIGHT    = 2;
const int SIDE_DOWN    = 2;


struct point {
    float x, y;
};
typedef struct point PStruct;

class MarchingSquare
{

public:
    MarchingSquare();
    MarchingSquare(int n);

    QVector4D* calcIsoline(double *rho, int n, double rhoVal);
    QVector4D lineFromBinary(double l, double r, double dl, double dr, double rhoVal, QString code);
    point interpolateSide(double l, double r, double dl, double dr, double rhoVal, int side);
private:
    QVector4D *isoline;

    void init(int n);

};

#endif // MARCHINGSQUARE_H
