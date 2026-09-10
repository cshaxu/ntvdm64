# S5 Service 11 返回至 SoftPC Monitor 合同

状态：源码合同已采证；经完整 CCPU/BIOS 组合的动态验证尚未闭合。

## 原始执行语义

`base/mvdm/softpc.new/base/ccpu386/c_main.c` 是 CCPU 的实际软件执行循环。
`c_cpu_simulate()` 保存当前模拟层的 `jmp_buf` 后进入 `ccpu(FALSE)`；该循环在每条指令后继续
到 `NEXT_INST`。这不是 V86 路径，也不是外部 CPU backend。

该文件对 C4 编码的处理给出了 BOP 的精确交接：当 LES opcode `C4` 后的 ModRM 满足
`(modRM & 0xfc) == 0xc4` 时，它识别为 `c4c?` BOP，读取完整编码以计算下一 EIP，然后：

```text
UPDATE_INTEL_IP
  -> in_C = 1
  -> bop(selector)       // bios.h: (*BIOS[selector])()
  -> in_C = 0
  -> SYNCH_TICK
  -> 返回 CCPU 的 NEXT_INST
```

因此对 NTIO 的 `C4 C4 50 11`，CCPU 负责消费三字节 BOP 编码并调用原 `BIOS[50h]`；
`MS_bop_0` 再消费 service byte `11h`。在 `demLoadDos` 返回后不存在另一种 CPU mode switch 或
自写 continuation：控制流应继续由同一个 CCPU loop 从更新后的 EIP 执行。

`c_cpu_continue()` 是异常路径用的 non-returning `longjmp`，不是正常 BOP service return。它不得
被作为 service 11 的正常恢复机制或新 public ABI。

## 当前动态证据边界

S5 direct 与 BIOS selector fixture 已经验证了 service 11 的原始 host chain 和 load 后状态；
它们故意直接调用 `BIOS[50h]`，因而不能证明上述 CCPU loop 的动态 return。

尝试复建既有 default-disabled `ntdos64-opennt-ccpu-software-monitor-sm2-fixture` 时，当前链接
未闭合。该 target 坚持链接完整历史 `BIOS[]` table，因表中所有函数指针都必须解析，因而拉入的
历史 EOI、timer、event、video 等对象暴露了大量未闭合 owner，例如：

- `ActivityCheckAfterTimeSlice`、`DisplayErrorTerm`；
- `InitSound`、`DemHeartBeat`、COM/LPT/FDC heartbeat；
- 旧 console/event API 与 mouse/display owner；
- 通用 `Video` binding、以及多项 display/video owner；`C_Video` 本身已由原 C-VID archive 闭合；
- 部分 CRT 与旧 host import contract。

这不是“service 11 需要这些 handler”的证明，也不能以 local no-op shim 或缩减 selector table
绕过。它是一个 build-closure inventory：完整 BIOS 表是 CCPU 动态 BOP 证据的前置条件，而不是
当前 S5 loader 读取的前置条件。

## 下一项实施前置

下一轮应把该 unresolved inventory 按原始 owner 分为：

1. 可由保留的历史 source/object 直接闭合；
2. 明确的现代 host seam，需 caller-first compatibility facade；
3. 缺失的 SoftPC/generated 语义，须有独立 reconstruction evidence；
4. 当前路径并未调用、只因完整 table 强制链接的可延后闭合项。

在这份分类完成前，不新增 generic BOP dispatcher、CPU fallback、设备 mock 或产品 runtime。
