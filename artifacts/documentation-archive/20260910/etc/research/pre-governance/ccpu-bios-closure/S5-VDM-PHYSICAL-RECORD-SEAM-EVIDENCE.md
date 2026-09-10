# S5 VDM Physical Record Seam Evidence

状态：2026-08-09。本报告仅记录由原始 `nt_mem.c` 新暴露的私有 VDM seam；它不授权实现
`VdmSetPhysRecStructs`，也不改变现有 runtime 组合。

## 已证实的调用合同

固定 NT4 `host/src/nt_mem.c` 在 `VdmAddVirtualMemory()` 与
`VdmRemoveVirtualMemory()` 中各调用一次：

```c
VdmSetPhysRecStructs(HostAddress, IntelAddress, Size);
```

调用发生在以下顺序中：

1. `VdmAddVirtualMemory` 先对 host 地址做 DWORD 对齐、把长度向上取整到页，申请 Intel 地址空间，
   再调用 `sas_overwrite_memory` 使 CPU cache/translation 失效；
2. 随后调用 `VdmSetPhysRecStructs(HostAddress, IntelAddress, Size)`，并把该 allocation header 标记为
   `HDR_REMAP_FLAG`；
3. `VdmRemoveVirtualMemory` 先对 Intel 地址页对齐并取得 header 的原长度，调用
   `sas_overwrite_memory`，再以 `HostAddress = IntelAddress + intelMem` 调用同一接口，随后清除
   `HDR_REMAP_FLAG` 并释放 Intel 地址空间。

源内注释明确说明它替换的是 `PhysicalPageREC.translation` entries。它是 host 线性页到 Intel 线性页的
重映射记录，不是 DOS 驱动器、BIOS、BOP 或普通 RAM allocator。

## 跨版本采证

| 样本 | 结论 |
| --- | --- |
| 固定 NT4 OpenNT | `nt_mem.c` 有两个调用点；没有本地定义或显式 prototype。 |
| NT4.5 | 同路径文件为逐字节相同快照：51,931 bytes，SHA-256 `55832EE249E18683E6D80C582AC8DFF47318A5D467D1B1017777FC961BDCCF5E`。 |
| XP SP1 | 同一旧 SoftPC 文件仍有两个调用点，并在调用前新增 `extern void VdmSetPhysRecStructs(ULONG, ULONG, ULONG);`；文件 52,015 bytes，SHA-256 `74C8166285AC038B7E7D427D43B410C2A8A9D6E2BF246F436BB83794AAC89EC8`。未找到定义。 |
| Win2000 本地 `private/mvdm` | 未发现旧 `nt_mem.c` 或 `VdmSetPhysRecStructs` 文本命中，不能据此反推实现已消失或语义改变。 |

因此，现有样本只足以确认三项 `ULONG` 参数及调用时序；仍不能确认记录表位置、结构布局、页粒度的
partial-range 行为、并发/锁、失败模式，或在现代 x64 进程中应采用何种 backing/translation 策略。

## 组合边界与优先级

进一步的原始 caller 审计给出关键限定：`dpmi32/dpmiselr.c::VdmAddDescriptorMapping` 被
`#ifndef i386` 包围，源注释明确写为 RISC 上 DIB.DRV / CreateDIBSection / WinG 所需的额外线性
地址间接层。它不是 NT4 x86 V86 版 NTDOS 的启动依赖。

同一快照中，`PhysicalPageREC`、`PhysicalPageRecords`、`PhysicalPageMemory` 和相关
`TranslationMapREC` 布局只存在于 C-VIDC、MIPS/PPC 生成接口头；固定 x86 CCPU 头只保留这些
global-slot 宏，未保留对应的 translation-owner source。`ccpusas4.c::c_sas_overwrite_memory` 对
CCPU 本身是空实现，和 `nt_mem.c` 调用它以失效 cache 的模型也不构成 CCPU route 已实现重映射的证据。

所以本 seam 不是当前 NT4 x86 host-chain 或 NTDOS post-load 的准入阻塞项。它应保留为未来 RISC/
C-VIDC 图形内存映射的高风险缺口；在找到丢失的 generated owner 前，不得为它新增现代 x64 行为。

## 当前处理

- `nt_mem.c` 已作为原始 source owner 进入 default-disabled no-SM0 closure；
- 该 closure 仍 clean-link failure，新增未解析项为 `VdmSetPhysRecStructs`；
- 没有本地实现、alias、no-op、固定 mapping、`/FORCE:UNRESOLVED` 或 runtime 入口；
- 后续必须先找到对应 historical owner、对象/导出、或足以恢复 `PhysicalPageREC.translation` 写入语义的
  独立原始证据，再提出单独 reconstruction admission；当前不将它排入 NT4 x86 启动链的下一步。

这条 seam 属于 SoftPC 的 host-memory translation 边界；它不能被转写成 core、mantle 或新的 DOS service
接口，也不应借由已存在的 `ntvdm64`、Bochs、PCjs 等参考实现直接替换。
