# S5 cmdSetInfo Historical Storage Closure

状态：2026-08-09。本记录只覆盖固定 NT4 `DosInit` 的
`C4 C4 54 05` 边界；它不是 COMMAND.COM 启动、软盘访问或 DOS runtime 的证明。

## 原始调用与数据所有者

NTDOS 通过 `BOP_CMD = 54h` 发出 service `05h`。实际链保持为：

```text
BIOS[54h] -> MS_bop_4 -> 完整 CmdDispatch[05h] -> cmdmisc.c::cmdSetInfo
```

`cmdSetInfo` 只将三个既有 guest 地址保存给历史侧：

```text
DS:DX -> pSCSInfo / pSCS_ToSync
DS:BX -> pIsDosBinary
DS:CX -> pFDAccess
```

第一次闭包尝试在写入 `pFDAccess` 时于模块 RVA `0001D289` 失败。这不是应以本地
shim 填补的 SoftPC 缺失语义。原始 owner 在
`base/mvdm/softpc.new/host/src/nt_fdisk.c:233`：

```c
WORD *pFDAccess = 0;
```

该 source unit 同时是 `nt_rflop.c` 的原始共享 owner；后两者只在真实固定盘或软盘
open/close 路径才解引用该指针。fixture 仅为保持原所有权而链接该 object group，未调用
任何 disk/floppy host operation，也没有选择、禁用或模拟 A:/B:、镜像、FDC/HDC 或介质策略。

## 已执行的有界采证

默认关闭的 i386 target
`ntdos64-opennt-original-cmdsetinfo-trace-fixture` 设置最小寄存器输入：

```text
CS:IP = 0070:0478, service byte at physical 00000B78 = 05
DS = 0100, DX = 0010, BX = 0100, CX = 0120
```

它通过原始完整 COMMAND dispatch table 返回：

```text
fixture-exit=0
cmdsetinfo fixture: ip=0479 scs=00001010 sync=000010DE binary=00001100 fdaccess=00001120
```

这些值是 NT4 x86 `softpc.h` 的原始 `GetVDMAddr(seg, off)` 线性 guest 值
`((ULONG)seg << 4) + off`，不是 host RAM pointer。`IP` 从 `0478` 推进到 `0479`，说明
service byte 仍由原始 `MS_bop_4` 消费。

## 约束与结论

1. fixture 链接完整的 `CmdDispatch`，不得缩减为只含 `05h` 的本地 service table。
2. `pFDAccess` 的 storage 必须由原始 `nt_fdisk.c` 保持；不得新增同名本地变量、替代
   floppy handler 或以指针赋值宣称 disk support 已恢复。
3. 此结果只证明 `54h/05h` 的 guest-address retention 以及历史 object ownership；它不
   准入 COMMAND、DEM、BIOS、DOS、设备或 host-policy runtime。
4. `50h/46h demGetDPBList` 仍须以与 `50h/0Fh demGetDrives` 相同的 DEM session 为前提，
   由原始 DEM 写入 guest DPB；不得从本 fixture 派生 synthetic DPB 或 drive policy。
