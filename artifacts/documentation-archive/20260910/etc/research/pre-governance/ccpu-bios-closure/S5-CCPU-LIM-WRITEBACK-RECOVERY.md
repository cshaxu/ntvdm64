# S5 CCPU LIM Writeback Recovery

状态：2026-08-09。本文记录一个仅用于历史源码静态闭包的恢复；它不是 EMS 功能、PC/AT 设备实现、host shim 或 runtime 准入。

## 证据链

1. `base/ccpu386/ccpusas4.c` 在 `SAS_MM_LIM` 的物理 byte/word 写入路径调用 `LIM_b_write` 与 `LIM_w_write`。
2. `host/inc/host_def.h` 同时定义 `LIM` 和 `NTVDM`。因此当前 CCPU SAS 源码保留 call site，而 NTVDM profile 选择该分支。
3. `base/dos/emm_mngr.c` 含完整的原始 `LIM_b_write`、`LIM_w_write`、`LIM_str_write` 及其 `patch_pages` 支持代码，但外层历史条件是 `#ifndef NTVDM`。普通 NTVDM object 因而不导出这三个符号。

这证明的是同一历史源码中存在 profile 不一致：不是可以由新代码或空操作填补的 host API。

## 限域恢复

overlay 仅把上述最后一个函数块改为：当且仅当 `NTDOS64_CCPU_LIM_WRITEBACK_RECONSTRUCTION` 定义时，允许其与原有 `NTVDM` profile 一同编译。函数体、调用顺序和 EMS 数据结构均未重写。

该宏只施加于 `ntdos64-opennt-ccpu-lim-writeback-reconstruction-objects`，后者只被 `ntdos64-opennt-direct-ccpu-full-source-closure-fixture` 消费。所有 target 均为 `EXCLUDE_FROM_ALL`；没有 runner、默认 CTest、BIOS、BOP、DEM 或 DOS 执行路径引用该对象。

## 验证

`llvm-nm --defined-only --extern-only` 对该 object 的结果恰为：

```
_LIM_b_write
_LIM_str_write
_LIM_w_write
```

重新执行 full-source closure 链接诊断后，未解析符号清单不再包含这三个名称。链接仍因既有的 C-VIDC composition、NT 私有 console/API、CCPU generated vector 等来源失败；没有使用 `/FORCE`、alias、fake handler 或 runtime fallback。

## 禁止推论

- 未观察到 NTIO/DosInit 已动态到达 `SAS_MM_LIM` 或 EMS caller。
- 不得把这项静态闭包恢复解读为 EMS 可用、LIM 设备可用或软盘/磁盘策略需求。
- 真实 caller 到达前，不得将该 object 链接入历史 runtime，也不得让 SM0、BIOS、BOP 或 DEM 替代其语义。
