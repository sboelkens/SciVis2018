#ifndef FLUIDS_H
#define FLUIDS_H

#include <rfftw.h>              //the numerical simulation FFTW library
//#include <GL/glut.h>            //the GLUT graphics library
#include <stdio.h>

void init_simulation(int n);
void FFT(int direction,void* vx);
int clamp(float x);
void solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt);
void diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt);
void set_forces(void);
void do_one_simulation_step(void);
void rainbow(float value,float* R,float* G,float* B);
void set_colormap(float vy);
void direction_to_color(float x, float y, int method);
void visualize(void);

void drag(int mx, int my);


#endif // FLUIDS_H
