# S5 demGetDrives 动态 Host Seam 采证

状态：2026-08-09。本记录固定 NT4 历史链中 `demGetDrives` 的一次
default-disabled、caller-first 执行采证。它证明原始 DEM owner 在现代 x86
宿主上可自行建立其 drive-type cache；它不是 NTIO 启动、CPU 执行、设备模拟、
或 DOS runtime 的准入证据。

## 已执行的原始职责链

采证 target `ntdos64-opennt-original-demgetdrives-byob-fixture` 保留并调用：

```text
BIOS[50h]
  -> MS_bop_0
    -> DemDispatch(0Fh)
      -> demGetDrives
        -> GetDriveTypeOem(A:, B:)
        -> GetPhysicalDriveType(C:..Z:)
```

fixture 只设置原始 SAS 中的 service byte `0Fh`，调用原 `DemInit`，随后观察
原始 `MS_bop_0` 的 IP 消耗、DEM globals 及 `PhysicalDriveTypes[26]`。它没有
添加 BOP/DEM handler、没有修改 drive type、没有构造 DPB，也没有把 fixture
结果作为 DOS 成功条件。

## 2026-08-09 运行记录

使用 OpenNT i386 CMake island、clang-cl 和 x86 MSVC 环境构建后，fixture 退出码为
`0`，输出如下：

```text
returned ip=0479 ax=0005 cf=0 ndrives=5 A=0 B=0
physical types 00 00 03 03 03 00 03 00 05 00 00 00 00 00 03 00 00 00 00 00 00 00 00 03 03 00
```

因此本次实际证明的历史结果是：`MS_bop_0` 从 `0478` 消耗 service byte 到
`0479`，`demGetDrives` 保留 `CF=0`，并按原始规则返回 `AX=nDrives=5`。26 个缓存
槽完全由原 `demgset.c` 写入；这些数值是本次宿主的观测，不是跨机器 profile、
release 策略或应被固定的预期值。

## 已证实的现代 seam

首次执行并未授权预先铺设 drive facade，而是依次触到三个历史/现代接缝：

1. 原始独立 include 集缺少 `RtlOemStringToUnicodeString` 与
   `RtlDosPathNameToNtPathName_U` 的 `NTAPI` 声明。`opennt_dem_ntdecl_compat.h`
   只补声明，使 x86 import 使用现代 `ntdll` 的 `@12`、`@16` ABI；它不含行为。
2. 现代 x86 `ntdll` 仍导出前两项，但已不导出历史 `RtlProcessHeap`。实际到达
   `demgset.c::GetPhysicalDriveType` 的 `RtlFreeHeap(RtlProcessHeap(), ...)` 后，
   `historical_nt_process_heap_v1.c` 将该单一旧 entry 映射为文档化
   `GetProcessHeap()`；分配后的释放仍由原 `RtlFreeHeap` 完成。
3. `GetDriveTypeOem`、`GetPhysicalDriveType`、`PhysicalDriveTypes`、`nDrives`、
   A/B 标记及后续 DPB 消费者都保持为原始 owner；没有为它们加入现代替代或产品
   policy。

该 host shim 只因原 caller 已被 trace 触达、且对应 ntdll export 缺失而存在。它不
参与 drive 分类、缓存、DPB、BOP/SVC、设备响应或正式 runtime。

## 当前输入复核与不得晋级条件

2026-08-09 在当前固定 i386 CMake island 中重新显式构建并运行
`ntdos64-opennt-original-demgetdrives-byob-fixture`。程序退出码为 `0`，其诊断
为：

```text
returned ip=0479 ax=0005 cf=0 ndrives=5 A=0 B=0
physical types 00 00 03 03 03 00 00 00 05 ... 03 03 00
```

驱动器类型是本机观测，不能成为 profile 常量；可复现的断言仅为原始
`MS_bop_0` 的 service-byte IP 消费、`CF=0` 与 `AL == nDrives`。

该成功运行仍有明确的隔离前提：

1. target 链接 `local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c`，
   仅为 bounded trace 提供旧 CCPU 生命周期；它不是 S5 software-monitor 或 runtime
   input；
2. `historical_bios_bridge_v1.c` 和
   `historical_vdm_session_context_v1.c` 均是 trace/context 辅助物。前者的公开
   record ABI 仍只准许 `50h/14h`，本 fixture 是在同一进程中直接调用原始
   `BIOS[50h]`，不可把它解释为 v1 transport 已支持 `0Fh`；
3. `opennt_dem_ntdecl_compat.h` 只补齐缺失的 NTAPI 声明；唯一在此次 caller
   路径上发生行为替代的 modern seam 仍是
   `historical_nt_process_heap_v1.c::RtlProcessHeap -> GetProcessHeap`，
   并且原始 `RtlFreeHeap` 继续拥有释放；
4. 所有 DEM 的 drive classification、26 槽 cache、寄存器写入和后续 DPB 数据流仍由
   原始 OpenNT owner 完成。

因此本 fixture 的正确角色是 S5 的可重复动态基线和未来 helper 的对照 oracle。它不得
链接进 `ntdos64-run`、未来 `ntvdm.exe` 或任何正常执行路径，也不得借此添加
drive policy、namespace、BIOS 或 DEM 替代物。

## 未完成项与下一步

本记录不改变 S5 的总体门槛：完整 historical host chain 仍需由 software monitor
按原始 BOP/CPU 语义到达，不能由该 direct fixture 取代。后续只应从真实 NTDOS
trace 的下一个 caller 出发，逐项确认 host dependency；尤其不得把这里的 A/B
观测扩展成“默认屏蔽软驱”或任意 drive namespace 策略。
