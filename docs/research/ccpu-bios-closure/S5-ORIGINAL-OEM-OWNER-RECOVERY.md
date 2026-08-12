# S5 Original OEM Owner Recovery

状态：2026-08-09。该记录是 S5 no-SM0 source-owner recovery 的第三次接入，不是 OEM
facade runtime 准入。接入对象是固定 OpenNT 原始 `base/mvdm/oemuni/file.c` 与 `process.c`
组成的 `ntdos64-opennt-oemuni-provider-objects`，不含本地 `oem_facade_v1` 或 archived
contained facade。

## 接入理由

固定 guest 的 `C4 C4 50 0F -> demGetDrives` 路径已由
`S5-NTDOS-FIRST-POSTLOAD-SVC.md` 证实。`demGetDrives` 调用 `GetDriveTypeOem`，其原始
owner 是 `oemuni/file.c`；因此该 source group 是第一个由真实 NTDOS post-load service
指令支持的 OEM owner，而不是为了降低静态链接数字而猜测加入的模块。

## 变更边界

只将原始 object group 加入两个 default-disabled、no-SM0 source-closure diagnostic：

- `ntdos64-opennt-historical-transport-v2-msbop-closure`；
- `ntdos64-opennt-historical-transport-v2-owner-closure`。

两个 target 仍不在 runner、默认 CTest 或 runtime 图中，也不使用 `/FORCE:UNRESOLVED`。
第二个 target 的 `BIOS[]` 完整静态表依旧宽于当前动态路径；第一个 target 只用于观测
`MS_bop_0 -> DemDispatch` 不含 BIOS[] 表宽度的闭包。

## 构建结果

日志位于：

```text
artifacts/build/current/opennt-r2-bridge-ninja-i686/
  s5-ntdos64-opennt-historical-transport-v2-msbop-closure-oem-owner-20260809.log
  s5-ntdos64-opennt-historical-transport-v2-msbop-closure-oem-intelmem-owner-20260809.log
  s5-ntdos64-opennt-historical-transport-v2-owner-closure-oem-intelmem-owner-20260809.log
```

这些 target 按预期仍为 clean-link failure；它们的价值是揭示原始 source 的下游，不是输出可运行
helper。仅加入 OEM group 的 BOP/DEM 对照闭包从 116 降到 98，且没有出现新的 unresolved symbol。

曾有一份名为 `*-owner-closure-oem-owner-*` 的完整 BIOS[] 日志，但当时 OEM object 实际仍停留在
另一条 helper 诊断中，不能作为完整 BIOS[] 已接入 OEM 的证据，现明确废止该解释。校正链接位置后，
完整 BIOS[] 闭包再加独立的原始 `nt_mem.c` owner，从 281 降至 262：其中 OEM group 实际提供下列
18 个 import，`nt_mem.c` 提供 `InitIntelMemory`、`FreeIntelMemory`，同时首次暴露一个新的
`VdmSetPhysRecStructs` 私有 VDM 依赖。完整静态表仍远宽于当前动态路径，不能与 98/97 的 BOP/DEM
对照数字直接比较。

原始 OEM group 消除了以下 18 个历史 facade import：

```text
CreateDirectoryOem       CreateFileOem          DeleteFileOem
FindFirstFileOem         FindNextFileOem        GetComputerNameOem
GetDiskFreeSpaceOem      GetDriveTypeOem        GetFileAttributesOem
GetFullPathNameOem       GetVolumeInformationOem
MoveFileOem              OutputDebugStringOem   RemoveDirectoryOem
RemoveFontResourceOem    SetCurrentDirectoryOem SetEnvironmentVariableOem
SetFileAttributesOem
```

这说明此前 BOP/DEM closure 中的 OEM unresolved 是“尚未接入原始 owner”，而不是需要自写
DOS SVC 的证据。

## 未解决的真实边界

原始 `file.c` 使用 NT4 的 `NtCurrentTeb()->StaticUnicodeString`、RTL OEM conversion 与
native path helpers。接入它并不证明这些行为可直接在现代进程安全执行。当前仍可见的下游
包括 `RtlOemStringToUnicodeString`、`RtlDosPathNameToNtPathName_U`、`RtlProcessHeap`、
`NtOpenSymbolicLinkObject`、`NtQuerySymbolicLinkObject` 与 `RtlEqualUnicodeString`。

因此目前正确结论是：

1. 原始 OEM source owner 已恢复到 source closure；
2. `GetDriveTypeOem` 的 caller 已由 NTDOS 真实指令证实；
3. 现代 TEB/RTL 兼容仍是下一层 host seam 研究，不得用单函数 no-op、固定盘符数或预置
   `numio` 掩盖；
4. 只有定义完整的输入、OEM conversion、error mapping、host-drive policy 与 failure
   semantics 后，才可讨论 modern OEM facade 的下一版本。

这保持了历史职责链：`demGetDrives` 仍拥有 `PhysicalDriveTypes[]`、A/B 状态、`nDrives`
和 AX/CF 返回；任何未来 host facade 只在它原本调用的 OEM/native 边界上工作。
