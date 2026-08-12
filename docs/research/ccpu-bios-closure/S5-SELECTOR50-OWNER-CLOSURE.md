# S5 Selector-50 Original Owner Closure

状态：源码与 2026-08-09 complete-table linker ledger 对账。此文定义下一轮恢复的
粒度；不授权以缩表、改写 dispatcher 或 local dummy 取得链接成功。

## 已证实的最短原始链

```text
BIOS[50h]                         base/.../bios/bios.c:293
  -> MS_bop_0                     host/src/nt_bop.c:146
       -> Sim32GetVDMPointer(CS:IP, 1, FALSE)
       -> DemDispatch(service)
       -> setIP(IP + 1)
       -> IDLE_disk() only if service != 14h and != 15h
```

对于当前的 `14h` date fixture，`IDLE_disk()` 按原始条件不会调用。`MS_bop_0` 没有
`TerminateVDM`、WOW、console、mouse、video 或 disk controller 的直接运行时调用；这些
符号属于同一 translation unit 中的其他 `MS_bop_n`，或由完整 BIOS table 拉入的其他
entry。不能仅因它们在 object/link graph 中出现，就把它们列为 `14h` 的动态依赖。

## 第二层：DEM 静态服务表

`base/mvdm/dos/dem/demdisp.c:98` 的原始 `apfnSVC[]` 含 **72** 个函数指针。它的
`DemDispatch()` 在验证范围后执行 `apfnSVC[iSvc]()`；因此即便现在只动态调用
`DemDispatch(14h)`，linker 仍须解析该表中所有 entry 的函数地址。

`14h` 本身指向 `demQueryDate()`（`demgset.c:448`），该函数实际只调用
`GetLocalTime()`，并用原始 register owner 设置 `DH/DL/CX/AL`。这证明“日期服务的
动态行为很窄”，但不证明“原 DEM dispatcher 的静态闭包很窄”。

同样，`11h` 指向 `demLoadDos()`；它是第一项 BYOB load 证据，但仍不能免除
`apfnSVC[]` 的完整静态闭包。

## 当前 linker 规模

`SM2-UNRESOLVED-SYMBOL-LEDGER.tsv` 的机械采样有 226 个 unresolved 记录。按 CMake
原 owner object group 计数：

| owner group | unresolved 数 | 与当前 `50h/14h` 的关系 |
| --- | ---: | --- |
| DEM | 58 | `apfnSVC[]` 静态带入；只有 `demQueryDate` 已动态证实。 |
| video | 50 | `BIOS[]` 静态带入；当前未动态到达。 |
| event host | 39 | static closure；当前未动态到达。 |
| comms | 21 | `BIOS[]` 静态带入；当前未动态到达。 |
| disks | 11 | `BIOS[]` / DEM 静态带入；`14h` 未调用。 |
| key/mouse | 9 | `BIOS[]` 静态带入；当前未动态到达。 |
| timer | 9 | static closure；当前 `14h` 还特意绕过 `IDLE_disk`。 |
| nt_bop | 7 | 同一 `nt_bop.c` 的其他 BOP entry；不是 `MS_bop_0` date path。 |
| support/system/EOI/other | 22 | 完整表、生命周期或 import 组；需逐 caller 判断。 |

这些数字是特定 default-disabled closure diagnostic 的链接数据，不能解释为 runtime
feature backlog。

## T-S5.1 clean-link snapshot

2026-08-09 已在未使用 `/FORCE:UNRESOLVED` 的 v2 helper target 上重新执行 link；
唯一新增的 link option 是 `/errorlimit:0`，用于完整保留诊断，不能使目标可执行。原始
输出保存在：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-v2-helper-clean-link-20260809-full.log
```

该日志包含 **244 个不同的 undefined symbols**。最先报告的一组是
`host_runtime_inquire`、`GetPerfCounter`，随后是 `nt_event.c` 的
`DisplayErrorTerm`、`VDMForWOW`、`ThreadInfo`、`TerminateVDM`、console input 和
mouse/display imports。这是 linker 报错顺序，不是动态 reachability 优先级；完整的
selector 与 service-table 静态闭包仍须按 caller/source owner 处理。

后续接入已证实的原 `C_Video` archive owner 后，helper 的同条件 clean-link count 为
228；这是一个 source-composition 修正，不影响通用 `Video` binding 未解的结论。详见
`docs/S5-VIDEO-INPUT-RECONCILIATION.md`。

## Source recovery check

首批两个符号不是 source-missing：

| symbol | 原始 source owner | 当前是否进入 v2 helper | 当前处置 |
| --- | --- | --- | --- |
| `host_runtime_inquire` | `overlay/base/mvdm/softpc.new/host/src/config.c:894` | 否 | 既有 `ntdos64-opennt-config-x86-prefix-trace-fixture` 在当前 toolchain clean-build、exit 0；`config.c` 仍有原始 `config`、资源与生命周期职责，且当前没有 `50h/14h` 动态 caller，故不为降错误数提前引入。 |
| `GetPerfCounter` | `base/mvdm/softpc.new/host/src/nt_timer.c:340` | 否 | 独立 `original-perf-counter` probe 证明 source 能编译，但 `nt_timer.c` 整个 object clean-link 仍有 29 个 lifecycle unresolved（EOI、RTC、sound、COM/LPT/FDC、DEM heartbeat、CPU interrupt 等）；它不是可单独接入的计时 API。 |

这个结论很重要：**“linker 先报出”不等于“缺失”或“应先恢复”**。当真实 trace 首次到达
这些 owner 时，应优先接入以上原始 source；只有确实找不到原始实现、且 caller contract 已
采证时，才讨论一个窄的现代 compatibility facade。

`ntdos64-opennt-original-perf-counter-fixture` 与其 29 项 clean-link 输出仅是
default-disabled owner-width probe；日志为
`artifacts/build/current/opennt-r2-bridge-ninja-i686/s5-original-perf-counter-clean-link-20260809.log`。
它没有启动 `host_timer_init` 或事件线程，因此也不证明 timer subsystem 已运行。

## 允许的恢复顺序

1. 保持原 `BIOS[]` 与 `apfnSVC[]`，不为测试改造为 selector/service mini-table。
2. 对 complete-table link log 的每个 unresolved 标注“仅静态”或“已被真实 trace 到达”。
3. 仅对后者恢复原 source owner；若其需要现代 host API，建立 caller-first、
   fail-closed 的 facade，并在治理台账中标为 `compat/host`。
4. 当 `50h/14h` 的完整原始闭包 clean-link 后，再让固定 record helper 运行它；不得
   使用 `/FORCE:UNRESOLVED` 作为 T-S5.1 helper 的验收。
5. `50h/11h`、shared SAS、NTIO prefix execution 依次跟进；每次只根据新的真实 stop
   扩展 owner 闭包。

现有直接 fixture 可以继续保存为 bounded trace evidence，但它们没有证明 T-S5.1 helper
已 clean-link，也不能替代这里的 owner-first closure work。
