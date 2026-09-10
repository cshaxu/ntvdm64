# S5 DEM OEM Facade Caller Map

状态：从完整 BIOS-table trace 的机械 link evidence 导出；不表示每个 caller 已动态到达。

## 已动态证明

`demmisc.c:demLoadDos` 是唯一已经由 NTIO service `11h` 实际到达的 caller。它使用
`CreateFileOem` 读取 BYOB `NTDOS.SYS`；受限 F1 facade 已闭合该调用，且原 loader 已返回。

其余下表条目来自完整 DEM object 的静态链接，不得因为“已出现在 unresolved ledger”就提前
实现或宣布可用。

## 未解 OEM/RTL 接缝与原 caller

| 原 DEM object | unresolved API | 分组 | 当前处置 |
| --- | --- | --- | --- |
| `demmisc.c` | `GetFullPathNameOem` | F2 directory/path | 未动态到达；保持未实现 |
| `demdir.c` | `CreateDirectoryOem`, `GetFileAttributesOem`, `RemoveDirectoryOem`, `SetCurrentDirectoryOem`, `SetEnvironmentVariableOem` | F1/F2/F5 | 目录与 per-session environment policy 尚未定义；fail closed |
| `demfcb.c` | `CreateFileOem`, `DeleteFileOem`, `FindFirstFileOem`, `FindNextFileOem`, `MoveFileOem`, `SetFileAttributesOem` | F1/F2 | FCB service caller 未动态到达；不复用 F1 resource-root-only read policy 扩展写权限 |
| `demgset.c` | `GetComputerNameOem`, `GetDiskFreeSpaceOem`, `GetDriveTypeOem`, `RtlOemStringToUnicodeString` | F3/F6/conversion | logical drive 与 volume profile 尚未定义；不读取宿主默认工作目录或环境 |
| `demsrch.c` | `RtlUpcaseUnicodeStringToCountedOemString` | search/conversion | 原生搜索路径与 guest namespace 尚未到达；保持未实现 |
| `overlay/.../demfile.c` | `GetVolumeInformationOem`, `RemoveFontResourceOem` | F3/F6 | volume/font policy 尚未定义；保持未实现 |
| `demdisp.c` | `OutputDebugStringOem` | F6 debug | 可以作为独立、无状态 debug forwarding 候选，但当前没有 runtime 准入 |

## 对 F1 的限制

目前 F1 不代表“文件系统已实现”：它仅以显式 resource root、read-oriented `CreateFileOem`
支撑 `demLoadDos`。`demfcb.c` 的 read/write/delete/rename/find 流程拥有不同的 caller contract，
至少需要独立的 namespace、share/lock、错误和路径逃逸策略。它们不能继承 NTDOS loader 的
resource-root shortcut。

## 下一项准入条件

优先级不是按 unresolved 数量，而是按原始 trace：

1. 先取得 CCPU loop 实际恢复执行后的下一条 NTIO/NTDOS caller；
2. 若它首先到达 OEM API，则只实现该函数所属的最小 fail-closed contract；
3. 若它先到 video、timer、interrupt 或 BIOS device owner，则先处理该 owner，不绕回 OEM；
4. 每次新增 API 都需更新 `SOURCE-MANIFEST.json`、local governance ledger、work order 与
   正反 fixture，且默认 runner 不得链接。
