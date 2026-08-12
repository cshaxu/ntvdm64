# S5 Drive Session Owner Contract

状态：2026-08-09。本记录固定 NT4 的原始 drive-discovery 与 DPB-building 数据流。它不是现代
drive provider 的实现授权，也不改变 BYOB、A:/B: 或 release 策略。

## 原始状态所有权

`demgset.c` 定义了整个 DEM session 共享的原始状态：

```text
PhysicalDriveTypes[26]
nDrives
IsAPresent
IsBPresent
```

`demGetDrives()` 是写入 owner；它由 NTDOS `DosInit` 的 `50h/0Fh` 调用。`demGetDPBList()`
在同一次 `DosInit` 后段由 `50h/46h` 调用，只读取 `PhysicalDriveTypes[]` 并向 `ES:BP` 目标
写 guest DPB 链。二者必须处于同一个 DEM session，不能分别从 host 重新猜测 drive 集合。

## 原始 discovery 算法

1. A:、B: 先经 `oemuni/file.c::GetDriveTypeOem("A:\\"/"B:\\")` 查询，以避免底层
   filesystem 直接探测软驱；
2. 若 A/B 返回 `DRIVE_FIXED`，再调用 `GetPhysicalDriveType`，用 native volume device type
   排除 SUBST directory；`DRIVE_NO_ROOT_DIR`、`DRIVE_REMOTE` 归为 `DRIVE_UNKNOWN`；
3. 原始 `GetPhysicalDriveType` 将 `A:\\`--`Z:\\` 做 OEM-to-Unicode 和 DOS-to-NT path 转换，
   `NtOpenFile(... FILE_NON_DIRECTORY_FILE)` 后调用 `NtQueryVolumeInformationFile`：remote 为
   unknown，CD-ROM 为 CDROM，virtual disk 为 RAMDISK，disk/disk-FS 依 removable bit 为
   removable 或 fixed；
4. 若 A: unknown，原代码同时置 `IsAPresent = FALSE` 与 `IsBPresent = FALSE`；若 B: unknown，
   单独置 `IsBPresent = FALSE`；
5. C:--Z: 都填入 cache。`nDrives` 从 2 起仅对第一个 unknown 之前连续出现的 removable、fixed、
   CDROM、RAMDISK 递增；它不是 26 位 availability bitmap；
6. 返回时设置 `AX = nDrives`、`CF = 0`。

这解释了为什么一个“只返回硬编码盘符数”的替代会破坏后续状态，即便第一个 service 看似已返回。

## 原始 DPB 消费算法

`demGetDPBList()` 从 `ES:BP` 得到 guest output cursor，遍历 0--25：仅当 cache 项为
`DRIVE_REMOVABLE` 或 `DRIVE_FIXED` 时写一个 DPB，填入 `DriveNum`/`Unit`，把 `Next` 链到
下一个 guest DPB；最后一个写入项的 `Next = -1`，并以更新后的 `BP` 返回。CD-ROM、RAMDISK、
remote、SUBST 及 unknown 都不进入 DPB 链。

因此该阶段的 host 与 guest 责任严格不同：host discovery 只建立 DEM cache；DEM 是 DPB 的唯一
writer；NTDOS 决定 output buffer 所在的 guest 地址及后续解释。不得让现代 host code 预写 DPB、
伪造 `BP` 或替换 `demGetDPBList`。

## 已定位的现代接缝

原始 A/B OEM thunk 使用 `NtCurrentTeb()->StaticUnicodeString`，而 C:--Z: 的 native probe 使用
NT4 的 RTL/Nt* 路径与 `RtlProcessHeap()`。在现代进程中，这些 private layout/API 不能直接假设
可用。可讨论的 shim 粒度只能是原始调用边界：

```text
OEM root string -> Unicode conversion -> host drive type / physical classification
```

它必须同时保留：A/B 不被一律屏蔽、USB/virtual floppy 的可访问性、SUBST/remote 的 original
classification、26 项稳定 cache、错误归 unknown 的语义，以及一次 `DosInit` 内 discovery/DPB 的
一致性。它不能成为新 DOS namespace、镜像策略、BIOS INT 13h、FDC/HDC 或 `demGetDrives` 的
替代实现。

## 当前结论

- 原始 OEM/native source owner 已进入 default-disabled source closure；
- 尚无获准的现代 implementation 或 runtime trace；
- `50h/0Fh` 与 `50h/46h` 应作为一个不可拆分的 session contract 排入后续 caller-first trace；
- 在该 contract 的错误、cache 生命周期与 guest DPB 写入均被验证前，不得宣称 NTDOS `DosInit`
  或 DOS drive support 已恢复。
