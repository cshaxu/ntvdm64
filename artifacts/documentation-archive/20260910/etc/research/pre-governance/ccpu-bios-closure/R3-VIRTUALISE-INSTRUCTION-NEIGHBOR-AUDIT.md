# R3 VirtualiseInstruction Neighbor Audit

状态：2026-08-09。本文只审计相邻的已实现接口；不为 `c_VirtualiseInstruction` 提供实现、shim 或 runtime 准入。

## 已证实的相邻语义

OpenNT `base/ccpu386/ccpusas4.c:1524` 定义：

```text
c_IOVirtualised(port, value*, offset, width) -> IBOOL
```

其注释与实现表明：当 CCPU 处于 VM 状态，或当前 CPL 高于 IOPL 时，它通过
`biosDoInst(BIOS_VIRTUALISE_SEGMENT, offset, value, 0, port)` 在 ROM 中执行等价的 I/O
指令；调用方根据布尔返回决定是否改走普通 `Ios_*` adapter。`support/ios.c` 的 `inb`、
`outb`、`inw`、`outw`、`ind`、`outd` 是该接口的直接 caller。

这是一条完整的、已命名的 I/O 虚拟化路径，不是 BOP、DEM 或 DOS service 路径。

## 与未恢复 slot 的关系

`c_sas_touch` 与 `c_VirtualiseInstruction` 只在
`ccpu386/sascdef.c::cSasPtrs` 的最后 callback 位置出现。可见 CCPU/C-VIDC/NTIO/BIOS/DEM
C 源码中没有它们的直接 caller。

未恢复 slot 的 ABI 是：

```text
c_VirtualiseInstruction(eipInRom, size, linearAddrOrPort, dataIn) -> IU32
```

它与 `c_IOVirtualised` 在以下方面均不相同：

- 没有 `port` 与 `value*` 的明确 in/out 约定；
- 额外带有 `eipInRom` 与 `size`；
- 返回 `IU32`，不是“已虚拟化/未虚拟化”的 `IBOOL`；
- 现有源码没有说明它是否修改 IP、FLAGS、寄存器、内存或异常状态。

因此，二者在 vector 中相邻及名称相似，最多证明存在共同的 virtualisation 家族；不能证明
`c_VirtualiseInstruction` 可以调用 `biosDoInst()`、可以返回 `dataIn`、可以消费 guest IP，
或可以被改造成通用 instruction trap。

## 结论与下一步

R3 继续保持 fail-closed。将来只有在受控 historical caller 真正抵达该 slot 后，才能同时
捕获四个 ABI 参数、调用前后 CPU state、ROM 指令字节、返回值和异常/中断结果；届时才可把
`c_IOVirtualised` 作为限定的对照 oracle。此前不得从该函数复制实现，也不得将其接入
S4 monitor、BOP bridge、BIOS、DEM 或 DOS runtime。
