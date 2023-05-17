/*
  Cristian Iñiguez Rodriguez 1566514
  3,464610757 seconds time elapsed
  gcc -Ofast -march=nehalem -fopenmp -g CH2.c -o CH2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Variable used to generate pseudo-random numbers
unsigned int seed;

// Function to generate pseudo-random numbers
unsigned int myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>13);
}

void stencil_block (unsigned short *In, unsigned short *Out, int x_init, int Bsz, int Ksz, int D, int MAX_VAL )
{
  int k;

  unsigned short *Tin  = (unsigned short*)malloc(sizeof(unsigned short) * (Bsz+2*Ksz+2) *D);
  unsigned short *Tout = (unsigned short*)malloc(sizeof(unsigned short) * (Bsz+2*Ksz+2)*D);
  
  memcpy(Tin,  &In[(x_init-Ksz - 1) * D], sizeof(unsigned short) * (Bsz+2*Ksz+2) * D);
  memcpy(Tout, &In[(x_init-Ksz - 1) * D], sizeof(unsigned short) * (Bsz+2*Ksz+2) * D);

  int aux = x_init + Bsz + Ksz;
  int min = (aux > D)?D-x_init:(Bsz+2*Ksz+1);

  for (k=0; k<Ksz; k++)
  { 
    #pragma omp parallel for
    for (int x = 1 + k; x < min - k; x++) 
    {
      for (int y=1; y < D-1; y++)
      { 
        unsigned short a = Tin[ x * D + y - D];
        unsigned short b = Tin[ x * D + y - 1];
        unsigned short c = Tin[ x * D + y + 1];
        unsigned short d = Tin[ x * D + y + D];
        unsigned short res;

        if (b > a) {
          b = b ^ a;
          a = b ^ a;
          b = b ^ a;
        }
        if (d > c) {
          d = d ^ c;
          c = d ^ c;
          d = d ^ c;
        }

        res  = (a > c)?c:a;
        res += (b > d)?b:d;
      
        Tout[x * D + y] = (res >= MAX_VAL)?res-MAX_VAL:res;
      }
    }
    unsigned short *tmp = Tout; 
    Tout = Tin; 
    Tin = tmp;
  }
  memcpy(&Out[x_init * D], &Tin[D+Ksz*D], sizeof(unsigned short) * Bsz * D);
  free(Tin);
  free(Tout);
}

void stencil_first( unsigned short *IN, unsigned short *OUT, int Bsz, int Ksz, int D, int MAX_VAL )
{
  int k;
  
  unsigned short *Tin  = (unsigned short*)malloc(sizeof(unsigned short) * (Bsz+Ksz+1) * D);
  unsigned short *Tout = (unsigned short*)malloc(sizeof(unsigned short) * (Bsz+Ksz+1) * D);
  
  memcpy(Tin,  IN, sizeof(unsigned short) * (Bsz+Ksz+1) * D);
  memcpy(Tout, IN, sizeof(unsigned short) * (Bsz+Ksz+1) * D);
  
  for (k=0; k<Ksz; k++)
  { 
    #pragma omp parallel for
    for (int x=1; x < (Bsz+Ksz-k); x++)
    {
      for (int y=1; y < D-1; y++)
      { 
        unsigned short a = Tin[ x * D + y - D];
        unsigned short b = Tin[ x * D + y - 1];
        unsigned short c = Tin[ x * D + y + 1];
        unsigned short d = Tin[ x * D + y + D];
        unsigned short res;

        if (b > a) {
          b = b ^ a;
          a = b ^ a;
          b = b ^ a;
        }
        if (d > c) {
          d = d ^ c;
          c = d ^ c;
          d = d ^ c;
        }

        res  = (a > c)?c:a;
        res += (b > d)?b:d;
      
        Tout[x * D + y] = (res >= MAX_VAL)?res-MAX_VAL:res;
        }
    }
    unsigned short *tmp = Tout; 
    Tout = Tin; 
    Tin = tmp;
  }

  memcpy(&OUT[D], &Tin[D], sizeof(unsigned short) * Bsz * D);
  free(Tin); 
  free(Tout);
}

void stencil_last_block( unsigned short *IN, unsigned short *OUT, int Bsz, int Ksz, int D, int MAX_VAL )
{
  int k, x;
  unsigned short *Tin  = (unsigned short*)malloc(sizeof(unsigned short) * (Bsz+Ksz) * D);
  unsigned short *Tout = (unsigned short*)malloc(sizeof(unsigned short) * (Bsz+Ksz) * D);
  
  memcpy(Tin,  &IN[(D-Bsz-Ksz)*D], sizeof(unsigned short) * (Bsz+Ksz) * D);
  memcpy(Tout, &IN[(D-Bsz-Ksz)*D], sizeof(unsigned short) * (Bsz+Ksz) * D);

  for (k=0; k<Ksz; k++)
  { 
    #pragma omp parallel for
    for (x = k + 1; x<(Bsz+Ksz)-1; x++) 
    {
      for (int y=1; y < D-1; y++)
      { 
        unsigned short a = Tin[ x * D + y - D];
        unsigned short b = Tin[ x * D + y - 1];
        unsigned short c = Tin[ x * D + y + 1];
        unsigned short d = Tin[ x * D + y + D];
        unsigned short res;

        if (b > a) {
          b = b ^ a;
          a = b ^ a;
          b = b ^ a;
        }
        if (d > c) {
          d = d ^ c;
          c = d ^ c;
          d = d ^ c;
        }

        res  = (a > c)?c:a;
        res += (b > d)?b:d;
      
        Tout[x * D + y] = (res >= MAX_VAL)?res-MAX_VAL:res;
      }
    }
  
    unsigned short *tmp = Tout; 
    Tout = Tin; 
    Tin = tmp;
  }

  memcpy(&OUT[(D - Bsz + 1)* D], &Tin[(Ksz+1) * D], sizeof(unsigned short) * Bsz * D);

  free(Tin); 
  free(Tout);
}


void PrintCHECK ( unsigned short BOARD[], int D, int DD)
{
  int index = 2*D;

  // Line x=1:
  for (int y=0; y<20; y++)
    printf("%4d ",BOARD[D+y]);
  printf("\n");

  // Line x=2:
  for (int y=0; y<20; y++)
    printf("%4d ",BOARD[index + y]);
  printf("\n");

  // Line x=D-2:
  for (int y=0; y< 20; y++)
    printf("%4d ",BOARD[(DD-index)+ y]);
  printf("\n"); 
}

void __attribute__ ((noinline)) 
  FillHistogram ( unsigned short *BOARD, unsigned *Freq, unsigned *LocalId, int DD, int ValMax )
{
  memset(Freq, 0, sizeof(unsigned)*ValMax);
  for(int i = 0; i < DD; i++)
  {
    unsigned short V = BOARD[i];
    LocalId[i] = Freq[V];
    Freq[V]++;
  }
}

void PrefixSum (unsigned* A, int n)
{ 
  unsigned nthr, *z, *x;
  x = (unsigned *)  malloc( n* sizeof(unsigned) );
  #pragma omp parallel
  {
    int i;
    #pragma omp single
    {
      nthr = omp_get_num_threads();
      z = (unsigned*)malloc(sizeof(unsigned)*nthr+1);
      z[0] = 0;
    }
    int tid = omp_get_thread_num();
    int sum = 0;
    #pragma omp for schedule(static) 
    for(i=0; i<n; i++) {
      x[i] = sum;
      sum += A[i];
    }
    z[tid+1] = sum;
    #pragma omp barrier

    int offset = 0;
    for(i=0; i<(tid+1); i++) {
        offset += z[i];
    }

    #pragma omp for schedule(static)
    for(i=0; i<n; i++) {
      x[i] += offset;
    }
  }
  memcpy(A, x, n*sizeof(unsigned));
  free(x);
  free(z);
}

unsigned binary_cmov (unsigned *arr, unsigned n, int key) {
	unsigned min = 0, max = n;    
  while (min < max) {
          int middle = (min + max) >> 1;
          asm ("cmpl %3, %2\n\tcmovg %4, %0\n\tcmovle %5, %1"
                : "+r" (min),
                  "+r" (max)
                : "r" (key), "g" (arr [middle]),
                  "g" (middle + 1), "g" (middle));
  }
  return --min;
}

void __attribute__ ((noinline)) 
  UpdateReversed ( unsigned short BOARD[], unsigned Freq[], unsigned LocalId[], int DD, int ValMax )
{
  #pragma omp parallel for
  for (int xy=0; xy<DD; xy++)
  {
    unsigned short V = BOARD[xy];
    unsigned pos     = Freq[V]+LocalId[xy];   
    BOARD[xy]        = binary_cmov(Freq, ValMax, DD-pos);
  }
}


void CopyBoundary(const unsigned short IN[], unsigned short OUT[], int D) {
    const int index1 = D * (D - 1);
    #pragma omp parallel for
    for (int x = 0; x < D; x++) { 
        OUT[x] = IN[x];
        OUT[x + index1] = IN[x + index1];
    }
   
    const int index2 = D - 1;
    #pragma omp parallel for
    for (int y = D; y < D*D; y+=D) { 
        OUT[y] = IN[y];
        OUT[y + index2] = IN[y + index2];
    }
}

int main (int argc, char **argv)
{
  int N=10, D=4002, Iter=50, MAX= 9999;
  seed = 12345;
  
  
  // obtain parameters at run time
  if (argc>1) { D   = atoi(argv[1]); }
  if (argc>2) { N   = atoi(argv[2]); }
  if (argc>3) { Iter= atoi(argv[3]); }

  printf("Challenge #2: DIM= %d, N= %d, Iter= %d\n", D, N, Iter);
  int DD = D*D;
  unsigned short *BOARD, *TMP, *aux;
  unsigned       *Freq, *LocID;

  aux  = (unsigned short *) malloc( DD* sizeof(unsigned short) );
  BOARD= (unsigned short *) malloc( DD* sizeof(unsigned short) );
  TMP  = (unsigned short *) malloc( DD* sizeof(unsigned short) );
  Freq = (unsigned *)       malloc( MAX* sizeof(unsigned) );
  LocID= (unsigned *)       malloc( DD* sizeof(unsigned) );

  for(int i = 0; i < DD; i++)
    aux[i] = myRandom() % MAX;
  
  #pragma omp distribute parallel for collapse(2)
  for (int x=0; x<D; x++)
    for (int y=0; y<D; y++)
      BOARD[x*D + y] = aux[x+D*y];

  memcpy(TMP, BOARD, DD* sizeof(unsigned short));
  int Bsz = 500;
  for (int i = 0; i < N; i++)
  {

    stencil_first(TMP, BOARD, Bsz, Iter, D, MAX);
    for (int x = Bsz + 1; x <= D-Bsz; x += Bsz)
      stencil_block( TMP, BOARD, x, Bsz, Iter, D, MAX );
    stencil_last_block(TMP, BOARD, Bsz, Iter, D, MAX);
    
    FillHistogram  ( BOARD, Freq, LocID, DD, MAX );
    PrefixSum      ( Freq, MAX );
    UpdateReversed ( BOARD, Freq, LocID, DD, MAX );    
    
    CopyBoundary(BOARD, TMP, D);
    unsigned short *tmp = BOARD; 
    BOARD = TMP;
    TMP = tmp;
  }

  PrintCHECK (TMP, D , DD);
  
  return 0;
}
