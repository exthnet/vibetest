# SIMD📁 `ChangeLog.md`
🤖PG1.4
- **ハードウェア**: Genkai (玄界) Single-Core（1コア）
- **モジュール**: Intel oneAPI 2023.2 (icx)

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：`ChangeLog_format_PM_override.md`に記載

---

### v1.1.0
**変更点**: "ブロッキング(64x64x256)+AVX-512による最適化"
**結果**: コード生成完了、実行待ち `- GFLOPS`
**コメント**: "L2キャッシュ効率向上のためブロッキング追加"

<details>

- **生成時刻**: `2025-12-30T01:39:30Z`
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
