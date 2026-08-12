# R1 Software Monitor Profile Decision

状态：source-backed architecture decision，2026-08-09。

## 结论

固定 NT4 的原始 x86 product build 不提供一个可直接移植到 x64 用户态的软件 CCPU
profile。若本项目继续实现不依赖 V86 的 x64 software monitor，它应被命名为依据
SoftPC/CCPU 原始语义的**自研重建**；不得宣称它是由 NT4 x86 build 原样恢复的 CCPU
composition。

这不阻止研究目标。它规定了正确的证据和所有权：OpenNT 的 CCPU、SAS、BIOS、BOP、DEM
代码可作为逐函数行为 oracle；缺失的 x86 software-monitor composition、运行时绑定和现代
host surface 必须由本项目自有代码明确实现、测试并标明来源，而不能伪装为历史缺件。

## 原始 build 证据

`src/opennt/base/mvdm/softpc.new/obj.vdm/cdefine.inc` 明确分叉：

```text
386:      MONITOR + C_VID + X86GFX
non-x86:  CPU_40_STYLE [+ CCPU + C_VID] + NEW_CPU + SPC386 + SIM32 + V7VGA
```

因此以下事项不能混为一个历史 profile：

- x86 的 V86 `MONITOR` 路径；
- RISC 的 `CPU_40_STYLE + CCPU` software-interpreter 路径；
- 未来 x64 用户态的自研 software monitor。

`host/src/config.c` 也与此一致：`InitNtCpuInfo()` 只位于 `CPU_40_STYLE` 的
`sas_init()` 后分支；x86 的后续原始 owner 是 `GetROMsMapped()`、font discovery、console
window、`InitUMBList()`、`host_runtime_init()` 与 floppy discovery。这些是历史 x86 product
startup 行为，不能用来证明 CCPU composition 已恢复。

## 对当前 R1 的含义

1. `ccpu486.lib` 或其 generator 若被找到，仍极有价值：可恢复非 x86 CCPU composition 的
   原始 ABI/时序，并作为软件 monitor 的强 oracle。
2. 找不到该输入时，不得把 `Video`、SAS tail slots 或 `InitNtCpuInfo` 以猜测方式填入
   NT4 x86 路径。
3. 需要让 NT4 DOS guest 在 x64 用户态执行时，下一条正式路线是定义一个独立的
   `reconstructed software monitor` 边界：CPU state、checked SAS/memory、ROM mapping、I/O、
   IRQ/event、BOP instruction semantics 与 host capability adapters。它必须保留原始
   `BIOS[] -> MS_bop_0 -> DemDispatch -> DEM` 职责链，不能变成自写 DOS SVC dispatcher。
4. 每项自研 monitor 语义应引用至少一个 OpenNT caller/owner，必要时以 Bochs、PCjs 或
   `ntvdm64` 交叉校验；后者都只是参考，不进入运行图。

## 近期队列

R1 的 historical-recovery 分支继续等待可审计的 `ccpu486.lib`/map/generator 输入。并行的
self-owned reconstruction 分支应先写 machine/monitor contract 与原始 BOP-entry differential
fixture，随后才实现最小 CPU execution slice。该分支的完成标准是经过原始 BOP/DEM 链的
可审计停点，不是“又处理了一个 DOS service”。
