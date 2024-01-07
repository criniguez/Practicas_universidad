#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef float real;
#define SOFTENING_SQUARED  0.001

// Data structures real3 and real4
typedef struct { real x, y, z; }    real3;
typedef struct { real x, y, z, w; } real4;

// Variable used to generate pseudo-random numbers
unsigned int seed;
unsigned const int RAND_MX= 1<<(32-13);

// Function to generate pseudo-random numbers
unsigned int myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>13);
}

real3 Interaction(real4 iPos, real4 jPos)
{
  real rx, ry, rz, distSqr, s;

  rx = jPos.x - iPos.x;  
  ry = jPos.y - iPos.y;  
  rz = jPos.z - iPos.z;

  distSqr = rx*rx+ry*ry+rz*rz;

  if (distSqr < SOFTENING_SQUARED) s = jPos.w / powf(SOFTENING_SQUARED,1.5);
  else                             s = jPos.w / powf(distSqr,1.5);

  real3 f;
  f.x = rx * s;  
  f.y = ry * s; 
  f.z = rz * s;

  return f;
}


void integrateForces (real3 * force, real4 *Puntos, real4 *Centroides, int *PtsToCentroid, int *Npts, int N, int C)
{
  real4 cm;
  int i, j;

  for (i=0; i<N; i++)
  {
    real fx=0, fy=0, fz=0;
 
    real4 P= Puntos[i];
    int MyCentroid = PtsToCentroid[i];
      
    for (j=0; j<N; j++)
      if (i!=j && PtsToCentroid[j] == MyCentroid)
      {
        real3 ff = Interaction(P, Puntos[j]);
        fx += ff.x;  fy += ff.y; fz += ff.z;
      }

    for (j=0; j<C; j++)
      if (j != MyCentroid)
      {
        real3 ff = Interaction(P, Centroides[j]);
        fx += ff.x;  fy += ff.y; fz += ff.z;
      }

    force[i].x = fx;  force[i].y = fy;  force[i].z = fz;
  }
}



void evolve(real4 * out, real4 * in, real3 * vel, real3 * force, real dt, int n)
{
  int i;
  for (i = 0; i < n; i++)
  {
    real fx = force[i].x, fy = force[i].y, fz = force[i].z;
    real px = in[i].x,    py = in[i].y,    pz = in[i].z,    invMass = in[i].w;
    real vx = vel[i].x,   vy = vel[i].y,   vz = vel[i].z;

    // acceleration = force / mass; 
    // new velocity = old velocity + acceleration * deltaTime
    vx += (fx * invMass) * dt;
    vy += (fy * invMass) * dt;
    vz += (fz * invMass) * dt;

    // new position = old position + velocity * deltaTime
    px += vx * dt;
    py += vy * dt;
    pz += vz * dt;

    out[i].x = px;
    out[i].y = py;
    out[i].z = pz;
    out[i].w = invMass;

    vel[i].x = vx;
    vel[i].y = vy;
    vel[i].z = vz;
  }
}


real dot(real v0[3], real v1[3])
{
  return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
}


real normalize(real vector[3])
{
  float dist = sqrt(dot(vector, vector));
  if (dist > 1e-6)
  {
    vector[0] /= dist;
    vector[1] /= dist;
    vector[2] /= dist;
  }
  return dist;
}

void cross(real out[3], real v0[3], real v1[3])
{
  out[0] = v0[1]*v1[2]-v0[2]*v1[1];
  out[1] = v0[2]*v1[0]-v0[0]*v1[2];
  out[2] = v0[0]*v1[1]-v0[1]*v1[0];
}

void randomizeBodies(real4* pos, real3* vel, 
                     float clusterScale, float velocityScale, int n)
{
  srand(42);
  float scale = clusterScale;
  float vscale = scale * velocityScale;
  float inner = 2.5f * scale;
  float outer = 4.0f * scale;

  int p = 0, v=0;
  int i = 0;
  while (i < n)
  {
    real x, y, z;
    // generate real numbers between -1.0 and +1.0
    x = myRandom() / (float) RAND_MX * 2 - 1;
    y = myRandom() / (float) RAND_MX * 2 - 1;
    z = myRandom() / (float) RAND_MX * 2 - 1;

    real point[3] = {x, y, z};
    real len = normalize(point);
    if (len > 1) // discard position and generate new one
      continue;

    pos[i].x= point[0] * (inner + (outer - inner)*myRandom() / (real) RAND_MX);
    pos[i].y= point[1] * (inner + (outer - inner)*myRandom() / (real) RAND_MX);
    pos[i].z= point[2] * (inner + (outer - inner)*myRandom() / (real) RAND_MX);
    pos[i].w= 1.0 + myRandom() / (float) RAND_MX;

    real axis[3] = {0.0f, 0.0f, 1.0f};

    if (1 - dot(point, axis) < 1e-6)
    {
      axis[0] = point[1];
      axis[1] = point[0];
      normalize(axis);
    }
    real vv[3] = {(real)pos[i].x, (real)pos[i].y, (real)pos[i].z};
    real vv0[3];

    cross(vv0, vv, axis);
    vel[i].x = vv0[0] * vscale;
    vel[i].y = vv0[1] * vscale;
    vel[i].z = vv0[2] * vscale;

    i++;
  }
}


real3 average(real4 * p, int n)
{
  int i;
  real3 av= {0.0, 0.0, 0.0};
  for (i = 0; i < n; i++)
  {
    av.x += p[i].x;
    av.y += p[i].y;
    av.z += p[i].z;
  }
  av.x /= n;
  av.y /= n;
  av.z /= n;
  return av;
}


real4 centro_masas(real4 * P, int n)
{
  real4 cm= {0.0, 0.0, 0.0, 0.0};
  for (int i = 0; i < n; i++)
  {
    cm.x += P[i].x*P[i].w;
    cm.y += P[i].y*P[i].w;
    cm.z += P[i].z*P[i].w;
    cm.w += P[i].w;
  }
  cm.x /= cm.w;
  cm.y /= cm.w;
  cm.z /= cm.w;
  return cm;
}


void generarCentroides(real4 *Puntos, real4 *Centroides, int N, int C)
{
  int Assign[C];

  for (int i=0; i<C; i++)
  { // First assignment
    Assign[i] = myRandom() % N;
  }

  // Sort assignments  
  for (int k=0; k<C-1; k++)
    for (int i=0; i<C-1; i++)
    { 
      if (Assign[i] >= Assign[i+1]) 
      { // swap
        int t= Assign[i]; Assign[i] = Assign[i+1]; Assign[i+1] = t;
      }
    }

  // Eliminate copies
  for (int i=0; i<C-1; i++)
  {
    if (Assign[i] >= Assign[i+1])
      Assign[i+1] = Assign[i]+1;
  }

  for (int i=0; i<C; i++)
  {
    Centroides[i] = Puntos[Assign[i]];
  }
}

int minim_arg(real *V, int C)
{
  real m = V[0];
  int i, r = 0;
  for (i=0; i<C; i++)
    if (m > V[i])
    {
      r = i;
      m = V[i];
    }
  return r;
}


// Distancia entre dos puntos
real distance ( real4 P1, real4 P2 )
{
  return sqrtf( (P1.x-P2.x)*(P1.x-P2.x) + (P1.y-P2.y)*(P1.y-P2.y) + (P1.z-P2.z)*(P1.z-P2.z));
}

void PuntosToCentroides(real4 *Puntos, real4 *Centroides, int *PtsToCentroid, int *Npts, int N, int C)
{
  int i, j, k, m;

  // Array Auxiliar: C debe ser un valor pequeño para definir estructura dinámica en stack
  real dist[C];

  for (i=0; i<C; i++)
    Npts[i]=0;

  for(i=0; i<N; i++)
  {
    for(j=0; j<C; j++)
      dist[j] = distance( Puntos[i], Centroides[j] );

    m = minim_arg(dist, C);
    PtsToCentroid [i] = m;
    Npts[m] += 1;
  }
}

void recalcularCentro (real4 *Puntos, real4 *Centroides, int *PtsToCentroid, int *Npts, int N, int C)
{
  real4 cm;
  int i, j, Total;

  Total=0;
  for (i=0; i<C; i++)
  {
    cm.x= 0.0;
    cm.y= 0.0;
    cm.z= 0.0;
    cm.w= 0.0;

    for (j=0; j<N; j++)
    {
      real4 P= Puntos[j];
      
      if (PtsToCentroid[j]== i)
      {
        cm.x += P.x;
        cm.y += P.y;
        cm.z += P.z;
        cm.w += 1.0;
      }
    }

    if (cm.w > 0)
    { 
      cm.x /= cm.w;
      cm.y /= cm.w;
      cm.z /= cm.w;
    }
    Centroides[i] = cm;
  }
}


int main (int argc, char **argv)
{
  int N=50000, C=4000, Iter=3;
  int i, j;

  seed = 12345;

  // obtain parameters at run time
  if (argc>1) { N   = atoi(argv[1]); }
  if (argc>2) { C   = atoi(argv[2]); }
  if (argc>3) { Iter= atoi(argv[3]); }

  printf("Challenge #1: N= %d, C= %d, Iter= %d\n", N, C, Iter);

  real dt = 0.01667;

  real4 *puntos  = (real4*) malloc(N * sizeof(real4));
  real4 *temp    = (real4*) malloc(N * sizeof(real4));
  real4 *centroid= (real4*) malloc(C * sizeof(real4));
  real3 *velocid = (real3*) malloc(N * sizeof(real3));
  real3 *fuerzas = (real3*) malloc(N * sizeof(real3));

  int   *PaCentrd= (int  *) malloc(N * sizeof(int));
  int   *Npts    = (int  *) malloc(C * sizeof(int));

  randomizeBodies   (puntos, velocid, 1.54f, 8.0f, N);
  generarCentroides (puntos, centroid, N, C);

  for (i = 0; i < Iter; i++)
  {
    for (j=0; j<10; j++) 
    {
      PuntosToCentroides(puntos, centroid, PaCentrd, Npts, N, C);
      recalcularCentro(puntos, centroid, PaCentrd, Npts, N, C);
    }
    integrateForces (fuerzas, puntos, centroid, PaCentrd, Npts, N, C);
    evolve          (temp, puntos, velocid, fuerzas, dt, N);
    for (j=0; j<N; j++)
      puntos[j] = temp[j];
  }

  real3 p_av =      average( puntos, N );
  real4 p_cm = centro_masas( puntos, N );

  printf("Posición central:(%f,%f,%f)\n",           p_av.x,        p_av.y,        p_av.z);
  printf("Centro de Masas: (%f,%f,%f,%f)\n",        p_cm.x,        p_cm.y,        p_cm.z,        p_cm.w);
  printf("Body[0]:         (%f,%f,%f,%f)\n",   puntos[0].x,   puntos[0].y,   puntos[0].z,   puntos[0].w);
  printf("Body[1]:         (%f,%f,%f,%f)\n",   puntos[1].x,   puntos[1].y,   puntos[1].z,   puntos[1].w);

  free(puntos);  free(temp);  free(velocid);  free(fuerzas);  free(PaCentrd); free(Npts);
  return 0;
}