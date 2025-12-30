# SIMD📁 `ChangeLog.md`
🤖PG1.6
- **ハードウェア**: Genkai single-core (1コア)
- **モジュール**: Intel oneAPI 2025.1.3 (icx)

## Change Log

- 基本の型: `ChangeLog_format.md`に記載
- PMオーバーライド: `ChangeLog_format_PM_override.md`に記載

---

### v1.1.3
**変更点**: "k-loop 4段アンローリング追加"
**結果**: 理論性能の69.0%達成 `55.2 GFLOPS`
**コメント**: "v1.1.0ベース+k-loop展開でパイプライン効率向上。PG1.1の55.6 GFLOPSにほぼ匹敵"

<details>

- **生成時刻**: `2025-12-30T01:55:05Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593455`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:56:50Z`
    - end_time: `2025-12-30T01:57:10Z`
    - runtime_sec: `20`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `55.2`
    - unit: `GFLOPS`
    - efficiency: `69.0%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -xCORE-AVX512 -qopt-zmm-usage=high -funroll-loops`
    - simd_type: `AVX-512`
    - block_i: `64`
    - block_k: `256`
    - block_j: `512`
    - micro_kernel: `4x24`
    - k_unroll: `4`

</details>

---

### v1.1.2
**変更点**: "4x24維持+キャッシュブロック調整+プリフェッチ"
**結果**: 理論性能の66.6%達成 `53.3 GFLOPS`
**コメント**: "プリフェッチ追加も効果限定的。v1.1.0と同等"

<details>

- **生成時刻**: `2025-12-30T01:53:28Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593445`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:54:30Z`
    - end_time: `2025-12-30T01:54:40Z`
    - runtime_sec: `10`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `53.3`
    - unit: `GFLOPS`
    - efficiency: `66.6%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`
    - block_i: `96`
    - block_k: `128`
    - block_j: `384`
    - micro_kernel: `4x24`

</details>

---

### v1.1.1
**変更点**: "6x16レジスタブロッキング+48x256x256キャッシュブロック（PG1.1参考）"
**結果**: 理論性能の65.1%達成 `52.1 GFLOPS`
**コメント**: "4x24より微減。Intel 2025ではv1.1.0の4x24が最適"

<details>

- **生成時刻**: `2025-12-30T01:49:51Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593429`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:51:30Z`
    - end_time: `2025-12-30T01:51:40Z`
    - runtime_sec: `10`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `52.1`
    - unit: `GFLOPS`
    - efficiency: `65.1%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`
    - block_i: `48`
    - block_k: `256`
    - block_j: `256`
    - micro_kernel: `6x16`

</details>

---

### v1.2.0
**変更点**: "B行列転置による連続メモリアクセス最適化"
**結果**: 理論性能の22.6%達成 `18.1 GFLOPS`
**コメント**: "水平加算のオーバーヘッドが大きく、v1.1.0より低性能"

<details>

- **生成時刻**: `2025-12-30T01:41:54Z`
- [x] **compile**
    - status: `success`
    - warnings: `-xCORE-AVX512 overrides -march=native`
- [x] **job**
    - id: `4593417`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:47:49Z`
    - end_time: `2025-12-30T01:48:00Z`
    - runtime_sec: `11`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `18.1`
    - unit: `GFLOPS`
    - efficiency: `22.6%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`
    - block_size: `64`
    - transpose_b: `true`

</details>

---

### v1.1.0
**変更点**: "4x24レジスタブロッキングマイクロカーネル実装"
**結果**: 理論性能の66.9%達成 `53.5 GFLOPS` **🏆SOTA**
**コメント**: "12個のAVX-512レジスタでCブロック保持、FMA命令フル活用。PG1.1の48.3 GFLOPSを上回る"

<details>

- **生成時刻**: `2025-12-30T01:39:56Z`
- [x] **compile**
    - status: `success`
- [x] **job**
    - id: `4593412`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:47:00Z`
    - end_time: `2025-12-30T01:47:10Z`
    - runtime_sec: `10`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `53.5`
    - unit: `GFLOPS`
    - efficiency: `66.9%`
- [x] **sota**
    - scope: `local`
    - previous: `-`
    - improvement: `first SOTA`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`
    - block_i: `64`
    - block_k: `256`
    - block_j: `512`
    - micro_kernel: `4x24`

</details>

---

### v1.0.0
**変更点**: "AVX-512 intrinsicsによるベクトル化とブロッキング最適化"
**結果**: 理論性能の21.9%達成 `17.5 GFLOPS`
**コメント**: "基本実装。レジスタブロッキングなしのため性能限定的"

<details>

- **生成時刻**: `2025-12-30T01:38:12Z`
- [x] **compile**
    - status: `success`
    - warnings: `-xCORE-AVX512 overrides -march=native`
- [x] **job**
    - id: `4593407`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T01:46:00Z`
    - end_time: `2025-12-30T01:46:10Z`
    - runtime_sec: `10`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `17.5`
    - unit: `GFLOPS`
    - efficiency: `21.9%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`

</details>
