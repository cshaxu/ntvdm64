# S5 NTIO Entry Instruction Ledger

状态：首段二进制/源码交叉采证完成，2026-08-09。

## 固定输入

本记录使用本仓库工具链生成的研究输入：

```text
artifacts/toolchain-runs/ntio-tools16-opennt-v1/
  base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS
```

该文件长度为 33792 字节。`ntio.map` 显示：`BIOS_DATA` 占 `0000:0000-036F`，
`init` 位于 `0000:0370`。入口 `0070:0000` 的 `E9 6D 03` 正是跳转至该同段偏移。
源级对应为 `msbio1.asm:38-40` 的 `jmp init` 与 `msinit.asm:92-254` 的 `init`。

这是本仓库构建出的研究产物，不是外部用户 BYOB 文件的兼容性宣称。

## 首段真实语义

`init` 不是 BOP 入口。其开头先展开 `FCLI`，访问 `0040:0314`，即固定
`ntvdmstate` 的 physical `0x714`；随后开始保存/替换 ROM interrupt vectors。
源级顺序如下：

```text
FCLI
xor ax, ax; mov ds, ax
push cs; pop es
mov cx, NUMROMVECTORS
mov si, offset RomVectors
next_int:
  lods cs:[si]
  cbw; shl ax, 1; shl ax, 1
  mov di, ax; xchg si, di
  lodsw; stosw; lodsw; stosw
  xchg si, di; loop next_int
```

锚点是 `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm:96-128`。生成二进制中
`0x370` 开始的第一段也证实 `FCLI` 展开为 virtual-interrupt state 的检查/更新，而不是
单个裸 `CLI`。`vint.h:36-53` 与 `msbio1.asm:55-59` 共同证明该 state 的地址/所有权。

## 当前受控执行证据

`tests/reconstructed_monitor_s4_test.c` 在 ordinary RAM 中复现从研究构建产物读取的两个
映像片段：`0070:0000` 的入口 `E9 6D 03`，以及 `0070:0370` 开始直到第二个 virtual-interrupt
macro 之后的首段。
它既不是 NTIO loader，也不执行完整文件。

当前独立 monitor 只验证以下严格有界的 byte route：

```text
0070:0000 E9 6D 03  ->  0070:0370
FCLI normal path: TEST [0040:0314],2400h; JNZ-not-taken;
LOCK AND [0040:0314],FDFFh; restore saved flags/AX/DS
xor ax,ax; mov ds,ax; push cs/pop es; mov cx,4; mov si,00EA
RomVectors loop: CS:LODSB; CBW; SHL AX,1; SHL AX,1; MOV/XCHG;
LODSW/STOSW; LODSW/STOSW; XCHG; LOOP
set INT 15h/19h IVT entries; set SS:SP=0000:0700
FSTI normal path: TEST pending/MIPS/RI; JNZ-not-taken;
LOCK OR [0040:0314],0200h; restore saved flags/AX/DS
initialize remaining guest IVT/data pointers; inspect keyboard flag; derive DOS data segment
-> 0070:0475 C4 C4 50, service byte 11h (typed BRIDGE_REFUSED)
```

fixture 将 fixed state word 初始化为 `0x0200`，并断言 `LOCK AND` 后 physical `0x714` 归零；
`TEST` 后 `JNZ` 按 ZF 不跳转；随后 FSTI 将该 state word 恢复为 `0x0200`。FSTI 的 pending
和 MIPS/RI 分支尚未执行，因而也未将其裸 `STI` fallback 视为可用。该 fixture 从实际 NTIO 映像
取得 `RomVectors` 的四个 interrupt number（`10h`、`15h`、
`19h`、`1Bh`）；其 IVT offset/segment 内容是受控 ordinary-RAM fixture 值，用来证明四组 word
copy 的数据流，而不是声称捕获了原始 BIOS 的实际 IVT。此证据证明上述受限 CPU semantics、
fixed-state guest-memory ABI、ROM-vector save loop、INT 15h/19h guest-IVT 写入与 entry-to-init
control flow；并证明进入 `SVC_DEMLOADDOS` 前的 guest CPU 初始化可到达原始 BOP encoding。该 BOP
的 selector 是 `50h`，service byte 是 `11h`，monitor 已将 IP 推进至该 service byte 后停止。它不
调用任何自写 service：当前 `BRIDGE_REFUSED` 正是“尚未组合原始 `MS_bop_0 -> DemDispatch -> DEM`”
的有意边界。不证明 `FSTI` 的异常分支、`SVC_DEMLOADDOS` 本体、BIOS、DEM、NTDOS 或 DOS program
能运行。

## S5.2 monitor 扩展合同

下列能力由真实入口直接要求，应按真实 x86 real-mode 语义增加到
`src/reconstruction/monitor/`，并各自有 ordinary-RAM positive test 与越界 fail-closed test：

| 类别 | 首个来源 | S5 允许范围 |
| --- | --- | --- |
| 16-bit register/segment transfer | `push/pop`、`mov`、`xchg` | 仅 real-mode visible register state；禁止隐式 mode transition。 |
| checked stack and data memory | `push`、`lods`、`stosw`、`test/and [disp16]` | 所有读写走同一 segment-to-physical ordinary-RAM 路由；失败不提交部分指令状态。 |
| flags | `FCLI`、`lahf/sahf`、`test`、`jcc`、`cli/sti` | 只实现有原始入口调用者的 flags 位；不存在虚拟硬件/IRQ 自动成功。 |
| control flow | `jmp`、`jcc`、`loop` | 使用 16-bit IP wrap 和已验证 target fetch；未知/越界 target typed stop。 |
| string data movement | `lods/stosw` 的 CS/DS/ES 使用 | 只从真实 source anchor 扩展；不把它扩展成通用 BIOS/DOS memory API。 |

`SVC SVC_DEMLOADDOS` 位于 `msinit.asm:247-252`，在上述初始化和 vector 保存之后。
因此 S5.2 不实现、也不调用该服务；直到 CPU prefix 可重放到该位置并取得独立 caller/ABI
证据前，SVC 仍是 typed stop。

## 禁止误读

- 不能把 `FCLI` 或 state-word 更新降格为 host 的全局布尔变量；它是 guest memory ABI。
- 不能因 guest 将写入 IVT 而复活宿主侧 synthetic IVT/BDA scaffold；这些是 NTIO 自己的
  guest 指令行为，必须经 checked memory access 执行。
- 不能把新 decoder 设计成任意 opcode override 接口。BOP 的历史 encoding 仍按 S3/S4
  独立处理；普通 opcode 由 monitor 的正式 CPU 语义执行。
- 任何 S5.2 subset 测试通过，只证明对应 byte stream 的 CPU mechanics，不证明 NTIO、
  NTDOS 或 DOS program 已可运行。
