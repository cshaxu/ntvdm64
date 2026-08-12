# S5 BIOS Static/Dynamic/Owner Ledger

状态：静态 table 与已证实动态路径的第一版对账，2026-08-09。此文件是链接闭包采证，
不是为每一项创建现代 shim 的工作单。

## 判定规则

原始 `base/mvdm/softpc.new/base/bios/bios.c:120-693` 用一个 C initializer 定义
`void (*BIOS[])()`。在给定编译宏下，initializer 中每个被选中的函数地址都是 linker
必须解析的**静态引用**；这与 CCPU 是否实际执行过该 selector 无关。`#if` 中未选中
的候选不属于该 build 的静态闭包。

动态可达性只接受下列证据：原始 CCPU 的 BOP decode trace，或直接调用原
`BIOS[selector]` 的 bounded fixture。未列入“动态证实”的 selector 一律不因 table
存在而升级为运行要求。

## 当前动态事实

| selector | 后续服务 | 证据 | 当前结论 |
| --- | --- | --- | --- |
| `50h` | `14h` date | 原 `BIOS[50h] -> MS_bop_0 -> DemDispatch` fixture | 直接 selector trace 已证实；仅是 trace fixture。 |
| `50h` | `11h demLoadDos` | 原 `DemInit` 后的 direct BYOB fixture；`demdisp.c`、`demmisc.c` | 原 DEM 读取并写入历史 SAS 已证实；尚未经完整 CCPU table 或跨进程 transport。 |
| 其余 | 无 | 无 | 未证实动态可达。 |

这里的 `14h`、`11h` 是 `MS_bop_0` 在 CS:IP 读取的 service byte，不是新的
`BIOS[]` selector。`MS_bop_0` 处理后把 IP 推进一个字节。

## 静态 table 分组

| selector 范围 | 由 `bios.c` 选中的历史 entry 家族 | 静态 owner 类别 | 动态证实 | 目前处置 |
| --- | --- | --- | --- | --- |
| `00h-08h` | reset、dummy/unexpected/illegal、可选 license/WD/debug、illegal op/driver | CCPU reset/error | 无 | 保留原 owner；不得为 date trace 补 lifecycle。 |
| `09h-1Fh` | keyboard、diskette、video、equipment、memory、disk、serial、cassette、printer、time、idle | PC/AT BIOS 与 display/input/device | 无 | 仅静态可见；不新增 keyboard/disk/video shim。 |
| `20h-2Fh` | build id、可选 VDD、command、HFX redirector | build/product/optional host integration | 无 | 由编译宏决定；不作为 base DOS boot 的前置。 |
| `30h-33h`、`49h-4Bh` | DPMI dispatch、DPMI exception/ring-0 hooks | DPMI/protected-mode | 无 | 延后到真实 selector trace。 |
| `34h-3Fh` | 可选 Novell driver network BOP | network driver | 无 | 不得因 table 预实现网络。 |
| `40h-48h` | diskette I/O、可选 EGA video，其余 illegal | FDC/video | 无 | 只记录 link visibility。 |
| `50h-5Fh` | `MS_bop_0` 至 `MS_bop_F`（或特定构建的替代 entries） | NT BOP dispatcher | 仅 `50h` | 当前首个真实 host/DEM 链；其余 MS BOP 未获授权。 |
| `60h-6Fh` | version、COM、config、EMS、return、suspend/terminate、driver/CD-ROM | product/config/EMS/device | 无 | 不以完整表为由补设备。 |
| `70h-7Fh` | RTC、redirector、D11、XMS/287、WORM | RTC/redirector/XMS/add-on | 无 | 保留原 source；等待 caller-first evidence。 |
| `80h-8Fh` | illegal | error owner | 无 | 无 shim。 |
| `90h-9Fh` | 可选 bootstrap、GDI/User/Swin、SoftWindows、sound | Windows/SoftWindows/display | 无 | 不进入当前软件 monitor route。 |
| `A0h-AFh` | IPX/TCP/ISWS、Swin redirector/file/task | network/HFX | 无 | 仅静态引用。 |
| `B0h-BFh` | virtual device trap、mouse install/interrupt/video/EM callbacks | virtual device/mouse | 无 | 不把 mouse/display owner 伪装成 BOP success。 |
| `C0h-CFh` | illegal 与可选 host mouse install | mouse | 无 | 同上。 |
| `D0h-DFh` | profiling 与 illegal | debug/profile | 无 | 不作为 runtime closure。 |
| `E0h-F7h` | illegal | error owner | 无 | 无 shim。 |
| `F8h-FFh` | 可选 driver trace、switch-to-real-mode、unsimulate、HG/control | debug/mode/monitor termination | 仅受限 SM-0 的 `FEh` return trace | 只能留在 bounded trace；不把 V86 或 mode transition 接入 x64 runtime。 |

## 静态引用与 host seam 的界线

当前 helper 以 direct object group 尝试链接完整 table 时，linker 报出的
console/event/video/timer/media imports 属于“静态 table 带来的 owner 闭包”。它们不说明
`50h/14h` 或 `50h/11h` 在运行时会访问这些 owner。相反，若未来确实出现一个 selector，
新的工作项必须同时记录：

1. selector、编译宏和 `bios.c` 行范围；
2. CCPU 或原 BIOS fixture 的动态到达证据；
3. entry 的原始 caller/source owner；
4. 该 caller 所需的最小现代 host seam，或缺失 SoftPC 语义的独立重建证据；
5. 该 seam 的 fail-closed 测试。

不得因为“该函数已在 table 中”而添加空实现、`/FORCE:UNRESOLVED`、简化 selector
table、`Video = C_Video`，或让 trace fixture 成为运行时行为。

## 对 T-S5.1 的直接影响

完整 visible-state accessors 与缩为 AX/CX/DX/CS/IP 的 date-only projection 均采用同一
完整 table provider group，且得到相同的 unresolved owner 集。因此下一步不是再缩 state
或补 register shim，而是依此表逐类标注完整 table 的 static closure，并只把真实 `50h`
路径的 owner 纳入下一轮 source closure。当前 v2 helper 维持 default-disabled、
unadmitted trace candidate；shared SAS、`50h/11h` transport 和 NTIO prefix execution
均尚未准入。

## 2026-08-09 clean-link 重采样

以默认关闭的
`ntdos64-opennt-historical-transport-v2-owner-closure` 重新执行 clean link（无
`/FORCE:UNRESOLVED`、无缩减 `BIOS[]`、无新 stub）。编译阶段完成，链接阶段报告
**260 个唯一未解析符号**。原始日志保留在
`artifacts/build/current/s5-owner-closure-link.log`，唯一符号清单为
`artifacts/build/current/s5-owner-closure-symbols.txt`。

这些不是 260 个“应立即补的 shim”。按 source owner 和产品层次归并后，至少有：

| 类别 | 观察 | 处置 |
| --- | --- | --- |
| 完整表静态 host/device 家族 | `host_com_*`、`host_fdisk_*`、`host_gfi_rdiskette_*`、`host_lpt_*`、console/UI 等 | 未有 selector 动态到达证据；不能因 link 失败提前实现 serial、diskette、printer、console 或 drive policy。 |
| C-VIDC / generated-video 面 | 96 个 `getVideo*`、`setVideo*` 和 `nt_*graph*` 相关 import | 表明当前 CCPU/C-VIDC composition 仍不完整。它是 R1 的 profile/composition 证据，不是 NTIO 已请求 video 的证据。 |
| 历史产品子系统 | `CmdDispatch`、`DpmiDispatch`、`DBGDispatch`、VDD/WOW、PIF/config、`InitialiseDosEmulation` 等 | 多数由完整 table 或非 `50h` 分支静态带入；只在真实 caller 到达后按 owner 单独排期。 |
| CCPU generated/profile 缺口 | `c_sas_touch`、`c_VirtualiseInstruction`、`EDL_fast_bop`、interrupt hook | 与已记录的 CCPU generated/composite 缺失一致；不得为 clean link 编写无语义 fallback。 |
| 已限域恢复的 LIM writeback | `LIM_b_write`、`LIM_w_write`、`LIM_str_write` | `ccpusas4.c` 的调用与 `emm_mngr.c` 中被 `NTVDM` profile 排除的原始函数体已对应；仅默认关闭的 full-source link diagnostic 可显式编译该原始块。没有 EMS 动态 caller 证据，绝不构成 EMS runtime、设备或 host shim 准入。 |
| 旧 NT/CRT 接缝 | `NtVdmControl`、部分 `Rtl*`/`Nt*`、`__sys_errlist` 等 | 先区分原始 import library 与真实现代 host seam；尚未获得动态 caller admission。 |

### Video 结果的精确含义

本次 target 的 unresolved 集中出现 generated `getVideo*` / `setVideo*`，但没有直接
报告 `_Video`。这**不能**解释为 generic 81-slot `Video` storage 已闭合：当前
target 将 C-VIDC 作为 archive 链接，且没有把 `ev_glue.c` 的 generic-`Video`
consumer 拉入最终 extraction。换言之，此结果只说明已被拉入的 CCPU-facing vector
surface 尚缺 composition；它不推翻
`S5-VIDEO-INPUT-RECONCILIATION.md` 所记录的“若完整 BIOS/C-VIDC closure 拉入
`ev_glue`，generic `Video` 仍是硬输入”。

因此下一步保持两条独立采证线：

1. R1 继续追踪 CCPU/C-VIDC 的完整 historical composition、generated input 和
   single storage owner；
2. S5 只按真正的 selector-50/DosInit caller 推进 source owner 和现代 host seam。

两线均不得用 alias、short vector、假 handler、缩表或 `/FORCE` 互相绕过。

## 2026-08-09 复核：v2 owner-closure 仍未收敛

在完成 shared-aperture/SAS 采证后，重新以单线程构建 default-disabled
`ntdos64-opennt-historical-transport-v2-owner-closure`。它保持无 SM0 archived adapter、无
`/FORCE:UNRESOLVED`，并在链接阶段失败。输出仍落在本记录的 260-symbol 基线类别内，代表性
未解析项包括：

```text
host_runtime_inquire, host_ica_lock, host_ica_unlock
GetConsoleInputWaitHandle, ReadConsoleInputExW, VDMConsoleOperation
host_com_*, host_lpt_*, host_flpy_*, host_fdisk_*
DpmiDispatch, CmdDispatch, DBGDispatch, NtVdmControl
setVideo*, getVideo*, c_sas_touch, c_VirtualiseInstruction, EDL_fast_bop
```

这次复核没有发现一个可由 shared aperture 或跨位数 transport 合法消除的新增/减少类别。它只确认：

1. S4/SAS 同 backing 解决的是 session memory ownership，不是完整 BIOS table 的 product
   composition；
2. 将 `50h` date trace 包装成更小的 transport record 不会减少 static initializer 的 owner closure；
3. 下一项高 ROI 工作仍是追溯缺失的 CCPU/C-VIDC composite input、generator 或历史 build output，
   并把每一项 source owner 与动态 caller 分离；
4. 不得以本次结果为依据新增 console、disk、COM/LPT、DPMI、COMMAND 或 `NtVdmControl` shim。

本节是可重复的 clean-link 负证据，不授权使该 target 可链接。
