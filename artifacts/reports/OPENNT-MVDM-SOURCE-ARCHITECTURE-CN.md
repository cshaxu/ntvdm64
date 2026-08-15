# OpenNT MVDM 源码结构分层说明

状态：架构说明与后续源码审计的目录导航，2026-08-09。

本文描述的是 OpenNT / NT4 的 MVDM（NTVDM）源码职责结构，不是
`ntdos64` 项目的实施分层，也不表示这些模块已在现代 x64 主机上完成
链接或运行恢复。

## 总览

```text
DOS / Win16 程序与系统镜像
        │
        ▼
DOS guest 层、WOW16 guest 层
        │
        ▼
SoftPC / CCPU 虚拟 PC 与软件 CPU 层
        │
        ▼
NTVDM host 服务、DEM、VDD、WOW32、NT 集成
        │
        ▼
NT4 用户态 / 内核态宿主设施
```

另有一条横向的 **composition（组装）** 关系：它不取代上述任一运行时层，
而是选择相应的 SoftPC、guest、bridge、资源和宿主模块，按具体产品配置
链接为 `ntvdm.exe` 或辅助模块。

## 模块分层

| 层 | OpenNT 典型目录/模块 | 主要职责 | 不应混同为 |
| --- | --- | --- | --- |
| DOS guest 上层 | `base/mvdm/dos/v86/` | `NTIO.SYS`、`NTDOS.SYS`、`COMMAND.COM`、DOS 工具、设备驱动、消息与 NLS 数据；保持 DOS guest 语义。 | NT 宿主服务或 PC 硬件模拟。 |
| WOW16 guest 上层 | `base/mvdm/wow16/` | Win16 runtime（如 `KERNEL`、`USER`、`GDI`）、工具和 `.DRV`。 | DOS `.SYS`/`.COM` payload，或完整的 WOW host bridge。 |
| 虚拟机器下层 | `base/mvdm/softpc.new/`，尤其 `base/` 子树 | SoftPC 的 CPU/CCPU、BIOS、内存、视频、磁盘、键鼠、系统支持等 PC 平台语义。 | DOS 本身；它是 DOS/WOW16 所依赖的机器层。 |
| guest 与 NT 的服务桥 | `base/mvdm/dos/dem/`、`base/mvdm/dos/command/` 等 | DEM、BOP/SVC 调度附近的 DOS 服务与宿主交接；把 guest 请求导向 NTDOS 或 NT host 所有者。 | 纯 guest 代码，或单纯 linker glue。 |
| WOW bridge | `base/mvdm/wow32/` 及配套 WOW host 组件 | Win16/WOW16 到 32 位 NT host 的 thunk、调度与 host 侧服务。 | `wow16/` 下的 Win16 DLL/driver 本身。 |
| Host 服务与设备接缝 | `base/mvdm/softpc.new/host/src/`、`base/mvdm/vdd/`、`vdmredir/`、XMS/DPMI/redirector 相关模块 | 文件、进程、控制台、窗口、设备、计时、VDD、重定向等 NT host 服务。 | guest DOS/Win16 产物。 |
| Composition / 产品壳 | `ieuvddex/`、`softpc.new/obj.vdm/`、`v86/scaffold/`、`sim16/` 和对应 makefile/link 配置 | 为特定 NTVDM 产品构成选择模块、资源、链接输入与启动配置。 | 独立的 CPU、DOS 或 BIOS 实现。 |

## 关键责任链

第一条启动/服务责任链可抽象为：

```text
CCPU / SoftPC monitor
  -> BIOS[]
  -> MS_bop_0
  -> DemDispatch
  -> DEM
  -> NTDOS 与 NT host 服务
```

这不是所有调用的完整调用图，但它说明了边界：SoftPC 提供执行与机器语义；
BIOS/BOP 是历史服务门；DEM 是 guest 与 NT host 之间的重要桥；NTDOS 是
DOS 系统映像一侧的所有者之一。

## 对常见理解的校正

### 1. SoftPC 是“guest 的下层”吗？

可以这样理解，但需要补充：SoftPC/CCPU 是 guest 所见虚拟机器的下层，
同时含有大量会调用 NT host 的代码。因此它横跨 guest-machine 语义与
host integration 边界，并不是纯粹的 guest 二进制模块。

### 2. DEM 属于哪一层？

DEM 不是 DOS guest 上层，也不是单纯的 composition。它是 host-side、但
面向 guest 服务语义的桥接组件。将 DEM 误认为 DOS payload，会掩盖 DOS
请求最终仍需要 NT 宿主所有者的事实。

### 3. WOW16 与 WOW32 的区别

`wow16/` 是 Win16 guest runtime/driver 世界；`wow32/` 是 host 侧的 32 位
桥接与 thunk 世界。五个已具备 source-build 证据的 `.DRV` 仅属于前者的
一小部分，绝不表示 WOW16 runtime 或 WOW bridge 已闭包。

### 4. “侵入式 / 非侵入式” host 依赖

这不是 OpenNT 原始源码的固有模块分类。原始 NT4 MVDM 通常直接、紧耦合地
依赖其时代的 NT 用户态、子系统、VDD、控制台、窗口、文件和进程机制。
“非侵入式 host seam”是现代重建时用于约束新增适配代码的治理概念：只在
已抵达的历史调用者要求缺失宿主能力时，才允许最窄的现代适配，而不能用它
重写 CPU、BIOS、BOP、DEM、DOS 或设备语义。

## 与 source-closure 记录的关系

DOS/WOW16 构建证据只覆盖 guest 产物的来源与状态，不能越级证明其宿主链。
当前明确区分为：

- DOS payload：`NTIO.SYS`、`NTDOS.SYS`、`COMMAND.COM` 与设备/工具；
- WOW16 dependency：driver、Win16 runtime 与工具；
- VDM host/support：SoftPC、CCPU、BIOS provider、DEM、DPMI、redirector、
  `ntvdm` composition 等；
- sample/development-only：VDD sample、测试和 build-time 工具。

请结合 `docs/DOS-WOW16-SOURCE-CLOSURE-RESULT.md` 与
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 阅读：前者是构建闭包证据，
后者约束现代重建时哪些历史边界可以恢复、哪些 host seam 可以被最小化适配。
