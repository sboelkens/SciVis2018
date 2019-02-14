#ifndef SIMULATION_H
#define SIMULATION_H

#include <math.h>
#include <rfftw.h>


struct velocity2d {
    fftw_real *vx, *vy;
};

typedef struct velocity2d Struct;

class Simulation
{
public:
    Simulation();
    Simulation(int n);

    Struct solve(int n, fftw_real visc, fftw_real dt);
    fftw_real* diffuse_matter(int n, fftw_real dt);
    void set_forces(int n);
    void drag(int n, int winWidth, int winHeight, int mx, int my);


private:
    //--- SIMULATION PARAMETERS ------------------------------------------------------------------------
    fftw_real *vx, *vy;             //(vx,vy)   = velocity field at the current moment
    fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
    fftw_real *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
    fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
    rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization

    void init_simulation(int n);
    void FFT(int direction, void* vx);
    int clamp(float x);
};

#endif // SIMULATION_H
