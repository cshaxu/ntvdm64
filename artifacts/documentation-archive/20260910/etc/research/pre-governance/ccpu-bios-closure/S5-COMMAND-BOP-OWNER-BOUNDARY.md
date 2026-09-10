# S5 COMMAND BOP Owner Boundary

状态：2026-08-09。本文记录固定 NT4 输入中 `DosInit` 的 `C4 C4 54 05`
调用所经过的原始 COMMAND owner 链，以及它对后续 software-monitor 和 host
工作的实际约束。它不是 COMMAND runtime 准入，也不授权新增 COMMAND、BOP、BIOS
或 DOS service 实现。

## 已证实的原始链

`base/mvdm/dos/v86/doskrnl/dos/msinit.asm` 在 `DosInit` 中设置：

```text
DS:DX = dosdata:SCS_COMSPEC       (SCSINFO)
DS:BX = dosdata:SCS_Is_Dos_Binary
DS:CX = dosdata:SCS_FDACCESS
CMDSVC SVC_CMDSETINFO             (C4 C4 54 05)
```

其中 `CMDSVC` 由 `base/mvdm/inc/CMDSVC.INC` 定义为 `BOP_CMD = 54h` 加一个
service byte。原始 `base/mvdm/softpc.new/host/src/nt_bop.c::MS_bop_4`：

1. 从当前 real-mode `CS:IP` 的 SAS 地址读取该 byte；
2. 调用 `CmdDispatch((ULONG)Command)`；
3. 将 `IP` 增加一。

`base/mvdm/softpc.new/base/bios/bios.c` 的原始完整 `BIOS[]` 在 slot `54h`
放置的正是 `MS_bop_4`。因此 software monitor 必须以通用的 BOP fetch/dispatch
语义将 selector `54h` 交给该历史 entry；不得由现代 shim 解码 `05h`，也不得
绕过 `MS_bop_4` 直接调用 `cmdSetInfo`。

## Service 05 的真实职责

`base/mvdm/dos/command/cmddisp.c::CmdDispatch` 的原始 `apfnSVCCmd[]` 表将
index `5` 映射到 `cmdSetInfo`。`base/mvdm/dos/command/cmdmisc.c::cmdSetInfo`
只完成下列三次原始 `GetVDMAddr`：

```text
DS:DX -> pSCSInfo
DS:BX -> pIsDosBinary
DS:CX -> pFDAccess
```

随后把 `pSCS_ToSync` 设为 `pSCSInfo->SCS_ToSync`。`SCSINFO` 的字段布局由
`base/mvdm/dos/command/cmd.h` 定义；该调用本身不读取 host filesystem、console、
clock、media 或 drive policy，也不写入 DOS guest 数据。

这项事实把它归为 **monitor guest-address route 的消费方**，而不是新的 host
capability seam。它要求的不是一个 `CMDSETINFO` shim，而是后续 monitor 为原始
`GetVDMAddr` 提供正确、受限的 real-mode segment:offset 到 guest-memory 路由。

## 不能缩表的原因

`CmdDispatch` 的原始编译单元静态初始化完整 17 项 `apfnSVCCmd[]`。即使动态 trace
当前只到 index `5`，该 object 仍对每项 COMMAND service 保留函数引用。Release
构建中该函数也没有 service-index 边界检查；因此 monitor 应保持 byte 获取和 IP
消费的历史语义，同时对未获准入的运行路径 fail-closed，而不能以本地 dispatcher
缩表、改写表项或伪造成功。

这意味着“COMMAND object group 可以编译”不等于“COMMAND subsystem 已可链接或运行”。
完整 COMMAND 运行将自然带来 original session、console、process、environment、
filesystem、redirector 和 event owners；它们只在 live trace 到达相应 caller 时才可
逐项进入 host-seam 审计。

## 重建证据

默认关闭 target `ntdos64-opennt-command-objects` 保持原始下列 11 个 NT4 source unit：

```text
cmd.c, cmddata.c, cmddisp.c, cmdexec.c, cmdexit.c, cmdmisc.c,
cmdpif.c, cmdredir.c, cmdconf.c, cmdkeyb.c, cmdenv.c
```

2026-08-09 在 i386 clang-cl OpenNT CMake island 运行：

```text
cmake --build artifacts/build/current/opennt-r2-bridge-ninja-i686 \
  --target ntdos64-opennt-command-objects --parallel 8
```

结果为成功（增量重建显示 `ninja: no work to do.`）；该 target 只生成 objects，既
不连接到 root runner，也不运行任何 guest 或 COMMAND code。对 `cmddisp.c.obj` 的
COFF undefined-symbol 检查仍显示全部 17 个 `cmd*` service；`cmdmisc.c.obj` 的
剩余输入包括原始 register accessor、`GetDriveTypeOem`、session/event 和 Win32/CRT
imports。这是后续 owner-first 审计的边界清单，不是实施待办的平铺列表。

## 当前结论与下一顺序

1. `54h/05h` 已成为固定 `DosInit` 路径的已知历史边，但不产生新的现代 host shim。
2. S5 的下一个实际 host seam 仍是先发生的 `50h/0Fh demGetDrives`；它必须与
   `50h/46h demGetDPBList` 作为一个 DEM session contract 审计。
3. monitor instruction/memory route 的扩展只能由真实 NTIO/NTDOS trace 的第一个
   stop 决定；不得为了让 `54h/05h` 独立运行而增加 synthetic COMMAND state。
4. 当 trace 真正越过 `cmdSetInfo` 并首次调用其他 COMMAND service 时，再按那个
   service 的原始 caller 和 owner 建立独立 seam record。
