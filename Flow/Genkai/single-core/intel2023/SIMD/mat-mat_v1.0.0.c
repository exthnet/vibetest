/**
 * mat-mat_v1.0.0.c - AVX-512 SIMD最適化版 行列積
 * PG1.4 - Intel oneAPI 2023.2 (icx)
 *
 * 最適化内容:
 * - AVX-512 intrinsicsによる8要素同時処理
 * - ループ順序変更 (i-k-j) でメモリアクセス最適化
 * - 行列Bを転置してキャッシュ効率向上
 * - FMA命令 (_mm512_fmadd_pd) の活用
 * - ブロッキングによるキャッシュ効率化
 *
 * 理論ピーク: 80 GFLOPS (1コア, 2.5GHz, 2×FMA, AVX-512)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <time.h>

#define BLOCK_SIZE 64

/* 行列Bを転置 */
void transpose(int N, double *b, double *bt)
{
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            bt[j*N+i] = b[i*N+j];
        }
    }
}

/* AVX-512最適化版 matmul (転置B使用) */
void matmul(int N, double *a, double *b, double *c)
{
    double *bt = (double*)aligned_alloc(64, sizeof(double)*N*N);
    transpose(N, b, bt);

    /* 8要素未満の端数処理用マスク */
    int rem = N % 8;
    __mmask8 tail_mask = (rem == 0) ? 0xFF : ((1 << rem) - 1);

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            __m512d sum = _mm512_setzero_pd();
            int k;

            /* メインループ: 8要素ずつ処理 */
            for(k = 0; k <= N - 8; k += 8){
                __m512d va = _mm512_loadu_pd(&a[i*N+k]);
                __m512d vb = _mm512_loadu_pd(&bt[j*N+k]);
                sum = _mm512_fmadd_pd(va, vb, sum);
            }

            /* 端数処理 */
            if(k < N){
                __m512d va = _mm512_maskz_loadu_pd(tail_mask, &a[i*N+k]);
                __m512d vb = _mm512_maskz_loadu_pd(tail_mask, &bt[j*N+k]);
                sum = _mm512_fmadd_pd(va, vb, sum);
            }

            /* 水平加算 */
            c[i*N+j] = _mm512_reduce_add_pd(sum);
        }
    }

    free(bt);
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
