#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

// SIMD optimized matrix multiplication with loop reordering (i,k,j)
// and AVX2 vectorization
void matmul(int N, double *a, double *b, double *c)
{
  int i, j, k;
  
  // Loop reorder: i,k,j for better memory access pattern
  for(i=0; i<N; i++){
    for(k=0; k<N; k++){
      double aik = a[i*N+k];
      __m256d va = _mm256_set1_pd(aik);
      
      // Process 4 doubles at a time with AVX2
      for(j=0; j<N-3; j+=4){
        __m256d vb = _mm256_loadu_pd(&b[k*N+j]);
        __m256d vc = _mm256_loadu_pd(&c[i*N+j]);
        vc = _mm256_fmadd_pd(va, vb, vc);
        _mm256_storeu_pd(&c[i*N+j], vc);
      }
      // Handle remaining elements
      for(; j<N; j++){
        c[i*N+j] += aik * b[k*N+j];
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

  if(argc < 2){
    N = 1000;
  }else{
    N = atoi(argv[1]);
  }
  printf("N = %d\n", N);

  // Aligned allocation for better SIMD performance
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

  free(c);
  free(b);
  free(a);

  return 0;
}
