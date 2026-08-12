# CCPU 缺失语义重建恢复矩阵

> **归档状态（2026-08-08）：** 本文保留此前对外部 execution backend、
> `ntvdm64/core` 与 Bochs/PCjs 的比较结论，不能作为当前实现计划。
> 当前唯一的执行路线是修复并组合 OpenNT SoftPC/CCPU；外部代码库只可用于
> 只读语义交叉校验，不得进入构建图、链接图或运行图。冲突时以
> `SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 为准。

状态：研究结论，2026-08-08。本报告修正此前过于狭窄的结论：缺少微软原始生成输入，
意味着不能声称按字节找回原始源码；但不意味着不能以证据为基础重建缺失语义。

本文仅为研究文档。它不启用正常 runner 路径，不准入自写 DOS runtime、
BIOS/BOP/DEM 替代物、V86 路径，也不让 `ntdos64-run` 新增依赖。

## 结论

可以。缺失的 CCPU 语义可分阶段重建，证据组合如下：

```text
OpenNT retained callers, types, and state layout
       +
Bochs 2.6 x86 execution and memory semantics
       +
PCjs PCx86 independent execution and machine evidence
       +
ntvdm64/core's independently implemented software CPU
       =
new, explicitly self-authored CCPU-compatible recovery layer
```

产物必须表述为“兼容性重建”，绝不能称为找回的微软源码。每一项在进入历史
link 或执行实验前，都必须有：源码侧合同、独立语义 oracle、以及 focused
differential fixture。

## 实际缺口

选定的 OpenNT CCPU 源码集已可通过 guarded x86 SAS header overlay 完成对象
闭合；但历史可执行 profile 仍有多类缺口，不能混为一谈。

| 缺口 | 当前证据 | 重建类别 | 准入状态 |
| --- | --- | --- | --- |
| `c_effective_addr(seg, offset)` | CCPU 类型/状态与非 CCPU 等价实现 | 已可描述的 x86 语义 | 可先实现隔离的重建版本 |
| `c_sas_touch(address, length)` | NTVdmx64 `sas4gen.patch` 新增的后代 SAS ABI slot；没有 NT4 原始 provider | 后代 patch surface，不是 NT4 SoftPC 缺损 | 排除出 NT4 reconstruction；保持 fail-closed trace-only |
| `c_VirtualiseInstruction(eipInRom, size, operand, data)` | NTVdmx64 `sas4gen.patch` 新增的后代 SAS ABI slot；`fmstubs.c` 仅 `int 3` | 后代 patch surface，不是 NT4 SoftPC 缺损 | 排除出 NT4 reconstruction；保持 fail-closed trace-only |
| generated monitor ABI（`GDP_PTR`、`ConstraintR*`、相关 `nt_cpu.c`） | 历史 caller 名称与 CCPU generated-state 引用 | 私有 generated ABI | 须单独重建 data-layout contract，不是 host shim |
| host lifecycle、console、timer、PIF、resource、VDM transport | 大部分原始 owner 仍存在源码树中 | 历史 host 现代化 | 单独的 modern-host shim 工作，不是 CPU recovery |

前三个名字位于 CCPU `SasVector` initializer 的末尾，文件为
`base/mvdm/softpc.new/base/ccpu386/sascdef.c`。后两个不透明 slot 在当前选定
历史树中没有剩余的直接 C caller，只能经 published vector 到达。这是有价值的
证据，但不意味着可以猜测其实现。

## 独立实现提供的证据

### OpenNT

- `ccpu386/c_page.c` 将 segmentation、page translation、protection、
  access-bit update 与 physical access 明确分开。任何重建都必须保留这个顺序，
  不能把 linear address 直接变成未经检查的 host pointer。
- `ccpu386/ccpusas4.c:c_IOVirtualised()` 提供了相近但不等价的先例：在 V86 或
  protected mode 中 IOPL 不足时，它通过 `biosDoInst()` 从 ROM helper 执行等价
  指令；否则由 caller 执行普通 I/O。
- `base/support/ios.c` 证实了 `IOVirtualised` 的 boolean 归属：true 表示
  ROM/helper 已完成操作，false 表示后续由普通 adapter dispatch 处理。它不能
  证明 `VirtualiseInstruction` 的返回编码或副作用。

### Bochs 2.6

本地源码：`D:\\home\\repos.hobby\\bochs-2.6-compat\\bochs-2.6`。

- `cpu/access.cc`、`cpu/access32.cc` 与 paging path 建模了独立的 segment check、
  linear access、physical mapping、privilege 与 exception。它们是
  `c_effective_addr` 和任何 checked `c_sas_touch` replacement 的强 oracle。
- `instrument/stubs/instrument.h` 暴露了 instruction before/after、
  linear/physical access、I/O、exception 和 interrupt observation point，足以
  做 trace 与 differential test。
- 这些 instrumentation callback 是 observation-only。若 bridge 要消费一条
  instruction、修改 registers 并 resume，必须显式扩展 Bochs 内部或受控修改
  execution loop；stock callback ABI 不提供这项能力。

因此 Bochs 可验证 x86 mechanics，也可运行实验性 guest backend；但它本身无法
揭示未文档化的 `SasVirtualiseInstruction` return contract。

### PCjs

本地源码：`D:\\home\\repos.hobby\\pcjs`，重点为
`machines/pcx86/modules/v2`。

PCjs 独立区分 bus physical access 和 CPU linear access，建模 real/protected mode
segment state、page translation 与 PC devices。当 Bochs 与 CCPU 的行为证据需要
裁决时，它可作为第二个语义参照。它是 JavaScript，且产品架构不同；不得把它
复制为实现或 host ABI。

### ntvdm64 Core

只读的 `ntvdm64` CPU 已有 logical-to-linear translation、
linear-to-physical page translation、segment loading、checked physical memory、
A20、ROM mapping 和 exception state 的软件实现。它是长期最合适的 execution
candidate，因为它是项目自有的通用 machine 实现。

它目前缺少中性的 decoded-transition 与 paused-RAM-transaction 设施，详见
`CORE-MANTLE-PREREQUISITE-IMPLEMENTATION-REQUEST.md`。这是 product boundary
问题，不是无法继续研究 CCPU 语义的证据。

## 重建方案

### R1：`c_effective_addr`，优先实现

此函数已有足够证据，可先做窄的重建：

```text
real mode 或 V86：               (segment << 4) + offset
protected non-V86，selector 有效：descriptor.base + offset
selector 在 table 之外：          0xffffffff
```

它返回 guest linear address。它不得做 host-pointer lookup、跳过 segment limit、
跳过 paging，或掩盖后续 CCPU fault。现有 fixture 已覆盖 real、V86、GDT、LDT 和
invalid-selector。将它加入更广的 link diagnostic 前，还应补充来自 Bochs 和
PCjs 的 differential vector：selector base、table-limit 与 invalid-selector
边界案例。

### R2：`c_sas_touch` / `c_VirtualiseInstruction` 的来源校正

2026-08-09 对 reference-only
`upstream/ntvdmx64/ntvdmpatch/patches/common/sas4gen.patch` 的逐行审计表明，这两个
typedef、`SasVector` fields、CCPU macro mappings 和 imports 都是该 patch 新增；同一树的
`patches/common/fmstubs.c` 对二者只有 `int 3` 占位，分别返回 `NULL` 与 `0`。当前工作区
`sascdef.c` 的 declaration/initializer 只有在对应的 patch-origin SAS header overlay 下才形成
这一 ABI surface；它不是可从固定 NT4 product profile 直接恢复的 CCPU function contract。

它们不得继续列为 NT4 SoftPC reconstruction 工作项。历史 wrapper/monitor 若遇到相应路径必须
受控停止；不得从相邻 `c_IOVirtualised()`、Bochs、PCjs 或后代 stub 推导成功语义。以下旧的
“先 trace”分析仅保留作为此前 patch surface 的审计记录，不构成准入。

### 已归档的旧 R2 分析：`c_sas_touch`

该 signature 返回 `IU8 *`，因此它在旧 SAS model 中必然表示一段连续、可由 host
寻址的 span。现代实现不能把任意 guest address 不加检查地暴露成 host pointer。

第一步实验应提供 fail-closed probe，只记录：

- address、length、当前 CPU mode 和 CPL；
- 该请求按 active SAS routing 属于 ordinary RAM、ROM、EMS、video 或 unmapped
  region；
- caller return address 与最终 stop reason。

只有 bounded historical path 实际抵达该 slot 后，才能测试候选合同。若 trace
证明它只用于 ordinary RAM，第一版候选为：

```text
验证 nonzero、non-overflow range
验证整个 range 都是连续的 ordinary RAM mapping
仅在当前调用期借出该 contiguous pointer
否则走历史 trace 已证明的 failure path
```

不得引入 provider callback、ROM pointer、video pointer、raw `ntvdm64` pointer
或隐式 materialization。若 trace 证明它有 mapping side effect，必须先给该副作用
命名并独立测试，才能扩展候选合同。

### 已归档的旧 R3 分析：`c_VirtualiseInstruction`

相邻的 `c_IOVirtualised()` 让一项假设显得合理：该函数可能在 normal guest
execution 必须 fault 或被 virtualize 时，以 monitor-visible context 执行提供的
ROM instruction。但这仍只是假设。

实现前，应在 vector slot 设置 trace point，捕获其四个输入、caller return address、
EFLAGS、CS:EIP，以及 surrounding historical caller 改写的全部状态。必须回答：

```text
eipInRom 处究竟是什么 instruction？
size 的含义是什么？
对本次调用，operand 是 linear memory address 还是 I/O port？
IU32 return 代表哪个 register/flag/memory result？
callee 是否消费 guest IP、注入 exception，还是仅执行 helper？
```

随后可用 Bochs 执行等价的 controlled instruction，得到独立的 before/after state
trace。重建函数必须是窄的 historical-helper adapter，不能成为通用 instruction
interception API，更不能成为 BOP/SVC dispatcher。

### R4：Generated Monitor ABI，按数据重建，不可猜测

`nt_cpu.c` 中的 `GDP_PTR` 与 `ConstraintR*` 引用证明存在 generated monitor/CCPU
coupling。它们不是 Windows API，不能用 console 或 filesystem shim 解决。

下一步必须枚举两侧每个 field use、size、alignment、initializer 与 callback
invocation。允许的 replacement 只能是 versioned C layout，并具有 compile-time
`sizeof`/offset assertion，以及实际驱动其 CCPU consumer 的 fixture。Bochs 和 PCjs
在这里帮助最小：它们能检查 CPU 行为，但不能恢复微软私有的 in-memory ABI。

## 已归档的 Execution Backend 决策

本节记录校正前的探索选项，仅保留其证据价值。它不授权建立 Bochs bridge、
不授权把 CCPU contract 绑定到其它项目 core，也不构成下一项任务。

现在有两条可并行的研究轨：

```text
Track A：重建 CCPU-compatible 的缺失语义
  目的：保留并运行原始历史 CCPU ownership chain。

Track B：把 Bochs 用作默认关闭的 guest execution oracle/backend
  目的：在 Track A 解决私有 SAS/monitor contract 时，运行 controlled instruction，
       获得 differential x86 evidence。
```

Track B 不得悄悄替代 Track A。Bochs bridge 只拥有 CPU state、physical memory、
I/O、interrupt injection、bounded stop reason 与 trace capture；它不提供自造 BIOS、
DOS runtime、BOP handler 或 DEM replacement。其 public bridge type 也必须不泄露
Bochs 类型，以便同一 harness 将来能绑定到 `ntvdm64/core`。

## 建议顺序

1. 将已测试的 R1 `c_effective_addr` reconstruction 准入下一层 *link diagnostic
   only*。
2. 为 R2、R3 建立默认关闭的 call-boundary tracer。它只记录并停止，不返回可用
   pointer、不 virtualize instruction，也不让 DOS 前进。
3. 按 field use 映射 `GDP_PTR` 与 `ConstraintR*`，产出 generated-ABI
   reconstruction dossier。
4. 建立 Bochs controlled fixture：owned RAM、real-mode instruction stream、一次
   physical-memory trace、一次 port-I/O trace、以及 teardown。不得让任何 BYOB DOS
   文件或 historical BIOS/DEM service 进入该 fixture。
5. 在语义域重叠处，将 R1/R2/R3 的 controlled trace 与 Bochs、PCjs 及只读
   `ntvdm64` core 对照。
6. 最后再决定 reconstructed CCPU 是否可执行 bounded historical prefix，还是将
   同一批已记录 contract 直接绑定到扩展后的项目 core。

## 非结论

- 本文不证明可以复现每个 historical CCPU quirk。
- 本文不证明两个 opaque SAS slot 对 NT4 NTDOS boot 都是必需的。
- 本文不允许把 Bochs、PCjs 或 `ntvdm64` 的源码复制进 OpenNT。
- 本文不使 Bochs 成为永久 product dependency，也不替代
  `core -> mantle -> external wrapper` 边界。
- 任何链接了 CCPU 的 executable 都不能因此宣称已是成功的 NTDOS runtime。
