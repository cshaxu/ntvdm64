# S5 Original Intel Memory Owner Recovery

状态：2026-08-09。本记录恢复的是 OpenNT 原始
`base/mvdm/softpc.new/host/src/nt_mem.c` 的 source ownership，不是为现代宿主提供一套新的内存
实现，也不是 runtime 准入。

## 依据与接入方式

在 `CPU_40_STYLE` profile 中，原始 `sim32.c::sas_init()` 调用 `InitIntelMemory()`，而
`sas_term()` 调用 `FreeIntelMemory()`。此前 no-SM0 closure 已采用原名 `sim32.c`，但没有将
`nt_mem.c` 的真实 owner 纳入，导致这两个符号被误列为缺口。

为避免第二份 `sim32.c`、也避免把现有 SAS fixture 的重命名 bridge 混进正式诊断，CMake 新增了
仅含 `nt_mem.c` 的 default-disabled object target：

```text
ntdos64-opennt-ccpu-host-intel-memory-owner-objects
```

它以原始 native `nt.h -> windows.h` 声明顺序、`SIM32` 和 `_X86_` 编译。单独 object build 成功；
编译 warning 只反映旧 SDK/CRT 的声明差异，不引入本地行为替代。

## 链接采证

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-ntdos64-opennt-historical-transport-v2-msbop-closure-oem-owner-20260809.log
  s5-ntdos64-opennt-historical-transport-v2-msbop-closure-oem-intelmem-owner-20260809.log
  s5-v2-full-owner-closure-no-sm0-original-sim32-20260809.log
  s5-ntdos64-opennt-historical-transport-v2-owner-closure-oem-intelmem-owner-20260809.log
```

`MS_bop_0 -> DemDispatch` 对照闭包的 unique unresolved 从 98 降到 97。消失的是
`InitIntelMemory` 与 `FreeIntelMemory`；新出现的是 `VdmSetPhysRecStructs`。后者来自
`nt_mem.c::VdmAddVirtualMemory` / `VdmRemoveVirtualMemory`，是 NT 私有 VDM 物理页记录接口，
不是普通 Win32 allocation 函数。

校正 OEM object 的完整 BIOS[] 接入位置后，完整 no-SM0 static closure 从 281 降到 262。该净变化
同时包含 OEM group 的 18 个 import 与本对象提供的两个 SAS owner，并引入同一个
`VdmSetPhysRecStructs`。这仍是失败的链接诊断；没有生成或执行 `ntvdm.exe`，也没有为该接口加入
shim、别名、固定返回值或 `/FORCE:UNRESOLVED`。

## 结论

1. `nt_mem.c` 是已经由真实 `sim32.c` 调用关系证实的历史 owner，应保留在 source closure；
2. `VdmSetPhysRecStructs` 是下一条需要进行 caller-first 调研的现代 host seam，不能被降格为
   “SAS 分配成功即可忽略”的实现细节；
3. 在得到该记录结构、生命周期、失败语义与调用上下文的历史证据前，不得新增现代替代实现；
4. 本 target 默认关闭，且不进入 runner、CTest 或任何正常运行路径。
