# S5 SM0 Adapter Symbol Triage

状态：2026-08-09。范围仅为 `local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c`
对 historical source closure 的退出审计；不增加 runtime 行为。

## 结论

SM0 foundation 不是可逐项“升级”为 host shim 的接口层。它为 bounded CCPU trace 提供了 fail-closed
占位定义，其中大多数已有 OpenNT source owner，或本来属于 CCPU generated vector/未采证路径。
因此退出策略应是按 helper 的真实 caller 逐个移除 SM0 定义，而不是将其改写为成功返回。

## 导出分类

| SM0 symbol | 历史 owner / 证据 | 分类 | 当前处置 |
| --- | --- | --- | --- |
| `c_effective_addr` | `overlay/.../ccpu386/c_effective_addr.c` | 已准入 `overlay/reconstruction` | 保持该唯一确认的缺失 SoftPC 语义恢复；不由 SM0 提供。 |
| `host_simulate` | `host/src/nt_cpu.c:82` | 原始 host CPU owner | 仅当真实 CCPU execution caller 到达时接入整个原始 owner closure。 |
| `host_clear_hw_int`, `host_set_hw_int`, `host_exint_hook`, `host_swint_hook` | `host/src/nt_cpu.c`；CCPU exception/ICA callers | 原始 host CPU owner | 不得保留 SM0 no-op 以假装中断或 exception 已处理。 |
| `host_EOI_hook`, `host_ica_lock`, `host_ica_unlock`, `WOWIdle` | `host/src/nt_eoi.c` | 原始 EOI/interrupt lifecycle owner | 仅与原始 ICA/host lifecycle 一并恢复。 |
| `host_error` | `overlay/.../host/src/nt_error.c:145` | 原始 error owner | 已在 source-composition 中与 SM0 冲突；必须移出 SM0 后才可接入。 |
| `host_mouse_install1`, `host_mouse_install2` | `host/src/nt_mouse.c:282,290` | 原始 mouse owner | 已在 source-composition 中与 SM0 冲突；不允许 fake mouse install。 |
| `host_timer_event`, `host_TimeStamp` | `host/src/nt_timer.c:384,540` | 原始 timer owner | 已在 source-composition 中与 SM0 冲突；timer owner 自带 lifecycle closure。 |
| `host_get_jump_restart`, `host_get_q_calib_val` | `cpu4gen.h` 将其定义为 `Cpu` generated vector slots | CCPU vector contract | 不是可独立定义的 host API；需先恢复正式 vector binding 的来源。 |
| `LIM_b_write`, `LIM_w_write`, `LIM_str_write` | `ccpusas4.c` 的 EMS write hooks；`base/dos/emm_mngr.c` 保留原始函数体但由 NTVDM profile 排除 | 已定位的原始 source owner；仅限 static closure | 只在默认关闭的 full-source diagnostic 以宏门控复用原函数体；不得由 SM0 提供，且在真实 EMS caller 到达前不准入 runtime。 |
| `EDL_fast_bop` | `c_main.c:3344` 的 fast-BOP decode caller | BOP 变体，owner 尚未在当前采证中定位 | 保持 stop condition；不得映射到手写 BOP/DEM handler。 |

## 对 v2 helper 的意义

`BIOS[50h] -> MS_bop_0 -> DemDispatch(14h)` 本身不应动态调用上表中的 mouse、timer、EOI、EMS
或 fast-BOP 路径。它们出现在 target 中的原因是完整 `BIOS[]` / provider 的静态闭包，而非 date
trace 已到达这些行为。

所以下一项不是把 SM0 foundation 从 helper 粗暴删除。必须先列出 v2 helper 当前真正使用的 SM0
符号，确认其仅为 trace-state、register/SAS 机械前置；然后才可为这些最小前置建立无 guest
行为的独立 harness，并让同名的历史 owner 不再被占位实现遮蔽。

## 禁止项

1. 不得将任何 SM0 fail-closed callback 改成 return success、constant false/true 或 host no-op。
2. 不得用 `#define`、linker alias、weak symbol 或重命名同时保留 SM0 与原始 owner。
3. 不得因 source-composition link error 而把 `nt_event.c`、timer、mouse 或 console paths 解释为
   已经需要现代 shim。
4. `EDL_fast_bop` 与 LIM hooks 在原始 owner 尚未定位前，仍是停止条件，不能进入 DOS runtime。
