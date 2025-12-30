#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <sys/time.h>

// キャッシュブロッキングサイズ（PG1.1参考: 48x256x256）
#define MC 48   // Aのブロック行数（MR=6の倍数）
#define KC 256  // 共通次元のブロックサイズ
#define NC 256  // Bのブロック列数（NR=16の倍数）

// マイクロカーネルサイズ
#define MR 6
#define NR 16

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// 6x16 マイクロカーネル（最適化版）
static inline void micro_kernel_6x16(int K, const double *A, int lda,
                                      const double *B, int ldb,
                                      double *C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd(), c01 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd(), c11 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd(), c21 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd(), c31 = _mm512_setzero_pd();
    __m512d c40 = _mm512_setzero_pd(), c41 = _mm512_setzero_pd();
    __m512d c50 = _mm512_setzero_pd(), c51 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        __m512d b0 = _mm512_loadu_pd(&B[k * ldb + 0]);
        __m512d b1 = _mm512_loadu_pd(&B[k * ldb + 8]);

        __m512d a0 = _mm512_set1_pd(A[0 * lda + k]);
        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c01 = _mm512_fmadd_pd(a0, b1, c01);

        __m512d a1 = _mm512_set1_pd(A[1 * lda + k]);
        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c11 = _mm512_fmadd_pd(a1, b1, c11);

        __m512d a2 = _mm512_set1_pd(A[2 * lda + k]);
        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c21 = _mm512_fmadd_pd(a2, b1, c21);

        __m512d a3 = _mm512_set1_pd(A[3 * lda + k]);
        c30 = _mm512_fmadd_pd(a3, b0, c30);
        c31 = _mm512_fmadd_pd(a3, b1, c31);

        __m512d a4 = _mm512_set1_pd(A[4 * lda + k]);
        c40 = _mm512_fmadd_pd(a4, b0, c40);
        c41 = _mm512_fmadd_pd(a4, b1, c41);

        __m512d a5 = _mm512_set1_pd(A[5 * lda + k]);
        c50 = _mm512_fmadd_pd(a5, b0, c50);
        c51 = _mm512_fmadd_pd(a5, b1, c51);
    }

    _mm512_storeu_pd(&C[0 * ldc + 0], _mm512_add_pd(_mm512_loadu_pd(&C[0 * ldc + 0]), c00));
    _mm512_storeu_pd(&C[0 * ldc + 8], _mm512_add_pd(_mm512_loadu_pd(&C[0 * ldc + 8]), c01));
    _mm512_storeu_pd(&C[1 * ldc + 0], _mm512_add_pd(_mm512_loadu_pd(&C[1 * ldc + 0]), c10));
    _mm512_storeu_pd(&C[1 * ldc + 8], _mm512_add_pd(_mm512_loadu_pd(&C[1 * ldc + 8]), c11));
    _mm512_storeu_pd(&C[2 * ldc + 0], _mm512_add_pd(_mm512_loadu_pd(&C[2 * ldc + 0]), c20));
    _mm512_storeu_pd(&C[2 * ldc + 8], _mm512_add_pd(_mm512_loadu_pd(&C[2 * ldc + 8]), c21));
    _mm512_storeu_pd(&C[3 * ldc + 0], _mm512_add_pd(_mm512_loadu_pd(&C[3 * ldc + 0]), c30));
    _mm512_storeu_pd(&C[3 * ldc + 8], _mm512_add_pd(_mm512_loadu_pd(&C[3 * ldc + 8]), c31));
    _mm512_storeu_pd(&C[4 * ldc + 0], _mm512_add_pd(_mm512_loadu_pd(&C[4 * ldc + 0]), c40));
    _mm512_storeu_pd(&C[4 * ldc + 8], _mm512_add_pd(_mm512_loadu_pd(&C[4 * ldc + 8]), c41));
    _mm512_storeu_pd(&C[5 * ldc + 0], _mm512_add_pd(_mm512_loadu_pd(&C[5 * ldc + 0]), c50));
    _mm512_storeu_pd(&C[5 * ldc + 8], _mm512_add_pd(_mm512_loadu_pd(&C[5 * ldc + 8]), c51));
}

void matmul(int N, double *a, double *b, double *c)
{
    int i, j;
    int ii, jj, kk;

    for (jj = 0; jj < N; jj += NC) {
        int jb = (jj + NC <= N) ? NC : N - jj;

        for (kk = 0; kk < N; kk += KC) {
            int kb = (kk + KC <= N) ? KC : N - kk;

            for (ii = 0; ii < N; ii += MC) {
                int ib = (ii + MC <= N) ? MC : N - ii;

                // マイクロカーネルでの処理
                for (i = ii; i + MR <= ii + ib; i += MR) {
                    for (j = jj; j + NR <= jj + jb; j += NR) {
                        micro_kernel_6x16(kb,
                                          &a[i * N + kk], N,
                                          &b[kk * N + j], N,
                                          &c[i * N + j], N);
                    }
                    // 端数処理（j方向）
                    for (; j < jj + jb; j++) {
                        double sum = 0.0;
                        for (int k = kk; k < kk + kb; k++) {
                            for (int ii2 = i; ii2 < i + MR && ii2 < N; ii2++) {
                                c[ii2 * N + j] += a[ii2 * N + k] * b[k * N + j];
                            }
                        }
                    }
                }
                // 端数処理（i方向）
                for (; i < ii + ib; i++) {
                    for (j = jj; j < jj + jb; j++) {
                        for (int k = kk; k < kk + kb; k++) {
                            c[i * N + j] += a[i * N + k] * b[k * N + j];
                        }
                    }
                }
            }
        }
    }
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

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i * N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            b[i * N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            c[i * N + j] = 0.0;
        }
    }

    start = get_time();
    matmul(N, a, b, c);
    end = get_time();

    elapsed = end - start;
    gflops = (2.0 * N * N * N) / (elapsed * 1e9);

    printf("Elapsed time: %.6f sec\n", elapsed);
    printf("Performance: %.1f GFLOPS\n", gflops);
    printf("Efficiency: %.1f%% (vs 80 GFLOPS peak)\n", gflops / 80.0 * 100.0);

    if (out == 1) {
        printf("result:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf(" %8.2f", c[i * N + j]);
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
