# S5 CCPU 完整 BIOS 表链接闭包清单

状态：第一轮 owner 分类完成；不得据此引入通用空 shim。

## 采证方法

目标是 default-disabled 的
`ntdos64-opennt-ccpu-software-monitor-sm2-fixture`。它让原 CCPU 执行受限
`C4 C4` BOP，且要求保留完整历史 `BIOS[]` 表；不得以最小 selector table 代替。

第一轮链接有 210 个未解符号。审计发现其中 `Sim32GetVDMPointer`、`DemDispatch`、
`VDMForWOW` 和旧 NT native imports 分别缺少原 `sim32`、原 DEM、既有的窄 session context
及 `ntdll.lib`。这些不是新 host 行为，已仅作为原 owner 输入加入此 trace target。

接入原 DEM 后曾有 242 个未解符号。随后审计确认 `C_Video` 由独立原 C-VID source closure
的 `cvidc/vglfunc.c` 所有；将原 `ntdos64-opennt-cvidc-archive` 接入后，未解项降为 226，且
`C_Video` 已不在 ledger 中。当前结论仍是完整 source closure 尚未达到可执行，不是可以用
local dummy 修复的链接错误。

逐符号、引用 CMake object target 与源 object 的机械采证见
`docs/research/ccpu-bios-closure/SM2-UNRESOLVED-SYMBOL-LEDGER.tsv`；它是从本次
default-disabled link log（含原 `sim32`、DEM、session context、C-VID owner）直接生成，
不包含任何建议性 shim。

## 当前分类

| 类别 | 典型符号/owner | 结论 | 处置 |
| --- | --- | --- | --- |
| 已纠正的组合遗漏 | `Sim32GetVDMPointer`、`DemDispatch`、`VDMForWOW`、NT native imports | 原始 source/context 或当前 import library 已有 | 仅纳入 SM-2 default-disabled fixture；不产生新 shim |
| OEM facade | `CreateFileOem`、`FindFirstFileOem`、`GetFullPathNameOem`、`GetDiskFreeSpaceOem`、`CreateDirectoryOem` 等 | DEM 的真实现代 host seam；F1 仅闭合 `CreateFileOem` | 继续按活跃 caller 与 29-entry work order 逐项恢复；禁止重用 archived path policy |
| video generated/runtime | 通用 `Video` vector、残余 `get/setVideo*` 与 display owner | `C_Video` 已由原 C-VID archive 闭合；但 `Video` 的历史 binding/initializer 仍未证实，非单个 Win32 wrapper | 保持 unresolved；先完成跨版本 source matrix 和 field-level evidence，禁止 `Video = C_Video`、假 vector 或 no-op video |
| console/event/mouse | `GetConsoleInputWaitHandle`、`ReadConsoleInputExW`、`VDMConsoleOperation`、`Mouse*`、`nt_mark_screen_refresh` | 历史控制台及输入产品层 host dependency | 仅当从真实 NTIO/NTDOS trace 到达 caller 后建立窄 facade；当前不为 BIOS 表静态引用提前实现 |
| timer/EOI/lifecycle | `ActivityCheckAfterTimeSlice`、`DemHeartBeat`、`InitSound`、`TerminateVDM`、PIF/timeslice symbols | 历史 process/event/timing 生命周期，而非 service 11 load 本体 | 分离 owner，记录当前 API availability；不得将其改造成 CPU fallback |
| disk/comms/printer/EMS | `host_fdisk_*`、`host_flpy_*`、`host_com_*`、`host_lpt_*`、`HoldEMMBackFillMemory` | 可配置设备/媒体策略，当前仅被完整表强制链接 | 延后至真实 caller；任何实现须保持 device owner 与 host backend 分离 |
| 可选 dispatcher | `CmdDispatch`、`DpmiDispatch`、`DBGDispatch`、`ms_bop` | 非 service 11 正常返回所需 | 不提前接入；只在对应 original BOP trace 被证明需要后处理 |

DEM 所属 OEM/RTL unresolved 的逐 caller 状态另见 `S5-DEM-OEM-CALLER-MAP.md`。它明确 F1
`CreateFileOem` 是当前唯一动态证明项。

video composition input 的本地只读复核见 `S5-VIDEO-INPUT-RECONCILIATION.md`；`C_Video`
owner 已闭合，但通用 `Video` binding 仍未授权 reconstruction。

## CCPU 与 host closure 的边界

受限 SM-0 fixture 当前可构建并执行：它由原 `c_cpu_simulate()` 执行 `C4 C4 FE`，经原
`c_cpu_unsimulate()` 返回。这证明 CCPU loop 的基础执行/return 不是本轮 226 项 unresolved
的同义词。但该 fixture 使用 archived trace foundation，不能提升为 runtime 能力。

相反，SM-2 的失败说明“让原 CCPU 通过完整 BIOS table 动态进入 BOP”仍需要一套可追溯的
历史 host closure。该 closure 应由上表逐项恢复，不得把外部 CPU backend、简化 BOP dispatcher
或局部 handler 当作替代。

## T-S5.1 static/dynamic/owner 三栏结论

跨进程 v2 helper 的实际链接实验补充了一个不能混淆的事实：它即使只调用
`BIOS[50h]`，也必须先解析原始 `bios.c` 里完整 `BIOS[]` initializer 的每一个函数地址。
因此一个 unresolved symbol 可能处在下列三种完全不同的关系中：

| 栏位 | 当前已证实内容 | 不能推出的结论 |
| --- | --- | --- |
| 静态 table | `bios.c` 的完整 `BIOS[]` 让 keyboard、diskette、video、serial、printer、mouse、DPMI、EMS 等 entry 对 linker 可见 | 这些 BOP 已由当前 date trace 或 NTIO prefix 动态执行 |
| 动态 reachability | 当前唯一动态 selector 是 `50h`；已由原链 fixture 证明 `MS_bop_0 -> DemDispatch`，并有 `14h` date 与 direct `11h demLoadDos` 的独立 source fixture | 完整 BIOS host closure 已经可运行，或 `11h` 已经经过 CCPU/完整表/跨进程 transport |
| host owner | link log 中的 console/event/video/timer/device unresolved 各有历史 caller/source owner；`C_Video` object owner 已闭合而通用 `Video` binding 未闭合 | 可以因为它们尚未动态到达而以 local dummy、空 vector 或 `/FORCE` 满足完整表 |

两次 v2 链接均使用同一完整 table provider group：第一次尝试完整 visible-state
accessor，第二次仅投影 AX/CX/DX/CS/IP。未解集合没有缩小，故目前没有证据把问题归咎于
register accessor。下一张逐项 ledger 的目标是为每个静态 table entry 记录 source owner、
当前静态引用、已证明的动态 selector、以及若将来真正到达时的 host seam；它不得把
link-visible 项直接升级为实现任务。

## 后续顺序

1. 对 226 项生成逐 symbol -> 原 object/source owner 的机械 ledger；
2. 优先完成真实 `demLoadDos` 已抵达的 OEM facade 组，而非 video、mouse 或 DPMI；
3. 为缺失/generated video 建立独立证据包，不使用 fallback vector；
4. 仅在每个 owner 可闭合后，重新尝试完整 BIOS-table trace。
