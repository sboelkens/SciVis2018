#include "marchingsquare.h"

MarchingSquare::MarchingSquare()
{
}


MarchingSquare::MarchingSquare(int n)
{
}

void MarchingSquare::init(int n)
{
}

QVector4D* MarchingSquare::calcIsoline(double *rho, int n, double rhoVal)
{
    int dim     = n * n * sizeof(QVector4D);
    isoline      = (QVector4D*) malloc(dim);
    for (int i = 0; i < n * n; i++)                      //Initialize data structures to 0
    { isoline[i] = QVector4D(0,0,0,0); }

    for (int j = 0; j < (n-1); j++)            //draw smoke
    {
        for (int i = 0; i < (n-1); i++)
        {
            int idx = (j*n) + i;
            int idxR = (j * n) + (i+1);
            int idxD = ((j+1) * n) + i;
            int idxDR = ((j+1) * n) + (i+1);

            QString code = "";
            code += (rho[idx] > rhoVal) ? "1" : "0";
            code += (rho[idxR] > rhoVal) ? "1" : "0";
            code += (rho[idxD] > rhoVal) ? "1" : "0";
            code += (rho[idxDR] > rhoVal) ? "1" : "0";

            isoline[idx] = lineFromBinary(rho[idx], rho[idxR], rho[idxD], rho[idxDR], rhoVal, code);
        }
    }

    return isoline;
}

QVector4D MarchingSquare::lineFromBinary(double l, double r, double dl, double dr, double rhoVal, QString code)
{
    PStruct point1;
    PStruct point2;
    if(code == "0000"){
        return QVector4D(0, 0, 0, 0);
    }
    if(code == "0001"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_LEFT);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_DOWN);
    }
    if(code == "0010"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_DOWN);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_RIGHT);
    }
    if(code == "0011"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_LEFT);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_RIGHT);
    }
    if(code == "0100"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_UP);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_RIGHT);
    }
    if(code == "0101"){
        return QVector4D(0, 0, 0, 0);
    }
    if(code == "0110"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_UP);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_DOWN);
    }
    if(code == "0111"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_LEFT);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_UP);
    }
    if(code == "1000"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_LEFT);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_UP);
    }
    if(code == "1001"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_UP);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_DOWN);
    }
    if(code == "1010"){
        return QVector4D(0, 0, 0, 0);
    }
    if(code == "1011"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_UP);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_RIGHT);
    }
    if(code == "1100"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_LEFT);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_RIGHT);
    }
    if(code == "1101"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_DOWN);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_RIGHT);
    }
    if(code == "1110"){
        point1 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_LEFT);
        point2 = interpolateSide(l, r, dl, dr, rhoVal, SIDE_DOWN);
    }
    if(code == "1111"){
        return QVector4D(0, 0, 0, 0);
    }

    return QVector4D(point1.x, point1.y, point2.x, point2.y);
}

point MarchingSquare::interpolateSide(double l, double r, double dl, double dr, double rhoVal, int side)
{
    PStruct point;
    if(side == SIDE_LEFT) {
        point.x = 0;
        point.y = static_cast<float>(abs(l-rhoVal) / abs(l-dl));
    } else if (side == SIDE_UP) {
        point.x = static_cast<float>(abs(l-rhoVal) / abs(l-r));
        point.y = 1;
    } else if (side == SIDE_RIGHT) {
        point.x = 1;
        point.y = static_cast<float>(abs(r-rhoVal) / abs(r-dr));
    } else if (side == SIDE_DOWN) {
        point.x = static_cast<float>(abs(dl-rhoVal) / abs(dl-dr));
        point.y = 0;
    }
    return point;
}
