# S5 NTDOS First Post-Load SVC

状态：2026-08-09。该记录为 S5 no-SM0 historical-chain 恢复，重新从已由历史 16-bit
toolchain 构建的 NT4 NTDOS 映像采证其首次 post-load host 需求。它独立复核旧 M18 的
静态事实，并将其接入当前的 source-owner closure；不取代或激活旧 mantle/contained-
namespace 设计，更不授权新建 DEM、BIOS、DOS 或 drive-policy 替代实现。

## 固定 guest 输入

输入为：

```text
artifacts/toolchain-runs/ntdos-tools16-historical-v1/
  base/mvdm/dos/v86/doskrnl/dos/NTDOS.SYS
```

- 长度：27,858 bytes；
- SHA-256：`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`；
- 起始三字节：`E9 A9 51`，跳转到映像内偏移 `0x51AC`；
- 在偏移 `0x51E9` 有字节序列 `C4 C4 50 0F`。

`NTDOS.MAP` 是未 strip 的链接映像映射，不能用来把 `0x51E9` 直接等同于一个最终
`NTDOS.SYS` public 地址。这里的归因不是靠地址猜测，而是靠固定的服务字节和源中唯一的
`SVC_DEMGETDRIVES` 使用点进行交叉验证。

## 源级对应

`base/mvdm/inc/DOSSVC.INC` 定义：

```asm
SVC_DEMGETDRIVES EQU 0FH
```

在 `base/mvdm/dos/v86/doskrnl/dos/msinit.asm` 的 `DosInit` 初始化序列中，DOS data
移动完成并读取 BIOS communication block 的 debug flag 后立即执行：

```asm
SVC SVC_DEMGETDRIVES
mov [numio], al
```

因此第一个已证实的 NTDOS post-load service 路径是：

```text
guest C4 C4 50 0F
  -> historical BIOS[50h]
  -> MS_bop_0
  -> DemDispatch(0x0f)
  -> demGetDrives
  -> original OEM / NT drive discovery
  -> AL = nDrives, CF = 0
  -> guest stores AL into numio
```

这条路径要求 monitor 按历史约定消费 `C4 C4 50`，使 `MS_bop_0` 从 CS:IP 读取 `0F` 并
仅推进 service-byte IP；它不是“外壳直接调 DEM”或“预置 numio”的许可。

## `demGetDrives` 的实际职责

原始定义在 `base/mvdm/dos/dem/demgset.c:351`。它不是固定返回盘符数：

1. 对 A:、B: 先调用 `GetDriveTypeOem`；remote/no-root 会归类为 unknown，fixed 会再做
   physical-drive 判定；
2. 对 C: 到 Z: 调用 `GetPhysicalDriveType`；连续可见的 removable/fixed/CD-ROM/RAM disk
   才计入 `nDrives`；
3. 将 `PhysicalDriveTypes[]`、`IsAPresent`、`IsBPresent` 和 `nDrives` 保留为随后 DEM、
   COMMAND 与设备路径共享的历史状态；
4. 以 `setAX(nDrives)`、`setCF(0)` 返回，NTDOS 只读取 AL。

`GetDriveTypeOem` 的原始 owner 是
`base/mvdm/oemuni/file.c:756`，它通过 OEM-to-Unicode 转换后调用 `GetDriveTypeW`。
`GetPhysicalDriveType` 则留在 `demgset.c`，使用原始 `RtlOemStringToUnicodeString`、
`RtlDosPathNameToNtPathName_U`、`NtOpenFile`、`NtQueryVolumeInformationFile`、`NtClose`
与 RTL heap 清理来排除 SUBST/remote 并识别真实 device type。

这证实驱动器发现是 NTDOS 初始化的真实 host boundary，也说明将来若实际到达 modern seam，
它不能把宿主 A:/B: 一律屏蔽：不存在、不可访问或 profile 明确禁用才可视为 inactive；传统、
USB 与虚拟软驱都必须由同一可观察策略处理。该句是历史 caller 的行为约束，不是在此阶段
创建新的 drive policy。

## 紧随其后的已知服务顺序

在同一 `DosInit` 路径中：

- 仅 debug 分支：`SVC_DEMSYSTEMSYMBOLOP` (`45h`)；
- 无条件：`SVC_DEMSETDTALOCATION` (`1Bh`)；
- 无条件：`SVC_DEMSETHARDERRORINFO` (`32h`)；
- 无条件、但非 DEM selector：`CMDSVC SVC_CMDSETINFO`；
- 后续初始化继续请求 `SVC_DEMGETDPBLIST` (`46h`)。

它们是后继的 guest-trace checkpoints，不因本记录而提前进入实现或 shim 队列。

## 当前可重建输入与限制

已验证 compile-only 的固定历史输入：

| target | 结果 | 日志 |
| --- | --- | --- |
| `ntdos64-opennt-dem-objects` | i386 object build exit 0 | `s5-ntdos-first-svc-dem-compile-20260809.log` |
| `ntdos64-opennt-oemuni-provider-objects` | i386 object build exit 0 | `s5-ntdos-first-svc-oemuni-compile-20260809.log` |

两项均不链接、不会执行 guest，也没有把 `GetDriveTypeOem` 变成局部 shim。完整 `DemDispatch`
表和 BIOS[] 的静态链接闭包仍未完成；因此本记录证明的是“下一条真实职责与所需原始 owner”，
不是 `0Fh` 已可在 runtime 成功执行。

## 下一步

将 no-SM0 historical closure 的恢复顺序从泛化的“补齐所有设备”收敛为：先让原始
`demGetDrives` 在完整 dispatcher 表的干净闭包中可调用；若最终只剩 modern Windows 缺失的
NT private API，才为这些**已被真实 NTDOS 指令抵达**的函数边界设计 fail-closed modern
facade。不得预置 `numio`、篡改 `PhysicalDriveTypes[]`，或将 `0Fh` 改为自写 service handler。
