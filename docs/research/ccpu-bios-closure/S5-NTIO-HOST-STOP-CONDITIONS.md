# S5 NTIO Host Stop Conditions

状态：2026-08-09，基于本仓库 Tools16 构建的 NT4 guest 输入、OpenNT source 与 no-SM0
host closure 的只读交叉采证。本文不授权执行 NTIO、DEM 或 DOS。

## 固定 guest 输入

```text
artifacts/toolchain-runs/ntio-tools16-opennt-v1/
  base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS
  base/mvdm/dos/v86/doskrnl/bios/ntio.map
```

`NTIO.SYS` 长度为 33792 bytes。map 证明 `0070:0000` 的 `E9 6D 03` 进入同一 image 的
`0000:0370 init`；对应 `msbio1.asm` 的 `jmp init` 和 `msinit.asm:93`。

## guest 侧的真实顺序

```text
0070:0000  E9 6D 03        -> 0070:0370 init
init:
  FCLI
  使用固定 guest-memory ABI 0040:0314 (physical 0714)
  保存 RomVectors 中的 INT 10h/15h/19h/1Bh IVT 项
  设置 SS:SP = 0000:0700
  FSTI
  初始化其余 guest IVT/data
  mov DI, DOS_LOAD_SEG
  SVC SVC_DEMLOADDOS      ; msinit.asm:247-250
```

`RomVectors` 与 `NUMROMVECTORS=4` 的原始定义在 `msbio1.asm:94-107`。这些内容说明 NTIO
自行进行 guest IVT 保存/改写；它们不能由 host 侧 scaffold、合成 BIOS 或 direct loader 代替。

## 目前第一个可重复 stop

独立 S4 monitor 已能在 ordinary RAM 中复现入口跳转、`FCLI/FSTI` 的已记录正常支路、首批
string/vector copy 与 `0070:0475 C4 C4 50 11` 的到达；它在 service byte 后以
`BRIDGE_REFUSED` 停止。该结果仅证明 guest prefix mechanics 到达原始 BOP encoding，未执行
`MS_bop_0`、`DemDispatch` 或 `demLoadDos`。

历史 host 侧则仍受完整 source closure 限制：

| 层 | 当前最小测量 | 结论 |
| --- | --- | --- |
| `MS_bop_0 -> DemDispatch(14h)`，无 BIOS table | 114 unresolved | `nt_bop.c` 与 `apfnSVC[]` 的 translation-unit/static-table 宽度；不是 date caller 清单。 |
| `BIOS[50h] -> MS_bop_0 -> DemDispatch(14h)`，完整 BIOS table | 282 unresolved | 另有 168 个 BIOS/provider 静态闭包项。 |
| 实际 `50h/11h` | 尚无 no-SM0 clean closure | 不得以已有 direct `demLoadDos` fixture 推断可执行。 |

相关日志与测量在：

```text
docs/research/ccpu-bios-closure/S5-BOP-STATIC-WIDTH-RESULT.md
docs/research/ccpu-bios-closure/S5-NO-SM0-CLOSURE-RESULT.md
```

## 对下一步的约束

1. `SVC_DEMLOADDOS` 是 NTIO prefix 的第一个 guest-to-host dispatch 边界，但不是当前允许跨越的
   点；必须先获得 no-SM0 的原始 `50h/11h` callable closure。
2. `FCLI/FSTI`、IVT copy、stack 与 fixed state word 是 software-monitor 的 guest CPU/memory
   semantics；它们不能被 host capability 或 BOP handler 吸收。
3. `reset()`、`io_init()`、VDD、console、设备和 filesystem 是原始 host 生命周期问题。只有真实
   prefix caller 抵达相应分支，才能逐项决定 source owner 或 narrow modern seam。
4. `NTIO.SYS` 的宿主读取、`NTDOS.SYS` 的 `demLoadDos` 读取与 DOS namespace 是不同层次：前者
   是启动前 resource load，后者必须保持原始 DEM owner。不得将它们合并为一个自写 loader。

## 下一项采证

保留 `50h/11h` 的既有 bounded trace 作为历史证据。由于 `DemDispatch` 的原始 `apfnSVC[]`
已静态引用所有 service，单独再做 `11h` static closure 不会提供新信息。下一项应先恢复 no-SM0
的原始 dispatcher callable closure；只有此后以受控、非 BYOB 的 `11h` invocation 到达真实
`demLoadDos` 时，才记录新增 owner 与第一个 typed stop。不得读取真实 BYOB 文件、不得启动
NTIO，也不得新增 OEM filesystem shim。
