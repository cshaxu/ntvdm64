# S5 Sysinit Entry Static Trace

状态：2026-08-09。本文是固定、由本仓库 OpenNT 源码重建的 `NTIO.SYS`
做出的静态入口采证。它只描述原始字节和源码归属；它不是 live trace、
不是新的 bootstrap，也不证明 DOS 已可运行。

## 固定输入

| 项目 | 值 |
| --- | --- |
| 输入 | `artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS` |
| 长度 | 33792 bytes |
| SHA-256 | `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` |
| `sysinit` map 地址 | `009D:02F9` |
| 文件线性偏移 | `0x0CC9` |
| 原始入口字节 | `E9 19 02` |
| 第一可执行目标 | 文件偏移 `0x0EE5` |

`sysinit1.asm:637` 的 `sysinit` 以跳转进入 `goinit`。针对固定输入的
`i8086` 反汇编确认该跳转为 `0x0CC9 -> 0x0EE5`；两处之间是 sysinit
数据，不是可顺序执行的指令流。

## 首段原始执行序列

`sysinit1.asm:818-855` 与固定 image 的 `0x0EE5` 起始字节一一对应：

```text
0x0EE5  FC                         cld
0x0EE6  C4 C4 12                   BOP 12h
0x0EE9  B1 06                      mov cl, 6
0x0EEB  D3 E0                      shl ax, cl
0x0EED  8B C8                      mov cx, ax
0x0EEF  49                         dec cx
0x0EF0  2E 89 0E 23 03             mov cs:[memory_size], cx
0x0EF5  0E 1F                      push cs / pop ds
0x0EF7  33 F6                      xor si, si
0x0EF9  8B FE                      mov di, si
0x0EFB  B8 30 7A                   mov ax, 7A30h
0x0EFE  E8 93 06                   call off_to_para
...                                 reserve sysinit, DOS, and BIOS space
0x0F0F  8E C1                      mov es, cx
0x0F11  B9 30 7A                   mov cx, 7A30h
0x0F14  F3 A5                      rep movsw
0x0F18  06                         push es
0x0F19  B8 4E 05                   mov ax, 054Eh
0x0F1C  50 CB                      push ax / retf
```

这里的 `C4 C4 12` 是历史 BOP 编码，不应按通用 x86 的 `LES` 解码。源码
明确注释它替代 `INT 12h`，由历史 32-bit side 返回 conventional-memory
大小（KB）于 `AX`。随后代码左移六位得到 paragraph，减一作为 arena，写入
`memory_size`。因此该 BOP 的角色是 **NTIO bootstrap 的 BIOS-memory
服务**，不是 NTDOS selector、不是 `DemDispatch`，也不可用自写 DOS handler
替代。

## 可由此证实的 machine/monitor 要求

1. 原始 monitor 必须截获 BOP 的三个字节，并把 selector `0x12` 交给历史
   `BIOS[0x12]` owner；执行器不应把它当作 `LES`。
2. 成功返回时，`AX` 必须来自该历史服务；`goinit` 随即消费它。静态 profile
   写一个内存顶端数值不能等价替代这次调用。
3. 入口还实际依赖 `CLD`、16-bit `SHL r16,CL`、CS segment 写、段寄存器
   装载、near `CALL`、`REP MOVSW`、以及 real-mode `RETF` 的正确语义。
4. `RETF` 的目标是由原始代码刚压入的 relocated `ES:054E`。它不是对 map
   地址的直接跳转，也不是外壳可以预先指定的 prepared entry。

这些是历史 SoftPC/monitor 的恢复边界。它们并不授权在 reconstruction 中
伪造 BOP 返回、预设 `ES`、跳过复制，或借由一个裸 CPU fixture 进入后续
`sysin`。

## 立即阻断与下一项证据

原始 `bios.c` 将 `BIOS[0x12]` 绑定为 `memory_size()`；后者只从原始 SAS
地址 `MEMORY_VAR` 读取一个 word 并写入 `AX`。这条 BOP 是直接 BIOS table
分派，**不经过** `MS_bop_0`。`MS_bop_0` 专属 BOP selector `0`，它才读取
后继 service byte 并调用 `DemDispatch`；完整历史运行仍需要两类 owner，
但不能把它们错误地折叠成一条每次必经的调用链。

目前尚没有已恢复且可组合的原始 BOP dispatcher 加完整 `BIOS[]` composition。
因此，`0x0EE6` 是这条真实入口的第一个 runtime stop boundary。允许的下一步是：

1. 从原始 BOP dispatch/`bios.c`/`mem_size.c` 恢复 selector `0x12` 的
   owner、入参、出参和 IP 消费规则；
2. 确认它能在原始 `CCPU -> BIOS[0x12] -> memory_size` 路径中被调用；
3. 仅以只记录/断言的 instrumentation 捕获一次真实的返回状态。

在完成这些项之前，不得以手写 `BOP 12h` 返回值、替代 IVT/BDA、或手写
`memory_size` 推进 `sysinit`。而在进入 `sysin` 后，仍须由原始运行得到
临时 DOS 段 `T`、`current_dos_location` 和 DOSINIT 调用状态；其后续门槛
见 `S5-SYSINIT-TEMPORARY-DOS-STATE-GATE.md`。

## 证据来源

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:806-855`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:958-981`
- `src/opennt/base/mvdm/softpc.new/base/bios/bios.c` (`BIOS[0x12]`)
- `src/opennt/base/mvdm/softpc.new/base/bios/mem_size.c` (`memory_size`)
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c` (`MS_bop_0` 的独立
  selector-0/DEM 身份)
- 固定 `NTIO.SYS` 的 `i8086` 只读反汇编（`0x0CC9..0x0F1D`）
