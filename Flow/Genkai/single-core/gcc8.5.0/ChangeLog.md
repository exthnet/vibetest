# SIMD📁 `ChangeLog.md`
🤖PG1.1
- **ハードウェア**: Genkai (玄界) single-core (1コア)
- **モジュール**: GCC 8.5.0 (default)
- **理論ピーク性能**: 80 GFLOPS (FP64, AVX-512, 2.5GHz)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

---

### v1.7.0 🏆 SOTA
**変更点**: "4x24マイクロカーネル (PG1.6手法参考)"
**結果**: 理論性能の69.5%達成 `55.6 GFLOPS`
**コメント**: "4行×24列のレジスタブロッキングでB行列の再利用を最大化"

<details>

- **生成時刻**: `2025-12-30T01:51:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593430`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:51:45Z`
    - end_time: `2025-12-30T01:51:50Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `55.6`
    - unit: `GFLOPS`
    - accuracy: `c[0][0]=3838895.05`
- [x] **sota**
    - scope: `local`
    - previous: `49.32`
    - improvement: `+12.7%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`
    - microkernel: `4x24`
    - block_i: `32`
    - block_k: `128`
    - block_j: `504`

</details>

---

### v1.6.1
**変更点**: "ブロックサイズ微調整 (24x64x全行)"
**結果**: 理論性能の60.2%達成 `48.2 GFLOPS`
**コメント**: "v1.6.0より若干低下、BLOCK_J=N全体では効率悪化"

<details>

- **生成時刻**: `2025-12-30T01:48:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593419`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:48:33Z`
    - end_time: `2025-12-30T01:48:38Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `48.2`
    - unit: `GFLOPS`
- **params**:
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`
    - block_i: `24`
    - block_k: `64`

</details>

---

### v1.6.0
**変更点**: "8x16レジスタブロッキング + プリフェッチ + 調整ブロック (32x128x512)"
**結果**: 理論性能の61.7%達成 `49.3 GFLOPS`
**コメント**: "8行同時処理とソフトウェアプリフェッチでB行列のキャッシュ効率最大化"

<details>

- **生成時刻**: `2025-12-30T01:46:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593411`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:47:08Z`
    - end_time: `2025-12-30T01:47:13Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `49.3`
    - unit: `GFLOPS`
    - accuracy: `c[0][0]=3838895.05`
- [x] **sota**
    - scope: `local`
    - previous: `48.34`
    - improvement: `+2.0%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`
    - block_i: `32`
    - block_k: `128`
    - block_j: `512`

</details>

---

### v1.5.0
**変更点**: "6x16レジスタブロッキング + 大型キャッシュブロック (48x256x256)"
**結果**: 理論性能の60.4%達成 `48.3 GFLOPS`
**コメント**: "レジスタ使用量を最大化し、L2キャッシュにフィットするブロックサイズに調整"

<details>

- **生成時刻**: `2025-12-30T01:43:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593398`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:43:20Z`
    - end_time: `2025-12-30T01:43:26Z`
    - runtime_sec: `6`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `48.3`
    - unit: `GFLOPS`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`
    - block_i: `48`
    - block_k: `256`
    - block_j: `256`

</details>

---

### v1.4.0
**変更点**: "4x8レジスタブロッキング + キャッシュブロッキング + AVX512"
**結果**: 理論性能の27.0%達成 `21.6 GFLOPS`
**コメント**: "4行同時処理でB行列の再利用率を向上"

<details>

- **生成時刻**: `2025-12-30T01:41:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593390`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:42:05Z`
    - end_time: `2025-12-30T01:42:10Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `21.6`
    - unit: `GFLOPS`
- **params**:
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`
    - block_size: `64`

</details>

---

### v1.3.0
**変更点**: "キャッシュブロッキング (64x64) + AVX512"
**結果**: 理論性能の21.6%達成 `17.3 GFLOPS`
**コメント**: "L2キャッシュ効率の大幅改善"

<details>

- **生成時刻**: `2025-12-30T01:40:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593386`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:41:01Z`
    - end_time: `2025-12-30T01:41:06Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `17.3`
    - unit: `GFLOPS`
- **params**:
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `AVX512`
    - block_size: `64`

</details>

---

### v1.2.0
**変更点**: "AVX512 intrinsics + ループ交換"
**結果**: 理論性能の8.0%達成 `6.4 GFLOPS`
**コメント**: "明示的AVX512使用、自動ベクトル化と同程度"

<details>

- **生成時刻**: `2025-12-30T01:39:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593384`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:39:59Z`
    - end_time: `2025-12-30T01:40:04Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `6.4`
    - unit: `GFLOPS`
- **params**:
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `AVX512`

</details>

---

### v1.1.0
**変更点**: "ループ交換 (i-j-k → i-k-j) でキャッシュ効率改善"
**結果**: 理論性能の7.5%達成 `6.0 GFLOPS`
**コメント**: "ベースライン比11.6倍、B行列の連続アクセス化"

<details>

- **生成時刻**: `2025-12-30T01:38:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593381`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:39:00Z`
    - end_time: `2025-12-30T01:39:05Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `6.0`
    - unit: `GFLOPS`
- **params**:
    - compile_flags: `-O3 -march=native`

</details>

---

### v1.0.0 (Baseline)
**変更点**: "オリジナルコード (i-j-k順序)"
**結果**: 理論性能の0.65%達成 `0.5 GFLOPS`
**コメント**: "キャッシュミス多発、最適化の出発点"

<details>

- **生成時刻**: `2025-12-30T01:37:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593380`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:37:54Z`
    - end_time: `2025-12-30T01:38:03Z`
    - runtime_sec: `9`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `0.5`
    - unit: `GFLOPS`
- **params**:
    - compile_flags: `-O3 -march=native`

</details>
