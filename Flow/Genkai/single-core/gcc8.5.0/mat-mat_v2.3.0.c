/* mat-mat_v2.3.0.c - AVX-512 4x16 micro-kernel (2 x 512-bit registers per row) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

#define BLOCK_I 64
#define BLOCK_K 256
#define BLOCK_J 64
#define MR 4
#define NR 16

static double packed_A[BLOCK_I * BLOCK_K] __attribute__((aligned(64)));
static double packed_B[BLOCK_K * BLOCK_J] __attribute__((aligned(64)));

static inline void pack_A(int M, int K, double *A, int lda, double *packed)
{
    for(int i=0; i<M; i+=MR){
        for(int k=0; k<K; k++){
            for(int ir=0; ir<MR && i+ir<M; ir++){
                packed[i*K + k*MR + ir] = A[(i+ir)*lda + k];
            }
        }
    }
}

static inline void pack_B(int K, int N, double *B, int ldb, double *packed)
{
    for(int j=0; j<N; j+=NR){
        for(int k=0; k<K; k++){
            for(int jr=0; jr<NR && j+jr<N; jr++){
                packed[(j/NR)*K*NR + k*NR + jr] = B[k*ldb + j + jr];
            }
        }
    }
}

// 4x16 AVX-512 micro-kernel: 2 x 512-bit registers per row = 16 elements
static inline void micro_kernel_4x16_avx512(int K, double *A, double *B, double *C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd(), c01 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd(), c11 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd(), c21 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd(), c31 = _mm512_setzero_pd();

    for(int k=0; k<K; k++){
        __m512d b0 = _mm512_load_pd(&B[k*NR]);
        __m512d b1 = _mm512_load_pd(&B[k*NR+8]);

        __m512d a0 = _mm512_set1_pd(A[k*MR+0]);
        __m512d a1 = _mm512_set1_pd(A[k*MR+1]);
        __m512d a2 = _mm512_set1_pd(A[k*MR+2]);
        __m512d a3 = _mm512_set1_pd(A[k*MR+3]);

        c00 = _mm512_fmadd_pd(a0, b0, c00); c01 = _mm512_fmadd_pd(a0, b1, c01);
        c10 = _mm512_fmadd_pd(a1, b0, c10); c11 = _mm512_fmadd_pd(a1, b1, c11);
        c20 = _mm512_fmadd_pd(a2, b0, c20); c21 = _mm512_fmadd_pd(a2, b1, c21);
        c30 = _mm512_fmadd_pd(a3, b0, c30); c31 = _mm512_fmadd_pd(a3, b1, c31);
    }

    __m512d t;
    t = _mm512_loadu_pd(&C[0*ldc]); _mm512_storeu_pd(&C[0*ldc], _mm512_add_pd(c00, t));
    t = _mm512_loadu_pd(&C[0*ldc+8]); _mm512_storeu_pd(&C[0*ldc+8], _mm512_add_pd(c01, t));
    t = _mm512_loadu_pd(&C[1*ldc]); _mm512_storeu_pd(&C[1*ldc], _mm512_add_pd(c10, t));
    t = _mm512_loadu_pd(&C[1*ldc+8]); _mm512_storeu_pd(&C[1*ldc+8], _mm512_add_pd(c11, t));
    t = _mm512_loadu_pd(&C[2*ldc]); _mm512_storeu_pd(&C[2*ldc], _mm512_add_pd(c20, t));
    t = _mm512_loadu_pd(&C[2*ldc+8]); _mm512_storeu_pd(&C[2*ldc+8], _mm512_add_pd(c21, t));
    t = _mm512_loadu_pd(&C[3*ldc]); _mm512_storeu_pd(&C[3*ldc], _mm512_add_pd(c30, t));
    t = _mm512_loadu_pd(&C[3*ldc+8]); _mm512_storeu_pd(&C[3*ldc+8], _mm512_add_pd(c31, t));
}

void matmul(int N, double *a, double *b, double *c)
{
    for(int ii=0; ii<N; ii+=BLOCK_I){
        int i_size = (ii+BLOCK_I<N) ? BLOCK_I : N-ii;
        for(int kk=0; kk<N; kk+=BLOCK_K){
            int k_size = (kk+BLOCK_K<N) ? BLOCK_K : N-kk;
            pack_A(i_size, k_size, &a[ii*N+kk], N, packed_A);

            for(int jj=0; jj<N; jj+=BLOCK_J){
                int j_size = (jj+BLOCK_J<N) ? BLOCK_J : N-jj;
                pack_B(k_size, j_size, &b[kk*N+jj], N, packed_B);

                for(int i=0; i<i_size-MR+1; i+=MR){
                    for(int j=0; j<j_size-NR+1; j+=NR){
                        micro_kernel_4x16_avx512(k_size, &packed_A[i*k_size],
                            &packed_B[(j/NR)*k_size*NR], &c[(ii+i)*N+jj+j], N);
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
    printf("N=%d AVX-512 4x16\n", N);

    double *a = aligned_alloc(64, sizeof(double)*N*N);
    double *b = aligned_alloc(64, sizeof(double)*N*N);
    double *c = aligned_alloc(64, sizeof(double)*N*N);

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
