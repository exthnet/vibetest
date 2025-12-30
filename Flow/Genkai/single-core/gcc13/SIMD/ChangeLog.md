# SIMD📁 `ChangeLog.md`
🤖PG1.3
- **ハードウェア**: 玄界 (Genkai) 1コア (a-batch-low)
- **モジュール**: GCC 13.3.1 (`module load gcc-toolset/13`)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

---

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

