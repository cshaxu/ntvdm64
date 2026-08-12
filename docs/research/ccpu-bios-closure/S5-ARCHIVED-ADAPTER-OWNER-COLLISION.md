# S5 Archived Adapter Owner Collision

状态：2026-08-09 的 default-disabled historical source-composition 采证。本文不授权 runtime、host shim 或 DOS/BOP/DEM 行为。

## 目的

S5 的 v2 helper 必须最终以原始历史 source owner 形成 clean link；它不能一面保留早期
SM0 trace adapter 对 host symbol 的占位实现，一面再把同名的 OpenNT owner object 加入链接。
本记录固定该组合规则，并保存 2026-08-09 的可复现证据。

## 触发条件与结果

v2 helper 的动态入口仍严格限定为：

```text
BIOS[50h] -> MS_bop_0 -> DemDispatch(14h) -> demQueryDate
```

为区分完整 `BIOS[]` 静态表所需的原始 source owner 与真正缺失的 host 行为，诊断 target
临时加入了以下 default-disabled 原始 owner object groups：

| group | 原始 source |
| --- | --- |
| error host | `overlay/base/mvdm/softpc.new/host/src/nt_error.c` |
| reset shell | `base/mvdm/softpc.new/host/src/nt_reset.c` |
| host timer | `base/mvdm/softpc.new/host/src/nt_timer.c` |
| display/input | `nt_graph.c`, `nt_fulsc.c`, `nt_mouse.c` |

在这些 object 与 `local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c` 同时存在时，
`lld-link` 在解析 undefined symbols 前即报告 5 个 duplicate symbols：

| symbol | 原始 owner | SM0 adapter 中的占位定义 |
| --- | --- | --- |
| `host_error` | `nt_error.c` | `ccpu_monitor_sm0_foundation.c:124` |
| `host_mouse_install1` | `nt_mouse.c` | `ccpu_monitor_sm0_foundation.c:132` |
| `host_mouse_install2` | `nt_mouse.c` | `ccpu_monitor_sm0_foundation.c:133` |
| `host_timer_event` | `nt_timer.c` | `ccpu_monitor_sm0_foundation.c:137` |
| `host_TimeStamp` | `nt_timer.c` | `ccpu_monitor_sm0_foundation.c:138` |

原始日志：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-helper-clean-link-owner-input-v4-20260809.log
```

该失败是有价值的 source-ownership 证据，不是可接受的链接策略。诊断没有使用
`/FORCE:UNRESOLVED`、别名、weak symbol、重命名或 local replacement。

## `nt_term.c` 的原始构建地位

`base/mvdm/softpc.new/host/src/nt_term.c` 保留在固定输入目录，但原始
`base/mvdm/softpc.new/host/src/sources` 的 `SOURCES=` 清单并不包含它。此前为它建立的
standalone clang-cl object gate 已移除；其 `VDMForWOW` 声明错误不再被视作需要恢复的
SoftPC 语义，也不再通过 local `extern`、常量 false、`ExitVDM` no-op 或强制成功绕过。

本研究仍可把该文件作为只读历史参考，但不得把它当作 NT4 host library 的已确认
`TerminateVDM` owner 或接入 v2 helper。若后续发现原始 build description 明确将它纳入另一个
历史 target，必须重新以该 build evidence 立项，而不是凭目录存在性恢复。

## 组合规则

1. `ccpu_monitor_sm0_foundation.c` 是 `archived/legacy-adapters`，只可用于 bounded trace。
2. 任何目标若要接入 `nt_error.c`、`nt_mouse.c`、`nt_timer.c` 等原始 owner，必须先将 SM0
   adapter 从该目标的 link graph 移出，不能删除或覆盖原始 symbol。
3. 这项移出需要由 helper 的 SAS/register/entry contract 另行证明；不得用新的 local
   callbacks 取代 SM0 callbacks 后宣称 clean closure。
4. v2 helper 在完成该迁移前仍是 source-closure diagnostic，不得运行为 DOS runtime。
5. private console imports、event thread、WOW termination 均没有因本记录而获得 shim 准入。

## 下一步

先对 SM0 foundation 的每个仍被 helper 使用的 symbol 做 caller/owner 清点，分成：

1. helper 自身的仅 trace-state 机械前置；
2. 已有原始 OpenNT owner；
3. 尚无原始 owner 或尚未采证的历史 host seam。

只有第一类可被最小的无行为 test harness 替代，且替代不能修改 guest 或 BOP/SVC 结果；第二类
应直接移交原始 source；第三类继续保持 stop condition，等待 caller-first 采证。
