#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

#define BLOCK_SIZE 64
#define UNROLL 4

// SIMD + cache blocking + loop unrolling
void matmul(int N, double *a, double *b, double *c)
{
  int i, j, k, ii, jj, kk;
  
  for(ii=0; ii<N; ii+=BLOCK_SIZE){
    for(kk=0; kk<N; kk+=BLOCK_SIZE){
      for(jj=0; jj<N; jj+=BLOCK_SIZE){
        int i_end = (ii+BLOCK_SIZE < N) ? ii+BLOCK_SIZE : N;
        int k_end = (kk+BLOCK_SIZE < N) ? kk+BLOCK_SIZE : N;
        int j_end = (jj+BLOCK_SIZE < N) ? jj+BLOCK_SIZE : N;
        
        // Unroll i-loop by 4
        for(i=ii; i<i_end-3; i+=UNROLL){
          for(k=kk; k<k_end; k++){
            double a0 = a[(i+0)*N+k];
            double a1 = a[(i+1)*N+k];
            double a2 = a[(i+2)*N+k];
            double a3 = a[(i+3)*N+k];
            __m256d va0 = _mm256_set1_pd(a0);
            __m256d va1 = _mm256_set1_pd(a1);
            __m256d va2 = _mm256_set1_pd(a2);
            __m256d va3 = _mm256_set1_pd(a3);
            
            for(j=jj; j<j_end-3; j+=4){
              __m256d vb = _mm256_loadu_pd(&b[k*N+j]);
              
              __m256d vc0 = _mm256_loadu_pd(&c[(i+0)*N+j]);
              __m256d vc1 = _mm256_loadu_pd(&c[(i+1)*N+j]);
              __m256d vc2 = _mm256_loadu_pd(&c[(i+2)*N+j]);
              __m256d vc3 = _mm256_loadu_pd(&c[(i+3)*N+j]);
              
              vc0 = _mm256_fmadd_pd(va0, vb, vc0);
              vc1 = _mm256_fmadd_pd(va1, vb, vc1);
              vc2 = _mm256_fmadd_pd(va2, vb, vc2);
              vc3 = _mm256_fmadd_pd(va3, vb, vc3);
              
              _mm256_storeu_pd(&c[(i+0)*N+j], vc0);
              _mm256_storeu_pd(&c[(i+1)*N+j], vc1);
              _mm256_storeu_pd(&c[(i+2)*N+j], vc2);
              _mm256_storeu_pd(&c[(i+3)*N+j], vc3);
            }
            for(; j<j_end; j++){
              c[(i+0)*N+j] += a0 * b[k*N+j];
              c[(i+1)*N+j] += a1 * b[k*N+j];
              c[(i+2)*N+j] += a2 * b[k*N+j];
              c[(i+3)*N+j] += a3 * b[k*N+j];
            }
          }
        }
        // Remainder for i
        for(; i<i_end; i++){
          for(k=kk; k<k_end; k++){
            double aik = a[i*N+k];
            __m256d va = _mm256_set1_pd(aik);
            for(j=jj; j<j_end-3; j+=4){
              __m256d vb = _mm256_loadu_pd(&b[k*N+j]);
              __m256d vc = _mm256_loadu_pd(&c[i*N+j]);
              vc = _mm256_fmadd_pd(va, vb, vc);
              _mm256_storeu_pd(&c[i*N+j], vc);
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
  int i, j, N;
  double *a, *b, *c;
  double start, end, elapsed, gflops;

  N = (argc < 2) ? 1000 : atoi(argv[1]);
  printf("N = %d, BLOCK_SIZE = %d, UNROLL = %d\n", N, BLOCK_SIZE, UNROLL);

  a = (double*)aligned_alloc(32, sizeof(double)*N*N);
  b = (double*)aligned_alloc(32, sizeof(double)*N*N);
  c = (double*)aligned_alloc(32, sizeof(double)*N*N);

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
  gflops = (2.0 * N * N * N) / elapsed / 1e9;

  printf("Time: %.6f sec\n", elapsed);
  printf("Performance: %.2f GFLOPS\n", gflops);
  printf("Checksum c[0][0]: %.6f\n", c[0]);
  printf("Checksum c[N-1][N-1]: %.6f\n", c[(N-1)*N+(N-1)]);

  free(c); free(b); free(a);
  return 0;
}
