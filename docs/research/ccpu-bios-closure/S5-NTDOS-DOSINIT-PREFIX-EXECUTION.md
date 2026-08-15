# S5 NTDOS DosInit Prefix Execution

状态：2026-08-09。本文记录固定 NT4 研究构建的 `NTDOS.SYS` 中，`DosInit` 到第一个
`SVC_DEMGETDRIVES` BOP 的 software-monitor 执行证据。它不执行 DEM service，不提供
drive policy，也不构成 NTDOS 或 DOS runtime 成功声明。

## 固定输入和代码锚点

输入是本仓库 Tools16 构建的：

```text
artifacts/toolchain-runs/ntdos-tools16-historical-v1/
  base/mvdm/dos/v86/doskrnl/dos/NTDOS.SYS
```

该文件为 27,858 bytes，SHA-256 为
`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`。
其中 `0x51AF` 的固定字节序列从 `msinit.asm::DosInit` 的 `cld`、保存 BIOS 参数、
复制 DOS data 到低内存、读取 `BiosComBlock.DemInfoFlag` 开始；`0x51E9` 是唯一对应的
`C4 C4 50 0F`。这一归因同时由：

```text
base/mvdm/dos/v86/doskrnl/dos/msinit.asm:232-293
base/mvdm/dos/v86/doskrnl/dos/NTDOS.MAP: DOSINIT
固定 NTDOS.SYS 的 0x51AF..0x51EC 字节
```

交叉确认。它不把 map 的未 strip 地址直接解释为最终文件地址。

## 已执行的普通 CPU 语义

`refs/archive/reconstruction/monitor/reconstructed_monitor_s4.c` 现以 ordinary RAM 仅补充这条
真实指令流所需的 real-mode mechanics：

```text
CLD
MOV r16,Sreg / MOV Sreg,r16
ADD AX,imm16 / AND AX,imm16
CS:/ES: 覆盖的已实证内存读
REP MOVSB（先验证完整 source/destination 区间，再以真实方向逐 byte 复制）
MOV AL,ES:[DI+disp8]
MOV [disp16],AL
```

它们全部仍经既有的 segment:offset ordinary-RAM route；没有为 DOS data、BIOS
communication block 或 BOP selector 新设 host-side storage。对于这段未使用的 modrm、
段覆盖、I/O、模式切换和中断路径仍返回 typed stop。

## 受控 fixture 和结果

`tests/reconstructed_monitor_s4_test.c` 把固定 62-byte `DosInit` 前缀放入受控 ordinary
RAM，并设置由原始汇编明确要求的最小初始关系：

```text
CS:IP             -> DosInit 前缀
DS:SI             -> BIOS 提供的 device-list 指针
ES:DI             -> BIOS communication block
DX                -> conventional-memory size
CS:[InitBioDataSeg] -> BData segment -> DosDataSg destination
```

fixture 验证：

1. `REP MOVSB` 把 `0x15E2` bytes 的 DOS data 从临时代码段复制到 `DosDataSg`；
2. 除 `SCS_ISDEBUG` 的原始写入位置外，目标字节与 source 相同；
3. `BiosComBlock.DemInfoFlag` 的读取按原始 `ES:DI+1` 进入 `SCS_ISDEBUG`；
4. 保存的 `DX`、`DS:SI`、`ES:DI` 和 stack 在 BOP 前按原始 push/pop 顺序恢复；
5. `DF` 被 `CLD` 清除；
6. monitor 最终 fetch `C4 C4 50 0F`，把 selector `50h` 和 service byte `0Fh` 交给
   bridge，但 bridge 明确拒绝。

验证命令：

```text
cmake --build artifacts/build/current/default-mingw-verify \
  --target ntdos64-reconstructed-monitor-s4-test --parallel 8
artifacts/build/current/default-mingw-verify/ntdos64-reconstructed-monitor-s4-test.exe
```

2026-08-09 的结果为 exit `0`。该 target 仍为 default-disabled，未进入 `ntdos64-run` 或
默认 CTest。

## 边界结论

这首次证明 reconstructed monitor 能从一段真实 NTDOS guest 指令流到达其首个 host
请求，而不替换 BOP/DEM/DOS dispatcher。它**没有**证明 `demGetDrives` 已被调用：

```text
software monitor
  -> C4 C4 50 0F
  -> typed bridge refusal
```

下一步不是让 bridge 返回硬编码 drive count，而是把同一个 monitor byte aperture 与原始
`BIOS[50h] -> MS_bop_0 -> DemDispatch -> demGetDrives` 的 i386 historical closure
组合；此后由原始 `demGetDrives` 的真实 caller 决定是否需要现代 OEM/native drive seam。
`demGetDPBList` 仍必须消费同一次 DEM session 的 cache，不能由外层预写 guest DPB。
