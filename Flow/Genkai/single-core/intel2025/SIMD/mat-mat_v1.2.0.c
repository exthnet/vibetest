#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <sys/time.h>

#define BLOCK_SIZE 64

// 64バイトアライメント確保
double* aligned_alloc_double(size_t n) {
    void* ptr = NULL;
    if (posix_memalign(&ptr, 64, n * sizeof(double)) != 0) {
        return NULL;
    }
    return (double*)ptr;
}

// B行列を転置
void transpose(int N, const double* __restrict__ src, double* __restrict__ dst) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dst[j * N + i] = src[i * N + j];
        }
    }
}

// 転置B使用版: C[i,j] += A[i,k] * B^T[j,k]
// これにより B^T[j,k] は j を固定すると k 方向に連続アクセス
void matmul(int N, double * __restrict__ a, double * __restrict__ bt, double * __restrict__ c)
{
    int ii, jj, i, j, k;

    // ブロッキング
    for (ii = 0; ii < N; ii += BLOCK_SIZE) {
        int i_end = (ii + BLOCK_SIZE < N) ? ii + BLOCK_SIZE : N;

        for (jj = 0; jj < N; jj += BLOCK_SIZE) {
            int j_end = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;

            for (i = ii; i < i_end; i++) {
                for (j = jj; j < j_end; j++) {
                    __m512d sum = _mm512_setzero_pd();

                    // k方向に8要素ずつ処理（AもB^Tも連続アクセス）
                    int k;
                    for (k = 0; k + 8 <= N; k += 8) {
                        __m512d a_vec = _mm512_loadu_pd(&a[i * N + k]);
                        __m512d bt_vec = _mm512_loadu_pd(&bt[j * N + k]);
                        sum = _mm512_fmadd_pd(a_vec, bt_vec, sum);
                    }

                    // 水平加算
                    double partial = _mm512_reduce_add_pd(sum);

                    // 残り要素
                    for (; k < N; k++) {
                        partial += a[i * N + k] * bt[j * N + k];
                    }

                    c[i * N + j] += partial;
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
    double *a = NULL, *b = NULL, *bt = NULL, *c = NULL;
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
    bt = aligned_alloc_double(N * N);
    c = aligned_alloc_double(N * N);

    if (!a || !b || !bt || !c) {
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

    // B行列を転置
    transpose(N, b, bt);

    // 性能測定
    double start = get_time();
    matmul(N, a, bt, c);
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
    free(bt);
    free(b);
    free(a);

    return 0;
}
