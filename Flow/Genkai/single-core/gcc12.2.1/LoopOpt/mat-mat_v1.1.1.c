/* mat-mat_v1.1.1.c
 * Loop Optimization + Cache Blocking (BLOCK_SIZE=32)
 * PG1.2 - GCC 12.2.1
 */
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 32

void matmul(int N, double *a, double *b, double *c)
{
  int i, j, k, ii, jj, kk;
  double aik;

  for(ii=0; ii<N; ii+=BLOCK_SIZE){
    for(kk=0; kk<N; kk+=BLOCK_SIZE){
      for(jj=0; jj<N; jj+=BLOCK_SIZE){
        int i_end = (ii+BLOCK_SIZE < N) ? ii+BLOCK_SIZE : N;
        int k_end = (kk+BLOCK_SIZE < N) ? kk+BLOCK_SIZE : N;
        int j_end = (jj+BLOCK_SIZE < N) ? jj+BLOCK_SIZE : N;

        for(i=ii; i<i_end; i++){
          for(k=kk; k<k_end; k++){
            aik = a[i*N+k];
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
  int i, j, k;
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
