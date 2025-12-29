# gcc8.5.0📁 `ChangeLog.md`
🤖PG1.1
- **ハードウェア**：玄界 (Genkai) single-core（1コア）
- **モジュール**：GCC 8.5.0 (default)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：なし

### v2.0.0
**変更点**: "4x8マイクロカーネル（レジスタブロッキング）実装"
**結果**: ベースライン比13.5倍高速化 `28.75 GFLOPS`
**コメント**: "4x8レジスタブロッキングで演算密度を最大化。v1.3.0比で2.3倍の大幅改善"

<details>

- **生成時刻**: `2025-12-29T14:50:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - options: `-O3 -march=native -mavx2 -mfma`
- [x] **job**
    - id: `4590899`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T14:50:00Z`
    - end_time: `2025-12-29T14:50:01Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `28.75`
    - unit: `GFLOPS`
    - checksum_c00: `3838895.050000`
    - checksum_cNN: `513888385.000000`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - block_i: `64`
    - block_k: `256`
    - block_j: `64`
    - mr: `4`
    - nr: `8`
    - speedup_vs_baseline: `13.5x`
    - speedup_vs_v1.3.0: `2.3x`

</details>

---

### v1.3.0
**変更点**: "ソフトウェアプリフェッチ追加（PREFETCH_DIST=8）"
**結果**: ベースライン比5.8倍高速化 `12.39 GFLOPS`
**コメント**: "B行列の先読みでキャッシュミス削減。v1.2.0比で1.8%改善"

<details>

- **生成時刻**: `2025-12-29T14:45:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - options: `-O3 -march=native -mavx2 -mfma`
- [x] **job**
    - id: `4590891`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T14:45:00Z`
    - end_time: `2025-12-29T14:45:01Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `12.39`
    - unit: `GFLOPS`
    - checksum_c00: `3838895.050000`
    - checksum_cNN: `513888385.000000`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - block_size: `64`
    - unroll_factor: `4`
    - prefetch_dist: `8`
    - speedup_vs_baseline: `5.8x`

</details>

---

### v1.2.0
**変更点**: "ループアンローリング追加（i方向4倍展開）"
**結果**: ベースライン比5.7倍高速化 `12.17 GFLOPS`
**コメント**: "i方向4倍アンローリングでレジスタ再利用を最大化。v1.1.0比で43%高速化"

<details>

- **生成時刻**: `2025-12-29T14:40:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - options: `-O3 -march=native -mavx2 -mfma`
- [x] **job**
    - id: `4590886`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T14:40:00Z`
    - end_time: `2025-12-29T14:40:01Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `12.17`
    - unit: `GFLOPS`
    - checksum_c00: `3838895.050000`
    - checksum_cNN: `513888385.000000`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - block_size: `64`
    - unroll_factor: `4`
    - speedup_vs_baseline: `5.7x`
    - speedup_vs_v1.1.0: `1.43x`

</details>

---

### v1.1.0
**変更点**: "キャッシュブロッキング追加（BLOCK_SIZE=64）"
**結果**: ベースライン比4.0倍高速化 `8.52 GFLOPS`
**コメント**: "64x64ブロックでL1/L2キャッシュ効率を改善。v1.0.0比で29%高速化"

<details>

- **生成時刻**: `2025-12-29T14:36:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - options: `-O3 -march=native -mavx2 -mfma`
- [x] **job**
    - id: `4590880`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T14:36:59Z`
    - end_time: `2025-12-29T14:36:59Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `8.52`
    - unit: `GFLOPS`
    - checksum_c00: `3838895.050000`
    - checksum_cNN: `513888385.000000`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - block_size: `64`
    - speedup_vs_baseline: `4.0x`
    - speedup_vs_v1.0.0: `1.29x`

</details>

---

### v1.0.0
**変更点**: "AVX2 SIMD最適化 + ループ順序変更(i,k,j) + FMA命令"
**結果**: ベースライン比3.1倍高速化 `6.62 GFLOPS`
**コメント**: "ループ順序をi,j,k→i,k,jに変更しメモリアクセスパターン改善。AVX2で4要素同時処理、FMA命令使用"

<details>

- **生成時刻**: `2025-12-29T14:34:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - options: `-O3 -march=native -mavx2 -mfma`
- [x] **job**
    - id: `4590874`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T14:34:42Z`
    - end_time: `2025-12-29T14:34:43Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `6.62`
    - unit: `GFLOPS`
    - checksum_c00: `3838895.050000`
    - checksum_cNN: `513888385.000000`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - baseline_perf: `2.13 GFLOPS`
    - speedup: `3.1x`

</details>

---

### Baseline (参考)
**変更点**: "オリジナルコードにタイミング計測追加"
**結果**: `2.13 GFLOPS`
**コメント**: "ベースライン測定用。ループ順序i,j,k、最適化なし(-O2)"

<details>

- **生成時刻**: `2025-12-29T14:33:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - options: `-O2`
- [x] **job**
    - id: `4590870`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T14:33:17Z`
    - end_time: `2025-12-29T14:33:18Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `2.13`
    - unit: `GFLOPS`
    - checksum_c00: `3838895.050000`
    - checksum_cNN: `513888385.000000`
- **params**:
    - N: `1000`

</details>
