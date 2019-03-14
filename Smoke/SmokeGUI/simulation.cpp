#include "simulation.h"

Simulation::Simulation()
{
    init_simulation(50);
}

Simulation::Simulation(int n)
{
    init_simulation(n);
}

Simulation::Simulation(int h, int v)
{
    init_simulation(h, v);
}

void Simulation::init_simulation(int n)
{
    init_simulation(n, n);
}

void Simulation::init_simulation(int h, int v)
{
    int i; size_t dim;

    dim     = h * 2*(v/2+1)*sizeof(fftw_real);        //Allocate data structures
    vx       = (fftw_real*) malloc(dim);
    vy       = (fftw_real*) malloc(dim);
    vx0      = (fftw_real*) malloc(dim);
    vy0      = (fftw_real*) malloc(dim);
    dim     = h * v * sizeof(fftw_real);
    fx      = (fftw_real*) malloc(dim);
    fy      = (fftw_real*) malloc(dim);
    rho     = (fftw_real*) malloc(dim);
    rho0    = (fftw_real*) malloc(dim);
    plan_rc = rfftw2d_create_plan(h, v, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
    plan_cr = rfftw2d_create_plan(h, v, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    for (i = 0; i < h * v; i++)                      //Initialize data structures to 0
    { vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = 0.0f; }
}

//FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
//     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform
void Simulation::FFT(int direction, void* vx)
{
    if(direction==1) rfftwnd_one_real_to_complex(plan_rc, (fftw_real*)vx, (fftw_complex*)vx);
    else             rfftwnd_one_complex_to_real(plan_cr, (fftw_complex*)vx, (fftw_real*)vx);
}

int Simulation::clamp(float x)
{ return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); }

//solve: Solve (compute) one step of the fluid flow simulation
Struct Simulation::solve(int n, fftw_real visc, fftw_real dt)
{
    return this->solve(n, n, visc, dt);
}


Struct Simulation::solve(int dim_h, int dim_v, fftw_real visc, fftw_real dt)
{
    fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;


    for (i=0;i<dim_h;i++)
       for (j=0;j<dim_v;j++)
       { vx[i+dim_h*j] = 0; vy[i+dim_h*j] = 0; }

    Struct velocity2d;
    velocity2d.x = vx;
    velocity2d.y = vy;

    return velocity2d;


    for (i=0;i<dim_h*dim_v;i++)
    { vx[i] += dt*vx0[i]; vx0[i] = vx[i]; vy[i] += dt*vy0[i]; vy0[i] = vy[i]; }

    for ( x=0.5f/dim_h,i=0 ; i<dim_h ; i++,x+=1.0f/dim_h )
       for ( y=0.5f/dim_v,j=0 ; j<dim_v ; j++,y+=1.0f/dim_v )
       {
          x0 = dim_h*(x-dt*vx0[i+dim_h*j])-0.5f;
          y0 = dim_v*(y-dt*vy0[i+dim_h*j])-0.5f;
          i0 = clamp(x0); s = x0-i0;
          i0 = (dim_h+(i0%dim_h))%dim_h;
          i1 = (i0+1)%dim_h;
          j0 = clamp(y0); t = y0-j0;
          j0 = (dim_v+(j0%dim_v))%dim_v;
          j1 = (j0+1)%dim_v;
          vx[i+dim_h*j] = (1-s)*((1-t)*vx0[i0+dim_h*j0]+t*vx0[i0+dim_h*j1])+s*((1-t)*vx0[i1+dim_h*j0]+t*vx0[i1+dim_h*j1]);
          vy[i+dim_h*j] = (1-s)*((1-t)*vy0[i0+dim_h*j0]+t*vy0[i0+dim_h*j1])+s*((1-t)*vy0[i1+dim_h*j0]+t*vy0[i1+dim_h*j1]);
       }

    for(i=0; i<dim_h; i++) {
      for(j=0; j<dim_v; j++)
      {
//          qDebug() << "pos" << (i+dim_h*j) << "pos0" << (i+(dim_h+2)*j);
          vx0[i+(dim_h+2)*j] = vx[i+dim_h*j]; vy0[i+(dim_h+2)*j] = vy[i+dim_h*j];
      }
    }

    FFT(1,vx0);
    FFT(1,vy0);

    for (i=0;i<=dim_h;i+=2)
    {
       x = 0.5f*i;
       for (j=0;j<dim_v;j++)
       {
          y = j<=dim_v/2 ? (fftw_real)j : (fftw_real)j-dim_v;
          r = x*x+y*y;
          if ( r==0.0f ) continue;
          f = (fftw_real)exp(-r*dt*visc);
          U[0] = vx0[i  +(dim_h+2)*j]; V[0] = vy0[i  +(dim_h+2)*j];
          U[1] = vx0[i+1+(dim_h+2)*j]; V[1] = vy0[i+1+(dim_h+2)*j];

          vx0[i  +(dim_h+2)*j] = f*((1-x*x/r)*U[0]     -x*y/r *V[0]);
          vx0[i+1+(dim_h+2)*j] = f*((1-x*x/r)*U[1]     -x*y/r *V[1]);
          vy0[i+  (dim_h+2)*j] = f*(  -y*x/r *U[0] + (1-y*y/r)*V[0]);
          vy0[i+1+(dim_h+2)*j] = f*(  -y*x/r *U[1] + (1-y*y/r)*V[1]);
       }
    }

    FFT(-1,vx0);
    FFT(-1,vy0);

    f = 1.0/(dim_h*dim_v);
    for (i=0;i<dim_h;i++)
       for (j=0;j<dim_v;j++)
       { vx[i+dim_h*j] = f*vx0[i+(dim_h+2)*j]; vy[i+dim_h*j] = f*vy0[i+(dim_h+2)*j]; }

//    Struct velocity2d;
    velocity2d.x = vx;
    velocity2d.y = vy;

    return velocity2d;
}

fftw_real* Simulation::diffuse_matter(int n, fftw_real dt)
{
    return this->diffuse_matter(n, n, dt);
}
// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
fftw_real* Simulation::diffuse_matter(int dim_h, int dim_v, fftw_real dt)
{
    fftw_real x, y, x0, y0, s, t;
    int i, j, i0, j0, i1, j1;

    for ( x=0.5f/dim_h,i=0 ; i<dim_h ; i++,x+=1.0f/dim_h )
        for ( y=0.5f/dim_v,j=0 ; j<dim_v ; j++,y+=1.0f/dim_v )
        {
            x0 = dim_h*(x-dt*vx[i+dim_h*j])-0.5f;
            y0 = dim_v*(y-dt*vy[i+dim_h*j])-0.5f;
            i0 = clamp(x0);
            s = x0-i0;
            i0 = (dim_h+(i0%dim_h))%dim_h;
            i1 = (i0+1)%dim_h;
            j0 = clamp(y0);
            t = y0-j0;
            j0 = (dim_v+(j0%dim_v))%dim_v;
            j1 = (j0+1)%dim_v;

            rho[i+dim_h*j] = (1-s)*((1-t)*rho0[i0+dim_h*j0]+t*rho0[i0+dim_h*j1])+s*((1-t)*rho0[i1+dim_h*j0]+t*rho0[i1+dim_h*j1]);
        }
    return rho;
}

void Simulation::set_forces(int n)
{
    this->set_forces(n, n);
}
//set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
//            Also dampen forces and matter density to get a stable simulation.
void Simulation::set_forces(int h, int v)
{
    int i;
    for (i = 0; i < h * v; i++)
    {
        rho0[i]  = 0.995 * rho[i];
        fx[i] *= 0.85;
        fy[i] *= 0.85;
        vx0[i]    = fx[i];
        vy0[i]    = fy[i];
    }
}

void Simulation::drag(int n, int winWidth, int winHeight, int mx, int my)
{
    this->drag(n, n, winWidth, winHeight, mx, my);
}

void Simulation::drag(int dim_h, int dim_v, int winWidth, int winHeight, int mx, int my)
{
    int xi,yi,X,Y; double  dx, dy, len;
    static int lmx=0,lmy=0;				//remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int)clamp((double)(dim_h + 1) * ((double)mx / (double)winWidth));
    yi = (int)clamp((double)(dim_v + 1) * ((double)(winHeight - my) / (double)winHeight));

    X = xi; Y = yi;

    if (X > (dim_h - 1))  X = dim_h - 1; if (Y > (dim_v - 1))  Y = dim_v - 1;
    if (X < 0) X = 0; if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx; dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
    fx[Y * dim_h + X] += dx;
    fy[Y * dim_h + X] += dy;
    rho[Y * dim_h + X] = 10.0f;
    lmx = mx; lmy = my;
}

Struct Simulation::get_force()
{
    Struct force2d;
    force2d.x = fx;
    force2d.y = fy;

    return force2d;
}

