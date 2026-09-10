# S5 Event Host Source Matrix

状态：针对 T-S5.1 clean-link 日志中 `nt_event.c` 的 source-owner 采证，2026-08-09。
它不是 event-loop 实现任务，更不授权让 `50h/14h` date trace 启动控制台线程。

## 结论

`nt_event.c` 是完整 BIOS table/static closure 中最大的单一 source owner（旧 SM-2
采样为 39 个 unresolved reference）。当前 `BIOS[50h] -> MS_bop_0 -> DemDispatch(14h)`
不调用 `nt_start_event_thread()`、`nt_event_loop()` 或 mouse/menu handling。故这些
符号出现在 clean-link log 中只说明 source unit 已被静态带入，不说明 event subsystem
是当前动态阻塞点。

## 历史 source owner

| `nt_event.c` 依赖 | 原始 owner | 说明 |
| --- | --- | --- |
| `DisplayErrorTerm` | `overlay/base/mvdm/softpc.new/host/src/nt_error.c:145` | 原始错误终止路径；不能以打印/忽略替代。 |
| `TerminateVDM` | `nt_term.c` 仅为目录内历史参考，未列入原始 `host/src/sources` | 非当前 NT4 host library 输入；不得据此建立 lifecycle shim。 |
| `VDMForWOW` | `overlay/base/mvdm/softpc.new/host/src/nt_reset.c:79` | 原始 session-mode global，不是新的 Boolean policy。 |
| `ThreadInfo` | `base/mvdm/softpc.new/host/src/nt_timer.c:185` | timer/event 共用 thread state；接入它会带来完整 timer lifecycle。 |
| `VdmUnhandledExceptionFilter` | `base/mvdm/softpc.new/host/src/nt_timer.c:964` | 与 timer source unit 同属，不可单独伪造 filter。 |
| `nt_mark_screen_refresh` | `base/mvdm/softpc.new/host/src/nt_graph.c:812` | 原始图形刷新 owner。 |
| `DoMouseInterrupt`、`MouseSystemMenuON/OFF`、`bPointerOff`、`MouseDisplay/Hide/InFocus` | `base/mvdm/softpc.new/host/src/nt_mouse.c` | 原始鼠标与菜单 state owner。 |
| `savedScreenState` | `base/mvdm/softpc.new/host/src/nt_fulsc.c:133` | 原始 fullscreen state owner。 |

这些都在现有历史 source tree；其中 `nt_timer.c`、`nt_graph.c`、`nt_mouse.c` 和
`nt_fulsc.c` 都是宽 source unit，必须在原始 caller 真正到达时以整个 owner closure
恢复，不能只导出一个变量或 callback。

## 现代 host seam 候选

下列 imports 只有 NT4 内部 console headers 的声明，目前没有可直接链接的现代 public
implementation：

| import | 历史声明 | `nt_event.c` 使用点 | 当前分类 |
| --- | --- | --- | --- |
| `GetConsoleInputWaitHandle` | `public/internal/base/inc/conroute.h:84` | event loop 等待输入 | private console seam |
| `ReadConsoleInputExW` | `public/internal/windows/inc/conapi.h:439` | nonblocking input batch read | private console seam |
| `VDMConsoleOperation` | `public/internal/windows/inc/conapi.h:38` | menu/fullscreen operation | private console seam |

这三项将来若被真实 event trace 到达，才可讨论 narrow modern facade。该 facade 必须保持
`nt_event.c` 的原函数边界、明确错误/停止语义，并与 console policy 隔离；不得以假成功、
空输入、常量 wait handle 或鼠标 no-op 推进 DOS。

## 当前处置

1. 不把 `nt_event.c` 或以上 owner 加入 v2 helper，只为降低 clean-link 数字。
2. 不引入 console shim，直到 NTIO prefix 或原始 host lifecycle 的动态 trace 实际进入
   event startup/read path。
3. 到达时先接入可用的原始 source owner；每个无法由 source 恢复的 private console
   import 才单独获得 capability/facade 合同与 fail-closed test。
