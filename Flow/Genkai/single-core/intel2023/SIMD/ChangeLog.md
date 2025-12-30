# SIMD📁 `ChangeLog.md`
🤖PG1.4
- **ハードウェア**: Genkai (玄界) Single-Core（1コア）
- **モジュール**: Intel oneAPI 2023.2 (icx)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

---

### v1.2.0
**変更点**: "レジスタブロッキング(4x8)+ブロッキング(64x64x256)"
**結果**: 理論性能の43.5%達成 `34.8 GFLOPS`
**コメント**: "v1.1.0から2.5倍改善。4行同時処理でレジスタ再利用最適化"

<details>

- **生成時刻**: `2025-12-30T01:47:00Z`
- [x] **compile**
    - status: `success`
    - log: `icx -O3 -march=native -mavx512f -mfma`
- [x] **job**
    - id: `4593415`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T10:47:46+09:00`
    - end_time: `2025-12-30T10:47:46+09:00`
    - runtime_sec: `0.06`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `34.8`
    - unit: `GFLOPS`
    - efficiency: `43.5%`
- [x] **sota**
    - scope: `local`
    - previous: `14.1`
    - improvement: `+146.8%`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `AVX-512`
    - block_size: `64x64x256`
    - reg_block: `4x8`

</details>

---

### v1.1.0
**変更点**: "ブロッキング(64x64x256)+AVX-512による最適化"
**結果**: 理論性能の17.6%達成 `14.1 GFLOPS`
**コメント**: "初回実行成功。更なる最適化が必要"

<details>

- **生成時刻**: `2025-12-30T01:39:30Z`
- [x] **compile**
    - status: `success`
    - log: `icx -O3 -march=native -mavx512f -mfma`
- [x] **job**
    - id: `4593393`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-30T10:43:02+09:00`
    - end_time: `2025-12-30T10:43:02+09:00`
    - runtime_sec: `0.14`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `14.1`
    - unit: `GFLOPS`
    - efficiency: `17.6%`
- [x] **sota**
    - scope: `local`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `AVX-512`
    - block_size: `64x64x256`

</details>

---

### v1.0.0
**変更点**: "AVX-512 intrinsics + 転置B + FMA最適化"
**結果**: コード生成完了、実行待ち `- GFLOPS`
**コメント**: "行列Bを転置してメモリアクセスパターン改善"

<details>

- **生成時刻**: `2025-12-30T01:38:00Z`
- [ ] **compile**
    - status: `pending`
- [ ] **job**
    - id: `-`
    - resource_group: `a-batch-low`
    - status: `pending`
- [ ] **test**
    - status: `pending`
    - performance: `-`
    - unit: `GFLOPS`
- **params**:
    - nodes: `1`
    - cores: `1`
    - matrix_size: `1000`
    - compile_flags: `-O3 -march=native -mavx512f -mfma`
    - simd_type: `AVX-512`

</details>
