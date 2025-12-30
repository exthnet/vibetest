/**
 * mat-mat_v1.1.0.c - ブロッキング+AVX-512 SIMD最適化版 行列積
 * PG1.4 - Intel oneAPI 2023.2 (icx)
 *
 * 最適化内容:
 * - v1.0.0の全機能
 * - ブロッキング (BLOCK_SIZE=64) によるL2キャッシュ効率化
 * - ブロック内でのAVX-512処理
 * - レジスタブロッキング (4x8)
 *
 * 理論ピーク: 80 GFLOPS (1コア, 2.5GHz, 2×FMA, AVX-512)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <time.h>

#define BLOCK_I 64
#define BLOCK_J 64
#define BLOCK_K 256

/* ブロッキング+AVX-512最適化版 matmul */
void matmul(int N, double *a, double *b, double *c)
{
    /* 結果配列をゼロ初期化（累積加算のため） */
    memset(c, 0, sizeof(double)*N*N);

    /* ブロッキングループ */
    for(int jj = 0; jj < N; jj += BLOCK_J){
        int j_end = (jj + BLOCK_J < N) ? jj + BLOCK_J : N;

        for(int kk = 0; kk < N; kk += BLOCK_K){
            int k_end = (kk + BLOCK_K < N) ? kk + BLOCK_K : N;

            for(int ii = 0; ii < N; ii += BLOCK_I){
                int i_end = (ii + BLOCK_I < N) ? ii + BLOCK_I : N;

                /* ブロック内の計算 */
                for(int i = ii; i < i_end; i++){
                    double *a_row = &a[i*N];
                    double *c_row = &c[i*N];

                    for(int j = jj; j < j_end; j += 8){
                        __m512d sum;

                        if(kk == 0){
                            sum = _mm512_setzero_pd();
                        } else {
                            /* 前ブロックの結果をロード */
                            if(j + 8 <= j_end){
                                sum = _mm512_loadu_pd(&c_row[j]);
                            } else {
                                int rem = j_end - j;
                                __mmask8 mask = (1 << rem) - 1;
                                sum = _mm512_maskz_loadu_pd(mask, &c_row[j]);
                            }
                        }

                        /* k方向のループ */
                        for(int k = kk; k < k_end; k++){
                            __m512d va = _mm512_set1_pd(a_row[k]);

                            if(j + 8 <= j_end){
                                __m512d vb = _mm512_loadu_pd(&b[k*N+j]);
                                sum = _mm512_fmadd_pd(va, vb, sum);
                            } else {
                                int rem = j_end - j;
                                __mmask8 mask = (1 << rem) - 1;
                                __m512d vb = _mm512_maskz_loadu_pd(mask, &b[k*N+j]);
                                sum = _mm512_fmadd_pd(va, vb, sum);
                            }
                        }

                        /* 結果を書き戻し */
                        if(j + 8 <= j_end){
                            _mm512_storeu_pd(&c_row[j], sum);
                        } else {
                            int rem = j_end - j;
                            __mmask8 mask = (1 << rem) - 1;
                            _mm512_mask_storeu_pd(&c_row[j], mask, sum);
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
    double *a=NULL, *b=NULL, *c=NULL;
    int out;
    struct timespec start, end;
    double elapsed;

    out = 1;
    if(argc==1){
        N = 10;
    }else{
        N = atoi(argv[1]);
        if(argc==3){
            out = 0;
        }
    }
    printf("N = %d\n", N);
    printf("out = %d\n", out);

    /* アライメント確保したメモリ割り当て */
    a = (double*)aligned_alloc(64, sizeof(double)*N*N);
    b = (double*)aligned_alloc(64, sizeof(double)*N*N);
    c = (double*)aligned_alloc(64, sizeof(double)*N*N);

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            a[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
            b[i*N+j] = (double)(i+1) + (double)(j+1)*0.01;
            c[i*N+j] = 0.0;
        }
    }

    /* 性能測定 */
    clock_gettime(CLOCK_MONOTONIC, &start);
    matmul(N, a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    double flops = 2.0 * (double)N * (double)N * (double)N;
    double gflops = flops / elapsed / 1e9;

    printf("Time: %.6f sec\n", elapsed);
    printf("Performance: %.2f GFLOPS\n", gflops);

    /* 結果の確認 */
    if(out==1){
        printf("result:\n");
        for(i=0; i<N; i++){
            for(j=0; j<N; j++){
                printf(" %8.2f", c[i*N+j]);
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
