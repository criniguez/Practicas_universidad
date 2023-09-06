#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double complex cs  = -0.62772 - 0.42193 * I;

long reduction(unsigned short *arr, size_t size)
{
    long sum = 0;
    #pragma omp parallel for shared(arr) reduction(+:sum)
    for (size_t i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

//cabs es basura no se paraleliza o algo del estilo 
double absolute(double complex z)
{
    return creal(z)*creal(z) + cimag(z)*cimag(z);
}


void calculate_z(unsigned short *out, int max_iterations, double complex *zs, int size) {
  unsigned short j;
  
  #pragma omp parallel shared(out, zs) private(j) 
  {
    #pragma omp for simd nowait schedule(dynamic, 1) 
    for (int i = 0; i < size; i++) {
        double complex z = zs[i];

        for (j = 0;  absolute(z) < 4.0 && j < max_iterations; j++) {
          z = z * z + cs;
        }
        out[i] = j;
      }
  }
}

void calc_pure_python(size_t size, int max_iterations) {
  const double x1 = -1.8, x2 = 1.8, y1 = -1.8, y2 = 1.8;
  
  double width  = size;
  double x_step = (x2 - x1) / width;
  double y_step = (y1 - y2) / width;
  
  size_t index = (size/2l)*(size-1l); 
  unsigned short *julia  = malloc((index - size + 1l) * sizeof(unsigned short));
  unsigned short *mid    = malloc((size - 1l)         * sizeof(unsigned short));
  unsigned short *border = malloc((size - 1l)         * sizeof(unsigned short));

  double *y = malloc(size * sizeof(double));
  double *x = malloc(size * sizeof(double));

  double complex *zs = malloc((size - 1l) * (size/2l +1l) * sizeof(double complex));


  #pragma omp parallel for simd
  for (size_t i = 0; i < size; i++) {
    y[i] = y2 + (double)i * y_step;
    x[i] = x1 + (double)i * x_step;
  }

  #pragma omp parallel for simd ///collapse(2)
  for (size_t i = 0; i < size/2 + 1; i++) {
    for (size_t j = 1; j < size; j++) {
      zs[(size-1l) * i + (j-1l)] = x[j] + y[i] * I;
    } 
  }
  printf( "%ld %ld %ld\n", size, index, index-size+1);

  calculate_z(border, max_iterations,  zs,         size - 1);  
  calculate_z(mid,    max_iterations, &zs[index ], size - 1);
  calculate_z(julia,  max_iterations, &zs[size-1], index - size + 1);
  
  long out1 = reduction(border, size - 1);
  long out2 = reduction(mid,    size - 1);
  long out3 = reduction(julia,  index - size + 1);

  printf("output: %ld\n", (out1+out3)*2+out2);
}

int main(void) {
  calc_pure_python(10000, 300);
  exit(0);

