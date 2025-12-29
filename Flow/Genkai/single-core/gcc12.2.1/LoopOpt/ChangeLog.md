# LoopOpt📁 `ChangeLog.md`
🤖PG1.2
- **ハードウェア**: 玄界 (Genkai) single-core（1コア）
- **モジュール**: GCC 12.2.1

## Change Log

- 基本の型：`ChangeLog_format.md`に記載
- PMオーバーライド：なし

---

### v1.2.0
**変更点**: "キャッシュブロッキング + 4xループアンローリング"
**結果**: `7.91 GFLOPS` (N=1000, 0.253秒)
**コメント**: "手動アンローリングはコンパイラ最適化を妨げ、v1.1.0より低下"

<details>

- **生成時刻**: `2025-12-29T14:42:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590894`, status: `success`
- [x] **test**: performance: `7.91`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `64`, unroll: `4x`

</details>

---

### v1.1.2
**変更点**: "キャッシュブロッキング (BLOCK_SIZE=128)"
**結果**: `8.37 GFLOPS` (N=1000, 0.239秒)
**コメント**: "BLOCK_SIZE=64より低下、L1キャッシュオーバーフロー"

<details>

- **生成時刻**: `2025-12-29T14:40:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590889`, status: `success`
- [x] **test**: performance: `8.37`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `128`

</details>

---

### v1.1.1
**変更点**: "キャッシュブロッキング (BLOCK_SIZE=32)"
**結果**: `8.10 GFLOPS` (N=1000, 0.247秒)
**コメント**: "BLOCK_SIZE=64より低下、ブロックオーバーヘッド増加"

<details>

- **生成時刻**: `2025-12-29T14:39:00Z`
- [x] **compile**: status: `success`
- [x] **job**: id: `4590887`, status: `success`
- [x] **test**: performance: `8.10`, unit: `GFLOPS`
- **params**: BLOCK_SIZE: `32`

</details>

---

### v1.1.0
**変更点**: "キャッシュブロッキング (BLOCK_SIZE=64) + i-k-j順序"
**結果**: `9.57 GFLOPS` (N=1000, 0.209秒)
**コメント**: "v1.0.0(6.25 GFLOPS)から1.5倍、ベースラインから4.5倍高速化"

<details>

- **生成時刻**: `2025-12-29T14:38:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - log: `N/A`
- [x] **job**
    - id: `4590883`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T23:38:21+0900`
    - end_time: `2025-12-29T23:38:22+0900`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `9.57`
    - unit: `GFLOPS`
    - accuracy: `検証済み（N=10で結果一致）`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - loop_order: `i-k-j`
    - BLOCK_SIZE: `64`

</details>

---

### v1.0.0
**変更点**: "i-k-j ループ順序によるキャッシュ局所性最適化"
**結果**: `6.25 GFLOPS` (N=1000, 0.320秒)
**コメント**: "ベースライン(i-j-k順)の2.14 GFLOPSから約2.9倍高速化"

<details>

- **生成時刻**: `2025-12-29T14:36:18Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
    - log: `N/A`
- [x] **job**
    - id: `4590875`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T23:35:44+0900`
    - end_time: `2025-12-29T23:35:45+0900`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `6.25`
    - unit: `GFLOPS`
    - accuracy: `検証済み（N=10で結果一致）`
- [x] **sota**
    - scope: `local`
- **params**:
    - N: `1000`
    - loop_order: `i-k-j`

</details>

---

### Baseline
**変更点**: "BaseCode (i-j-k順) のベースライン測定"
**結果**: `2.14 GFLOPS` (N=1000, 0.935秒)
**コメント**: "最適化なしの基準性能"

<details>

- **生成時刻**: `2025-12-29T14:32:00Z`
- [x] **compile**
    - status: `success`
    - warnings: `none`
- [x] **job**
    - id: `4590869`
    - resource_group: `a-batch-low`
    - start_time: `2025-12-29T23:33:17+0900`
    - end_time: `2025-12-29T23:33:18+0900`
    - runtime_sec: `1`
    - status: `success`
- [x] **test**
    - status: `pass`
    - performance: `2.14`
    - unit: `GFLOPS`
- **params**:
    - N: `1000`
    - loop_order: `i-j-k (original)`

</details>
