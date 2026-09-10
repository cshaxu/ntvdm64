# S5 NTIO Strategy Execution Fixture

状态：2026-08-09。本记录是 `CON` device-chain 采证的可执行补充，不是 runtime
设计或 DOS 兼容性结论。

## 目标与边界

`ntdos64-reconstructed-monitor-s4-ntio-strategy-fixture` 是独立的
`EXCLUDE_FROM_ALL` 命令行目标。调用者必须明确传入 source-built `NTIO.SYS`；
fixture 拒绝长度不是 `33792` bytes 的输入，将完整文件原样放入 1 MiB RAM 的
`0070:0000`，并只从原始 `strategy` 地址 `0070:01EC` 执行。

它不链接 `ntdos64-run`，不注册 CTest，不配置 bridge，不含 BOP、DEM、DOS、
BIOS device 或 host handler。它也不创建 DOS request caller 或 synthetic
return address；唯一构造的是调用该 strategy 时 CPU 应已有的 `ES:BX` 输入状态。

## 固定输入与实际结果

输入为：

```text
artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS
33,792 bytes
SHA-256 CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937
```

2026-08-09 实际构建并运行 fixture。它在完整原始映像上执行：

```text
0070:01EC  2E 89 1E 18 00  mov cs:[0018], bx
0070:01F1  2E 8C 06 1A 00  mov cs:[001A], es
0070:01F6  CB              retf
```

传入 `ES:BX = 3456:1234` 后，原始 `ptrsav` 字节成为 `34 12 56 34`。
fixture 现有意只执行前两条 strategy 写入并停在 `0070:01F6` 的 `CB` 前；它不为
该 `retf` 制造返回帧，也不构造 device status、console 结果或 host 成功。

## 准入的唯一 CPU 语义

首次运行在 `CS: MOV [disp16],BX` 停止。S4 已有相邻
`CS: MOV [disp16],Sreg` 的受限路径，因此仅加入同样 direct-disp16 形式的
`CS: MOV [disp16],r16`：完整取指并验证 ModRM direct form 后才执行单次 checked
word write。它没有扩展为任意 segment prefix、任意 memory addressing 或任意
ModRM 解码。

原始 `retf` 已作为通用、受限 real-mode 语义准入。`DEVIOCALL2` 的
`CALL DWORD PTR [CALLDEVAD]` 是 caller 证据；`CB` 先完整读取 `SS:SP` 的 IP
和 `SS:SP+2` 的 CS，二者都可读后才一次性增加 SP 4 并提交 CS:IP。独立 mechanics
test 验证正常 far-return frame 与跨 RAM 边界故障时 CS:IP:SP 完全不变。整像
fixture 仍不自己设置这一个 frame，因而不能通过设置假的返回状态把 `strategy`
接到 `con_entry` 或跳过 NTDOS 的 `DEVIOCALL2`。

## 已准入的原始调用语义

`dev.asm:532-541` 证明 `DEVIOCALL2` 先将 device header 中的 strategy far pointer
写入 `CALLDEVAD`，再执行 `CALL DWORD PTR [CALLDEVAD]`；strategy 返回后才写入
interrupt pointer 并进行第二次 far call。因此 S4 仅加入实际编码 `FF 1E disp16`
的 `CALL m16:16`，并不接受其它 `FF` 组或任意地址形式。

执行时先读取完整的 offset:segment target，验证 target 与两个将写入的 stack words
均在 RAM 中；然后按 far-call 约定依次压入原 CS 与 return IP，使 `retf` 可从
`SS:SP` 先取 IP、后取 CS。任一预检失败都会返回 `MEMORY_FAULT`，CS:IP:SP 与
stack 内容保持不变。独立 mechanics test 覆盖正常 `FF /3 -> retf` 配对和 stack
underflow 的原子失败。

这只是原始 DOS caller 使用的 CPU 控制流语义。它没有加载 NTDOS、没有构造
`CALLDEVAD`、没有调用 strategy/interrupt，也没有写入 request status。

## 对后续工作的影响

下一项是 caller-first 地执行原始 DOS `DEVIOCALL2` 的 far indirect call，之后
才能继续实际 `con_entry -> cdev -> chardev_entry -> bc_exvec` 路径。该进展仍只是
SoftPC monitor mechanics 采证，不能被作为可运行 DOS、CON device 或现代 host shim
的证据。
