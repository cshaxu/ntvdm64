# R1 CCPU Profile Separation

> **Historical evidence, not a product configuration.** This 2026-08-08
> investigation records the archived source profile that defined both
> `CPU_30_STYLE` and `CPU_40_STYLE`. Current production compilation, linking,
> runtime, and acceptance are strictly `CPU_40_STYLE` only. CPU30/V86 is
> retired and may appear below solely when quoting historical source evidence.

## 2026-08-08 运行证据更新

本轮 reset trace 已证明本任务不是抽象的宏清理：旧 profile 令
`ccpusas4.c::phy_r8()` 从绝对地址零读取 `read_pointers`，因此在进入
SAS 路由前即发生访问异常。将 CCPU/C-VID 目标收敛到
`CPU_30_STYLE + CPU_40_STYLE + CCPU + SPC386 + C_VID`，并移除
`MONITOR`、`X86GFX` 后，该向量已由原始 `ega_read.c` 正确初始化。

新的、独立的停点是 `ev_glue.c::setMarkPointers()` 经通用
`Video.setMarkPointers` 的空函数指针跳转。源码同时含有已初始化的
`C_Video`（生成的 `vglfunc.c`）及零初始化的 `Video`（`ntstubs.c`）。
在找回历史构建中二者的正式绑定方式前，禁止用本地 `Video = C_Video`
赋值、假 handler 或 host shim 使 trace 继续。此前将故障归因于
`c_sas_overwrite_memory()` 或 SAS 栈损坏的判断已撤销。

## 2026-08-09 组合边界

进一步核对 `obj.vdm/cdefine.inc` 与 `obj.vdm/sources` 后确认：NT4 x86
产品配置默认选择 `MONITOR + C_VID + X86GFX` 及 `a4*.lib`，而
`ccpu486.lib` 是非 x86/RISC 的软件 CPU 选择。因此当前将 CCPU archive
与 x86 Sim32 host-SAS owner 合并的 reset fixture 只能用于暴露调用和数据
合同，不能作为“已恢复的 NT4 运行装配”或后续 runtime 的基础。

下一步必须先建立 RISC CCPU 的原始对象、生成输入、库顺序和全局向量拥有者
清单。只有该清单证明 `Video` 应从 `C_Video` 初始化而源码输入确实缺失时，
才可新增最小 reconstruction overlay；否则应修正对象选择或停止该组合。

`base/ccpu386/ccpudefs.inc` 与 `base/cvidc/ccpudefs.inc` 也不能混为一谈：
前者不定义 `C_VID`，后者定义。CMake 已相应移除 CCPU、其 FPU link profile、
`ntstubs.c` 与 CCPU `vglob.c` 的额外 `C_VID`，但保留 C-VIDC 自己的定义。
这是对历史编译输入的校正，不改变任何源文件行为。

## 当前硬门槛：CpuVector 装配者

进一步审计显示 `ntstubs.c` 不是完整的 CCPU runtime 装配单元。它在非
`PIG` 分支定义零初始化的 `Cpu` 和 `Video`，并包含多项 no-op 回退；同一
文件中 `C_Video` 仅有 `extern` 声明，未被消费。与此同时，
`ccpu386/c_getset.c` 明确经 `Cpu.Video` 调用视频向量，`C_Video` 则是
C-VIDC 的完整生成向量。故正确问题是“谁在历史 RISC composition 中完成
`Cpu.Private`、`Cpu.Sas`、`Cpu.Video` 的整组装配”，不是单独填充
`Video.setMarkPointers`。

在找到该装配者、其缺失生成输入，或证明它不在可获得源码快照前：

* 不添加 `Video = C_Video` 或 `Cpu.Video = &C_Video` 的本地初始化；
* 不以 `ntstubs.c` 的 no-op 回退当作正式 runtime 行为；
* reset fixture 继续只作为架构 trace，不能作为可启动 host 的证据。

状态：已确认的 source/build-profile 边界，2026-08-08。

## 结论

OpenNT NT4 的 `obj.vdm/CDEFINE.INC` 明确选择两个互斥的 CPU 构建路径：

```text
386/x86:       MONITOR + C_VID + X86GFX
non-x86:       CPU_40_STYLE; when CCPU is selected, also CCPU + C_VID
```

`obj.vdm/sources` 在未定义 `CCPU` 时选择 `a4$(PD).lib`，即历史 x86
monitor/V86 CPU 库；定义 `CCPU` 时才选择 `ccpu486.lib`。同时，
`base/ccpu386/ccpudefs.inc` 固定了 CCPU 解释器自身的
`CPU_30_STYLE + CPU_40_STYLE + CCPU + SPC386` 定义。

因此，不能把 x86 的 `MONITOR/C_VID/X86GFX` 宏附加到完整的 CCPU source
集合，并把它称为“x86 CCPU”。这不是一个可补齐的局部配置差异，而是两个不同的
历史 CPU 后端。

## 已执行的反证

2026-08-08 的 default-disabled build probe 曾对完整 CCPU 对象集合去除
`CPU_40_STYLE` 并加入 `MONITOR/C_VID/X86GFX`。该 probe 在
`ccpu386/c_main.c` 编译时即停止，出现以下 source ABI 冲突：

- `cpu_interrupt_map`：`c_main.c` 的 `IUM32` 与 `cpu.h` 的 `word` 冲突；
- `CPU_SAD_INT`：CCPU 的 interrupt switch 使用该枚举，但 `cpu.h` 的
  MONITOR 分支不定义它。

这说明错误发生于编译时 profile ABI，而不是 SAS 分配、VGA、ROM、host API 或
可由 shim 填补的运行时缺口。该 probe 的所有 CMake target 已撤回，未进入任何
默认或 trace runtime。

## 对重建路线的约束

1. 历史 x86 V86 monitor 仅作为 source/behavior comparison，不作为 x64 执行前提。
2. CCPU 仍是可研究的 SoftPC 软件解释器来源；其 `CPU_40_STYLE` profile 必须保持
   内部一致，不能混入 x86 MONITOR 声明和布局。
3. reset trace 中的 CCPU/CVIDC 问题应从 CCPU profile 自己的 generated input、
   `Gdp` workspace 和 initialization order 追溯；不得通过切换到 V86 profile、
   自写 video callback 或假 SAS route 推进。
4. 下一个 live 问题仍是 `ev_glue.c::setMarkPointers()` 到 generated C_VIDC
   `S_2126_SimpleMark` 的零目标跳转。需要找回或由原始 caller 约束重建其生成输入
   与初始化合同。
