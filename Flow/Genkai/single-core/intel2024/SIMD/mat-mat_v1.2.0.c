#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <sys/time.h>

// キャッシュブロッキングサイズ
#define MC 256
#define KC 256
#define NC 512

// マイクロカーネルサイズ
#define MR 6
#define NR 16

// パック用バッファ（静的確保でアライメント保証）
static double packed_A[MC * KC] __attribute__((aligned(64)));
static double packed_B[KC * NC] __attribute__((aligned(64)));

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Aのパッキング: MC×KC ブロックを MR単位で連続配置
static void pack_A(int M, int K, const double *A, int lda, double *packed)
{
    for (int i = 0; i < M; i += MR) {
        int mr = (i + MR <= M) ? MR : M - i;
        for (int k = 0; k < K; k++) {
            for (int ii = 0; ii < mr; ii++) {
                packed[ii] = A[(i + ii) * lda + k];
            }
            for (int ii = mr; ii < MR; ii++) {
                packed[ii] = 0.0;  // パディング
            }
            packed += MR;
        }
    }
}

// Bのパッキング: KC×NC ブロックを NR単位で連続配置
static void pack_B(int K, int N, const double *B, int ldb, double *packed)
{
    for (int j = 0; j < N; j += NR) {
        int nr = (j + NR <= N) ? NR : N - j;
        for (int k = 0; k < K; k++) {
            for (int jj = 0; jj < nr; jj++) {
                packed[jj] = B[k * ldb + j + jj];
            }
            for (int jj = nr; jj < NR; jj++) {
                packed[jj] = 0.0;  // パディング
            }
            packed += NR;
        }
    }
}

// 6x16 マイクロカーネル（パック形式用）
static inline void micro_kernel_6x16_packed(int K, const double *A,
                                             const double *B,
                                             double *C, int ldc)
{
    __m512d c00 = _mm512_setzero_pd(), c01 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd(), c11 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd(), c21 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd(), c31 = _mm512_setzero_pd();
    __m512d c40 = _mm512_setzero_pd(), c41 = _mm512_setzero_pd();
    __m512d c50 = _mm512_setzero_pd(), c51 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        // Bはパック済みなので連続アクセス
        __m512d b0 = _mm512_load_pd(&B[k * NR + 0]);
        __m512d b1 = _mm512_load_pd(&B[k * NR + 8]);

        // Aもパック済みなので連続アクセス
        __m512d a0 = _mm512_set1_pd(A[k * MR + 0]);
        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c01 = _mm512_fmadd_pd(a0, b1, c01);

        __m512d a1 = _mm512_set1_pd(A[k * MR + 1]);
        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c11 = _mm512_fmadd_pd(a1, b1, c11);

        __m512d a2 = _mm512_set1_pd(A[k * MR + 2]);
        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c21 = _mm512_fmadd_pd(a2, b1, c21);

        __m512d a3 = _mm512_set1_pd(A[k * MR + 3]);
        c30 = _mm512_fmadd_pd(a3, b0, c30);
        c31 = _mm512_fmadd_pd(a3, b1, c31);

        __m512d a4 = _mm512_set1_pd(A[k * MR + 4]);
        c40 = _mm512_fmadd_pd(a4, b0, c40);
        c41 = _mm512_fmadd_pd(a4, b1, c41);

        __m512d a5 = _mm512_set1_pd(A[k * MR + 5]);
        c50 = _mm512_fmadd_pd(a5, b0, c50);
        c51 = _mm512_fmadd_pd(a5, b1, c51);
    }

    // 結果をCに書き戻し
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
    for (int jj = 0; jj < N; jj += NC) {
        int jb = (jj + NC <= N) ? NC : N - jj;

        for (int kk = 0; kk < N; kk += KC) {
            int kb = (kk + KC <= N) ? KC : N - kk;

            // Bブロックをパック
            pack_B(kb, jb, &b[kk * N + jj], N, packed_B);

            for (int ii = 0; ii < N; ii += MC) {
                int ib = (ii + MC <= N) ? MC : N - ii;

                // Aブロックをパック
                pack_A(ib, kb, &a[ii * N + kk], N, packed_A);

                // マイクロカーネルでの計算
                int m_blocks = (ib + MR - 1) / MR;
                int n_blocks = (jb + NR - 1) / NR;

                for (int i = 0; i < m_blocks; i++) {
                    int i_pos = ii + i * MR;
                    if (i_pos + MR > N) continue;

                    for (int j = 0; j < n_blocks; j++) {
                        int j_pos = jj + j * NR;
                        if (j_pos + NR > N) continue;

                        micro_kernel_6x16_packed(kb,
                                                  &packed_A[i * MR * kb],
                                                  &packed_B[j * NR * kb],
                                                  &c[i_pos * N + j_pos], N);
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
