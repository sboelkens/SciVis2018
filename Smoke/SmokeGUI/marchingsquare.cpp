#include "marchingsquare.h"

MarchingSquare::MarchingSquare()
{
}

QVector<QVector2D> MarchingSquare::calcIsoline(double *rho, int n, double isoline_value)
{
    isolines.clear();
    isolines.squeeze();

    wn = 2.0 / static_cast<double>(n + 1);   // Grid cell width
    hn = 2.0 / static_cast<double>(n + 1);

    double px, py;

    rhoVal = isoline_value;

    for (int j = 0; j < (n-1); j++)
    {
        for (int i = 0; i < (n-1); i++)
        {
            int idx = (j*n) + i;
            int idxR = (j * n) + (i+1);
            int idxU = ((j+1) * n) + i;
            int idxUR = ((j+1) * n) + (i+1);

            QString code = "";
            code += (rho[idxU] > rhoVal) ? "1" : "0";
            code += (rho[idxUR] > rhoVal) ? "1" : "0";
            code += (rho[idxR] > rhoVal) ? "1" : "0";
            code += (rho[idx] > rhoVal) ? "1" : "0";

            px = wn + static_cast<double>(i) * wn - 1.0;
            py = hn + static_cast<double>(j) * hn - 1.0;

            lineFromBinary(rho[idx], rho[idxR], rho[idxU], rho[idxUR], code, px, py);
        }
    }

    return isolines;
}

void MarchingSquare::lineFromBinary(double dl, double dr, double ul, double ur, QString code, double px, double py)
{
    if(code == "0000"){
        return;
    }
    if(code == "0001"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
    }
    if(code == "0010"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
    }
    if(code == "0011"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
    }
    if(code == "0100"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
    }
    if(code == "0101"){
        double avg = (dl + dr + ul + ur) / 4;
        if(avg < rhoVal) {
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
        } else {
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
        }
    }
    if(code == "0110"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
    }
    if(code == "0111"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
    }
    if(code == "1000"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
    }
    if(code == "1001"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
    }
    if(code == "1010"){
        double avg = (dl + dr + ul + ur) / 4;
        if(avg < rhoVal) {
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
        } else {
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
            isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
        }
    }
    if(code == "1011"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_UP, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
    }
    if(code == "1100"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
    }
    if(code == "1101"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_RIGHT, px, py));
    }
    if(code == "1110"){
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_LEFT, px, py));
        isolines.append(interpolateSide(dl, dr, ul, ur, SIDE_DOWN, px, py));
    }
    if(code == "1111"){
        return;
    }
}

QVector2D MarchingSquare::interpolateSide(double dl, double dr, double ul, double ur, int side, double px, double py)
{
    PStruct point;
    if(side == SIDE_LEFT) {
        point.x = 0;
        point.y = static_cast<float>(abs(dl-rhoVal) / abs(dl-ul));
    } else if (side == SIDE_UP) {
        point.x = static_cast<float>(abs(ul-rhoVal) / abs(ul-ur));
        point.y = 1;
    } else if (side == SIDE_RIGHT) {
        point.x = 1;
        point.y = static_cast<float>(abs(dr-rhoVal) / abs(dr-ur));
    } else if (side == SIDE_DOWN) {
        point.x = static_cast<float>(abs(dl-rhoVal) / abs(dl-dr));
        point.y = 0;
    }
    float gridX = static_cast<float>(px) + (point.x * static_cast<float>(wn));
    float gridY = static_cast<float>(py) + (point.y * static_cast<float>(hn));
    return QVector2D(gridX, gridY);
}
