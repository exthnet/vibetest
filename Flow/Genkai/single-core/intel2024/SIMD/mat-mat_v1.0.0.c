#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <sys/time.h>

// ブロックサイズ（L2キャッシュ最適化: 2MB、3行列で約682KB使用可能）
#define BLOCK_SIZE 64

// 時間計測用
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// AVX-512を使用した最適化matmul
// B行列を転置してメモリアクセスパターンを改善
void matmul(int N, double *a, double *b, double *c)
{
    int i, j, k, ii, jj, kk;

    // B行列の転置を作成（連続メモリアクセスのため）
    double *bt = (double*)aligned_alloc(64, sizeof(double) * N * N);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            bt[j * N + i] = b[i * N + j];
        }
    }

    // ブロッキング + AVX-512
    for (ii = 0; ii < N; ii += BLOCK_SIZE) {
        for (jj = 0; jj < N; jj += BLOCK_SIZE) {
            for (kk = 0; kk < N; kk += BLOCK_SIZE) {
                int i_end = (ii + BLOCK_SIZE < N) ? ii + BLOCK_SIZE : N;
                int j_end = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;
                int k_end = (kk + BLOCK_SIZE < N) ? kk + BLOCK_SIZE : N;

                for (i = ii; i < i_end; i++) {
                    for (j = jj; j < j_end; j++) {
                        __m512d sum = _mm512_setzero_pd();
                        double scalar_sum = 0.0;

                        // AVX-512で8要素ずつ処理
                        for (k = kk; k + 8 <= k_end; k += 8) {
                            __m512d va = _mm512_loadu_pd(&a[i * N + k]);
                            __m512d vb = _mm512_loadu_pd(&bt[j * N + k]);
                            sum = _mm512_fmadd_pd(va, vb, sum);
                        }

                        // 残りの要素（端数処理）
                        for (; k < k_end; k++) {
                            scalar_sum += a[i * N + k] * bt[j * N + k];
                        }

                        // AVX-512の結果を水平加算
                        c[i * N + j] += _mm512_reduce_add_pd(sum) + scalar_sum;
                    }
                }
            }
        }
    }

    free(bt);
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

    // 64バイトアライメント（AVX-512用）
    a = (double*)aligned_alloc(64, sizeof(double) * N * N);
    b = (double*)aligned_alloc(64, sizeof(double) * N * N);
    c = (double*)aligned_alloc(64, sizeof(double) * N * N);

    // 初期化
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i * N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            b[i * N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            c[i * N + j] = 0.0;
        }
    }

    // メイン計算部（時間計測）
    start = get_time();
    matmul(N, a, b, c);
    end = get_time();

    elapsed = end - start;
    gflops = (2.0 * N * N * N) / (elapsed * 1e9);

    printf("Elapsed time: %.6f sec\n", elapsed);
    printf("Performance: %.1f GFLOPS\n", gflops);
    printf("Efficiency: %.1f%% (vs 80 GFLOPS peak)\n", gflops / 80.0 * 100.0);

    // 結果の確認（必要に応じて）
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
