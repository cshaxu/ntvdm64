# S5 demGetDPBList Execution Admission

状态：2026-08-09。`50h/46h` 的 source-level caller、session owner 和写入算法已经
确定；本记录说明为何当前不建立会“通过”的 standalone execution fixture。

## 已确认的原始语义

NTDOS `msinit.asm::DosInit` 在 `charinit` 完成后设置：

```text
BP = offset MSDAT001e
ES = DOS data segment
SVC SVC_DEMGETDPBLIST       ; C4 C4 50 46
```

原始链为：

```text
BIOS[50h] -> MS_bop_0 -> DemDispatch(46h) -> demgset.c::demGetDPBList
```

`demGetDPBList` 只读取同一个 DEM session 的 `PhysicalDriveTypes[26]`。对于
`DRIVE_REMOVABLE` 和 `DRIVE_FIXED` 项，它直接在 `GetVDMAddr(ES,BP)` 处写 `DPB` 的
`DriveNum`、`Unit` 和 `Next`，最后更新 BP。它不做 drive discovery、device-chain
traversal、磁盘 I/O 或 host policy；这些职责不能被本地 trace 取代。

## 当前执行前置条件与 CCPU SAS profile

固定 NT4 x86 `base/mvdm/inc/softpc.h` 定义：

```c
#define GetVDMAddr(usSeg,usOff) (((ULONG)usSeg << 4) + usOff)
```

直接使用上述宏的 NT4 x86 V86 profile 会得到 raw guest-linear 数值。这里不能反向猜测它
一定通过“把 RAM 映射到地址零”完成：同一源码树的 `host/src/nt_mem.c::InitIntelMemory`
（`CPU_40_STYLE` 分支）以 `VirtualAlloc(NULL, ...)` 取得任意 `intelMem` host window，且
`public/internal/base/inc/vdm.h` 把 `VdmStartExecution`、`NtVdmControl` 声明为历史 x86
VDM host ABI。现有资料不足以把这些不同 profile 的实现细节合并为 V86 mapping 结论。

但本项目的 `dem-ccpu-sas-objects` 不使用 raw macro：受限 overlay
`overlay/base/mvdm/inc/softpc.h` 仅在 `NTDOS64_CCPU_SAS_GETVDMADDR` 下将
`GetVDMAddr(seg, off)` 转交原始 `ccpusas4.c::c_GetLinAdd`。后者返回同一 CCPU/SAS
aperture 的 host pointer。该 overlay 已作为 `overlay/reconstruction` 登记，且只用于
default-disabled CCPU SAS trace；它不是 V86 mapping 复原，也不授权通用 host pointer
shortcut。故 `demGetDPBList` 可以在该明确 profile 下成为一次真实原始 guest-write 采证。

此前 `1Bh`、`32h`、`54h/05h` 的现有 fixture 分别只保存 pointer 值；其中 COMMAND object
仍使用 raw x86 macro，不能从其数值证明 DEM 的可写 aperture。`46h` 必须独立验证。

## 禁止的捷径

1. fixture 预写 DPB、伪造 BP，或直接调用 `demGetDPBList` 后手工修补输出；
2. 扩大既有受限 overlay，或为单个 `46h` service 建立新的本地地址转换替身；
3. 为让低地址可写而建立无验证的 null-page mapping，或将 raw guest-linear 值暴露为通用
   host pointer；
4. 将 `demGetDrives` cache 硬编码，而不经原始 `0Fh` discovery owner。

这些做法会分别伪造 DEM 输出、重定义历史 ABI，或越过 software-monitor 的统一
`segment -> linear -> physical -> mapping` 语义。

## 下一项准入条件

以下条件已足以新增默认关闭的 `50h/0Fh -> 50h/46h` 组合 trace：

1. 已登记的 `c_GetLinAdd` CCPU/SAS aperture 作为唯一 guest-memory route，不再新增 alias；
2. 原始 `demGetDrives` 与 `demGetDPBList` 运行在同一 DEM session，保留其 26-slot cache；
3. fixture 同时验证原始 BP 消费、DPB `Next=-1` 终结、以及 failure observer 不修补输出。

这将是 monitor/guest-memory semantics 的下一条真实消费者，不是 disk/FDC/HDC、A:/B: 或
contained-media implementation 的准入。
