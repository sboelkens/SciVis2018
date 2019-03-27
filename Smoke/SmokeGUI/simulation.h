#ifndef SIMULATION_H
#define SIMULATION_H

#include <math.h>
#include <rfftw.h>
#include <QString>
#include <QDebug>


struct dir2d {
    fftw_real *x, *y;
};

typedef struct dir2d Struct;

class Simulation
{
public:
    Simulation();
    Simulation(int n);

    fftw_real* getRho();
    fftw_real* getVx();
    fftw_real* getVy();
    Struct getV();
    fftw_real* getFx();
    fftw_real* getFy();
    Struct getF();
    fftw_real* getDivV();
    fftw_real* getDivF();
    fftw_real* getIsoline();

    void solve(int n, fftw_real visc, fftw_real dt);
    void diffuse_matter(int n, fftw_real dt);
    void set_forces(int n);
    void divergenceV(int n);
    void divergenceF(int n);
    void calcIsoline(int n, fftw_real rhoVal);
    void drag(int n, int winWidth, int winHeight, int mx, int my);


private:
    //--- SIMULATION PARAMETERS ------------------------------------------------------------------------
    fftw_real *vx, *vy;             //(vx,vy)   = velocity field at the current moment
    fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
    fftw_real *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
    fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
    rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization
    fftw_real *divV, *divF;
    fftw_real *isoline;

    void init_simulation(int n);
    void FFT(int direction, void* vx);
    int clamp(float x);
    fftw_real divergence(int j, int i, int n, fftw_real *x, fftw_real *y);
};

#endif // SIMULATION_H
