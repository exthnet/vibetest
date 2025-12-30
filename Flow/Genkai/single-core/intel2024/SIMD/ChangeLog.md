# SIMD📁 `ChangeLog.md`
🤖PG1.5
- **ハードウェア**：玄界 (Genkai) single-core（1コア）
- **モジュール**：Intel oneAPI 2024.1 (icx)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

### v1.3.0
**変更点**: "キャッシュブロック48x256x256に調整（PG1.1参考）"
**結果**: 性能25%向上 `42.2 GFLOPS`
**コメント**: "MC=48でL1キャッシュ効率改善。理論性能の52.7%達成"

<details>

- **生成時刻**: `2025-12-30T01:50:00Z`
- [x] **compile**
    - status: `success`
    - log: `コンパイル警告なし`
- [x] **job**
    - id: `4593422`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T10:50:00+09:00`
    - end_time: `2025-12-30T10:50:01+09:00`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `42.2`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
    - previous: `33.7`
    - improvement: `+25.2%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX-512`
    - micro_kernel: `6x16`
    - cache_blocking: `MC=48, KC=256, NC=256`

</details>

### v1.2.0
**変更点**: "パック形式メモリレイアウト導入を試行"
**結果**: 実行失敗 `SegFault`
**コメント**: "パッキング処理のバグによりSegmentation fault発生"

<details>

- **生成時刻**: `2025-12-30T01:48:00Z`
- [x] **compile**
    - status: `success`
- [ ] **job**
    - status: `error`
    - message: `Segmentation fault (core dumped)`

</details>

### v1.1.0
**変更点**: "6x16マイクロカーネル（レジスタブロッキング）導入"
**結果**: 性能60%向上 `33.7 GFLOPS`
**コメント**: "Cの6x16タイルをレジスタに保持、FMA効率が大幅改善。理論性能の42.1%"

<details>

- **生成時刻**: `2025-12-30T01:44:00Z`
- [x] **compile**
    - status: `success`
    - log: `コンパイル警告なし`
- [x] **job**
    - id: `4593401`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T10:44:50+09:00`
    - end_time: `2025-12-30T10:44:50+09:00`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `33.7`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
    - previous: `21.0`
    - improvement: `+60.5%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX-512`
    - micro_kernel: `6x16`
    - cache_blocking: `MC=256, KC=256, NC=256`

</details>

### v1.0.0
**変更点**: "AVX-512 intrinsics + ブロッキング(64x64)による初期実装"
**結果**: 初回実装完了 `21.0 GFLOPS`
**コメント**: "B行列転置+AVX-512 FMAで基本的なベクトル化。理論性能の26.3%"

<details>

- **生成時刻**: `2025-12-30T01:42:00Z`
- [x] **compile**
    - status: `success`
    - log: `コンパイル警告なし`
- [x] **job**
    - id: `4593388`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T10:41:42+09:00`
    - end_time: `2025-12-30T10:41:43+09:00`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `21.0`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX-512`
    - block_size: `64`

</details>
