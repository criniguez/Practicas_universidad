#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <nmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <float.h>



// Data structures real3 and real4
typedef struct {float *x, *y, *z;     } real3; 
typedef struct {float *x, *y, *z, *w; } real4;

// Variable used to generate pseudo-random numbers
unsigned int seed;
unsigned const int RAND_MX = 1 << (32 -13);

// Function to generate pseudo-random numbers
unsigned int myRandom() {
  seed = (214013 * seed + 2531011);
  return (seed >> 13);
}

void Interaction(real4 p1, real4 p2, int i, int j, float out[3])
{
  float rx, ry, rz, distSqr, s;

  rx = p2.x[j] - p1.x[i];  
  ry = p2.y[j] - p1.y[i];  
  rz = p2.z[j] - p1.z[i];

  distSqr = rx * rx + ry * ry + rz * rz;

  s = (distSqr < 0.001f) ? 0.001f : distSqr;
  s = p2.w[j]/(s * sqrtf(s));
    
  out[0] = rx * s;
  out[1] = ry * s;
  out[2] = rz * s;
}


void integrateForces (real3 force, real4 Puntos, real4 Centroides, int *PtsToCentroid, int N, int C)
{
  real4 cm;
  int i, j;
  float ff[3];


  for (i = 0; i < N; i++)
  {
    float fx = 0.0f, fy = 0.0f, fz = 0.0f;
    int MyCentroid = PtsToCentroid[i];
    int arr[C]; 
    int k = 0;
__m128i zero = _mm_setzero_si128();
for (j = 0; j < N; j += 4) {
    // Load four elements of PtsToCentroid into xmm0
    __m128i xmm0 = _mm_loadu_si128((__m128i*)&PtsToCentroid[j]);
    
    // Compare xmm0 with MyCentroid
    __m128i xmm1 = _mm_cmpeq_epi32(xmm0, _mm_set1_epi32(MyCentroid));
    
    // Convert the result to an integer mask
    int mask = _mm_movemask_ps((__m128)xmm1);
    
    // Use the mask to gather the indices of the matching elements
    // and store them in arr

    if (mask & 0x1) arr[k++] = j;
    if (mask & 0x2) arr[k++] = j + 1;
    if (mask & 0x4) arr[k++] = j + 2;
    if (mask & 0x8) arr[k++] = j + 3;
}

	// Handle the remaining elements (if any) using the scalar code
	for (; j < N; j++) {
	    int prueba = (PtsToCentroid[j] == MyCentroid);
	    arr[k += prueba] = j;
	}
/*
    for (j = 0; j < k; j++)
    {
    	float cmp = i != arr[j];
	Interaction(Puntos, Puntos, i, arr[j], ff);
	fx += ff[0] * cmp;  fy += ff[1] * cmp; fz += ff[2] * cmp;

    }
*/
    for (j = 0; j < k; j++)
    {
        if (i != arr[j]) {
        	Interaction(Puntos, Puntos, i, arr[j], ff);
        	fx += ff[0];  fy += ff[1]; fz += ff[2];
	}
    }

    for (j = 0; j < MyCentroid; j++) 
    {
      Interaction(Puntos, Centroides, i, j, ff);
      fx += ff[0];  fy += ff[1]; fz += ff[2];
    }
    
    for (j = MyCentroid + 1; j < C; j++) 
    {
      Interaction(Puntos, Centroides, i, j, ff);
      fx += ff[0];  fy += ff[1]; fz += ff[2];
    }

    force.x[i] = fx;  force.y[i] = fy;  force.z[i] = fz;
  }
}

// vectorized evolve
void evolve(real4 out, real4 in, real3 vel, real3 force, float dt, int n)
{

  for (int i = 0; i < n; i++) 
  {
    vel.x[i] = vel.x[i] + (force.x[i] * in.w[i]) * dt;
    out.x[i] = in.x[i] + vel.x[i] * dt;
  }

  for (int i = 0; i < n; i++) 
  {
    vel.y[i] = vel.y[i] + (force.y[i] * in.w[i]) * dt;
    vel.z[i] = vel.z[i] + (force.z[i] * in.w[i]) * dt; 
  }

  for (int i = 0; i < n; i++) 
  {
    out.y[i] = in.y[i] + vel.y[i] * dt;
    out.z[i] = in.z[i] + vel.z[i] * dt;
  }

  memcpy(out.w, in.w, n * sizeof(float));
}


float dot(float v0[3], float v1[3])
{
  return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
}


float normalize(float vector[3])
{
  float dist = sqrt(dot(vector, vector));
  if (dist > 1e-6f)
  {
    vector[0] /= dist;
    vector[1] /= dist;
    vector[2] /= dist;
  }
  return dist;
}

void cross(float out[3], float v0[3], float v1[3])
{
  out[0] = v0[1]*v1[2]-v0[2]*v1[1];
  out[1] = v0[2]*v1[0]-v0[0]*v1[2];
  out[2] = v0[0]*v1[1]-v0[1]*v1[0];
}

void randomizeBodies(real4 pos, real3 vel, 
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
    float x, y, z;
    // generate float numbers between -1.0 and +1.0
    x = myRandom() / (float) RAND_MX * 2 - 1;
    y = myRandom() / (float) RAND_MX * 2 - 1;
    z = myRandom() / (float) RAND_MX * 2 - 1;

    float point[3] = {x, y, z};
    float len = normalize(point);
    if (len > 1) // discard position and generate new one
      continue;

    pos.x[i] = point[0] * (inner + (outer - inner)*myRandom() / (float) RAND_MX);
    pos.y[i] = point[1] * (inner + (outer - inner)*myRandom() / (float) RAND_MX);
    pos.z[i] = point[2] * (inner + (outer - inner)*myRandom() / (float) RAND_MX);
    pos.w[i] = 1.0 + myRandom() / (float) RAND_MX;

    float axis[3] = {0.0f, 0.0f, 1.0f};

    if (1.0f - dot(point, axis) < 1e-6f)
    {
      axis[0] = point[1];
      axis[1] = point[0];
      normalize(axis);
    }
    float vv[3] = {(float)pos.x[i], (float)pos.y[i], (float)pos.z[i]};
    float vv0[3];

    cross(vv0, vv, axis);
    vel.x[i] = vv0[0] * vscale;
    vel.y[i] = vv0[1] * vscale;
    vel.z[i] = vv0[2] * vscale;

    i++;
  }
}


void average(real4 p, float av[3], int n)
{
  int i;
  av[0] = 0.0f;
  av[1] = 0.0f;
  av[2] = 0.0f;

  for (i = 0; i < n; i++)
  {
    av[0] += p.x[i];
    av[1] += p.y[i];
    av[2] += p.z[i];
  }
  av[0] /= n;
  av[1] /= n;
  av[2] /= n;
}


void centro_masas(real4 P, float cm[4], int n)
{

  float res1 = 0.0f, res2 = 0.0f, res3 = 0.0f, res4 = 0.0f;
  
  for (int i = 0; i < n; i++)
  {
    res1 += P.x[i] * P.w[i];
    res2 += P.y[i] * P.w[i];
    res3 += P.z[i] * P.w[i];
    res4 += P.w[i];
  }
  
  res1 /= res4;
  res2 /= res4;
  res3 /= res4;

  cm[0] = res1;
  cm[1] = res2;
  cm[2] = res3;
  cm[3] = res4;
}



void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];
  
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0; 
    j = 0; 
    k = l; 
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
  
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
  
void mergeSort(int arr[], int l, int r)
{
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void generarCentroides(real4 Puntos, real4 Centroides, int N, int C)
{
  int Assign[C];

  for (int i=0; i<C; i++)
  { // First assignment
    Assign[i] = myRandom() % N;
  }

  // Sort assignments  
  mergeSort(Assign, 0, C - 1);

  // Eliminate copies
  for (int i = 0; i < C - 1; i++)
  {
    if (Assign[i] >= Assign[i+1])
      Assign[i+1] = Assign[i]+1;
  }

  for (int i = 0; i < C; i++)
  {
    Centroides.x[i] = Puntos.x[Assign[i]];
    Centroides.y[i] = Puntos.y[Assign[i]];
    Centroides.z[i] = Puntos.z[Assign[i]];
    Centroides.w[i] = Puntos.w[Assign[i]];
  }
}


// SSE adaptation of https://en.algorithmica.org/hpc/algorithms/argmin/
int argmin(float *a, int n) {
    // indices on the current iteration
    __m128 cur = _mm_setr_ps(0.0f, 1.0f, 2.0f, 3.0f);
    // the current minimum for each slice
    __m128 min = _mm_set1_ps(FLT_MAX);
    // its index (argmin) for each slice
    __m128 idx = _mm_setzero_ps();

    int i;
    for (i = 0; i < n - 3; i += 4) {
        // load a new SIMD block
        __m128 x = _mm_load_ps(&a[i]);
        // find the slices where the minimum is updated
        __m128 mask = _mm_cmplt_ps(x, min);
        // update the indices
        idx = _mm_blendv_ps(idx, cur, mask);
        // update the minimum
        min = _mm_min_ps(x, min);
        // update the current indices
        const __m128 four = _mm_set1_ps(4.0f);
        cur = _mm_add_ps(cur, four);
    }

    // find the argmin in the "min" register and return its float index

    float min_arr[4], idx_arr[4];
    
    _mm_storeu_ps(min_arr, min);
    _mm_storeu_ps(idx_arr, idx);

    int k = 0;
    float m = min_arr[0];
    int i2 = i;

    for (i = 1; i < 4; i++)
        if (min_arr[i] < m)
            m = min_arr[k = i];

    int argmin = (int) idx_arr[k];

    // handle remaining elements
    for (i = i2; i < n; i++) 
    {
    	argmin = (a[i] < m) ? i : argmin;
    	m = (a[i] < m) ? a[i] : m;
    }

    return argmin;
}

void PuntosToCentroides(real4 Puntos, real4 Centroides, int *PtsToCentroid, int *Npts, int N, int C)
{
  int i, j;

  // Array Auxiliar: C debe ser un valor pequeño para definir estructura dinámica en stack
  float dist[C];
  
  for(i = 0; i < N; i++)
  {
    float x = Puntos.x[i], y = Puntos.y[i], z = Puntos.z[i];
    
    for(j = 0; j < C; j++) {
      float x2 = Centroides.x[j], y2 = Centroides.y[j], z2 = Centroides.z[j];
      dist[j] = (x - x2) * (x - x2) + (y - y2) * (y - y2) + (z - z2) * (z - z2);
    }
    
    int m = argmin(dist, C);
    PtsToCentroid[i] = m;
    ++Npts[m];
  }
}


void recalcularCentro (real4 Puntos, real4 Centroides, int *PtsToCentroid, int *Npts, int N, int C)
{
  int i, j;

  for (i = 0; i < C; i++)
  {
    float x   = 0.0f, y = 0.0f, z = 0.0f;
    float aux = Npts[i];

    if (Npts[i] > 0)
    { 
      for (j = 0; j < N; j++)
      {
        if (__builtin_expect(PtsToCentroid[j] == i, 0)) //branch prediction [[unlikely]]
        {
          x += Puntos.x[j];
          y += Puntos.y[j];
          z += Puntos.z[j];
        }
      }

      x /= aux;
      y /= aux;
      z /= aux;
   }
    
    Centroides.x[i] = x;
    Centroides.y[i] = y;
    Centroides.z[i] = z;
    Centroides.w[i] = aux;
  }
}

int main (int argc, char **argv)
{
  int N = 50000, C = 4000, Iter = 3;
  int i, j;

  seed = 12345;

  // obtain parameters at run time
  if (argc > 1) { N    = atoi(argv[1]); }
  if (argc > 2) { C    = atoi(argv[2]); }
  if (argc > 3) { Iter = atoi(argv[3]); }

  printf("Challenge #1: N= %d, C= %d, Iter= %d\n", N, C, Iter);

  float dt = 0.01667f;

  float p_av[3], p_cm[3]; 

  real4 puntos;   
  puntos.x = (float*) malloc (N * sizeof(float));
  puntos.y = (float*) malloc (N * sizeof(float));
  puntos.z = (float*) malloc (N * sizeof(float));
  puntos.w = (float*) malloc (N * sizeof(float));

  real4 temp;     
  temp.x = (float*) malloc (N * sizeof(float));
  temp.y = (float*) malloc (N * sizeof(float));
  temp.z = (float*) malloc (N * sizeof(float));
  temp.w = (float*) malloc (N * sizeof(float));

  real4 centroid; 
  centroid.x = (float*) malloc (N * sizeof(float));
  centroid.y = (float*) malloc (N * sizeof(float));
  centroid.z = (float*) malloc (N * sizeof(float));
  centroid.w = (float*) malloc (N * sizeof(float));

  real3 velocid;  
  velocid.x = (float*) malloc (N * sizeof(float));
  velocid.y = (float*) malloc (N * sizeof(float));
  velocid.z = (float*) malloc (N * sizeof(float));

  real3 fuerzas;  
  fuerzas.x = (float*) malloc (N * sizeof(float));
  fuerzas.y = (float*) malloc (N * sizeof(float));
  fuerzas.z = (float*) malloc (N * sizeof(float));

  int   *PaCentrd = (int  *) malloc(N * sizeof(int));
  int   *Npts     = (int  *) malloc(C * sizeof(int));

  randomizeBodies   (puntos, velocid, 1.54f, 8.0f, N);
  generarCentroides (puntos, centroid, N, C);

  for (i = 0; i < Iter; i++)
  {
    for (j = 0; j < 10; j++) 
    {
      PuntosToCentroides(puntos, centroid, PaCentrd, Npts, N, C);
      recalcularCentro  (puntos, centroid, PaCentrd, Npts, N, C);
      memset(Npts, 0, C * sizeof(float));
    }

    integrateForces (fuerzas, puntos, centroid, PaCentrd, N, C);
    evolve          (temp, puntos, velocid, fuerzas, dt, N);

    float *tmpx = temp.x; 
    temp.x      = puntos.x; 
    puntos.x    = tmpx; 

    float *tmpy = temp.y; 
    temp.y      = puntos.y; 
    puntos.y    = tmpy; 

    float *tmpz = temp.z; 
    temp.z      = puntos.z; 
    puntos.z   = tmpz; 

    float *tmpw = temp.w; 
    temp.w      = puntos.w; 
    puntos.w    = tmpw; 
  }

  average      (puntos, p_av, N);
  centro_masas (puntos, p_cm, N);

  printf("Posición central:(%f,%f,%f)\n",           p_av[0],        p_av[1],        p_av[2]);
  printf("Centro de Masas: (%f,%f,%f,%f)\n",        p_cm[0],        p_cm[1],        p_cm[2],        p_cm[3]);
  printf("Body[0]:         (%f,%f,%f,%f)\n",   puntos.x[0],   puntos.y[0],   puntos.z[0],   puntos.w[0]);
  printf("Body[1]:         (%f,%f,%f,%f)\n",   puntos.x[1],   puntos.y[1],   puntos.z[1],   puntos.w[1]);

  exit(0);
}