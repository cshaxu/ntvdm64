# R1 CCPU Host Bridge Negative Audit

状态：进行中，2026-08-09。

## 目的

本文限定 OpenNT NT4 SoftPC 的 host CPU bridge 所负责的范围，避免将
CCPU/C-VIDC 向量装配误归类为一次漏掉的 host 初始化调用。本文不授权
本地 fallback、单字段赋值或 runtime patch。

## 审计对象

* `base/mvdm/softpc.new/host/src/nt_cpu.c`
* `base/mvdm/softpc.new/host/src/nt_cprgs.c`
* `base/mvdm/softpc.new/base/ccpu386/ntstubs.c`
* `base/mvdm/softpc.new/base/ccpu386/c_getset.c`
* `base/mvdm/softpc.new/base/cvidc/vglfunc.c`

## 已证实职责

在 `CCPU` 且 `CPU_30_STYLE` 路径中，`nt_cprgs.c` 的
`load_sw_cpu_access_functions()` 仅安装 host-facing 寄存器读写函数和
`host_simulate_func = c_cpu_simulate`。它不写入 `Cpu`、`Video`、
`Cpu.Private`、`Cpu.Sas` 或 `Cpu.Video`。

`nt_cpu.c` 的 `host_cpu_init()` 在 CCPU 配置中为空。其
`InitNtCpuInfo()` 是 `CPU_40_STYLE` 的 NT debugger-register bridge，依赖
生成的 GDP 布局；它同样不装配通用 `CpuVector` 或 `VideoVector`。因此，
无论是否能恢复 GDP bridge，都不能把它当作当前
`Video.setMarkPointers == NULL` 的修复点。

## 当前组合缺口

`ntstubs.c` 在非 `PIG` 配置只定义零初始化的全局 `Cpu` 与 `Video`，并且
只声明 `extern C_Video`；没有复制、别名或初始化操作。相反：

* `cvidc/vglfunc.c` 提供完整的 `C_Video` 静态表；
* `ccpu386/c_getset.c` 通过 `Cpu.Video` 读取和写入 video latches；
* host/C-VIDC 代码还通过 `Cpu.Private` 调用大量 CPU-private vector
  操作；
* SAS 有可见对照模式：`c_sas_init()` 安装完整 `cSasPtrs` 表。

所以当前崩溃首先表现为 `Video.setMarkPointers == NULL`，但真正缺失的是
完整的 CCPU composition/link-time vector initialization，而不是一个视频
回调或 host capability。仅执行 `Video = C_Video`、`Cpu.Video = &C_Video`
或填充一个观察到的字段都违反恢复准入条件。

2026-08-09 的受限复制实验已撤回。它先证明 `C_Video` 的三个 mark slots
均非零；但当装配单元显式使用完整 `base/cvidc/evidgen.h` 时，也证明
`ntstubs.c` 的 `Video` 存储按同名但较短的 `ccpu386/evidgen.h` ABI 编译。
因此完整表复制会越过该存储并破坏相邻状态。此前以短 ABI 编译时虽然能显示
前缀绑定，却是以错误 offset 解释完整表，不能作为成功证据。字段级矩阵见
`docs/R1-CCPU-CVIDC-ABI-MATRIX.md`。

该实验不再保留任何 overlay、CMake target 或 trace 接线。结论是必须找回
匹配完整 C-VIDC ABI 的历史 composition/storage 单元；不得用局部 callback、
短表复制或 `CpuVector` 推定来绕过此对象布局不匹配。

## 构建输入核对

固定 NT4、OpenNT 4.5 与本地 XP 源码树都保留：

* `obj.vdm` 的 CCPU 链接输入名 `ccpu486.lib`；
* 独立 `base/ccpu386` 与 `base/cvidc` 静态库描述；
* `ccpu386/sources` 对 `host/genPg/inc` 的 include 依赖。

三棵可用源码树均没有可见的 `ccpu486.lib`、`host/genPg` 或将 CCPU 和
C-VIDC 合并为该库的规则。故不能把本地 CMake 对两个源目录的直接拼接
视为历史 build graph 的等价物。

## 恢复准入条件

在新增 reconstruction overlay 前，必须满足下列一项：

1. 找到同代的 `ccpu486.lib`、生成器、packaging rule 或 build log，能够
   给出 `CpuVector` 全字段的提供者、装配顺序和 reset 生命周期；或
2. 由多份同代源码、生成 header、完整静态 initializer 与真实调用者共同
   导出全字段规格，并为每个字段建立正反 fixture。

该恢复单元必须同时解释 `Cpu.Private`、`Cpu.Sas`、`Cpu.Video`、全局
`Video` 及其所有权，不能以当前 reset trace 的第一个空指针为边界。

## 下一步

1. 在可获得的历史构建输出、SDK、符号和构建日志中继续寻找
   `ccpu486.lib` 与 `host/genPg` 的输入。
2. 从 `CpuVector` 的结构定义、C-VIDC 的静态表及 CCPU 私有操作中列出
   字段级 provider 矩阵。
3. 在矩阵闭合前，保持 reset trace 为默认关闭的 instrumentation；不得将
   它扩展成 DOS runtime。
