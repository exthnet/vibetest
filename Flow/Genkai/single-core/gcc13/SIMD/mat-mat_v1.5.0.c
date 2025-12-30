/*
 * mat-mat_v1.5.0.c - Matrix Multiplication with 4x24 Micro-kernel
 * PG1.3 - GCC 13.3.1 SIMD Optimization
 *
 * v1.5.0: 4x24 micro-kernel (inspired by PG1.6's 53.5 GFLOPS approach)
 *         4 rows × 24 columns = 4 rows × 3 AVX-512 registers
 *         + Aggressive prefetching
 *         + Optimized cache blocking
 *
 * Compile: gcc -O3 -march=native -mavx512f -mfma -funroll-loops mat-mat.c -o a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

/* Cache blocking parameters */
#define MC 64
#define KC 256
#define NC 240   /* Multiple of 24 */

/* Micro-kernel dimensions */
#define MR 4
#define NR 24

/* 4x24 micro-kernel: 4 rows x 24 cols (3 AVX-512 registers per row) */
static inline void micro_kernel_4x24(int K, const double *A, int lda,
                                      const double *B, int ldb,
                                      double *C, int ldc)
{
    /* 4 rows x 3 registers = 12 accumulators */
    __m512d c00 = _mm512_setzero_pd(), c01 = _mm512_setzero_pd(), c02 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd(), c11 = _mm512_setzero_pd(), c12 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd(), c21 = _mm512_setzero_pd(), c22 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd(), c31 = _mm512_setzero_pd(), c32 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        /* Prefetch next B rows */
        if (k + 2 < K) {
            _mm_prefetch((const char*)&B[(k+2) * ldb], _MM_HINT_T0);
            _mm_prefetch((const char*)&B[(k+2) * ldb + 8], _MM_HINT_T0);
            _mm_prefetch((const char*)&B[(k+2) * ldb + 16], _MM_HINT_T0);
        }

        /* Load B row (24 elements = 3 AVX-512 registers) */
        __m512d b0 = _mm512_loadu_pd(&B[k * ldb]);
        __m512d b1 = _mm512_loadu_pd(&B[k * ldb + 8]);
        __m512d b2 = _mm512_loadu_pd(&B[k * ldb + 16]);

        /* Broadcast A elements */
        __m512d a0 = _mm512_set1_pd(A[0 * lda + k]);
        __m512d a1 = _mm512_set1_pd(A[1 * lda + k]);
        __m512d a2 = _mm512_set1_pd(A[2 * lda + k]);
        __m512d a3 = _mm512_set1_pd(A[3 * lda + k]);

        /* FMA operations */
        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c01 = _mm512_fmadd_pd(a0, b1, c01);
        c02 = _mm512_fmadd_pd(a0, b2, c02);

        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c11 = _mm512_fmadd_pd(a1, b1, c11);
        c12 = _mm512_fmadd_pd(a1, b2, c12);

        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c21 = _mm512_fmadd_pd(a2, b1, c21);
        c22 = _mm512_fmadd_pd(a2, b2, c22);

        c30 = _mm512_fmadd_pd(a3, b0, c30);
        c31 = _mm512_fmadd_pd(a3, b1, c31);
        c32 = _mm512_fmadd_pd(a3, b2, c32);
    }

    /* Store with accumulation */
    _mm512_storeu_pd(&C[0*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[0*ldc]), c00));
    _mm512_storeu_pd(&C[0*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[0*ldc+8]), c01));
    _mm512_storeu_pd(&C[0*ldc+16], _mm512_add_pd(_mm512_loadu_pd(&C[0*ldc+16]), c02));

    _mm512_storeu_pd(&C[1*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[1*ldc]), c10));
    _mm512_storeu_pd(&C[1*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[1*ldc+8]), c11));
    _mm512_storeu_pd(&C[1*ldc+16], _mm512_add_pd(_mm512_loadu_pd(&C[1*ldc+16]), c12));

    _mm512_storeu_pd(&C[2*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[2*ldc]), c20));
    _mm512_storeu_pd(&C[2*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[2*ldc+8]), c21));
    _mm512_storeu_pd(&C[2*ldc+16], _mm512_add_pd(_mm512_loadu_pd(&C[2*ldc+16]), c22));

    _mm512_storeu_pd(&C[3*ldc], _mm512_add_pd(_mm512_loadu_pd(&C[3*ldc]), c30));
    _mm512_storeu_pd(&C[3*ldc+8], _mm512_add_pd(_mm512_loadu_pd(&C[3*ldc+8]), c31));
    _mm512_storeu_pd(&C[3*ldc+16], _mm512_add_pd(_mm512_loadu_pd(&C[3*ldc+16]), c32));
}

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j, k;
    int ii, jj, kk;

    /* Blocked j-k-i loop order */
    for (jj = 0; jj < N; jj += NC) {
        int j_end = (jj + NC < N) ? jj + NC : N;

        for (kk = 0; kk < N; kk += KC) {
            int k_end = (kk + KC < N) ? kk + KC : N;
            int k_len = k_end - kk;

            for (ii = 0; ii < N; ii += MC) {
                int i_end = (ii + MC < N) ? ii + MC : N;

                /* Process 4x24 tiles */
                for (i = ii; i + MR <= i_end; i += MR) {
                    for (j = jj; j + NR <= j_end; j += NR) {
                        micro_kernel_4x24(k_len,
                                         &a[i * N + kk], N,
                                         &b[kk * N + j], N,
                                         &c[i * N + j], N);
                    }
                    /* Remaining columns (less than 24) */
                    for (; j < j_end; j++) {
                        for (int ii2 = i; ii2 < i + MR; ii2++) {
                            double sum = 0.0;
                            for (k = kk; k < k_end; k++) {
                                sum += a[ii2*N + k] * b[k*N + j];
                            }
                            c[ii2*N + j] += sum;
                        }
                    }
                }
                /* Remaining rows */
                for (; i < i_end; i++) {
                    for (j = jj; j < j_end; j++) {
                        double sum = 0.0;
                        for (k = kk; k < k_end; k++) {
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
