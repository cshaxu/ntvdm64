# S5 NTDOS `charinit` Device Boundary

状态：2026-08-09。本文只记录固定 NT4 source-built `NTDOS.SYS` 在 `CMDSETINFO`
返回后的第一个未执行 guest 边界；不新增 runtime、host shim、BOP/DEM handler 或 device
implementation。

## 已抵达的位置

default-disabled S4 monitor 已从固定 `NTDOS.SYS` 的 file offset `0x5251` 执行到
`msinit.asm:453` 的原始 near `call charinit`，并验证 call target 可 checked-fetch。这里的
输入状态来自更早的历史服务成功返回，而非由 S4 合成：`DS:SI` 指向 NTIO/BIOS 交给 DOS 的
首个 device header，`ES:DI` 指向 BIOS communication block。

## 原始责任链

`base/mvdm/dos/v86/doskrnl/dos/msinit.asm:788-804` 的 `charinit`：

1. 在 DOS data `devcall` request packet 写入长度、unit 0、`devinit` command 与零 status；
2. 令 `ES:BX` 指向该 packet；
3. 调用原始 DOS kernel `DEVIOCALL2`；
4. 返回至 `DosInit`。

`base/mvdm/dos/v86/doskrnl/dos/dev.asm:525-542` 说明 `DEVIOCALL2` 的职责：它从
`DS:SI` device header 取 strategy address，先调用 strategy，再从 header 取 interrupt
address 并调用 interrupt。也就是说，实际 device initialization 的控制流属于 guest device
driver，不属于 DEM、BOP dispatcher 或 host console shim。

```text
NTIO/BIOS-owned device chain (DS:SI)
  -> NTDOS charinit builds guest request packet
  -> NTDOS DEVIOCALL2
  -> guest device header.strategy
  -> guest device header.interrupt
  -> packet status / original DOS continuation
```

## 当前缺口的精确定义

缺口不是“实现 CON”或“让 `charinit` 返回成功”。要继续这一条真实路径，必须先有：

1. 已验证的 NTIO/BIOS prepared-entry contract，提供实际 device-chain bytes、首 header 的
   segment:offset，以及 BIOS communication block；
2. software monitor 对该调用路径实际出现的 real-mode instructions、far indirect calls、stack
   和 memory forms 的 caller-first 支持；
3. 首个真实 device header 指向的 guest driver image 与其 request-packet/status contract；
4. 仅在该 driver 触达具体 host seam 时，再按原始 caller 建立最窄的 console/input/time/media
   capability。

在第 1 项前，制造固定 header、假 strategy/interrupt、预写 request status，或直接使
`charinit` 成功，都会把 NTIO/BIOS 的产品职责错误地下沉为 S4 或现代 shim。它们均被禁止。

## 对路线的影响

这把后续高 ROI 工作重新排序为：先恢复并 trace NTIO/BIOS 的 device-chain prepared entry，
再扩展 monitor 到由真实 driver 字节要求的最小指令子集，最后才依据实际 caller 准入 host
capability。它不要求、也不授权预先实现完整 PC/AT、console、keyboard、disk、FDC/HDC 或
`core`/`mantle` 抽象。

该结论同样解释了为什么 S4 当前应在 `charinit` call target 停止：它是一个清晰、可复核的
guest-to-guest device boundary，而不是可以由普通 bridge 继续跨越的 host service boundary。
