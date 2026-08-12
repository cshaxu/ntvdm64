# SoftPC Reconstruction Roadmap

状态：当前执行路线，2026-08-09。

## 前提结论

NT4 的原始 x86 产品使用 V86 `MONITOR`，不是可直接移植的 CCPU software-CPU
profile；CCPU/C-VIDC 的完整组合属于非 x86 路径，且其 `ccpu486.lib` 生成输入缺失。
因此，CCPU、SAS、BIOS、BOP 与 DEM 仍是最重要的逐函数语义证据，但不能把一个
CCPU/C-VIDC link fixture 写成 x64 runtime 已恢复。

目标不变：以历史职责链运行，而不是重写 DOS 服务：

```text
software monitor -> original BIOS[] -> original MS_bop_0 -> original DemDispatch -> original DEM -> NTDOS
```

`software monitor` 是依据 OpenNT 行为证据独立重建的执行部分；`BIOS[]` 之后的
历史 owner 不得被本地 BOP/DEM/DOS 替代物绕开。所有历史 i386 函数指针和 CRT
对象都留在历史闭包内，跨位宽边界只能传递版本化、定宽的 record。

## 已完成的前置工作

1. G0：固定 OpenNT NT4 输入、来源 manifest、独立 i386 clang-cl CMake island 与
   guest 工件治理已完成。
2. G1：本地 custom code 与 OpenNT overlay 已分类；早期自写 BOP/DEM、IVT/BDA、
   owned monitor、R2 monitor 与外部 backend probe 已退出 root runtime/CTest。
3. G2：`c_effective_addr` 是唯一已准入的缺失 SoftPC 语义恢复；
   `c_sas_touch`、`c_VirtualiseInstruction` 仍为 fail-closed trace guard。
4. G3：原始 i386 侧已有受控的 selector-50/date 链采证、直接
   `BIOS[50h] -> MS_bop_0 -> DemDispatch` 采证，以及原始 SAS 初始化/释放采证；
   三者都不是 guest execution 或 runtime。
5. G4：已确认 CCPU/C-VIDC composition 缺失，且它不是 NT4 x86 软件 monitor 的
   可构建输入。该恢复调查保留为证据分支，不再阻塞 x86 host-chain 研究。

## 后续任务

### S1: Historical Host Closure

从现有 selector-50 fixture 抽取一个**历史侧**最小闭包：在 i386 闭包内部真实调用
`BIOS[selector]`，保留 `MS_bop_0 -> DemDispatch` 的原始边界，并用无副作用、可重复
的 date service 记录输入、寄存器变化与返回。清除任何为“让 DOS 前进”而存在的本地
BIOS/BOP/DEM handler。

验收：i386 target、历史输入 manifest、进入的原始符号、调用顺序和失败路径均可复现；
未绑定 selector 停止，不合成 DOS 结果。

进度：第一条 date-service direct table invocation 已完成；证据见
`S1-HISTORICAL-HOST-CLOSURE.md`。S1 尚未证明完整 BIOS table、reset 或设备闭包。

### S2: Bridge Record Contract

在没有跨位宽函数指针的前提下，设计并实现一个极窄 `extern "C"` record ABI 的两端。
第一版只表达：ABI version、selector、可复制的 real-mode register snapshot、受检查的
guest-memory request、明确 disposition 和 redacted event。历史侧 bridge 内部调用
`(*BIOS[selector])()`；现代侧不得看到 `BIOS[]`、`Video`、SAS host pointer、CRT
对象或 DEM 内部结构。

验收：一个 selector 进入原始历史 handler 后回到 record 结果；错误 ABI、越界 memory
request、未绑定 selector 和未知 disposition 均 fail-closed。此阶段不加载 NTIO。

进度：v1 已验证为 i386 trace-only record adapter；它只允许 `BIOS[50h]` 的
`0x14` date trace，并覆盖未初始化、错误 ABI、未支持 selector 的拒绝。证据见
`S2-HISTORICAL-BRIDGE-RECORD-CONTRACT.md`。跨位宽 transport 与 general memory
window 仍未开始。

### S3: Monitor Semantic Specification

以 S1/S2 的真实调用要求和 OpenNT `c_main.c` 为依据，写字段级 monitor 规格：CPU
visible/hidden state、segment/linear/physical route、memory mapping、ROM、I/O、IRQ/event、
BOP encoding 的 IP 消费和 `FE` invocation exit。每条语义须标明 OpenNT caller/owner；
Bochs、PCjs、NTVDMx64、ntvdm64 仅作交叉校验。

验收：规格没有 DOS service switch、没有合成 IVT/BDA、没有“任意指令 hook”，并能为每个
bridge record 字段给出历史 owner。

进度：字段级实现前规格已写入 `S3-RECONSTRUCTED-MONITOR-SEMANTICS.md`。S4 尚未创建
任何执行代码。

### S4: Minimal Reconstructed Execution Slice

在 S3 审核后，以独立的新代码实现最小 real-mode CPU/memory execution slice：普通指令按
CPU 语义执行，已证实 BOP encoding 只转交 S2 bridge；未知 opcode、mode transition、
I/O、IRQ、映射异常先 fail-closed。归档 R2 与 owned monitor 只能作为测试反例/对照，
不能复用为实现基底。

验收：一段非 DOS 的 bounded byte stream 可经真实 bridge 抵达原始
`BIOS[] -> MS_bop_0 -> DemDispatch`，并从原始 `FE` 语义停止。不得以“处理一个 BOP”
宣称 DOS 已运行。

进度：portable S4 mechanics 已实现并在 root MinGW fixture 验证；同一源码的 i386
integration fixture 已通过 S2 record 到达原始 date chain，随后由 `D6 FE` 停止。仍缺
x64-to-i386 transport、更多普通指令、mode/I/O/IRQ/mapping policy 与任何 NTIO 输入。
证据见 `S4-RECONSTRUCTION-ADMISSION.md`。

### S5: Reset and NTIO Prefix Trace

当前进度：已从精确 NTIO prefix 经原始 `50h/11h` 完成
`MS_bop_0 -> DemDispatch -> demLoadDos` 的 BYOB `NTDOS.SYS` 读取和返回。`DemInit`
所需的 non-WOW host context 已以最窄的原符号存储闭合；`CreateFileOem` 的 NT4
私有 TEB `StaticUnicodeString` 假设则由受限 F1 facade 闭合。CCPU/SAS profile 中的
`GetVDMAddr` 已被限定为到原 CCPU SAS owner 的地址转换。两项均是已证实的现代 host
seam，不是 DEM、BOP、DOS 或 SoftPC CPU 语义替代。细节见
`S5-ORIGINAL-DEMLOADDOS-CLOSURE.md`。

紧随 NTDOS 首个 `50h/0Fh` caller 的原始 `demGetDrives` 现已在一个 default-disabled
fixture 中通过 `BIOS[50h] -> MS_bop_0 -> DemDispatch(0Fh)` 返回。该 fixture 保留
原 `GetDriveTypeOem`、`GetPhysicalDriveType` 与 26 槽 DEM cache；实际 trace 只暴露
两个现代 x86 ABI 声明缺口和一个已移除的 `RtlProcessHeap` export，后者以
`GetProcessHeap` 的窄 host seam 闭合。它不引入 drive policy，也不构成 NTIO/CPU/DOS
runtime claim。详见 `research/ccpu-bios-closure/S5-DEMGETDRIVES-DYNAMIC-HOST-SEAMS.md`。

仅在 S4 成立后，按实际第一个 caller 恢复 reset、SAS、ROM/resource、configuration 与
host foundation。每一个失效的旧 host API 都先形成 caller-first seam record；shim 只能
适配 process/context/event/console/filesystem/timing，不能定义 BIOS、设备或 DOS 成功。

S4 与历史 SAS 的独立 backing 已由同一 named section 消除：default-disabled i386 helper 经原始
`host_sas_init/host_sas_term` 完成 `sas_init` / `sas_term` 后，x64 S4 已在该 view 的同一 physical
offset 执行一条 ordinary real-mode `NOP`。该接缝只位于原始 host-allocation owner，未以 BOP
copy-back 或 DOS-data 同步替代。下一门槛不是继续扩展 S4 指令集，而是审计 versioned、fixed-width
跨位数 invocation record 的 CPU-state/result 原子提交；在它调用原始 `BIOS[selector]` 前，仍不得
声称 S4 到 `BIOS[50h]` 已连通。详见 `research/ccpu-bios-closure/S5-S4-SAS-APERTURE-AUDIT.md`、
`research/ccpu-bios-closure/S5-S4-SHARED-APERTURE-CONTRACT.md` 和
`research/ccpu-bios-closure/S5-S4-SHARED-APERTURE-EXECUTION.md`。

现有 `historical_transport_v2` 不能承担该门槛：它是 CCPU/SM0 的 date-trace 对照，且完整
source closure 已显露 archived adapter 与原始 host owner 的 duplicate-symbol 冲突。它不得以
链接技巧恢复，也不得扩展 selector/service。下一项实现前必须先闭合完整 `BIOS[]` 的 static owner
evidence；随后才为 reconstructed monitor 编写不包含 CCPU lifecycle 或 BOP/DEM 语义的 facade
contract/fixture。详见 `research/ccpu-bios-closure/S5-TRANSPORT-V2-BASELINE-REJECTION.md`。

验收：BYOB NTIO 在受限 budget 内产生可重复的 prefix trace；每个尚未实现的设备、I/O 或
mode 路径都形成可记录 stop，而不是伪造继续。

#### S5 补充：已闭合的首个原始 guest write

在同一个原始 `DemInit` session 内，default-disabled i386 fixture 现已实际执行
`50h/0Fh -> demGetDrives`，随后执行 `50h/46h -> demGetDPBList`。后者经已登记的
受限 CCPU/SAS `GetVDMAddr` reconstruction profile 写出 7 个 33-byte DPB，推进
`BP` 至 `00f7`，并以 `Next=-1` 终结链。fixture 只提供寄存器和 destination aperture；
它不伪造 DEM cache/DPB，也不进入 disk、media、drive policy 或 runtime。详见
`research/ccpu-bios-closure/S5-DEMGETDPBLIST-CCPU-SAS-CLOSURE.md`。

这证明 guest-memory mapping/translation 是真实的原始 consumer；它不证明 V86
mapping，也不提前授权 PC/AT controller 或 host capability。S5 后续仍须沿连续的
原始 `DosInit` 调用序列选择下一个 consumer，并先完成 caller、owner、输入/输出和
失效 host dependency 的记录，之后才可增加 default-disabled trace。

### S6: NTDOS and Program Smoke Tests

S5 已定位 `DosInit` 返回后的第一条 command-host 边：`54h/0Ch ->
cmdGetConfigSys`。它读取/生成历史 Windows profile 的 CONFIG temporary file，因而
不得直接执行为 contained runtime。它要求一个保持原 BOP、`DS:DX` 输出与随后的
DOS open/read 顺序的 profile-owned configuration projection；该 projection 不属于
SoftPC CPU 或通用 machine core。详见
`research/ccpu-bios-closure/S5-SYSINIT-CONFIG-HOST-SEAM.md`。

在 S5 的真实 trace 基础上，按需接通 NTDOS、COMMAND.COM `/C`、文件/目录、clock、
keyboard、display、media 等历史 host dependency。每个 capability 都有历史 caller、
containment policy 和正反夹具。

当前实施顺序受 `S5-CCPU-BIOS-CLOSURE-INVENTORY.md` 约束：完整 `BIOS[]` table 的静态链接
闭包会同时暴露 OEM、video、console、device、timer、DPMI 等多个 owner；这不授权按表项
批量填充 shim。首先只处理已由原 `demLoadDos` caller 证明的 OEM facade，并将 video generated
缺口与产品设备策略保持为独立任务。

验收：固定 BYOB NT4 EN-US profile 的有界 command smoke test 经过完整历史链；Win32
PE32/PE32+ 仍由现代 CLI 明确转交宿主，而非进入 DOS guest。

### S7: Downstream Handoff

只在 S1--S6 获得运行链实证后，输出一份中性能力报告：哪些 machine、memory、event、
media 与 host contracts 可由将来的 core/mantle 消费。该阶段只交付报告，不在本仓库引入
core.dll、mantle.dll 或外部 CPU 后端。

## 并行但不阻塞的证据分支

- R1：继续寻找 `ccpu486.lib`、generator、map/listing 或 build log，并以完整 C-VIDC
  `Video` storage/initializer 的真实 caller 为准。它服务于 SoftPC 语义考古，不是 S1--S6
  的 x86 execution gate。
- R2：继续从 NT4.5、NT5/XP 采集 source matrix，记录差异，绝不把后续版本代码混入 NT4
  runtime 输入。

## ROI 顺序

最高 ROI 是 S1、S2：它们直接把“原始链实际需要什么”变成可测事实，并决定后续 monitor
的真实 ABI。S3、S4 只在该事实基础上开始。S5 以后才会接触真实 NTIO；S6 是首次面向
用户的 DOS 可运行性验证。R1 的继续搜索成本低但收益不确定，故保持并行、不可阻塞。
