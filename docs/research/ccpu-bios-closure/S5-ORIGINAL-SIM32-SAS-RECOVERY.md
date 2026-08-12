# S5 Original Sim32 SAS Recovery

状态：2026-08-09，default-disabled no-SM0 source-closure 采证。

## 问题

原有 `ntdos64-opennt-sim32-objects` 为 SM0 fixture 将 `host_sas_init` 与 `host_sas_term`
重命名，令 `ccpusas4.c` 仍由 archived SM0 foundation 分配 aperture。这一策略适用于早期
bounded fixture，但不适用于证明原始 source ownership 的 no-SM0 closure。

## 恢复

新增独立 `ntdos64-opennt-sim32-owner-closure-objects`：

- 输入仍是固定 OpenNT `host/src/sim32.c`；
- 使用原始 `SIM32`、`_X86_` profile；
- 保留原始 `host_sas_init` / `host_sas_term` 名称；
- 仅 no-SM0 的 `BIOS[]` 与 `MS_bop_0` closure diagnostics 消费它；
- 原 SM0 target、其 renamed allocator 和 runtime 禁入状态完全不变。

该 source 的 native header 顺序需要与既有 Sim32 gate 一致，因此只将新 target 加入同一
compiler preamble exclusion；这是一项声明顺序兼容，不引入 local behavior。

## 验证

`ntdos64-opennt-historical-transport-v2-msbop-closure` 重新链接结果：

| 输入 | 不同 unresolved | `host_sas_init/term` |
| --- | ---: | --- |
| renamed Sim32 / SM0 aperture 旧对照 | 114 | 均 unresolved |
| original-name Sim32 / no-SM0 对照 | 112 | 均已由原始 source 提供 |

duplicate symbols 仍为 0，未使用 `/FORCE:UNRESOLVED`、alias 或 local SAS replacement。

日志：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-msbop-closure-no-sm0-original-sim32-v2-20260809.log
```

## 边界

这仅恢复 SAS allocation owner，未证明 `c_cpu_init`、`MS_bop_0`、`DemDispatch` 或任何 DOS
service 可运行。后续每次只接入一个已定位的原始 owner，并重新测量静态闭包；不得以这个结果
重启 SM0、合成 BIOS、BOP/SVC handler 或 host-memory facade。
