#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

#define BLOCK_I 64
#define BLOCK_K 256
#define BLOCK_J 64
#define MR 4
#define NR 8

// Micro-kernel: 4x8 register blocking
static inline void micro_kernel_4x8(int K, double *A, int lda, double *B, int ldb, double *C, int ldc)
{
    __m256d c00 = _mm256_setzero_pd();
    __m256d c01 = _mm256_setzero_pd();
    __m256d c10 = _mm256_setzero_pd();
    __m256d c11 = _mm256_setzero_pd();
    __m256d c20 = _mm256_setzero_pd();
    __m256d c21 = _mm256_setzero_pd();
    __m256d c30 = _mm256_setzero_pd();
    __m256d c31 = _mm256_setzero_pd();

    for(int k=0; k<K; k++){
        __m256d b0 = _mm256_loadu_pd(&B[k*ldb]);
        __m256d b1 = _mm256_loadu_pd(&B[k*ldb+4]);
        
        __m256d a0 = _mm256_set1_pd(A[0*lda+k]);
        __m256d a1 = _mm256_set1_pd(A[1*lda+k]);
        __m256d a2 = _mm256_set1_pd(A[2*lda+k]);
        __m256d a3 = _mm256_set1_pd(A[3*lda+k]);
        
        c00 = _mm256_fmadd_pd(a0, b0, c00);
        c01 = _mm256_fmadd_pd(a0, b1, c01);
        c10 = _mm256_fmadd_pd(a1, b0, c10);
        c11 = _mm256_fmadd_pd(a1, b1, c11);
        c20 = _mm256_fmadd_pd(a2, b0, c20);
        c21 = _mm256_fmadd_pd(a2, b1, c21);
        c30 = _mm256_fmadd_pd(a3, b0, c30);
        c31 = _mm256_fmadd_pd(a3, b1, c31);
    }
    
    __m256d t;
    t = _mm256_loadu_pd(&C[0*ldc]); c00 = _mm256_add_pd(c00, t); _mm256_storeu_pd(&C[0*ldc], c00);
    t = _mm256_loadu_pd(&C[0*ldc+4]); c01 = _mm256_add_pd(c01, t); _mm256_storeu_pd(&C[0*ldc+4], c01);
    t = _mm256_loadu_pd(&C[1*ldc]); c10 = _mm256_add_pd(c10, t); _mm256_storeu_pd(&C[1*ldc], c10);
    t = _mm256_loadu_pd(&C[1*ldc+4]); c11 = _mm256_add_pd(c11, t); _mm256_storeu_pd(&C[1*ldc+4], c11);
    t = _mm256_loadu_pd(&C[2*ldc]); c20 = _mm256_add_pd(c20, t); _mm256_storeu_pd(&C[2*ldc], c20);
    t = _mm256_loadu_pd(&C[2*ldc+4]); c21 = _mm256_add_pd(c21, t); _mm256_storeu_pd(&C[2*ldc+4], c21);
    t = _mm256_loadu_pd(&C[3*ldc]); c30 = _mm256_add_pd(c30, t); _mm256_storeu_pd(&C[3*ldc], c30);
    t = _mm256_loadu_pd(&C[3*ldc+4]); c31 = _mm256_add_pd(c31, t); _mm256_storeu_pd(&C[3*ldc+4], c31);
}

void matmul(int N, double *a, double *b, double *c)
{
    for(int ii=0; ii<N; ii+=BLOCK_I){
        for(int kk=0; kk<N; kk+=BLOCK_K){
            for(int jj=0; jj<N; jj+=BLOCK_J){
                int i_end = (ii+BLOCK_I<N) ? ii+BLOCK_I : N;
                int k_end = (kk+BLOCK_K<N) ? kk+BLOCK_K : N;
                int j_end = (jj+BLOCK_J<N) ? jj+BLOCK_J : N;
                int K_block = k_end - kk;
                
                for(int i=ii; i<i_end-MR+1; i+=MR){
                    for(int j=jj; j<j_end-NR+1; j+=NR){
                        micro_kernel_4x8(K_block, &a[i*N+kk], N, &b[kk*N+j], N, &c[i*N+j], N);
                    }
                    // Remainder j
                    for(int j=(j_end/NR)*NR; j<j_end; j++){
                        for(int k=kk; k<k_end; k++){
                            for(int ir=0; ir<MR && i+ir<i_end; ir++){
                                c[(i+ir)*N+j] += a[(i+ir)*N+k] * b[k*N+j];
                            }
                        }
                    }
                }
                // Remainder i
                for(int i=(i_end/MR)*MR; i<i_end; i++){
                    for(int k=kk; k<k_end; k++){
                        double aik = a[i*N+k];
                        for(int j=jj; j<j_end; j++){
                            c[i*N+j] += aik * b[k*N+j];
                        }
                    }
                }
            }
        }
    }
}

double get_time() { struct timeval tv; gettimeofday(&tv, NULL); return tv.tv_sec + tv.tv_usec*1e-6; }

int main(int argc, char **argv)
{
    int N = (argc<2) ? 1000 : atoi(argv[1]);
    printf("N=%d, BLOCK_I=%d, BLOCK_K=%d, BLOCK_J=%d, MR=%d, NR=%d\n", N, BLOCK_I, BLOCK_K, BLOCK_J, MR, NR);

    double *a = aligned_alloc(32, sizeof(double)*N*N);
    double *b = aligned_alloc(32, sizeof(double)*N*N);
    double *c = aligned_alloc(32, sizeof(double)*N*N);

    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            a[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
            b[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
            c[i*N+j] = 0.0;
        }
    }

    double start = get_time();
    matmul(N, a, b, c);
    double elapsed = get_time() - start;
    double gflops = (2.0*N*N*N) / elapsed / 1e9;

    printf("Time: %.6f sec\nPerformance: %.2f GFLOPS\n", elapsed, gflops);
    printf("Checksum c[0][0]: %.6f\nChecksum c[N-1][N-1]: %.6f\n", c[0], c[(N-1)*N+(N-1)]);

    free(c); free(b); free(a);
    return 0;
}
