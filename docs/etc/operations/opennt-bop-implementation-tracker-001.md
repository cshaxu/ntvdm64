# OpenNT 203 BOP 实施追踪表

> **用途：** 全项目唯一的 203-entry 实施跟踪底册。它从 `t225-s57` 原始 owner ledger 建立入口和 owner 基数，但“现状”按 T230 的严格口径填写：路由、fixture、显式失败或 deferred 都不是功能完成。每次实施必须更新对应行和本表的生成时间；不得以此表替代 `STATUS.md` 的当前任务权威。

## 状态口径

| 标记 | 含义 |
| --- | --- |
| 局部 | 有部分 provider/fixture 或原始 no-op，但尚未达到该 family 的完整 source/ABI/Direct/Readonly/native 验收。 |
| 未完成 | 当前为 deferred、unavailable、placeholder，或尚未完成 owner package。 |
| 完成 | 仅当原始 source 契约、Direct/Readonly、Overlay ABI、全族回归和该任务要求的 native observation 全部通过时才可使用；当前没有条目可标为完成。 |

生成基线：`docs/etc/research/t225-s57-bop-owner-package-profile-ledger-001.json`（203 entries）；实际状态复核：T229/P24、T230/S1。生成日期：2026-08-18。

## 1. DEM / DOS（73）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `50:00` | `demChgFilePtr` | 改变已打开文件的读写位置 | Direct 原始 `demhndl.c` 镜像已局部闭合；native/runtime 未验证 | 通过 token/checked-memory/typed-result shim 直接调用原函数；成功定位与无效 token 的 AX/CF 失败已测 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、`bop/shim/demhndl_shim.*`；T230 S2 map；待 native 验证 |
| `50:01` | `demChMod` | 查询或修改文件属性 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 body + OEM/guest/token shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`；T230 S3；待 native 验证 |
| `50:02` | `demClose` | 关闭 DOS 文件句柄 | Direct 原始 `demhndl.c` 镜像已局部闭合；native/runtime 未验证 | 原始可选 seek/close 顺序保留；仅以 shim close 同时退役不透明 token，局部关闭回归已测 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、`bop/shim/demhndl_shim.*`；T230 S2 map；待 native 验证 |
| `50:03` | `demCreate` | 创建或截断文件 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 create/volume-label body + token shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`、`demlabel.c`；T230 S3 |
| `50:04` | `demCreateDir` | 创建目录 | OpenNT `demdir.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 body + OEM directory shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demdir.c`；T230 S4 |
| `50:05` | `demDelete` | 删除文件 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 body + OEM pathname shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`；T230 S3 |
| `50:06` | `demDeleteDir` | 删除目录 | OpenNT `demdir.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 body + OEM directory shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demdir.c`；T230 S4 |
| `50:07` | `demDeleteFCB` | 按 FCB 删除文件 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:08` | `demFileTimes` | 查询或设置文件日期时间 | Direct 原始 `demhndl.c` 镜像已局部闭合；native/runtime 未验证 | 原始 get/set/device-time 分支保留；get-time Direct 调用已测，完整 host session 集成待后续包验证 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、T230 S2 map；待 native 验证 |
| `50:09` | `demFindFirst` | 目录枚举起始 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:0A` | `demFindFirstFCB` | FCB 目录枚举起始 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:0B` | `demFindNext` | 目录枚举继续 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:0C` | `demFindNextFCB` | FCB 目录枚举继续 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:0D` | `demGetBootDrive` | 取得启动驱动器 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:0E` | `demGetDriveFreeSpace` | 取得驱动器剩余空间 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:0F` | `demGetDrives` | 取得驱动器位图 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:10` | `demGSetMediaID` | 读取/设置介质标识 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:11` | `demLoadDos` | 装入 NTDOS guest 映像 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:12` | `demOpen` | 打开文件 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 local-file body + opaque token；named-pipe Redirector 仍按原始失败路径 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`；T230 S3 |
| `50:13` | `demQueryCurrentDir` | 取得当前目录 | OpenNT `demdir.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 CDS validation/reset body + fixed-width guest copy seam | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demdir.c`；T230 S4 |
| `50:14` | `demQueryDate` | 取得宿主日期 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:15` | `demQueryTime` | 取得宿主时间 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:16` | `demRead` | 从文件读入 guest 缓冲区 | Direct 原始 `demhndl.c` 镜像已局部闭合；native/runtime 未验证 | 原始 seek/read/flush/free/error顺序保留；checked guest bounce 回写与 host round-trip 已测 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、`bop/shim/demhndl_shim.*`；T230 S2 map；待 native 验证 |
| `50:17` | `demRename` | 重命名文件 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 cross-drive/same-name checks + OEM shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`；T230 S3 |
| `50:18` | `demSetCurrentDir` | 改变当前目录 | OpenNT `demdir.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 process-CWD / `=X:` environment body；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demdir.c`；T230 S4 |
| `50:19` | `demSetDate` | 设置宿主日期 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:1A` | `demSetDefaultDrive` | 改变默认驱动器/当前目录 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:1B` | `demSetDTALocation` | 登记 DOS DTA 缓冲区 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:1C` | `demSetTime` | 设置宿主时间 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:1D` | `demSetV86KernelAddr` | 登记 V86 内核地址 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:1E` | `demWrite` | 从 guest 缓冲区写文件 | Direct 原始 `demhndl.c` 镜像已局部闭合；native/runtime 未验证 | 原始 seek/write/zero-length/truncate/disk-full 分支保留；checked guest → host write 已测 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、`bop/shim/demhndl_shim.*`；T230 S2 map；待 native 验证 |
| `50:1F` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:20` | `demRenameFCB` | 按 FCB 重命名文件 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:21` | `demIOCTL` | DOS 设备/驱动器 IOCTL | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:22` | `demCreateNew` | 仅在不存在时创建文件 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 create-new body + token shim；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`；T230 S3 |
| `50:23` | `demDiskReset` | 重置磁盘/软盘状态 | 局部：仅恢复 pFDAccess 清零；原始 raw-media reset 未实现 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:24` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:25` | `demGetDPB` | 取得 DOS Drive Parameter Block | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:26` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:27` | `demCommit` | 提交文件缓冲 | Direct 原始 `demhndl.c` 镜像已局部闭合；native/runtime 未验证 | 原始 `FlushFileBuffers` 非致命失败语义与 CF 清除保留，Direct 调用已测 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`；T230 S2 map；待 native 验证 |
| `50:28` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:29` | `demAbsRead` | 绝对扇区读取 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:2A` | `demAbsWrite` | 绝对扇区写入 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:2B` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:2C` | `demCreateFCB` | 按 FCB 创建文件 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:2D` | `demOpenFCB` | 按 FCB 打开文件 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:2E` | `demCloseFCB` | 按 FCB 关闭文件 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:2F` | `demFCBIO` | FCB 文件读写 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:30` | `demDate16` | 16 位日期转换 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:31` | `demGetFileInfo` | 取得文件属性、时间、日期和大小 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:32` | `demSetHardErrorInfo` | 登记硬错误信息 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:33` | `demRetry` | 硬错误重试控制 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:34` | `demLoadDosAppSym` | 装入 DOS 应用符号 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:35` | `demFreeDosAppSym` | 释放 DOS 应用符号 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:36` | `demEntryDosApp` | 进入 DOS 应用生命周期 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:37` | `demDOSDispCall` | DOS dispatcher 调用 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:38` | `demDOSDispRet` | DOS dispatcher 返回 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:39` | `demOutputString` | 控制台输出字符串 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:3A` | `demInputString` | 控制台输入字符串 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:3B` | `demIsDebug` | 查询调试状态 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:3C` | `demTerminatePDB` | 终止 PDB 的搜索状态 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:3D` | `demExitVDM` | 退出 VDM | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:3E` | `demWOWFiles` | WOW 文件转换/查询 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:3F` | `demLockOper` | 文件/记录锁定操作 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:40` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:41` | `demGetComputerName` | 取得计算机名 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:42` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部：FastRead 兼容 provider；原始全契约未验收 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:43` | `demNotYetImplemented` | OpenNT 原始未实现服务：CF-clear no-op（50:42 例外为 FastRead 兼容路径） | 局部已验证：原始 CF-clear no-op；T230 全族/原生验收未完成 | 原始 no-op（已本地验证） | 原始 no-op（已本地验证） | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:44` | `demCheckPath` | 检查 DOS 路径 | OpenNT `demfile.c` Direct 镜像局部闭合；native/runtime 未验证 | 原始 device/path probe body；局部 Direct 回归已通过 | 不在 v2 范围 | 不在 v2 范围 | `demdisp.c`、`demfile.c`；T230 S3 |
| `50:45` | `demSystemSymbolOp` | 系统符号操作 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:46` | `demGetDPBList` | 取得 DPB 链表 | 局部：存在 source-derived/host-capability 路径；T230 严格功能验收未完成 | 局部路径，T230 未验收 | 局部路径，T230 未验收 | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/dem/demdisp.c；T230` |
| `50:47` | `demPipeFileDataEOF` | 检查命名管道数据/EOF | Direct 原始 `demhndl.c` 镜像已局部闭合；COMMAND pipe backend 未组合，native/runtime 未验证 | 原函数与其 CF/ZF 合同已通过 optional pipe callback 进入；无 backend 时保留 source-shaped non-EOF 查询，不伪造 pipe 服务 | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、`bop/shim/demhndl_shim.*`；T230 S2 map；待 COMMAND pipe 包 |
| `50:48` | `demPipeFileEOF` | 检查命名管道 EOF | Direct 原始 `demhndl.c` 镜像已局部闭合；COMMAND pipe backend 未组合，native/runtime 未验证 | 原函数与 EOF/size register 合同已通过 optional pipe callback 进入；无 backend 不伪造 EOF | 不在 v2 范围 | 不在 v2 范围 | `bop/opennt/dem/demhndl.c`、`bop/shim/demhndl_shim.*`；T230 S2 map；待 COMMAND pipe 包 |

## 2. COMMAND（17）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `54:00` | `cmdExitVDM` | COMMAND 请求退出 VDM | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:01` | `cmdGetNextCmd` | 取得下一条 COMMAND 命令 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:02` | `cmdComSpec` | 取得 COMSPEC | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:03` | `cmdSaveWorld` | 保存 COMMAND 执行上下文 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:04` | `cmdGetCurrentDir` | 取得当前目录 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:05` | `cmdSetInfo` | 登记 COMMAND/DEM 共享状态 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:06` | `cmdGetStdHandle` | 取得标准输入输出句柄 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:07` | `cmdCheckBinary` | 检查二进制程序类型 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:08` | `cmdExec` | 执行命令/子进程 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:09` | `cmdInitConsole` | 初始化控制台 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:0A` | `cmdExecComspec32` | 执行 32 位 COMSPEC 路径 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:0B` | `cmdReturnExitCode` | 返回子进程退出码 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:0C` | `cmdGetConfigSys` | 取得 CONFIG.SYS | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:0D` | `cmdGetAutoexecBat` | 取得 AUTOEXEC.BAT | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:0E` | `cmdGetKbdLayout` | 取得键盘布局 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:0F` | `cmdGetInitEnvironment` | 取得初始环境块 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |
| `54:10` | `cmdGetStartInfo` | 取得启动信息 | 未完成：仅有限 route/fixture；完整 COMMAND package 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dos/command/cmddisp.c；对应 owner package` |

## 3. XMS（12）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `52:00` | `xmsA20` | 控制 A20 地址线 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:01` | `xmsMoveBlock` | 扩展内存块搬移 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:02` | `xmsAllocBlock` | 分配扩展内存块 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:03` | `xmsFreeBlock` | 释放扩展内存块 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:04` | `xmsSysPageSize` | 查询系统页大小 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:05` | `xmsQueryExtMem` | 查询扩展内存容量 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:06` | `xmsInitUMB` | 初始化 UMB | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:07` | `xmsRequestUMB` | 申请 UMB | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:08` | `xmsReleaseUMB` | 释放 UMB | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:09` | `xmsNotifyHookI15` | INT 15h/XMS 通知钩子 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:0A` | `xmsQueryFreeExtMem` | 查询空闲扩展内存 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |
| `52:0B` | `xmsReallocBlock` | 重分配扩展内存块 | 未完成：部分机械 seam；完整 XMS family 未实施 | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/xms.486/xmsdisp.c；对应 owner package` |

## 4. DPMI（25）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `53:00` | `DpmiSetDescriptorEntry` | 设置 LDT/描述符项 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:01` | `switch_to_protected_mode` | 切换到保护模式 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:02` | `DpmiSetProtectedmodeInterrupt` | 设置保护模式中断处理器 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:03` | `DpmiGetFastBopEntry` | 取得快速 BOP 入口 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:04` | `DpmiInitDosx` | 初始化 DOSX/DPMI | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:05` | `DpmiInitApp` | 初始化 DPMI 应用 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:06` | `DpmiXlatInt21Call` | 转换 INT 21h 调用 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:07` | `DpmiAllocateXmem` | 分配 DPMI 扩展内存 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:08` | `DpmiFreeXmem` | 释放 DPMI 扩展内存 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:09` | `DpmiReallocateXmem` | 重分配 DPMI 扩展内存 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:0A` | `DpmiSetFaultHandler` | 设置异常处理器 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:0B` | `DpmiGetMemoryInfo` | 取得 DPMI 内存信息 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:0C` | `DpmiDpmiInUse` | 进入 DPMI 使用状态 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:0D` | `DpmiDpmiNoLongerInUse` | 退出 DPMI 使用状态 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:0E` | `DpmiSetDebugRegisters` | 设置调试寄存器 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:0F` | `DpmiPassTableAddress` | 传递表地址 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:10` | `DpmiFreeAppXmem` | 释放应用扩展内存 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:11` | `DpmiPassPmStackInfo` | 传递保护模式栈信息 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:12` | `DpmiVcdPmSvcCall32` | 32 位 VCD 服务调用 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:13` | `DpmiFreeAllXmem` | 释放全部扩展内存 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:14` | `DpmiIntHandlerIret16` | 16 位中断 IRET 返回 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:15` | `DpmiIntHandlerIret32` | 32 位中断 IRET 返回 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:16` | `DpmiFaultHandlerIret16` | 16 位异常 IRET 返回 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:17` | `DpmiFaultHandlerIret32` | 32 位异常 IRET 返回 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |
| `53:18` | `DpmiUnhandledExceptionHandler` | 处理未处理的 DPMI 异常 | 未完成：保护模式 composition deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/dpmi32/dpmi32.c；对应 owner package` |

## 5. Redirector（50）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `57:00` | `SVC_RDRINITIALIZE` | Redirector/网络 IPC：INITIALIZE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:01` | `SVC_RDRUNINITIALIZE` | Redirector/网络 IPC：UNINITIALIZE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:02` | `SVC_RDRQNMPIPEINFO` | Redirector/网络 IPC：QNMPIPEINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:03` | `SVC_RDRQNMPHANDSTATE` | Redirector/网络 IPC：QNMPHANDSTATE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:04` | `SVC_RDRSETNMPHANDSTATE` | Redirector/网络 IPC：SETNMPHANDSTATE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:05` | `SVC_RDRPEEKNMPIPE` | Redirector/网络 IPC：PEEKNMPIPE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:06` | `SVC_RDRTRANSACTNMPIPE` | Redirector/网络 IPC：TRANSACTNMPIPE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:07` | `SVC_RDRCALLNMPIPE` | Redirector/网络 IPC：CALLNMPIPE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:08` | `SVC_RDRWAITNMPIPE` | Redirector/网络 IPC：WAITNMPIPE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:09` | `SVC_RDRDELETEMAILSLOT` | Redirector/网络 IPC：DELETEMAILSLOT | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:0A` | `SVC_RDRGETMAILSLOTINFO` | Redirector/网络 IPC：GETMAILSLOTINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:0B` | `SVC_RDRMAKEMAILSLOT` | Redirector/网络 IPC：MAKEMAILSLOT | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:0C` | `SVC_RDRPEEKMAILSLOT` | Redirector/网络 IPC：PEEKMAILSLOT | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:0D` | `SVC_RDRREADMAILSLOT` | Redirector/网络 IPC：READMAILSLOT | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:0E` | `SVC_RDRWRITEMAILSLOT` | Redirector/网络 IPC：WRITEMAILSLOT | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:0F` | `SVC_RDRTERMINATE` | Redirector/网络 IPC：TERMINATE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:10` | `SVC_RDRTRANSACTAPI` | Redirector/网络 IPC：TRANSACTAPI | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:11` | `SVC_RDRIREMOTEAPI` | Redirector/网络 IPC：IREMOTEAPI | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:12` | `SVC_RDRNULLTRANSACTAPI` | Redirector/网络 IPC：NULLTRANSACTAPI | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:13` | `SVC_RDRSERVERENUM` | Redirector/网络 IPC：SERVERENUM | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:14` | `SVC_RDRUSEADD` | Redirector/网络 IPC：USEADD | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:15` | `SVC_RDRUSEDEL` | Redirector/网络 IPC：USEDEL | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:16` | `SVC_RDRUSEENUM` | Redirector/网络 IPC：USEENUM | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:17` | `SVC_RDRUSEGETINFO` | Redirector/网络 IPC：USEGETINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:18` | `SVC_RDRWKSTAGETINFO` | Redirector/网络 IPC：WKSTAGETINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:19` | `SVC_RDRWKSTASETINFO` | Redirector/网络 IPC：WKSTASETINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:1A` | `SVC_RDRMESSAGEBUFFERSEND` | Redirector/网络 IPC：MESSAGEBUFFERSEND | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:1B` | `SVC_RDRGETCDNAMES` | Redirector/网络 IPC：GETCDNAMES | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:1C` | `SVC_RDRGETCOMPUTERNAME` | Redirector/网络 IPC：GETCOMPUTERNAME | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:1D` | `SVC_RDRGETUSERNAME` | Redirector/网络 IPC：GETUSERNAME | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:1E` | `SVC_RDRGETDOMAINNAME` | Redirector/网络 IPC：GETDOMAINNAME | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:1F` | `SVC_RDRGETLOGONSERVER` | Redirector/网络 IPC：GETLOGONSERVER | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:20` | `SVC_RDRHANDLEGETINFO` | Redirector/网络 IPC：HANDLEGETINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:21` | `SVC_RDRHANDLESETINFO` | Redirector/网络 IPC：HANDLESETINFO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:22` | `SVC_RDRGETDCNAME` | Redirector/网络 IPC：GETDCNAME | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:23` | `SVC_RDRREADASYNCNMPIPE` | Redirector/网络 IPC：READASYNCNMPIPE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:24` | `SVC_RDRWRITEASYNCNMPIPE` | Redirector/网络 IPC：WRITEASYNCNMPIPE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:25` | `SVC_NETBIOS5C` | NetBIOS 网络服务：NETBIOS5C | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:26` | `SVC_NETBIOS5CINTERRUPT` | NetBIOS 网络服务：NETBIOS5CINTERRUPT | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:27` | `SVC_DLC_5C` | DLC 网络链路服务：DLC_5C | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:28` | `SVC_VDM_WINDOW_INIT` | 初始化 Redirector 的 VDM 窗口关联 | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:29` | `SVC_RDRRETURN_MODE` | Redirector/网络 IPC：RETURN_MODE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:2A` | `SVC_RDRSET_MODE` | Redirector/网络 IPC：SET_MODE | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:2B` | `SVC_RDRGET_ASG_LIST` | Redirector/网络 IPC：GET_ASG_LIST | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:2C` | `SVC_RDRDEFINE_MACRO` | Redirector/网络 IPC：DEFINE_MACRO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:2D` | `SVC_RDRBREAK_MACRO` | Redirector/网络 IPC：BREAK_MACRO | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:2E` | `SVC_RDRSERVICECONTROL` | Redirector/网络 IPC：SERVICECONTROL | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:2F` | `SVC_RDRINTACK` | Redirector/网络 IPC：INTACK | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:30` | `SVC_RDRINTACK2` | Redirector/网络 IPC：INTACK2 | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |
| `57:31` | `SVC_NETBIOSCHECK` | NetBIOS 网络服务：NETBIOSCHECK | 未完成：当前为 source-derived unavailable terminal | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/rdrsvc.h；对应 owner package` |

## 6. WOW16 host composition（1）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `51` | `BOP_WOW` | WOW16 host-composition 入口 | 未完成：WOW host composition absent/deferred | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |

## 7. Debugger（16）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `56:00` | `DBG_SEGLOAD` | 调试器：段装入通知 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:01` | `DBG_SEGMOVE` | 调试器：段移动通知 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:02` | `DBG_SEGFREE` | 调试器：段释放通知 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:03` | `DBG_MODLOAD` | 调试器：模块装入通知 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:04` | `DBG_MODFREE` | 调试器：模块释放通知 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:05` | `DBG_SINGLESTEP` | 调试器：单步事件 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:06` | `DBG_BREAK` | 调试器：断点事件 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:07` | `DBG_GPFAULT` | 调试器：一般保护异常 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:08` | `DBG_DIVOVERFLOW` | 调试器：除法溢出 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:09` | `DBG_INSTRFAULT` | 调试器：指令异常 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:0A` | `DBG_TASKSTART` | 调试器：任务开始 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:0B` | `DBG_TASKSTOP` | 调试器：任务停止 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:0C` | `DBG_DLLSTART` | 调试器：DLL 开始 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:0D` | `DBG_DLLSTOP` | 调试器：DLL 停止 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:0E` | `DBG_ATTACH` | 调试器：附加 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |
| `56:0F` | `DBG_TOOLHELP` | 调试器：Toolhelp 服务 | 未完成：当前为 deferred diagnostic stop | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/dbgsvc.h；对应 owner package` |

## 8. Top-level / machine / BIOS（9）

| 入口 | 原始 handler | 作用（高层） | 当前实际状态 | Direct | Readonly | Overlay | 原始 source / 下一步 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `59` | `BOP_NOSUPPORT` | 不支持服务终端 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `5A` | `BOP_WAITIFIDLE` | VDM 空闲等待 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `5B` | `BOP_DBGBREAKPOINT` | 调试断点 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `5C` | `BOP_KBD` | 键盘服务 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `5D` | `BOP_VIDEO` | 视频服务 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `5E` | `BOP_NOTIFICATION` | 宿主通知 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `5F` | `BOP_UNIMPINT` | 未实现中断处理 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `FD` | `BOP_SWITCHTOREALMODE` | 切回实模式 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |
| `FE` | `BOP_UNSIMULATE` | 退出/停止模拟 | 未完成：machine/top-level 仅有受限 stop/resume placeholder | 未实现/未完成 package | 未实现/未完成 package | ABI 预留，未启用 | `src/opennt/base/mvdm/inc/bop.h；对应 owner package` |

## 汇总

| Family | 条目 | 当前“完成”数 | 说明 |
| --- | ---: | ---: | --- |
| 1. DEM / DOS（73） | 73 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 2. COMMAND（17） | 17 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 3. XMS（12） | 12 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 4. DPMI（25） | 25 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 5. Redirector（50） | 50 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 6. WOW16 host composition（1） | 1 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 7. Debugger（16） | 16 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| 8. Top-level / machine / BIOS（9） | 9 | 0 | 严格完成数；7 个 DEM 原始 no-op 仅为局部已验证。 |
| **总计** | **203** | **0** | 不将 mapped/deferred/unavailable/local fixture 计为完成。 |
