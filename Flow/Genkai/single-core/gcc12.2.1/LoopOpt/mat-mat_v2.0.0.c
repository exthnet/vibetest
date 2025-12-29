/* mat-mat_v2.0.0.c
 * 4x8 Micro-kernel with Cache Blocking
 * Based on PG1.1's successful strategy
 * PG1.2 - GCC 12.2.1
 */
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 64
#define MR 4
#define NR 8

void matmul(int N, double * restrict a, double * restrict b, double * restrict c)
{
  int i, j, k, ii, jj, kk;

  for(ii=0; ii<N; ii+=BLOCK_SIZE){
    for(kk=0; kk<N; kk+=BLOCK_SIZE){
      for(jj=0; jj<N; jj+=BLOCK_SIZE){
        int i_end = (ii+BLOCK_SIZE < N) ? ii+BLOCK_SIZE : N;
        int k_end = (kk+BLOCK_SIZE < N) ? kk+BLOCK_SIZE : N;
        int j_end = (jj+BLOCK_SIZE < N) ? jj+BLOCK_SIZE : N;

        /* MR x NR micro-kernel */
        int i_end_mr = ii + ((i_end - ii) / MR) * MR;
        int j_end_nr = jj + ((j_end - jj) / NR) * NR;

        for(i=ii; i<i_end_mr; i+=MR){
          for(j=jj; j<j_end_nr; j+=NR){
            /* 4x8 register block */
            double c00=0, c01=0, c02=0, c03=0, c04=0, c05=0, c06=0, c07=0;
            double c10=0, c11=0, c12=0, c13=0, c14=0, c15=0, c16=0, c17=0;
            double c20=0, c21=0, c22=0, c23=0, c24=0, c25=0, c26=0, c27=0;
            double c30=0, c31=0, c32=0, c33=0, c34=0, c35=0, c36=0, c37=0;

            for(k=kk; k<k_end; k++){
              double a0 = a[i*N+k];
              double a1 = a[(i+1)*N+k];
              double a2 = a[(i+2)*N+k];
              double a3 = a[(i+3)*N+k];

              double b0 = b[k*N+j];
              double b1 = b[k*N+j+1];
              double b2 = b[k*N+j+2];
              double b3 = b[k*N+j+3];
              double b4 = b[k*N+j+4];
              double b5 = b[k*N+j+5];
              double b6 = b[k*N+j+6];
              double b7 = b[k*N+j+7];

              c00 += a0*b0; c01 += a0*b1; c02 += a0*b2; c03 += a0*b3;
              c04 += a0*b4; c05 += a0*b5; c06 += a0*b6; c07 += a0*b7;
              c10 += a1*b0; c11 += a1*b1; c12 += a1*b2; c13 += a1*b3;
              c14 += a1*b4; c15 += a1*b5; c16 += a1*b6; c17 += a1*b7;
              c20 += a2*b0; c21 += a2*b1; c22 += a2*b2; c23 += a2*b3;
              c24 += a2*b4; c25 += a2*b5; c26 += a2*b6; c27 += a2*b7;
              c30 += a3*b0; c31 += a3*b1; c32 += a3*b2; c33 += a3*b3;
              c34 += a3*b4; c35 += a3*b5; c36 += a3*b6; c37 += a3*b7;
            }

            c[i*N+j] += c00; c[i*N+j+1] += c01; c[i*N+j+2] += c02; c[i*N+j+3] += c03;
            c[i*N+j+4] += c04; c[i*N+j+5] += c05; c[i*N+j+6] += c06; c[i*N+j+7] += c07;
            c[(i+1)*N+j] += c10; c[(i+1)*N+j+1] += c11; c[(i+1)*N+j+2] += c12; c[(i+1)*N+j+3] += c13;
            c[(i+1)*N+j+4] += c14; c[(i+1)*N+j+5] += c15; c[(i+1)*N+j+6] += c16; c[(i+1)*N+j+7] += c17;
            c[(i+2)*N+j] += c20; c[(i+2)*N+j+1] += c21; c[(i+2)*N+j+2] += c22; c[(i+2)*N+j+3] += c23;
            c[(i+2)*N+j+4] += c24; c[(i+2)*N+j+5] += c25; c[(i+2)*N+j+6] += c26; c[(i+2)*N+j+7] += c27;
            c[(i+3)*N+j] += c30; c[(i+3)*N+j+1] += c31; c[(i+3)*N+j+2] += c32; c[(i+3)*N+j+3] += c33;
            c[(i+3)*N+j+4] += c34; c[(i+3)*N+j+5] += c35; c[(i+3)*N+j+6] += c36; c[(i+3)*N+j+7] += c37;
          }
          /* Remainder j */
          for(; j<j_end; j++){
            for(k=kk; k<k_end; k++){
              double aik = a[i*N+k];
              c[i*N+j] += aik * b[k*N+j];
              c[(i+1)*N+j] += a[(i+1)*N+k] * b[k*N+j];
              c[(i+2)*N+j] += a[(i+2)*N+k] * b[k*N+j];
              c[(i+3)*N+j] += a[(i+3)*N+k] * b[k*N+j];
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
