#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

// v1.5.0: ブロックサイズ32 + 6x16レジスタブロッキング + AVX512
#define BLOCK_I 48
#define BLOCK_K 256
#define BLOCK_J 256

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
  int i, j, k, ii, jj, kk;

  for(kk=0; kk<N; kk+=BLOCK_K){
    for(jj=0; jj<N; jj+=BLOCK_J){
      for(ii=0; ii<N; ii+=BLOCK_I){
        int i_end = (ii+BLOCK_I < N) ? ii+BLOCK_I : N;
        int k_end = (kk+BLOCK_K < N) ? kk+BLOCK_K : N;
        int j_end = (jj+BLOCK_J < N) ? jj+BLOCK_J : N;

        for(i=ii; i<=i_end-6; i+=6){
          for(j=jj; j<=j_end-16; j+=16){
            __m512d c00 = _mm512_loadu_pd(&c[(i+0)*N+j]);
            __m512d c01 = _mm512_loadu_pd(&c[(i+0)*N+j+8]);
            __m512d c10 = _mm512_loadu_pd(&c[(i+1)*N+j]);
            __m512d c11 = _mm512_loadu_pd(&c[(i+1)*N+j+8]);
            __m512d c20 = _mm512_loadu_pd(&c[(i+2)*N+j]);
            __m512d c21 = _mm512_loadu_pd(&c[(i+2)*N+j+8]);
            __m512d c30 = _mm512_loadu_pd(&c[(i+3)*N+j]);
            __m512d c31 = _mm512_loadu_pd(&c[(i+3)*N+j+8]);
            __m512d c40 = _mm512_loadu_pd(&c[(i+4)*N+j]);
            __m512d c41 = _mm512_loadu_pd(&c[(i+4)*N+j+8]);
            __m512d c50 = _mm512_loadu_pd(&c[(i+5)*N+j]);
            __m512d c51 = _mm512_loadu_pd(&c[(i+5)*N+j+8]);

            for(k=kk; k<k_end; k++){
              __m512d b0 = _mm512_loadu_pd(&b[k*N+j]);
              __m512d b1 = _mm512_loadu_pd(&b[k*N+j+8]);

              __m512d a0 = _mm512_set1_pd(a[(i+0)*N+k]);
              c00 = _mm512_fmadd_pd(a0, b0, c00);
              c01 = _mm512_fmadd_pd(a0, b1, c01);

              __m512d a1 = _mm512_set1_pd(a[(i+1)*N+k]);
              c10 = _mm512_fmadd_pd(a1, b0, c10);
              c11 = _mm512_fmadd_pd(a1, b1, c11);

              __m512d a2 = _mm512_set1_pd(a[(i+2)*N+k]);
              c20 = _mm512_fmadd_pd(a2, b0, c20);
              c21 = _mm512_fmadd_pd(a2, b1, c21);

              __m512d a3 = _mm512_set1_pd(a[(i+3)*N+k]);
              c30 = _mm512_fmadd_pd(a3, b0, c30);
              c31 = _mm512_fmadd_pd(a3, b1, c31);

              __m512d a4 = _mm512_set1_pd(a[(i+4)*N+k]);
              c40 = _mm512_fmadd_pd(a4, b0, c40);
              c41 = _mm512_fmadd_pd(a4, b1, c41);

              __m512d a5 = _mm512_set1_pd(a[(i+5)*N+k]);
              c50 = _mm512_fmadd_pd(a5, b0, c50);
              c51 = _mm512_fmadd_pd(a5, b1, c51);
            }

            _mm512_storeu_pd(&c[(i+0)*N+j], c00);
            _mm512_storeu_pd(&c[(i+0)*N+j+8], c01);
            _mm512_storeu_pd(&c[(i+1)*N+j], c10);
            _mm512_storeu_pd(&c[(i+1)*N+j+8], c11);
            _mm512_storeu_pd(&c[(i+2)*N+j], c20);
            _mm512_storeu_pd(&c[(i+2)*N+j+8], c21);
            _mm512_storeu_pd(&c[(i+3)*N+j], c30);
            _mm512_storeu_pd(&c[(i+3)*N+j+8], c31);
            _mm512_storeu_pd(&c[(i+4)*N+j], c40);
            _mm512_storeu_pd(&c[(i+4)*N+j+8], c41);
            _mm512_storeu_pd(&c[(i+5)*N+j], c50);
            _mm512_storeu_pd(&c[(i+5)*N+j+8], c51);
          }
          // j remainder
          for(; j<j_end; j++){
            for(k=kk; k<k_end; k++){
              double bkj = b[k*N+j];
              c[(i+0)*N+j] += a[(i+0)*N+k] * bkj;
              c[(i+1)*N+j] += a[(i+1)*N+k] * bkj;
              c[(i+2)*N+j] += a[(i+2)*N+k] * bkj;
              c[(i+3)*N+j] += a[(i+3)*N+k] * bkj;
              c[(i+4)*N+j] += a[(i+4)*N+k] * bkj;
              c[(i+5)*N+j] += a[(i+5)*N+k] * bkj;
            }
          }
        }
        // i remainder
        for(; i<i_end; i++){
          for(k=kk; k<k_end; k++){
            double aik = a[i*N+k];
            for(j=jj; j<=j_end-8; j+=8){
              __m512d cij = _mm512_loadu_pd(&c[i*N+j]);
              __m512d bkj = _mm512_loadu_pd(&b[k*N+j]);
              __m512d aik_v = _mm512_set1_pd(aik);
              cij = _mm512_fmadd_pd(aik_v, bkj, cij);
              _mm512_storeu_pd(&c[i*N+j], cij);
            }
            for(; j<j_end; j++){
              c[i*N+j] += aik * b[k*N+j];
            }
          }
        }
      }
    }
  }
}

double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char **argv)
{
  int i, j;
  int N;
  double *a=NULL, *b=NULL, *c=NULL;
  double start, end, elapsed;
  double gflops;

  N = 1000;
  if(argc >= 2){
    N = atoi(argv[1]);
  }
  printf("N = %d\n", N);

  a = (double*)aligned_alloc(64, sizeof(double)*N*N);
  b = (double*)aligned_alloc(64, sizeof(double)*N*N);
  c = (double*)aligned_alloc(64, sizeof(double)*N*N);

  for(i=0; i<N; i++){
    for(j=0; j<N; j++){
      a[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
      b[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
      c[i*N+j] = 0.0;
    }
  }

  start = get_time();
  matmul(N, a, b, c);
  end = get_time();

  elapsed = end - start;
  gflops = 2.0 * N * N * N / (elapsed * 1e9);

  printf("Time: %.6f sec\n", elapsed);
  printf("Performance: %.2f GFLOPS\n", gflops);
  printf("Verification c[0][0]: %.2f\n", c[0]);

  free(c);
  free(b);
  free(a);

  return 0;
}
