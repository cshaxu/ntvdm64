# S5 Host Seam Admission Snapshot

状态：2026-08-09，基于已到达原始 caller 的只读汇总。本文不是 host shim backlog；它只区分已证实
owner、可继续采证的 seam 与当前必须拒绝的路径。

## 结论先行

当前高 ROI 不在于把所有历史 Win32 dependency 接上，而在于保持三种边界不混淆：

1. **历史机器状态**：BDA、CMOS、SAS、BIOS handler 的输入。它们不是 filesystem/console shim。
2. **contained resource**：已由 BYOB identity 选中的 `NTDOS.SYS` 等受限资源，可在已证明的原始 caller
   处逐项以同一身份 handle 提供。
3. **ambient product integration**：历史安装目录、用户 profile、临时文件、进程/console 全局状态。这类
   不能因为 caller 已到达就自动准入；应先设计显式、session-local policy，或拒绝并记录 stop。

## 当前分类表

| 原始到达点 | 原始职责与输入 | 分类 | 当前处理 | 证据 |
| --- | --- | --- | --- | --- |
| `BIOS[12h] -> memory_size` | BDA `MEMORY_VAR` -> AX | 历史机器状态 | source audit；不得预写 AX 或自写 selector | `S5-SYSINIT-MEMORY-BOP-BOUNDARY.md` |
| `BIOS[15h] -> cassette_io(AH=88h)` | CMOS extended-memory bytes -> AX | 历史机器状态 | source audit；完整 BIOS/reset closure 前不得单项 runtime 替代 | `S5-SYSINIT-MEMORY-BOP-BOUNDARY.md` |
| `50h/11h -> demLoadDos` | 已选 BYOB `NTDOS.SYS`，live `DI:0` load address | contained resource | 已有 default-disabled 原始链 fixture；仅 descriptor-bound identity read | `S5-ORIGINAL-DEMLOADDOS-CLOSURE.md` |
| `50h/0Fh -> demGetDrives` | `GetDriveTypeOem`、`GetPhysicalDriveType`、DEM drive cache | host media policy | 已有 caller-first trace；不从 trace 提升为 drive policy | `S5-DEMGETDRIVES-DYNAMIC-HOST-SEAMS.md` |
| `54h/0Ch -> cmdGetConfigSys` | historical Windows CONFIG/profile/temp-file projection，写 `DS:DX` | ambient product integration | 拒绝直接运行；先有 session-local projection contract | `S5-SYSINIT-CONFIG-HOST-SEAM.md` |
| `cmdexec` 的 native image 分支 | `CreateProcess`、console/process coordination | ambient product integration | 与 DOS `COMMAND.COM /C` 分开，继续 deferred | `HISTORICAL-COMMAND-OBJECT-BOUNDARY-AUDIT.md` |

## 准入规则

一个现代实现只有在以下全部成立时才可能从“研究记录”进入专门的 default-disabled fixture：

1. 能定位一个**已实际到达的原始 caller**，并说明其输入/输出 ownership；
2. 能表明其替代的是失效的 host/context/resource dependency，而不是 DOS SVC、BOP dispatch 或 PC/AT
   handler 本身；
3. 未成功时能够停止并记录 redacted diagnostics，且不会让 DOS 前进；
4. 不读取 ambient `%WINDIR%`、registry、`PATH`、当前工作目录或用户文件来猜测 guest policy；
5. 不把 fixture 的静态成功提升为完整 `BIOS[]`、SoftPC monitor 或 DOS runtime 成功。

当前只有 `demLoadDos` 的 descriptor-bound resource read 具备受控 fixture 证据。它的成功说明原始
service 的资源读取可被约束，不说明机器启动、NTIO 执行、COMMAND、native image handoff 或任意 device
已经可用。

## 后续优先级

1. 继续完成 historical `12h/15h` 所依赖的 reset/BDA/CMOS state 的静态 owner closure；不写单项 emulator。
2. 对 `demGetDrives` 只记录 host-drive/image-backed/disabled policy 所需的真实观察，避免固定屏蔽 A:/B:。
3. 先把 CONFIG projection 设计成独立 contract，并做正反拒绝 fixture；它不能借用历史 Windows 安装目录。
4. 直到原始 command path 的 contained namespace owner 清楚前，不启动 native Win32 handoff 的实现。
