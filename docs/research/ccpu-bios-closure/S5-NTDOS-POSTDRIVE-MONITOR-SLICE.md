# S5 NTDOS Post-Drive Monitor Slice

## 32h Post-Service COMMAND Boundary

在历史 `50h/32h` 已完成后的连续 `DosInit` 路径中，固定字节为：

```text
BA FC 12                mov dx,12fc
BB CC 13                mov bx,13cc
B9 D0 13                mov cx,13d0
C4 C4 54 05             CMDSVC SVC_CMDSETINFO
```

source anchor 为 `msinit.asm:346-353`。该段只准备原始 `cmdSetInfo` 的三个 guest
地址输入，随后到达 `BIOS[54h] -> MS_bop_4 -> CmdDispatch(05h)`；service byte、
selector 和 IP consumption 均仍由 monitor 的通用 BOP decoder 和历史 bridge 负责。

同一 default-disabled S4 fixture 从 `32h` 已返回的 state 执行该 4-instruction slice，
确认 `DX=12fc`、`BX=13cc`、`CX=13d0`，随后在 `C4 C4 54 05` bridge refusal 处停止。
2026-08-09 测试 exit `0`。该证据不执行或替代 `cmdSetInfo`，也不把 COMMAND host
状态、disk backend 或 `pFDAccess` 引入 software monitor。

## 1Bh Post-Service Continuation

固定 `NTDOS.SYS` 在历史 `50h/1Bh` 已完成并把 IP 推进到 service byte 后，紧接着执行：

```text
5E                      pop si
BA F9 12                mov dx,12f9
53                      push bx
BB 48 00                mov bx,0048
C4 C4 50 32             SVC_DEMSETHARDERRORINFO
```

source anchor 为 `msinit.asm:327-343`。该段恢复前一段保存的 DOSWOW pointer，设置原始
hard-error caller 的 `DX/BX`，并到达 `50h/32h`；它没有读取 host I/O、设备或磁盘。

同一 default-disabled fixture 使用 `CS=1000h`/`DS=0` 分离 aperture，以真实 stack bytes
验证 `SI=9abc` 被 `POP` 恢复、`DX=12f9`、`BX=0048`，以及 `PUSH BX` 后 stack word 为
旧 `BX=4567`。monitor 在 `C4 C4 50 32` 消费 BOP instruction bytes 后收到 bridge
refusal 并停止。2026-08-09 测试 exit `0`。

这仍然不执行 `demSetDTALocation` 或 `demSetHardErrorInfo`。它只证明两次历史 service
之间的 ordinary CPU/stack mechanics 能保持原始 caller 所需的 state。

状态：2026-08-09。本记录只扩大 default-disabled S4 software-monitor 的普通 CPU
mechanics；它不执行 `demGetDrives`、`demSetDTALocation` 或任何 DOS runtime。

## 固定原始字节与来源

本仓库 Tools16 构建的 `NTDOS.SYS`（27,858 bytes，SHA-256
`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`）在
`DosInit` 的第一个 `C4 C4 50 0F` 后含下列 normal-startup bytes：

```text
A2 46 00                 mov [numio], al
F6 06 80 14 01           test byte ptr [SCS_ISDEBUG], 1
74 2E                    jz normal-path
...
52 B8 2E 03 BA 32 03     push dx; mov ax,032e; mov dx,0332
51 B9 25 03 56 BE 82 14  push cx; mov cx,0325; push si; mov si,1482
C4 C4 50 1B              SVC_DEMSETDTALOCATION
```

其 source anchor 为 `base/mvdm/dos/v86/doskrnl/dos/msinit.asm:292-353`：非 debug
分支保存由原 `demGetDrives` 返回的 `AL`，并为下一条原 `demSetDTALocation` caller
设置寄存器。BOP 的 service result 仍归历史链所有。

## 新增且受限的 CPU 语义

`reconstructed_monitor_s4.c` 现支持 `F6 /0` 的一个具体 real-mode form：
`TEST byte ptr [disp16], imm8`。实现顺序为：

1. 通过已有 `DS:disp16 -> segment -> linear -> ordinary-RAM` 路由读取字节；
2. 只按 TEST 更新 CF/PF/ZF/SF；
3. 消费全部 5 个 instruction bytes；
4. 任意其他 `F6` addressing mode 或 extension 仍 fail-closed。

它不向 guest 写测试结果，不解释 `SCS_ISDEBUG` 的业务含义，也不增加 host callback。

## 受控验证

`tests/reconstructed_monitor_s4_test.c` 把 code 放在 `CS=1000h`，把 DOS data 放在
`DS=0`。这保留了真实 `A2 0046` write，同时避免 fixture 自身把 code 与 data 放在同一
物理区域而掩盖真实内存语义。

测试从“原 `50h/0Fh` 已返回”的 state 开始，`AL=5` 且 `SCS_ISDEBUG=0`：

1. `A2` 写入 guest `numio`；
2. `TEST` 设 ZF，`JZ` 跳过 debug-only block；
3. 原始普通指令设置 `AX=032e`、`DX=0332`、`CX=0325`、`SI=1482`，并保留三次 stack push；
4. monitor fetch `C4 C4 50 1B`，提交历史 BOP IP 消费，bridge 拒绝后 typed stop。

验证命令：

```text
cmake --build artifacts/build/current/default-mingw-verify \
  --target ntdos64-reconstructed-monitor-s4-test --parallel 8
artifacts/build/current/default-mingw-verify/ntdos64-reconstructed-monitor-s4-test.exe
```

2026-08-09 结果为 exit `0`。该 target 保持 `EXCLUDE_FROM_ALL`，不进入
`ntdos64-run`、默认 CTest 或任何 runtime。

## 边界结论

这项结果说明：在历史 `0Fh` 真正完成之后，software monitor 已具备把该 guest 的普通
CPU state 推进到下一条历史 host boundary 的一小段机制。它不说明跨位宽 transport、
shared SAS、完整 `BIOS[]` closure、`1Bh` service、IVT/PSP 初始化或 DOS 启动已完成。
下一个实现增量仍必须由 transport 闭包或新的真实 post-BOP stop 决定，不能由此扩展
自写 DEM/BOP handler。

## 54h/05h 之后至 `charinit` 的初始化切片

`CMDSVC SVC_CMDSETINFO` 的历史执行、返回状态和 IP 消费不由本 monitor 伪造。本节从
“原始 `BIOS[54h] -> MS_bop_4 -> CmdDispatch(05h) -> cmdSetInfo` 已成功返回”的状态开始，
执行紧随其后的真实 guest bytes，直到原始 `call charinit` 已完成控制流转移为止；不会执行
`charinit` 的第一条指令。

固定 Tools16 source-built `NTDOS.SYS` 为 27,858 bytes，SHA-256 为
`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`。其中：

```text
file + 0x524d  C4 C4 54 05       CMDSVC SVC_CMDSETINFO
file + 0x5251  5B 59 5A ...      post-CMDSETINFO slice
```

源代码锚点为 `base/mvdm/dos/v86/doskrnl/dos/msinit.asm:353-453`。该切片恢复
`BX/CX/DX`，写入 BIOS communication block 与 DOS data/code pointers，建立 DOS stack，
初始化两个 Win386 instance-table 的 segment fields，写入 `initiret` IVT vector，最后通过
原始 near `CALL` 转移至 `charinit`。

为严格只覆盖这段真实字节，S4 新增的机制仅为：

1. `MOV r/m16,Sreg` 的 direct `disp16` form，以及本切片使用的 `DS:[SI]` form；
2. `CS:` override 下 `MOV direct-disp16,Sreg`；
3. `MOV direct-disp16,r16`；
4. register-direct `ADD r16,imm`；
5. 经 checked fetch、stack preflight 后原子提交的 near `CALL rel16`。

这些不是通用 BIOS、IVT 或 Win386 实现：所有未列出的 form 仍 typed fail-closed。特别是
monitor 不解释 BIOS communication block、instance table 或 IVT 的业务含义，也不运行
`charinit`、`cmdSetInfo`、COMMAND、DEM 或 host I/O。

`tests/reconstructed_monitor_s4_test.c` 使用 code `CS=1000h`、初始 DOS data `DS=0`、
真实三字 stack 输入及一个仅用于 checked target fetch 的 `charinit` NOP。它以 76 条 instruction
budget 执行该精确 slice，断言：near call 后 IP 为原始计算的 target、返回地址压入新 DOS stack，
以及真实 `BiosDataPtr` 两字不覆盖 code。2026-08-09 以
`ntdos64-reconstructed-monitor-s4-test` 退出 `0` 验证。

该 target 仍为 `EXCLUDE_FROM_ALL`，没有加入 `ntdos64-run` 或默认 CTest。它增加的是 CPU
mechanics 的可审计证据，不改变“完整历史服务链优先、无自写 DOS runtime”的路线。
