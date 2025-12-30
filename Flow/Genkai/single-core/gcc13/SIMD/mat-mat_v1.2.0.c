/*
 * mat-mat_v1.2.0.c - Matrix Multiplication with AVX-512 + Optimized Blocking
 * PG1.3 - GCC 13.3.1 SIMD Optimization
 *
 * v1.2.0: Larger register blocking (8x8 micro-kernel)
 *         + Software prefetching
 *         + Block size tuned for L2 cache (48KB effective)
 *
 * Compile: gcc -O3 -march=native -mavx512f -mfma -funroll-loops mat-mat.c -o a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

/* Block size tuned for L2 cache */
#define BLOCK_SIZE 48

/* 8x8 micro-kernel using AVX-512 with software prefetching */
static inline void micro_kernel_8x8(int K, const double *A, int lda,
                                     const double *B, int ldb,
                                     double *C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd();
    __m512d c40 = _mm512_setzero_pd();
    __m512d c50 = _mm512_setzero_pd();
    __m512d c60 = _mm512_setzero_pd();
    __m512d c70 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        /* Prefetch next iteration's B row */
        if (k + 2 < K) {
            _mm_prefetch((const char*)&B[(k+2) * ldb], _MM_HINT_T0);
        }

        __m512d b0 = _mm512_loadu_pd(&B[k * ldb]);

        __m512d a0 = _mm512_set1_pd(A[0 * lda + k]);
        __m512d a1 = _mm512_set1_pd(A[1 * lda + k]);
        __m512d a2 = _mm512_set1_pd(A[2 * lda + k]);
        __m512d a3 = _mm512_set1_pd(A[3 * lda + k]);
        __m512d a4 = _mm512_set1_pd(A[4 * lda + k]);
        __m512d a5 = _mm512_set1_pd(A[5 * lda + k]);
        __m512d a6 = _mm512_set1_pd(A[6 * lda + k]);
        __m512d a7 = _mm512_set1_pd(A[7 * lda + k]);

        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c30 = _mm512_fmadd_pd(a3, b0, c30);
        c40 = _mm512_fmadd_pd(a4, b0, c40);
        c50 = _mm512_fmadd_pd(a5, b0, c50);
        c60 = _mm512_fmadd_pd(a6, b0, c60);
        c70 = _mm512_fmadd_pd(a7, b0, c70);
    }

    /* Store results with accumulation */
    _mm512_storeu_pd(&C[0 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[0 * ldc]), c00));
    _mm512_storeu_pd(&C[1 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[1 * ldc]), c10));
    _mm512_storeu_pd(&C[2 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[2 * ldc]), c20));
    _mm512_storeu_pd(&C[3 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[3 * ldc]), c30));
    _mm512_storeu_pd(&C[4 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[4 * ldc]), c40));
    _mm512_storeu_pd(&C[5 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[5 * ldc]), c50));
    _mm512_storeu_pd(&C[6 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[6 * ldc]), c60));
    _mm512_storeu_pd(&C[7 * ldc], _mm512_add_pd(_mm512_loadu_pd(&C[7 * ldc]), c70));
}

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j, k;
    int ii, jj, kk;
    int BS = BLOCK_SIZE;

    /* Blocked matrix multiplication with 8x8 tiles */
    for (ii = 0; ii < N; ii += BS) {
        int i_end = (ii + BS < N) ? ii + BS : N;
        for (kk = 0; kk < N; kk += BS) {
            int k_end = (kk + BS < N) ? kk + BS : N;
            for (jj = 0; jj < N; jj += BS) {
                int j_end = (jj + BS < N) ? jj + BS : N;

                /* Process 8x8 tiles using AVX-512 */
                for (i = ii; i + 8 <= i_end; i += 8) {
                    for (j = jj; j + 8 <= j_end; j += 8) {
                        micro_kernel_8x8(k_end - kk,
                                        &a[i * N + kk], N,
                                        &b[kk * N + j], N,
                                        &c[i * N + j], N);
                    }
                    /* Handle remaining columns (less than 8) */
                    for (; j < j_end; j++) {
                        for (int ii2 = i; ii2 < i + 8; ii2++) {
                            double sum = 0.0;
                            for (k = kk; k < k_end; k++) {
                                sum += a[ii2*N + k] * b[k*N + j];
                            }
                            c[ii2*N + j] += sum;
                        }
                    }
                }
                /* Handle remaining rows (less than 8) */
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

    /* Aligned memory allocation for AVX-512 (64-byte alignment) */
    a = (double*)aligned_alloc(64, sizeof(double) * N * N);
    b = (double*)aligned_alloc(64, sizeof(double) * N * N);
    c = (double*)aligned_alloc(64, sizeof(double) * N * N);

    if (!a || !b || !c) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Initialize matrices */
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i*N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            b[i*N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            c[i*N + j] = 0.0;
        }
    }

    /* Main computation with timing */
    start = get_time();
    matmul(N, a, b, c);
    end = get_time();

    elapsed = end - start;
    gflops = (2.0 * (double)N * (double)N * (double)N) / (elapsed * 1e9);

    printf("Elapsed time: %.6f sec\n", elapsed);
    printf("Performance: %.1f GFLOPS\n", gflops);

    /* Result output (optional) */
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
