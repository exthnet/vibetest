# SIMD📁 `ChangeLog.md`
🤖PG1.3
- **ハードウェア**: 玄界 (Genkai) 1コア (a-batch-low)
- **モジュール**: GCC 13.3.1 (`module load gcc-toolset/13`)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

---

### v1.5.0
**変更点**: "4x24マイクロカーネル（PG1.6参考）"
**結果**: 理論性能の52.0%達成 `41.6 GFLOPS`
**コメント**: "4行×24列(3AVX-512reg)。レジスタ圧迫でv1.3.0より性能低下"

<details>

- **生成時刻**: `2025-12-30T01:51:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593435`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:52:15Z`
    - end_time: `2025-12-30T01:52:15Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `41.6`
    - unit: `GFLOPS`
- **params**:
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize`
    - simd_type: `AVX-512`
    - micro_kernel: `4x24`

</details>

### v1.4.0
**変更点**: "Bマトリクスパッキング + 6x16カーネル"
**結果**: 理論性能の44.9%達成 `35.9 GFLOPS`
**コメント**: "パッキングオーバーヘッドで性能低下。N=1000では効果なし"

<details>

- **生成時刻**: `2025-12-30T01:49:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593428`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:50:46Z`
    - end_time: `2025-12-30T01:50:49Z`
    - runtime_sec: `3`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `35.9`
    - unit: `GFLOPS`
- **params**:
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize`
    - simd_type: `AVX-512`
    - micro_kernel: `6x16`

</details>

### v1.3.0
**変更点**: "6x16マイクロカーネル + 48x256x256キャッシュブロック"
**結果**: 理論性能の56.4%達成 `45.1 GFLOPS`
**コメント**: "SE1共有のPG1.1手法を参考。現時点のローカルSOTA"

<details>

- **生成時刻**: `2025-12-30T01:47:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593418`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:48:28Z`
    - end_time: `2025-12-30T01:48:28Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `45.1`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
- **params**:
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize`
    - simd_type: `AVX-512`
    - micro_kernel: `6x16`
    - block_size: `48x256x256`

</details>

### v1.2.0
**変更点**: "8x8マイクロカーネル + プリフェッチ"
**結果**: 理論性能の55.0%達成 `44.0 GFLOPS`
**コメント**: "プリフェッチ追加でv1.1.0から若干改善"

<details>

- **生成時刻**: `2025-12-30T01:45:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593403`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:46:29Z`
    - end_time: `2025-12-30T01:46:29Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `44.0`
    - unit: `GFLOPS`
- **params**:
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize`
    - simd_type: `AVX-512`
    - micro_kernel: `8x8`
    - block_size: `48`

</details>

### v1.1.0
**変更点**: "AVX-512 intrinsics + キャッシュブロッキング(64x64)実装"
**結果**: 理論性能の52.4%達成 `41.9 GFLOPS`
**コメント**: "4x8マイクロカーネル、FMA活用。v1.0.0比5.7倍高速化"

<details>

- **生成時刻**: `2025-12-30T01:39:00Z`
- [x] **compile**
    - status: `success`
    - log: `job_v1.1.0.sh.4593399.out`
- [x] **job**
    - id: `4593399`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:43:31Z`
    - end_time: `2025-12-30T01:43:32Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `41.9`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize`
    - simd_type: `AVX-512`
    - block_size: `64`

</details>

### v1.0.0
**変更点**: "ループ順序変更(i-k-j) + 自動ベクトル化ヒント"
**結果**: 理論性能の9.3%達成 `7.4 GFLOPS`
**コメント**: "ベースライン実装。restrict修飾子、aligned_alloc使用"

<details>

- **生成時刻**: `2025-12-30T01:38:00Z`
- [x] **compile**
    - status: `success`
    - log: `job_v1.0.0.sh.4593391.out`
- [x] **job**
    - id: `4593391`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:42:52Z`
    - end_time: `2025-12-30T01:42:54Z`
    - runtime_sec: `2`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `7.4`
    - unit: `GFLOPS`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops -ftree-vectorize`
    - simd_type: `auto-vectorization`

</details>

