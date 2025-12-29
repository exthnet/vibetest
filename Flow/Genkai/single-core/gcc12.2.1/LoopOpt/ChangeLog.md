# LoopOpt📁 `ChangeLog.md`
🤖PG1.2
- **ハードウェア**: 玄界 (Genkai) single-core（1コア）
- **モジュール**: GCC 12.2.1

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：なし

---

### v3.2.0 ★NEW LOCAL SOTA★
**変更点**: "AVX-512 8x16マイクロカーネル（MR=8に拡大）"
**結果**: `37.21 GFLOPS` (N=1000, 0.054秒)
**コメント**: "v3.1.0から6.6%向上。理論性能の46.5%達成。50%目標まであと7%"

<details>

- **生成時刻**: `2025-12-29T15:55:00Z`
- [x] **compile**: options: `-O3 -march=native -mavx512f -mavx512vl -mfma`
- [x] **job**: id: `4590988`, status: `success`
- [x] **test**: performance: `37.21`, unit: `GFLOPS`, checksum: `一致`
- [x] **sota**: scope: `local`
- **params**: BLOCK_I: `64`, BLOCK_K: `256`, BLOCK_J: `64`, mr: `8`, nr: `16`

</details>

---

### v3.1.0
**変更点**: "AVX-512 4x16マイクロカーネル + 端処理対応（PG1.1 v2.3.1ベース）"
**結果**: `34.92 GFLOPS` (N=1000, 0.057秒)
**コメント**: "PG1.1(34.37 GFLOPS)を1.6%上回る！理論性能の43.7%達成。gcc12.2.1がgcc8.5.0より高速"

<details>

- **生成時刻**: `2025-12-29T15:45:00Z`
- [x] **compile**: options: `-O3 -march=native -mavx512f -mavx512vl -mfma`
- [x] **job**: id: `4590951`, status: `success`
- [x] **test**: performance: `34.92`, unit: `GFLOPS`, checksum: `一致`
- [x] **sota**: scope: `local`
- **params**: BLOCK_I: `64`, BLOCK_K: `256`, BLOCK_J: `64`, mr: `4`, nr: `16`

</details>

---

### v3.0.0
**変更点**: "AVX-512 intrinsics + 4x8マイクロカーネル + パッキング（PG1.1 v2.2.3ベース）"
**結果**: `33.26 GFLOPS` (N=1000, 0.060秒)
**コメント**: "30 GFLOPS目標達成！gcc12.2.1でもAVX-512 intrinsicsで高性能。理論性能の41.6%"

<details>

- **生成時刻**: `2025-12-29T15:35:00Z`
- [x] **compile**: options: `-O3 -march=native -mavx512f -mavx512vl -mfma`
- [x] **job**: id: `4590928`, status: `success`
- [x] **test**: performance: `33.26`, unit: `GFLOPS`, checksum: `一致`
- [x] **sota**: scope: `local`
- **params**: BLOCK_I: `64`, BLOCK_K: `256`, BLOCK_J: `64`, mr: `4`, nr: `8`

</details>

---

### v1.9.0
**変更点**: "PGO (Profile-Guided Optimization) 適用"
**結果**: `22.22 GFLOPS` (N=1000, 0.09秒)
**コメント**: "PGOもv1.5.0と同等。gcc12.2.1での最適化は限界か"

<details>

- **生成時刻**: `2025-12-29T15:25:00Z`
- [x] **compile**: options: `-O3 -march=native -ffast-math -fprofile-use`
- [x] **job**: id: `4590923`, status: `success`
- [x] **test**: performance: `22.22`, unit: `GFLOPS`
- **params**: PGO: `enabled`

</details>

---

### v1.8.0
**変更点**: "omp simd pragma追加"
**結果**: `22.22 GFLOPS` (N=1000, 0.09秒)
**コメント**: "omp simd pragmaもv1.5.0と同等の性能"

<details>

- **生成時刻**: `2025-12-29T15:20:00Z`
- [x] **compile**: options: `-O3 -march=native -ffast-math -fopenmp-simd`
- [x] **job**: id: `4590917`, status: `success`
- [x] **test**: performance: `22.22`, unit: `GFLOPS`

</details>

---

### v1.7.0
**変更点**: "BLOCK_SIZE=48 + pragma ivdep"
**結果**: `18.18 GFLOPS` (N=1000, 0.11秒)
**コメント**: "BLOCK_SIZE=48はv1.5.0(64)より低下"

<details>

- **生成時刻**: `2025-12-29T15:15:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590912`, status: `success`
- [x] **test**: performance: `18.18`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `48`

</details>

---

### v1.6.0
**変更点**: "追加アグレッシブオプション (-fno-signed-zeros等)"
**結果**: `22.22 GFLOPS` (N=1000, 0.09秒)
**コメント**: "追加オプションはv1.5.0と同等。効果なし"

<details>

- **生成時刻**: `2025-12-29T15:10:00Z`
- [x] **compile**: options: `-O3 -march=native -ffast-math -fno-signed-zeros -fno-trapping-math -fassociative-math -freciprocal-math`
- [x] **job**: id: `4590911`, status: `success`
- [x] **test**: performance: `22.22`, unit: `GFLOPS`

</details>

---

### v1.5.0 ★LOCAL SOTA★
**変更点**: "64バイトアライメント + -ffast-math オプション追加"
**結果**: `22.22 GFLOPS` (N=1000, 0.09秒)
**コメント**: "-ffast-mathとメモリアライメントでv1.1.0_avx512から2.1倍高速化。新LOCAL SOTA"

<details>

- **生成時刻**: `2025-12-29T15:05:00Z`
- [x] **compile**: options: `-O3 -march=native -mavx512f -mavx512vl -funroll-loops -ftree-vectorize -ffast-math`
- [x] **job**: id: `4590908`, status: `success`
- [x] **test**: performance: `22.22`, unit: `GFLOPS`
- [x] **sota**: scope: `local`
- **params**: BLOCK_SIZE: `64`, alignment: `64-byte`, fast-math: `enabled`

</details>

---

### v1.1.0_avx512
**変更点**: "v1.1.0 + AVX-512オプション明示追加"
**結果**: `10.42 GFLOPS` (N=1000, 0.192秒)
**コメント**: "-march=nativeが既にAVX-512有効化済み。v1.1.0_optと同等"

<details>

- **生成時刻**: `2025-12-29T14:54:00Z`
- [x] **compile**: options: `-O3 -march=native -mavx512f -mavx512vl -funroll-loops -ftree-vectorize`
- [x] **job**: id: `4590905`, status: `success`
- [x] **test**: performance: `10.42`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `64`

</details>

---

### v2.0.0
**変更点**: "4x8マイクロカーネル + キャッシュブロッキング（PG1.1戦略適用）"
**結果**: `5.39 GFLOPS` (N=1000, 0.371秒)
**コメント**: "gcc12.2.1ではマイクロカーネルが効果的でない。v1.1.0_optが最適"

<details>

- **生成時刻**: `2025-12-29T14:52:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590904`, status: `success`
- [x] **test**: performance: `5.39`, unit: `GFLOPS`, accuracy: `検証済み`
- **params**: BLOCK_SIZE: `64`, MR: `4`, NR: `8`

</details>

---

### v1.1.0_opt
**変更点**: "v1.1.0 + -funroll-loops -ftree-vectorize オプション追加"
**結果**: `10.36 GFLOPS` (N=1000, 0.193秒)
**コメント**: "コンパイラオプション追加でv1.1.0(9.57 GFLOPS)から8%向上。新LOCAL SOTA"

<details>

- **生成時刻**: `2025-12-29T14:48:00Z`
- [x] **compile**: status: `success`, options: `-O3 -march=native -funroll-loops -ftree-vectorize`
- [x] **job**: id: `4590902`, status: `success`
- [x] **test**: performance: `10.36`, unit: `GFLOPS`
- [x] **sota**: scope: `local`
- **params**: BLOCK_SIZE: `64`, compiler_opts: `extended`

</details>

---

### v1.3.0
**変更点**: "キャッシュブロッキング + 2x2レジスタブロッキング"
**結果**: `7.60 GFLOPS` (N=1000, 0.263秒)
**コメント**: "レジスタブロッキングもコンパイラ最適化を妨げ、v1.1.0より低下"

<details>

- **生成時刻**: `2025-12-29T14:46:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590901`, status: `success`
- [x] **test**: performance: `7.60`, unit: `GFLOPS`, accuracy: `検証済み`
- **params**: BLOCK_SIZE: `64`, register_block: `2x2`

</details>

---

### v1.2.0
**変更点**: "キャッシュブロッキング + 4xループアンローリング"
**結果**: `7.91 GFLOPS` (N=1000, 0.253秒)
**コメント**: "手動アンローリングはコンパイラ最適化を妨げ、v1.1.0より低下"

<details>

- **生成時刻**: `2025-12-29T14:42:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590894`, status: `success`
- [x] **test**: performance: `7.91`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `64`, unroll: `4x`

</details>

---

### v1.1.2
**変更点**: "キャッシュブロッキング (BLOCK_SIZE=128)"
**結果**: `8.37 GFLOPS` (N=1000, 0.239秒)
**コメント**: "BLOCK_SIZE=64より低下、L1キャッシュオーバーフロー"

<details>

- **生成時刻**: `2025-12-29T14:40:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590889`, status: `success`
- [x] **test**: performance: `8.37`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `128`

</details>

---

### v1.1.1
**変更点**: "キャッシュブロッキング (BLOCK_SIZE=32)"
**結果**: `8.10 GFLOPS` (N=1000, 0.247秒)
**コメント**: "BLOCK_SIZE=64より低下、ブロックオーバーヘッド増加"

<details>

- **生成時刻**: `2025-12-29T14:39:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590887`, status: `success`
- [x] **test**: performance: `8.10`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `32`

</details>

---

### v1.1.0
**変更点**: "キャッシュブロッキング (BLOCK_SIZE=64) + i-k-j順序"
**結果**: `9.57 GFLOPS` (N=1000, 0.209秒)
**コメント**: "v1.0.0(6.25 GFLOPS)から1.5倍、ベースラインから4.5倍高速化"

<details>

- **生成時刻**: `2025-12-29T14:38:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - log: `N/A`
- [x] **job**
    - id: `4590883`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T23:38:21+0900`
    - end_time: `2025-12-29T23:38:22+0900`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `9.57`
    - unit: `GFLOPS`
    - accuracy: `検証済み（N=10で結果一致）`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - loop_order: `i-k-j`
    - BLOCK_SIZE: `64`

</details>

---

### v1.0.0
**変更点**: "i-k-j ループ順序によるキャッシュ局所性最適化"
**結果**: `6.25 GFLOPS` (N=1000, 0.320秒)
**コメント**: "ベースライン(i-j-k順)の2.14 GFLOPSから約2.9倍高速化"

<details>

- **生成時刻**: `2025-12-29T14:36:18Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - log: `N/A`
- [x] **job**
    - id: `4590875`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T23:35:44+0900`
    - end_time: `2025-12-29T23:35:45+0900`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `6.25`
    - unit: `GFLOPS`
    - accuracy: `検証済み（N=10で結果一致）`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - loop_order: `i-k-j`

</details>

---

### Baseline
**変更点**: "BaseCode (i-j-k順) のベースライン測定"
**結果**: `2.14 GFLOPS` (N=1000, 0.935秒)
**コメント**: "最適化なしの基準性能"

<details>

- **生成時刻**: `2025-12-29T14:32:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
- [x] **job**
    - id: `4590869`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T23:33:17+0900`
    - end_time: `2025-12-29T23:33:18+0900`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `2.14`
    - unit: `GFLOPS`
- **params**:
    - N: `1000`
    - loop_order: `i-j-k (original)`

</details>
