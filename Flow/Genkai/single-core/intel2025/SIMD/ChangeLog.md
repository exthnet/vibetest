# SIMD📁 `ChangeLog.md`
🤖PG1.6
- **ハードウェア**: Genkai single-core (1コア)
- **モジュール**: Intel oneAPI 2025.1.3 (icx)

## Change Log

- 基本の型: `ChangeLog_format.md`に記載
- PMオーバーライド: `ChangeLog_format_PM_override.md`に記載

---

### v1.2.0
**変更点**: "B行列転置による連続メモリアクセス最適化"
**結果**: コンパイル・実行待ち `-`
**コメント**: "A,B^T両方がk方向に連続アクセス、水平加算で内積計算"

<details>

- **生成時刻**: `2025-12-30T01:41:54Z`
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
    - compile_flags: `-O3 -march=native -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`
    - block_size: `64`
    - transpose_b: `true`

</details>

---

### v1.1.0
**変更点**: "4x24レジスタブロッキングマイクロカーネル実装"
**結果**: コンパイル・実行待ち `-`
**コメント**: "12個のAVX-512レジスタでCブロック保持、FMA命令フル活用"

<details>

- **生成時刻**: `2025-12-30T01:39:56Z`
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
    - compile_flags: `-O3 -march=native -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`
    - block_i: `64`
    - block_k: `256`
    - block_j: `512`
    - micro_kernel: `4x24`

</details>

---

### v1.0.0
**変更点**: "AVX-512 intrinsicsによるベクトル化とブロッキング最適化"
**結果**: コンパイル・実行待ち `-`
**コメント**: "i,k,jループ順、64x64ブロッキング、8要素同時処理"

<details>

- **生成時刻**: `2025-12-30T01:38:12Z`
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
    - compile_flags: `-O3 -march=native -xCORE-AVX512 -qopt-zmm-usage=high`
    - simd_type: `AVX-512`

</details>
