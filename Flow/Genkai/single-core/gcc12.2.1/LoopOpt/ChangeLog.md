# SIMD📁 `ChangeLog.md`
🤖PG1.2
- **ハードウェア**：Genkai single-core (1コア)
- **モジュール**：GCC 12.2.1 + AVX-512

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

---

### v2.2.1
**変更点**: "4x24マイクロカーネル実装、Bパネルパッキング"
**結果**: 理論性能の48.8%達成 `39.01 GFLOPS`
**コメント**: "12アキュムレータ(4行×3ベクタ)でFMA効率向上、v2.1.0比+14%"

<details>

- **生成時刻**: `2025-12-30T01:57:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593453`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:56:30Z`
    - end_time: `2025-12-30T01:56:31Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `39.01`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
    - previous: `34.11`
    - improvement: `+14.4%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`

</details>

---

### v2.1.0
**変更点**: "B行列転置+8x8レジスタブロッキング、BLOCK_K=512に拡大"
**結果**: 理論性能の42.6%達成 `34.11 GFLOPS`
**コメント**: "8行同時処理で命令レベル並列性向上、v2.0.0比+9%"

<details>

- **生成時刻**: `2025-12-30T01:52:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593426`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:52:30Z`
    - end_time: `2025-12-30T01:52:31Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `34.11`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
    - previous: `31.35`
    - improvement: `+8.8%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`

</details>

---

### v2.0.0
**変更点**: "B行列転置による連続メモリアクセス最適化"
**結果**: 理論性能の39.2%達成 `31.35 GFLOPS`
**コメント**: "転置によりBのストライドアクセスを解消、v1.2.0比+41%"

<details>

- **生成時刻**: `2025-12-30T01:48:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593420`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:48:30Z`
    - end_time: `2025-12-30T01:48:31Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `31.35`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
    - previous: `22.25`
    - improvement: `+40.9%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`

</details>

---

### v1.2.0
**変更点**: "レジスタブロッキング(4x8)追加、ブロックサイズ最適化"
**結果**: 理論性能の27.8%達成 `22.25 GFLOPS`
**コメント**: "BLOCK_I=64,K=256,J=64で4行同時処理、v1.1.0比+37%"

<details>

- **生成時刻**: `2025-12-30T01:42:00Z`
- [x] **compile**
    - status: `success`
    - log: `リモート実行`
- [x] **job**
    - id: `4593392`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:42:00Z`
    - end_time: `2025-12-30T01:42:01Z`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `22.25`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
    - previous: `16.28`
    - improvement: `+36.7%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`

</details>

---

### v1.1.0
**変更点**: "キャッシュブロッキング(64x64)追加"
**結果**: 理論性能の20.4%達成 `16.28 GFLOPS`
**コメント**: "BLOCK_SIZE=64でL2キャッシュ効率改善、v1.0.0比+158%"

<details>

- **生成時刻**: `2025-12-30T01:40:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593387`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:41:00Z`
    - end_time: `2025-12-30T01:41:27Z`
    - runtime_sec: `27`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `16.28`
    - unit: `GFLOPS`
- [x] **sota**
    - scope: `local`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma -funroll-loops`
    - simd_type: `AVX512`

</details>

---

### v1.0.0
**変更点**: "AVX-512 SIMD化、ikjループ順序に変更"
**結果**: 理論性能の7.9%達成 `6.31 GFLOPS`
**コメント**: "8要素同時処理、FMA使用、aligned_alloc導入"

<details>

- **生成時刻**: `2025-12-30T01:38:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593385`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:40:00Z`
    - end_time: `2025-12-30T01:40:09Z`
    - runtime_sec: `9`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `6.31`
    - unit: `GFLOPS`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `AVX512`

</details>

---

### v0.0.0
**変更点**: "ベースライン（時間計測追加のみ）"
**結果**: 理論性能の2.7%達成 `2.12 GFLOPS`
**コメント**: "BaseCodeのijkループ順序のまま、最適化なし"

<details>

- **生成時刻**: `2025-12-30T01:37:00Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593382`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:39:00Z`
    - end_time: `2025-12-30T01:39:05Z`
    - runtime_sec: `5`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `2.12`
    - unit: `GFLOPS`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `none`

</details>
