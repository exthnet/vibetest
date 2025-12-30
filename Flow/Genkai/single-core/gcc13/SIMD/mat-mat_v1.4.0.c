/*
 * mat-mat_v1.4.0.c - Matrix Multiplication with Data Packing
 * PG1.3 - GCC 13.3.1 SIMD Optimization
 *
 * v1.4.0: Pack B matrix for contiguous memory access
 *         + 6x16 micro-kernel with better register utilization
 *         + Optimized loop ordering (j-k-i for better B reuse)
 *
 * Compile: gcc -O3 -march=native -mavx512f -mfma -funroll-loops mat-mat.c -o a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <immintrin.h>

/* Cache blocking parameters */
#define MC 48
#define KC 256
#define NC 256

/* Micro-kernel dimensions */
#define MR 6
#define NR 16

/* Packed B buffer (KC x NC) */
static double *Bp = NULL;

/* Pack a KC x NR panel of B into contiguous memory */
static inline void pack_B(int K, int N, const double *B, int ldb, double *Bp, int nr)
{
    for (int k = 0; k < K; k++) {
        for (int j = 0; j < nr; j++) {
            Bp[k * NR + j] = B[k * ldb + j];
        }
        for (int j = nr; j < NR; j++) {
            Bp[k * NR + j] = 0.0;
        }
    }
}

/* 6x16 micro-kernel with packed B */
static inline void micro_kernel_6x16_packed(int K, const double *A, int lda,
                                             const double *Bp,
                                             double *C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd(), c01 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd(), c11 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd(), c21 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd(), c31 = _mm512_setzero_pd();
    __m512d c40 = _mm512_setzero_pd(), c41 = _mm512_setzero_pd();
    __m512d c50 = _mm512_setzero_pd(), c51 = _mm512_setzero_pd();

    const double *Bp_ptr = Bp;

    for (int k = 0; k < K; k++) {
        /* Prefetch */
        _mm_prefetch((const char*)(Bp_ptr + 2*NR), _MM_HINT_T0);

        /* Load packed B (contiguous) */
        __m512d b0 = _mm512_load_pd(Bp_ptr);
        __m512d b1 = _mm512_load_pd(Bp_ptr + 8);
        Bp_ptr += NR;

        __m512d a0 = _mm512_set1_pd(A[0 * lda + k]);
        __m512d a1 = _mm512_set1_pd(A[1 * lda + k]);
        __m512d a2 = _mm512_set1_pd(A[2 * lda + k]);
        __m512d a3 = _mm512_set1_pd(A[3 * lda + k]);
        __m512d a4 = _mm512_set1_pd(A[4 * lda + k]);
        __m512d a5 = _mm512_set1_pd(A[5 * lda + k]);

        c00 = _mm512_fmadd_pd(a0, b0, c00); c01 = _mm512_fmadd_pd(a0, b1, c01);
        c10 = _mm512_fmadd_pd(a1, b0, c10); c11 = _mm512_fmadd_pd(a1, b1, c11);
        c20 = _mm512_fmadd_pd(a2, b0, c20); c21 = _mm512_fmadd_pd(a2, b1, c21);
        c30 = _mm512_fmadd_pd(a3, b0, c30); c31 = _mm512_fmadd_pd(a3, b1, c31);
        c40 = _mm512_fmadd_pd(a4, b0, c40); c41 = _mm512_fmadd_pd(a4, b1, c41);
        c50 = _mm512_fmadd_pd(a5, b0, c50); c51 = _mm512_fmadd_pd(a5, b1, c51);
    }

    /* Store with accumulation */
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
}

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j, k;
    int ii, jj, kk;

    /* Allocate packing buffer */
    if (Bp == NULL) {
        Bp = (double*)aligned_alloc(64, KC * NR * sizeof(double));
    }

    /* Optimized loop order: j-k-i for better B reuse */
    for (jj = 0; jj < N; jj += NC) {
        int j_end = (jj + NC < N) ? jj + NC : N;

        for (kk = 0; kk < N; kk += KC) {
            int k_end = (kk + KC < N) ? kk + KC : N;
            int k_len = k_end - kk;

            /* Process NR-wide panels */
            for (j = jj; j + NR <= j_end; j += NR) {
                /* Pack B panel */
                pack_B(k_len, N, &b[kk * N + j], N, Bp, NR);

                /* Process all rows with this packed B */
                for (ii = 0; ii < N; ii += MC) {
                    int i_end = (ii + MC < N) ? ii + MC : N;

                    for (i = ii; i + MR <= i_end; i += MR) {
                        micro_kernel_6x16_packed(k_len,
                                                &a[i * N + kk], N,
                                                Bp,
                                                &c[i * N + j], N);
                    }
                    /* Remaining rows */
                    for (; i < i_end; i++) {
                        for (int jj2 = 0; jj2 < NR; jj2++) {
                            double sum = 0.0;
                            for (k = kk; k < k_end; k++) {
                                sum += a[i*N + k] * b[k*N + j + jj2];
                            }
                            c[i*N + j + jj2] += sum;
                        }
                    }
                }
            }
            /* Remaining columns */
            for (; j < j_end; j++) {
                for (i = 0; i < N; i++) {
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

    if (Bp) free(Bp);
    free(c);
    free(b);
    free(a);

    return 0;
}
