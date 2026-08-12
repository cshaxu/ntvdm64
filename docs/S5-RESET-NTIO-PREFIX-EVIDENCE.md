# S5 Reset and NTIO Prefix Evidence

状态：caller-first 采证中，2026-08-09。

## 范围与结论

本记录只定义 NT4 OpenNT 历史 host 在把 `NTIO.SYS` 交给 guest 之前实际做了什么，
并据此约束 S5。它不授权新建 BIOS、IVT/BDA、BOP/DEM、DOS service、PC/AT device 或
现代 host shim。

已经可以确认的历史启动顺序是：

```text
config() -> SAS allocation / optional ROM mapping
        -> InitialiseDosEmulation()
        -> fixed VDM state preservation -> io_init() -> reset()
        -> installable VDD setup -> single-command-shell initialization
        -> load NTIO.SYS at 0070:0000 -> restore fixed state -> CS:IP = 0070:0000
```

这里的 `reset()` 是原始 SoftPC BIOS owner；它不是本项目可以用小型自写替代物取代的
普通 helper。S5 的目标是取得真实 prefix trace 和逐项缺口，而不是在没有 caller 证据时
让 NTIO “继续前进”。

## 原始锚点

| 事实 | 原始锚点 | 对 S5 的含义 |
| --- | --- | --- |
| i386 配置按 conventional + XMS + EMS 决定 SAS 大小，并调用 `sas_init` | `src/opennt/base/mvdm/softpc.new/host/src/config.c:506-531` | S5 必须显式拥有 per-session RAM/SAS 大小和分配结果；不能把宿主 pointer 当 guest memory。具体内存策略仍是产品配置，不从历史默认值硬编码。 |
| 图形构建会在其他组件使用 memory 前调用 `GetROMsMapped()` | `config.c:543-546` | ROM 是 mapping/拓扑前置事实；S5 只能记录和验证实际映射，不能借此合成 BIOS image。 |
| DOS 初始化先保存固定 VDM state，再 `io_init()`、`reset()` | `src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c:119-147` | 这些调用的相对次序是 prefix trace 的首个不可交换合同。 |
| `io_init()` 将整个端口路由表先设为 historical empty adapter | `src/opennt/base/mvdm/softpc.new/base/support/ios.c:959-1001` | 它是 I/O routing 初始化，不是“所有设备可用”。S5 首个 stop 可以是 guest 对尚未由原始 owner 注册的 port；不得把 empty read/write 伪装成设备成功。 |
| VDD setup 发生在 reset 之后；`scs_init` 在 NTIO 装载之前 | `nt_msscs.c:149-165` | 任何将 VDD/command 逻辑提前折入 monitor 的方案都不符合此 owner 顺序。 |
| NTIO 从 system directory 打开，以原文件长度读至 `get_byte_addr((0x70 << 4) + 0)` | `nt_msscs.c:169-203`；`src/opennt/base/mvdm/inc/vint.h:50-52` | 固定历史入口的物理 load address 为 `0x700`，对应 `0070:0000`。读取是 host loader 行为，不是 DOS filesystem service。 |
| 读取会覆盖固定 state；代码随后恢复它，并设置 `CS=0x70`、`IP=0` | `nt_msscs.c:216-221` | 未来 entry handoff 必须是原子验证：所有 load/precondition 成功后才提交 guest visible entry state；失败不得留下部分启动状态。 |
| 原始 BIOS reset 负责 PIC、CMOS、PPI、timer、keyboard、video、DMA、mouse、HDA、disk post、host reset 和 event queue 等 | `src/opennt/base/mvdm/softpc.new/base/bios/reset.c:800-1082` | 这些是要由 prefix trace 逐项暴露的 dependency 列表，不是现在要全部复制到 monitor 的实现清单。 |
| 在 `NTVDM && X86GFX` 分支，`sas_fills` 与通用 `setup_ivt()` 被条件编译排除 | `reset.c:781-787` | 历史 i386 NTVDM 不证明“每次启动均须由本地 scaffold 写 IVT/BDA”。旧 scaffold 只能继续留在 trace/archive，不得升格。 |
| NTVDM 分支是在 disk post 后才调用 `host_reset()` 和 `q_event_init()` | `reset.c:1079-1083` | console/event shim 若存在，必须在相应原始 caller 已可到达后才可被讨论，不能作为启动前置成功条件。 |

## 固定 VDM state 的所有权

`src/opennt/base/mvdm/inc/vint.h:36-53` 定义了该对象的地址和位含义：它位于
`0x70:0x14`（physical `0x714`），长度为 4 字节；其中包括 virtual-interrupt、MIPS、
EXEC、real-mode 与 real-interrupt 标志。`NTIO.SYS` 原始源码在
`src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm:55-59` 将自己的 `ntvdmstate`
安排在同一 BIOS-data offset，并用汇编断言验证偏移。

因此它是 host 和 guest 共同约定的、固定的 guest-memory ABI：S5 可以在经过 checked
mapping 的 RAM 中保存/恢复这 4 字节，但不得把它搬到 host-only state、修改其地址，或以
新的抽象标志替换原始位语义。它同时说明了为什么 NTIO image 从 `0x700` 装载后需要恢复
`0x714`：这不是普通 loader 的可选保护，而是历史 ABI 的一部分。

## S5 最小工作包

1. 建立**只读 prefix trace schema**：阶段、原始 caller、必要的 guest-visible state、
   mapping class、以及 typed stop reason。它不得输出 NTIO bytes、host path 或持久化 guest
   memory 内容。
2. 在原始 i386 island 中，先为 `InitialiseDosEmulation` 的固定状态保存、load-address
   计算、`CS:IP` handoff 建立可重复的 bounded evidence；没有完整 reset dependency 时应在
   第一个缺失 caller 处停止并记录。
3. 只在 S4 的 monitor 已支持所需 ordinary-RAM load 和原始历史 bridge transport 后，才
   设计跨位宽 handoff。该 transport 仍不可暴露原始函数指针、SAS pointer 或 CRT ownership。
4. 对每一个真实 trace stop，单独提出 host seam 或 monitor semantics 任务；不允许以
   fake BOP、synthetic IVT/BDA、默认设备成功或 DOS SVC handler 跨越该 stop。

## 当前禁止项

- 不直接运行 BYOB `NTIO.SYS`。S4 现有的 `E9 rel16`、有限 register/segment/stack/flags 指令
  仅覆盖经过记录的首段 byte route，仍不具备真实 NTIO prefix 所需的 `FCLI` 展开、string/memory、
  mapping、I/O、interrupt 或 mode semantics。
- 不将 `v86/scaffold` 的 `softpc.c`、`fakebop.c` 或临时 IVT/BDA 写入迁入正常运行路径；它们
  与 x64 software monitor 路线不是启动规格。
- 不先行实现 CMOS、FDC/HDC、video、keyboard、clock、console、filesystem 或 VDD policy；
  每项必须由第一个实际 prefix caller 触发并独立审计。

## 既有 trace 的适用性校正

仓库中已有的 `config_prefix_*`、`session-byob-*` 与 reset trace targets 是早期的
`trace/fixtures` 证据，不是 S5 runtime 基线。它们默认关闭，且部分组合会引入：

- 仍为 `compat/host candidate` 的 BYOB path 与 `direct_cli` transport；
- 旧的 CCPU/C-VIDC profile 组合；
- 为后续未到达符号保留的 `/FORCE:UNRESOLVED` trap。

这些目标可证明当时某个原始 caller 曾被到达，但不证明它们提供了可恢复的 software monitor、
跨位宽 transport、设备语义或 NTIO 可执行性。特别是旧 reset trace 在 C-VIDC generated
video/global-data 组合处的停止，只能作为 R1 source-recovery 证据；它不得成为 S5 对
`src/reconstruction/monitor` 的阻塞条件，也不得通过重新启用 local CCPU adapter、video
callback、synthetic IVT/BDA 或 host shim 来绕过。

S5 的有效基线仍是 S4 的独立 software monitor、S2 的定宽 historical bridge 与本文件列出的
原始 `nt_msscs.c` caller 顺序。若要复用任何既有 trace，必须先移除其 candidate/archived
依赖，确认它只记录或 fail-closed 停止，并为新的组合建立独立 manifest 与验收。

## 下一项采证

先从 `reset.c` 的 NTVDM build 选择、`io_init()` 的真实 owner 与 `FIXED_NTVDMSTATE_LINEAR`
的定义/初始化，确定一个没有 host 设备模拟的最小 i386 reset-prefix slice。只有该 slice
可构建且其首个 typed stop 可重复，才开始扩展 monitor instruction 或 host seam。
