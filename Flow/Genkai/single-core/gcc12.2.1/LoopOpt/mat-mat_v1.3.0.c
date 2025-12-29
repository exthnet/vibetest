/* mat-mat_v1.3.0.c
 * Cache Blocking + Register Blocking (2x2 micro-kernel)
 * PG1.2 - GCC 12.2.1
 */
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 64

void matmul(int N, double *a, double *b, double *c)
{
  int i, j, k, ii, jj, kk;

  for(ii=0; ii<N; ii+=BLOCK_SIZE){
    for(kk=0; kk<N; kk+=BLOCK_SIZE){
      for(jj=0; jj<N; jj+=BLOCK_SIZE){
        int i_end = (ii+BLOCK_SIZE < N) ? ii+BLOCK_SIZE : N;
        int k_end = (kk+BLOCK_SIZE < N) ? kk+BLOCK_SIZE : N;
        int j_end = (jj+BLOCK_SIZE < N) ? jj+BLOCK_SIZE : N;
        int i_end2 = ii + ((i_end - ii) / 2) * 2;
        int j_end2 = jj + ((j_end - jj) / 2) * 2;

        /* 2x2 register blocking */
        for(i=ii; i<i_end2; i+=2){
          for(j=jj; j<j_end2; j+=2){
            double c00 = c[i*N+j];
            double c01 = c[i*N+j+1];
            double c10 = c[(i+1)*N+j];
            double c11 = c[(i+1)*N+j+1];

            for(k=kk; k<k_end; k++){
              double a0k = a[i*N+k];
              double a1k = a[(i+1)*N+k];
              double bk0 = b[k*N+j];
              double bk1 = b[k*N+j+1];

              c00 += a0k * bk0;
              c01 += a0k * bk1;
              c10 += a1k * bk0;
              c11 += a1k * bk1;
            }

            c[i*N+j] = c00;
            c[i*N+j+1] = c01;
            c[(i+1)*N+j] = c10;
            c[(i+1)*N+j+1] = c11;
          }
          /* Remainder j */
          for(; j<j_end; j++){
            for(k=kk; k<k_end; k++){
              c[i*N+j] += a[i*N+k] * b[k*N+j];
              c[(i+1)*N+j] += a[(i+1)*N+k] * b[k*N+j];
            }
          }
        }
        /* Remainder i */
        for(; i<i_end; i++){
          for(k=kk; k<k_end; k++){
            double aik = a[i*N+k];
            for(j=jj; j<j_end; j++){
              c[i*N+j] += aik * b[k*N+j];
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

  a = (double*)malloc(sizeof(double)*N*N);
  b = (double*)malloc(sizeof(double)*N*N);
  c = (double*)malloc(sizeof(double)*N*N);
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
