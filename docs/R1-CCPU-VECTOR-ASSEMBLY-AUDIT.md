# R1 CCPU Vector Assembly Audit

> **Historical evidence, not a product configuration.** Any references below
> to `CPU_30_STYLE` quote archived OpenNT source. The project selects only
> `CPU_40_STYLE` for production compilation, linking, runtime, and acceptance.

相关的 host CPU bridge 排除证据见
`docs/R1-CCPU-HOST-BRIDGE-NEGATIVE-AUDIT.md`。
同名 CCPU/C-VIDC generated-header 的字段级 ABI 证据见
`docs/R1-CCPU-CVIDC-ABI-MATRIX.md`。

状态：进行中，2026-08-09。

## 目的

本文记录 OpenNT NT4 SoftPC 的 CCPU/C-VIDC 向量装配边界。它不定义现代
runtime，也不授权任何本地 fallback；唯一目标是判定固定源码快照中缺失的是
何种历史输入，以及恢复前需要哪些证据。

## 已证实的输入

* `base/ccpu386/sources` 产出 `ccpu386` 静态库，且其
  `ccpudefs.inc` 定义 `CPU_30_STYLE`、`CPU_40_STYLE`、`CCPU`、
  `SPC386`、`ANSI`、`NTVDM`。
* `base/cvidc/sources` 产出独立 `Cvidc` 静态库；`cvidc/ccpudefs.inc`
  在上述定义外增加 `C_VID`。
* `obj.vdm/sources` 在 `CCPU` 构建选择
  `obj.vdm/obj/*/ccpu486.lib`，但源码快照中没有该库，也没有可见的
  `Cvidc.lib` 引用或合并规则。
* `ccpu386/sources` 的 include list 引用 `host/genPg/inc`；固定 OpenNT
  快照与本地 XP 比较树均不存在 `host/genPg`。
* C-VIDC 的 `vglfunc.c` 提供完整、静态初始化的 `C_Video`。
* `ccpu386/ntstubs.c` 在非 `PIG` 构建仅定义零初始化的 `Cpu` 与 `Video`。
  它声明 `extern C_Video`，但没有引用或复制它。
* `ccpu386/c_getset.c` 通过 `Cpu.Video` 访问视频向量，证明 CCPU 的
  正常语义依赖完整 `CpuVector` 装配，不能只填某一个 `Video` 字段。

## 同代交叉核对

本地 OpenNT 4.5（`nt/private/mvdm/softpc.new`）与 XP 源码树均保留相同的
`ntstubs.c`、`C_Video` 和 `c_getset.c` 结构；两者也都只在 `obj.vdm`
引用 `ccpu486.lib`，不提供可见的该库、C-VIDC 合并规则或 `host/genPg`
目录。因此这不是 NT4 快照的偶发单文件遗漏，也不能从较新树直接复制一个
实现作为答案。

SAS 提供了对照范式：`ccpusas4.c::c_sas_init()` 调用
`SasSetPointers(&cSasPtrs)`，其中 `cSasPtrs` 是同代生成的完整表。对
CPU/Video 搜索未发现对应的 `CpuSetPointers`、`VideoSetPointers` 或等价
装配调用。这将当前缺口更准确地限定为历史 CCPU composition/link-time
vector initialization，而不是一个尚未定位的普通 C 函数。

## 排除结论

当前 reset trace 的 `Video.setMarkPointers == NULL` 不是以下任一问题：

* SAS aperture 未创建；trace 已在 `config` 与 `c_cpu_init` 后验证 RAM
  aperture 有效。
* `c_sas_overwrite_memory()` 改坏堆栈；此前相关判断已撤销。
* 可由 BIOS、BOP、DEM、DOS 或现代 host capability 修复的依赖。
* 可接受以 `Video = C_Video` 或 `Cpu.Video = &C_Video` 直接替代的单项
  回调问题。

## 运行证据

按 NT4 的 CCPU/C-VIDC 独立 profile 重建
`ntdos64-opennt-session-byob-reset-trace-fixture` 后，执行仍稳定停于：

```text
init_vga_globals -> ev_glue.c::setMarkPointers
  -> Video.setMarkPointers == NULL
```

此前 `ccpusas4.c::phy_r8()` 的 `read_pointers` 零地址已因 profile 校正而
消失。该 fixture 是默认关闭的 instrumentation，不可作为正式 host 或
可启动 DOS runtime 的证据。

## 恢复准入条件

在新增任何 reconstruction overlay 前，必须满足以下其一：

1. 找到 NT4 同代的生成器、库打包规则或已构建的 `ccpu486.lib`，并由其证明
   `CpuVector` 的初始化顺序及全部字段来源；或
2. 由多个同代 source snapshot、生成 header 和实际调用者共同证明缺失的
   装配单元及其完整字段表，并为其建立逐字段 fixture。

若只证明 `Cpu.Video`，仍不足以进入 runtime：`Cpu.Private` 与 `Cpu.Sas`
同属同一向量契约，必须同时说明其来源、生命周期与 reset 语义。

## 下一步

1. 在可获得的历史构建输出、build logs、SDK/树外工具目录中检索
   `ccpu486.lib` 与 `host/genPg` 的生成输入。
2. 对照 NT4、NT5/XP 的 `CpuVector`、`C_Video`、`ntstubs.c` 布局，制作
   字段级差异表，禁止跨版本混用实现。
3. 若仍找不到装配者，将“缺失 CCPU composition generator/link step”作为
   明确 reconstruction 候选，而不是把 reset trace 延伸为 DOS runtime。
