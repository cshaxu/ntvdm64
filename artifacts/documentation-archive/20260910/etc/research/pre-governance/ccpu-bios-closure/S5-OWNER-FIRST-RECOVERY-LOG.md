# S5 Owner-First Recovery Log

状态：2026-08-09。记录 no-SM0 `MS_bop_0 -> DemDispatch` link-only closure 中每一次单一
原始 owner 恢复；不构成 runtime 准入。

| 次序 | 接入的原始 owner | 直接消除 | 新增/暴露的下游 | 总 unresolved | 结论 |
| --- | --- | --- | --- | ---: | --- |
| 0 | 无 SM0、但 Sim32 的 SAS 名称仍被旧 fixture 改名 | 无 | `host_sas_init`, `host_sas_term` | 114 | SM0 仍遮蔽 SAS source ownership。 |
| 1 | 原始名称 `sim32.c` (`SIM32`, `_X86_`) | `host_sas_init`, `host_sas_term` | 无新增 source family | 112 | 可保留；见 `S5-ORIGINAL-SIM32-SAS-RECOVERY.md`。 |
| 2 | `nt_error.c` | `host_error` | `host_nls_get_msg`、错误字符串、timer suspend/resume、console/session state、`VDMForWOW`、`TerminateVDM` | 115 | 可作为真实 owner 证据保留，但不能为减少数字而继续盲接 NLS/timer/console。 |
| 3 | 原始 `oemuni/file.c + process.c` | 18 个 `*Oem` facade import，包括 `GetDriveTypeOem` | 仅 BOP/DEM 对照闭包：无新增 symbol；仍有 RTL/NT native path、console、device 与 CCPU owner | 98 | 由固定 NTDOS 的 `50h/0Fh demGetDrives` 直接证明需要；只恢复原始 source owner，不提供 modern facade。详见 `S5-ORIGINAL-OEM-OWNER-RECOVERY.md`。 |
| 4 | 原始 `host/src/nt_mem.c`（不重复引入 `sim32.c`） | `InitIntelMemory`、`FreeIntelMemory` | `VdmSetPhysRecStructs` | 97 | `CPU_40_STYLE` 下原始 SAS allocator 的下一层私有 VDM 记录依赖；只作 default-disabled source closure，不得以本地物理记录 shim 推进 runtime。详见 `S5-ORIGINAL-INTEL-MEMORY-OWNER-RECOVERY.md`。 |

最新日志：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-msbop-closure-no-sm0-original-sim32-v2-20260809.log
  s5-v2-msbop-closure-no-sm0-sim32-error-owner-20260809.log
  s5-ntdos64-opennt-historical-transport-v2-msbop-closure-oem-intelmem-owner-20260809.log
```

## 规则

1. 一个 original owner 可以增加 unresolved count；这表示其真实下游职责已被揭示，不是应以
   shim/常量/no-op 消去的错误。
2. 只有被真实 guest/host trace 首次抵达的 owner，才可从此表继续进入下一步恢复。
3. `nt_error.c` 所展开的 NLS、timer、console/session 链尚未由 date path 动态抵达，因此保持
   source evidence，不准入 modern host seam。
4. 所有 target 均 default-disabled，且不使用 `/FORCE:UNRESOLVED`、alias 或 local replacement。
