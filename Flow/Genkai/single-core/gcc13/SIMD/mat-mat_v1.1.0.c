/*
 * mat-mat_v1.1.0.c - Matrix Multiplication with AVX-512 + Cache Blocking
 * PG1.3 - GCC 13.3.1 SIMD Optimization
 *
 * v1.1.0: AVX-512 intrinsics with 8-wide FMA
 *         + Cache blocking for L2 cache optimization
 *         + Loop unrolling (4x8 micro-kernel)
 *
 * Compile: gcc -O3 -march=native -mavx512f -mfma -funroll-loops mat-mat.c -o a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <immintrin.h>

/* Block size tuned for L2 cache (2MB per core) */
#define BLOCK_SIZE 64

/* 4x8 micro-kernel using AVX-512 */
static inline void micro_kernel_4x8(int K, const double *A, int lda,
                                     const double *B, int ldb,
                                     double *C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        __m512d b0 = _mm512_loadu_pd(&B[k * ldb]);

        __m512d a0 = _mm512_set1_pd(A[0 * lda + k]);
        __m512d a1 = _mm512_set1_pd(A[1 * lda + k]);
        __m512d a2 = _mm512_set1_pd(A[2 * lda + k]);
        __m512d a3 = _mm512_set1_pd(A[3 * lda + k]);

        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c30 = _mm512_fmadd_pd(a3, b0, c30);
    }

    /* Store results */
    __m512d r0 = _mm512_loadu_pd(&C[0 * ldc]);
    __m512d r1 = _mm512_loadu_pd(&C[1 * ldc]);
    __m512d r2 = _mm512_loadu_pd(&C[2 * ldc]);
    __m512d r3 = _mm512_loadu_pd(&C[3 * ldc]);

    _mm512_storeu_pd(&C[0 * ldc], _mm512_add_pd(r0, c00));
    _mm512_storeu_pd(&C[1 * ldc], _mm512_add_pd(r1, c10));
    _mm512_storeu_pd(&C[2 * ldc], _mm512_add_pd(r2, c20));
    _mm512_storeu_pd(&C[3 * ldc], _mm512_add_pd(r3, c30));
}

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j, k;
    int ii, jj, kk;
    int BS = BLOCK_SIZE;

    /* Blocked matrix multiplication */
    for (ii = 0; ii < N; ii += BS) {
        int i_end = (ii + BS < N) ? ii + BS : N;
        for (kk = 0; kk < N; kk += BS) {
            int k_end = (kk + BS < N) ? kk + BS : N;
            for (jj = 0; jj < N; jj += BS) {
                int j_end = (jj + BS < N) ? jj + BS : N;

                /* Process 4x8 tiles using AVX-512 */
                for (i = ii; i + 4 <= i_end; i += 4) {
                    for (j = jj; j + 8 <= j_end; j += 8) {
                        micro_kernel_4x8(k_end - kk,
                                        &a[i * N + kk], N,
                                        &b[kk * N + j], N,
                                        &c[i * N + j], N);
                    }
                    /* Handle remaining columns */
                    for (; j < j_end; j++) {
                        for (k = kk; k < k_end; k++) {
                            c[i*N + j] += a[i*N + k] * b[k*N + j];
                            c[(i+1)*N + j] += a[(i+1)*N + k] * b[k*N + j];
                            c[(i+2)*N + j] += a[(i+2)*N + k] * b[k*N + j];
                            c[(i+3)*N + j] += a[(i+3)*N + k] * b[k*N + j];
                        }
                    }
                }
                /* Handle remaining rows */
                for (; i < i_end; i++) {
                    for (j = jj; j < j_end; j++) {
                        for (k = kk; k < k_end; k++) {
                            c[i*N + j] += a[i*N + k] * b[k*N + j];
                        }
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
