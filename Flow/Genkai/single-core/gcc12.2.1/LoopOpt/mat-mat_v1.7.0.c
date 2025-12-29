/* mat-mat_v1.7.0.c
 * Cache Blocking (BLOCK_SIZE=48) + simplified inner loop
 * Optimized for better vectorization with -ffast-math
 * PG1.2 - GCC 12.2.1
 */
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 48
#define ALIGN 64

void matmul(int N, double * restrict A, double * restrict B, double * restrict C)
{
  int ii, jj, kk, i, j, k;

  for(ii=0; ii<N; ii+=BLOCK_SIZE){
    int i_max = (ii+BLOCK_SIZE < N) ? ii+BLOCK_SIZE : N;
    for(kk=0; kk<N; kk+=BLOCK_SIZE){
      int k_max = (kk+BLOCK_SIZE < N) ? kk+BLOCK_SIZE : N;
      for(jj=0; jj<N; jj+=BLOCK_SIZE){
        int j_max = (jj+BLOCK_SIZE < N) ? jj+BLOCK_SIZE : N;

        for(i=ii; i<i_max; i++){
          double * restrict Ci = &C[i*N];
          double * restrict Ai = &A[i*N];
          for(k=kk; k<k_max; k++){
            double aik = Ai[k];
            double * restrict Bk = &B[k*N];
            #pragma GCC ivdep
            for(j=jj; j<j_max; j++){
              Ci[j] += aik * Bk[j];
            }
          }
        }
      }
    }
  }
}

int main(int argc, char **argv)
{
  int i, j;
  int N;
  double *a=NULL, *b=NULL, *c=NULL;
  int out;

  out = 1;
  if(argc==1){
    N = 10;
  }else{
    N = atoi(argv[1]);
    if(argc==3){
      out = 0;
    }
  }
  printf("N = %d\n", N);
  printf("out = %d\n", out);

  a = (double*)aligned_alloc(ALIGN, sizeof(double)*N*N);
  b = (double*)aligned_alloc(ALIGN, sizeof(double)*N*N);
  c = (double*)aligned_alloc(ALIGN, sizeof(double)*N*N);

  if(!a || !b || !c) {
    fprintf(stderr, "Memory allocation failed\n");
    return 1;
  }

  for(i=0; i<N; i++){
    for(j=0; j<N; j++){
      a[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
      b[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
      c[i*N+j] = 0.0;
    }
  }

  matmul(N, a, b, c);

  if(out==1){
    printf("result:\n");
    for(i=0; i<N; i++){
      for(j=0; j<N; j++){
        printf(" %8.2f", c[i*N+j]);
      }
      printf("\n");
    }
    printf("\n");
  }

  free(c);
  free(b);
  free(a);

  return 0;
}
