#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <sys/time.h>

// L2キャッシュ(2MB)最適化: 小さめのブロックで頻繁に再利用
#define BLOCK_I 96
#define BLOCK_K 128
#define BLOCK_J 384

// 64バイトアライメント確保
double* aligned_alloc_double(size_t n) {
    void* ptr = NULL;
    if (posix_memalign(&ptr, 64, n * sizeof(double)) != 0) {
        return NULL;
    }
    return (double*)ptr;
}

// 高性能行列積カーネル（4x24レジスタブロッキング - v1.1.0ベース）
static inline void micro_kernel_4x24(int K, const double* __restrict__ A, int lda,
                                      const double* __restrict__ B, int ldb,
                                      double* __restrict__ C, int ldc)
{
    // 4行 x 24列のCブロックをレジスタに保持
    __m512d c00 = _mm512_setzero_pd();
    __m512d c01 = _mm512_setzero_pd();
    __m512d c02 = _mm512_setzero_pd();
    __m512d c10 = _mm512_setzero_pd();
    __m512d c11 = _mm512_setzero_pd();
    __m512d c12 = _mm512_setzero_pd();
    __m512d c20 = _mm512_setzero_pd();
    __m512d c21 = _mm512_setzero_pd();
    __m512d c22 = _mm512_setzero_pd();
    __m512d c30 = _mm512_setzero_pd();
    __m512d c31 = _mm512_setzero_pd();
    __m512d c32 = _mm512_setzero_pd();

    for (int k = 0; k < K; k++) {
        // プリフェッチ（次のイテレーション用）
        if (k + 4 < K) {
            _mm_prefetch((const char*)&B[(k+4) * ldb], _MM_HINT_T0);
        }

        // Bの1行をロード（24要素 = 3つのAVX-512レジスタ）
        __m512d b0 = _mm512_loadu_pd(&B[k * ldb + 0]);
        __m512d b1 = _mm512_loadu_pd(&B[k * ldb + 8]);
        __m512d b2 = _mm512_loadu_pd(&B[k * ldb + 16]);

        // Aの4要素をブロードキャストしてFMA
        __m512d a0 = _mm512_set1_pd(A[0 * lda + k]);
        c00 = _mm512_fmadd_pd(a0, b0, c00);
        c01 = _mm512_fmadd_pd(a0, b1, c01);
        c02 = _mm512_fmadd_pd(a0, b2, c02);

        __m512d a1 = _mm512_set1_pd(A[1 * lda + k]);
        c10 = _mm512_fmadd_pd(a1, b0, c10);
        c11 = _mm512_fmadd_pd(a1, b1, c11);
        c12 = _mm512_fmadd_pd(a1, b2, c12);

        __m512d a2 = _mm512_set1_pd(A[2 * lda + k]);
        c20 = _mm512_fmadd_pd(a2, b0, c20);
        c21 = _mm512_fmadd_pd(a2, b1, c21);
        c22 = _mm512_fmadd_pd(a2, b2, c22);

        __m512d a3 = _mm512_set1_pd(A[3 * lda + k]);
        c30 = _mm512_fmadd_pd(a3, b0, c30);
        c31 = _mm512_fmadd_pd(a3, b1, c31);
        c32 = _mm512_fmadd_pd(a3, b2, c32);
    }

    // Cに書き戻し（アキュムレート）
    _mm512_storeu_pd(&C[0 * ldc + 0],  _mm512_add_pd(_mm512_loadu_pd(&C[0 * ldc + 0]),  c00));
    _mm512_storeu_pd(&C[0 * ldc + 8],  _mm512_add_pd(_mm512_loadu_pd(&C[0 * ldc + 8]),  c01));
    _mm512_storeu_pd(&C[0 * ldc + 16], _mm512_add_pd(_mm512_loadu_pd(&C[0 * ldc + 16]), c02));
    _mm512_storeu_pd(&C[1 * ldc + 0],  _mm512_add_pd(_mm512_loadu_pd(&C[1 * ldc + 0]),  c10));
    _mm512_storeu_pd(&C[1 * ldc + 8],  _mm512_add_pd(_mm512_loadu_pd(&C[1 * ldc + 8]),  c11));
    _mm512_storeu_pd(&C[1 * ldc + 16], _mm512_add_pd(_mm512_loadu_pd(&C[1 * ldc + 16]), c12));
    _mm512_storeu_pd(&C[2 * ldc + 0],  _mm512_add_pd(_mm512_loadu_pd(&C[2 * ldc + 0]),  c20));
    _mm512_storeu_pd(&C[2 * ldc + 8],  _mm512_add_pd(_mm512_loadu_pd(&C[2 * ldc + 8]),  c21));
    _mm512_storeu_pd(&C[2 * ldc + 16], _mm512_add_pd(_mm512_loadu_pd(&C[2 * ldc + 16]), c22));
    _mm512_storeu_pd(&C[3 * ldc + 0],  _mm512_add_pd(_mm512_loadu_pd(&C[3 * ldc + 0]),  c30));
    _mm512_storeu_pd(&C[3 * ldc + 8],  _mm512_add_pd(_mm512_loadu_pd(&C[3 * ldc + 8]),  c31));
    _mm512_storeu_pd(&C[3 * ldc + 16], _mm512_add_pd(_mm512_loadu_pd(&C[3 * ldc + 16]), c32));
}

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    // ブロッキング（L2/L3キャッシュ最適化）
    for (int jj = 0; jj < N; jj += BLOCK_J) {
        int j_end = (jj + BLOCK_J < N) ? jj + BLOCK_J : N;

        for (int kk = 0; kk < N; kk += BLOCK_K) {
            int k_end = (kk + BLOCK_K < N) ? kk + BLOCK_K : N;
            int K_block = k_end - kk;

            for (int ii = 0; ii < N; ii += BLOCK_I) {
                int i_end = (ii + BLOCK_I < N) ? ii + BLOCK_I : N;

                // マイクロカーネル適用（4x24ブロック）
                int i, j;
                for (i = ii; i + 4 <= i_end; i += 4) {
                    for (j = jj; j + 24 <= j_end; j += 24) {
                        micro_kernel_4x24(K_block,
                                          &a[i * N + kk], N,
                                          &b[kk * N + j], N,
                                          &c[i * N + j], N);
                    }
                    // 残りのj（24の倍数でない部分）
                    for (; j < j_end; j++) {
                        for (int k = kk; k < k_end; k++) {
                            c[i * N + j] += a[i * N + k] * b[k * N + j];
                            c[(i+1) * N + j] += a[(i+1) * N + k] * b[k * N + j];
                            c[(i+2) * N + j] += a[(i+2) * N + k] * b[k * N + j];
                            c[(i+3) * N + j] += a[(i+3) * N + k] * b[k * N + j];
                        }
                    }
                }
                // 残りのi（4の倍数でない部分）
                for (; i < i_end; i++) {
                    for (j = jj; j < j_end; j++) {
                        for (int k = kk; k < k_end; k++) {
                            c[i * N + j] += a[i * N + k] * b[k * N + j];
                        }
                    }
                }
            }
        }
    }
}

double get_time() {
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

    // アライメント確保したメモリ割り当て
    a = aligned_alloc_double(N * N);
    b = aligned_alloc_double(N * N);
    c = aligned_alloc_double(N * N);

    if (!a || !b || !c) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // 初期化
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i * N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            b[i * N + j] = (double)(i + 1) + (double)(j + 1) * 0.01;
            c[i * N + j] = 0.0;
        }
    }

    // 性能測定
    double start = get_time();
    matmul(N, a, b, c);
    double end = get_time();

    double elapsed = end - start;
    double flops = 2.0 * (double)N * (double)N * (double)N;
    double gflops = flops / elapsed / 1e9;

    printf("Time: %.6f sec\n", elapsed);
    printf("Performance: %.1f GFLOPS\n", gflops);

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
