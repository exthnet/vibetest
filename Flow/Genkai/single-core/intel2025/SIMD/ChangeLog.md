# SIMD📁 `ChangeLog.md`
🤖PG1.6
- **ハードウェア**: Genkai single-core (1コア)
- **モジュール**: Intel oneAPI 2025.1.3 (icx)

## Change Log

- 基本の型: `ChangeLog_format.md`に記載
- PMオーバーライド: `ChangeLog_format_PM_override.md`に記載

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
