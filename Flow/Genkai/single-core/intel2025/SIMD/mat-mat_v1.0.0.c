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

void matmul(int N, double * __restrict__ a, double * __restrict__ b, double * __restrict__ c)
{
    int i, j, k, ii, jj, kk;

    // ブロッキング + i,k,jループ順 + AVX-512ベクトル化
    for (ii = 0; ii < N; ii += BLOCK_SIZE) {
        for (kk = 0; kk < N; kk += BLOCK_SIZE) {
            for (jj = 0; jj < N; jj += BLOCK_SIZE) {

                int i_end = (ii + BLOCK_SIZE < N) ? ii + BLOCK_SIZE : N;
                int k_end = (kk + BLOCK_SIZE < N) ? kk + BLOCK_SIZE : N;
                int j_end = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;

                for (i = ii; i < i_end; i++) {
                    for (k = kk; k < k_end; k++) {
                        __m512d a_vec = _mm512_set1_pd(a[i * N + k]);

                        // 8要素ずつ処理
                        int j;
                        for (j = jj; j + 8 <= j_end; j += 8) {
                            __m512d b_vec = _mm512_loadu_pd(&b[k * N + j]);
                            __m512d c_vec = _mm512_loadu_pd(&c[i * N + j]);
                            c_vec = _mm512_fmadd_pd(a_vec, b_vec, c_vec);
                            _mm512_storeu_pd(&c[i * N + j], c_vec);
                        }

                        // 残り要素の処理
                        for (; j < j_end; j++) {
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
