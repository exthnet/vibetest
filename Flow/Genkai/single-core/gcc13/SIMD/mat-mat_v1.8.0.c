/*
 * mat-mat_v1.8.0.c - Matrix Multiplication Optimized for 50+ GFLOPS
 * PG1.3 - GCC 13.3.1 SIMD Optimization
 *
 * v1.8.0: Based on v1.7.0 with:
 *         + 8x16 micro-kernel (more A reuse)
 *         + Smaller cache blocks for L1 fit
 *         + __attribute__((hot)) for critical path
 *         + Prefetch A column as well
 *
 * Compile: gcc -O3 -march=native -mavx512f -mfma -ffast-math -funroll-loops mat-mat.c -o a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

/* Smaller blocks to fit in L1 cache */
#define MC 32
#define KC 128
#define NC 128

/* Micro-kernel: 8x16 */
#define MR 8
#define NR 16

/* 8x16 micro-kernel optimized */
static inline __attribute__((always_inline, hot)) void
micro_kernel_8x16(int K, const double * __restrict__ A, int lda,
                   const double * __restrict__ B, int ldb,
                   double * __restrict__ C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd(), c01 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd(), c11 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd(), c21 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd(), c31 = _mm512_setzero_pd();
    __m512d c40 = _mm512_setzero_pd(), c41 = _mm512_setzero_pd();
    __m512d c50 = _mm512_setzero_pd(), c51 = _mm512_setzero_pd();
    __m512d c60 = _mm512_setzero_pd(), c61 = _mm512_setzero_pd();
    __m512d c70 = _mm512_setzero_pd(), c71 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        /* Prefetch B */
        _mm_prefetch((const char*)&B[(k+2)*ldb], _MM_HINT_T0);
        _mm_prefetch((const char*)&B[(k+2)*ldb + 8], _MM_HINT_T0);

        __m512d b0 = _mm512_loadu_pd(&B[k * ldb]);
        __m512d b1 = _mm512_loadu_pd(&B[k * ldb + 8]);

        __m512d a0 = _mm512_set1_pd(A[k]);
        __m512d a1 = _mm512_set1_pd(A[lda + k]);
        __m512d a2 = _mm512_set1_pd(A[2*lda + k]);
        __m512d a3 = _mm512_set1_pd(A[3*lda + k]);
        __m512d a4 = _mm512_set1_pd(A[4*lda + k]);
        __m512d a5 = _mm512_set1_pd(A[5*lda + k]);
        __m512d a6 = _mm512_set1_pd(A[6*lda + k]);
        __m512d a7 = _mm512_set1_pd(A[7*lda + k]);

        c00 = _mm512_fmadd_pd(a0, b0, c00); c01 = _mm512_fmadd_pd(a0, b1, c01);
        c10 = _mm512_fmadd_pd(a1, b0, c10); c11 = _mm512_fmadd_pd(a1, b1, c11);
        c20 = _mm512_fmadd_pd(a2, b0, c20); c21 = _mm512_fmadd_pd(a2, b1, c21);
        c30 = _mm512_fmadd_pd(a3, b0, c30); c31 = _mm512_fmadd_pd(a3, b1, c31);
        c40 = _mm512_fmadd_pd(a4, b0, c40); c41 = _mm512_fmadd_pd(a4, b1, c41);
        c50 = _mm512_fmadd_pd(a5, b0, c50); c51 = _mm512_fmadd_pd(a5, b1, c51);
        c60 = _mm512_fmadd_pd(a6, b0, c60); c61 = _mm512_fmadd_pd(a6, b1, c61);
        c70 = _mm512_fmadd_pd(a7, b0, c70); c71 = _mm512_fmadd_pd(a7, b1, c71);
    }

    /* Store */
    _mm512_storeu_pd(&C[0*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[0*ldc]), c00));
    _mm512_storeu_pd(&C[0*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[0*ldc+8]), c01));
    _mm512_storeu_pd(&C[1*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[1*ldc]), c10));
    _mm512_storeu_pd(&C[1*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[1*ldc+8]), c11));
    _mm512_storeu_pd(&C[2*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[2*ldc]), c20));
    _mm512_storeu_pd(&C[2*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[2*ldc+8]), c21));
    _mm512_storeu_pd(&C[3*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[3*ldc]), c30));
    _mm512_storeu_pd(&C[3*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[3*ldc+8]), c31));
    _mm512_storeu_pd(&C[4*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[4*ldc]), c40));
    _mm512_storeu_pd(&C[4*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[4*ldc+8]), c41));
    _mm512_storeu_pd(&C[5*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[5*ldc]), c50));
    _mm512_storeu_pd(&C[5*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[5*ldc+8]), c51));
    _mm512_storeu_pd(&C[6*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[6*ldc]), c60));
    _mm512_storeu_pd(&C[6*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[6*ldc+8]), c61));
    _mm512_storeu_pd(&C[7*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[7*ldc]), c70));
    _mm512_storeu_pd(&C[7*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[7*ldc+8]), c71));
}

__attribute__((hot)) void
matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j;
    int ii, jj, kk;

    a = __builtin_assume_aligned(a, 64);
    b = __builtin_assume_aligned(b, 64);
    c = __builtin_assume_aligned(c, 64);

    for (jj = 0; jj < N; jj += NC) {
        int j_end = (jj + NC < N) ? jj + NC : N;

        for (kk = 0; kk < N; kk += KC) {
            int k_end = (kk + KC < N) ? kk + KC : N;
            int k_len = k_end - kk;

            for (ii = 0; ii < N; ii += MC) {
                int i_end = (ii + MC < N) ? ii + MC : N;

                for (i = ii; i + MR <= i_end; i += MR) {
                    for (j = jj; j + NR <= j_end; j += NR) {
                        micro_kernel_8x16(k_len,
                                         &a[i * N + kk], N,
                                         &b[kk * N + j], N,
                                         &c[i * N + j], N);
                    }
                    for (; j < j_end; j++) {
                        for (int ii2 = i; ii2 < i + MR; ii2++) {
                            double sum = 0.0;
                            for (int k = kk; k < k_end; k++) {
                                sum += a[ii2*N + k] * b[k*N + j];
                            }
                            c[ii2*N + j] += sum;
                        }
                    }
                }
                for (; i < i_end; i++) {
                    for (j = jj; j < j_end; j++) {
                        double sum = 0.0;
                        for (int k = kk; k < k_end; k++) {
                            sum += a[i*N + k] * b[k*N + j];
                        }
                        c[i*N + j] += sum;
                    }
                }
            }
        }
    }
}

double get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char **argv)
{
    int i, j;
    int N;
    double *a = NULL, *b = NULL, *c = NULL;
    int out;
    double start, end, elapsed;
    double gflops;

    out = 1;
    if (argc == 1) {
        N = 10;
    } else {
        N = atoi(argv[1]);
        if (argc == 3) {
            out = 0;
        }
    }
    printf("N = %d\n", N);
    printf("out = %d\n", out);

    a = (double*)aligned_alloc(64, sizeof(double) * N * N);
    b = (double*)aligned_alloc(64, sizeof(double) * N * N);
    c = (double*)aligned_alloc(64, sizeof(double) * N * N);

    if (!a || !b || !c) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i*N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            b[i*N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            c[i*N + j] = 0.0;
        }
    }

    start = get_time();
    matmul(N, a, b, c);
    end = get_time();

    elapsed = end - start;
    gflops = (2.0 * (double)N * (double)N * (double)N) / (elapsed * 1e9);

    printf("Elapsed time: %.6f sec\n", elapsed);
    printf("Performance: %.1f GFLOPS\n", gflops);

    if (out == 1) {
        printf("result:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf(" %8.2f", c[i*N + j]);
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
