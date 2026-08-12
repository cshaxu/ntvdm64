# S5 DemGetDrives Host Seam Contract

状态：2026-08-09。本文将固定 NT4 `demGetDrives` 的原始 host dependency
写成恢复合同。它不实现 drive discovery，不提供新 DEM service，也不允许预写
`PhysicalDriveTypes` 让 NTDOS 前进。

## 原始调用与返回

`msinit.asm:292-293` 在 `DosInit` 内执行：

```text
C4 C4 50 0F                 SVC_DEMGETDRIVES
mov [numio], al             NTDOS only consumes AL
```

原始链为：

```text
CCPU BOP decode -> BIOS[50h] -> MS_bop_0
  -> DemDispatch(SVC_DEMGETDRIVES) -> demGetDrives
```

`demgset.c:351-429` 实际执行 `setAX(nDrives)` 与 `setCF(0)`，所以回调在完整
历史 state 上写整个 `AX`；NTDOS 随即只读取低字节 `AL`。不能将源注释中的
“AL is number of drives”误写成仅改写 AL 的 ABI。

## 精确的原始盘符算法

`demGetDrives` 维护 process-local 的 `PhysicalDriveTypes[26]`、`nDrives`、
`IsAPresent` 和 `IsBPresent`，并按下列顺序调用原始 host APIs：

1. 对 A: 和 B:，调用 `GetDriveTypeOem("?:\\")`。源码明确这么做是为了避免
   filesystem 主动读取软盘驱动器。
2. A:/B: 的 `DRIVE_NO_ROOT_DIR` 与 `DRIVE_REMOTE` 都归一为 `DRIVE_UNKNOWN`；
   若先得到 `DRIVE_FIXED`，再调用原始 `GetPhysicalDriveType(n)`，以排除 SUBST
   directory。
3. A: 是 unknown 时同时令 `IsAPresent`、`IsBPresent` 为 false；B: 是 unknown
   时令 `IsBPresent` 为 false。
4. `nDrives` 从 2 开始。C:--Z: 全部调用 `GetPhysicalDriveType(n)`：它把 `X:\`
   经 `RtlOemStringToUnicodeString`、`RtlDosPathNameToNtPathName_U`、`NtOpenFile`
   和 `NtQueryVolumeInformationFile` 分类为 fixed/removable/CD-ROM/RAM disk；
   remote、directory/SUBST、不可打开、或未知 device type 都是 unknown。
5. 计数只接受连续的 removable/fixed/CD-ROM/RAM disk；遇到第一个 unknown 后，
   仍记录后续盘符类型，但不再递增 `nDrives`。

这说明 `numio` 不是 host 盘符总数，也不是一组可任意排序的 mounts；它是历史算法
得到的连续低盘符边界。

## 现代 host seam 的允许范围

未来 modern-host compatibility shim 必须保留上述 **DEM caller、API 边界和数据流**。
它只可让原始 `GetDriveTypeOem`/`GetPhysicalDriveType` 所需要的语义在现代 host 上
可用，返回相同的 drive-type 域；不得在 `demGetDrives` 外重写 `nDrives`、`AX`、CF、
`IsAPresent`、`IsBPresent` 或 guest `numio`。

最低 policy 需要显式区分：

| policy | A:/B: 结果来源 | 约束 |
| --- | --- | --- |
| `auto` | Windows 可访问 DOS volume 的真实分类 | 传统 FDC、USB 软驱和虚拟软驱均可出现；不可一律屏蔽。 |
| `disabled` | 明确报告 unknown | 只能由产品 profile 显式指定。 |
| `host-drive` | 真实指定 host volume 的分类 | 仍须遵循 original removable/fixed/remote/SUBST 归类。 |
| `image-backed` | 由已挂载、受限 image provider 暴露为同一 drive-type contract | 不得绕过原始 DEM 直接预写 drive array 或 DPB。 |

该 policy 是 session/profile 输入，不是 DOS ABI。任何路径逃逸、物理设备访问、挂载或
image I/O 的授权都必须另由原始 reached caller 和 containment gate 证明。

## 尚未准入的工作

- 不增加 selector `50h/0Fh` 的 local service handler；
- 不将 current host drive enumeration 当作无条件成功或对 host 的 ambient discovery；
- 不在 S4 或 BYOB loader 中预写 NTDOS `numio`、DEM globals 或 DPB；
- 不以一个 disk-free/path test 推断 `demGetDrives` 可运行。

下一项有效验收是：当原始 CCPU/BIOS/MS_bop_0/DEM 组合实际可运行时，在一个
default-disabled、受限 session 中记录 original call 前后完整 AX、FLAGS、
`PhysicalDriveTypes[0..25]` 和 `IsA/IsB`，并以 profile 所选 policy 验证拒绝路径。
这之前本文件只是 source-backed admission contract。

## 证据来源

- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm:292-293`
- `src/opennt/base/mvdm/dos/dem/demdisp.c:114`
- `src/opennt/base/mvdm/dos/dem/demgset.c:39-44, 215-429`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c:146-159`
- `docs/research/ccpu-bios-closure/S5-NTDOS-DOSINIT-EARLY-SVC-LEDGER.md`
