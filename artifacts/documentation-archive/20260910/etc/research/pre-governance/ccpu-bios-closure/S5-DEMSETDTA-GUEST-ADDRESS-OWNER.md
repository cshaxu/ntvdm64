# S5 demSetDTALocation Guest Address Owner 采证

状态：2026-08-09。本记录覆盖固定 NT4 `DosInit` 在 `50h/0Fh` 之后的第一个无条件
service：`SVC_DEMSETDTALOCATION (1Bh)`。它证明原始 DEM 和 CCPU/SAS 的 real-mode
地址路由可保存 NTDOS 提供的 host-visible 指针；它不是 DOS data 初始化或后续文件服务
的运行声明。

## 原始调用及职责

```text
BIOS[50h] -> MS_bop_0 -> DemDispatch(1Bh) -> demSetDTALocation
```

原 `demgset.c::demSetDTALocation` 在同一 `DS` 下调用五次原始 `GetVDMAddr`：

| NTDOS 寄存器/数据 | 原始 DEM global |
| --- | --- |
| `DS:AX` | `pulDTALocation` |
| `DS:DX` | `pusCurrentPDB` |
| `DS:CX` | `pExtendedError` |
| `DS:SI` | `DOSWOWDATA` 临时读取 |
| `DS:((WORD)DOSWOWDATA.lpSftAddr)` | `pSFTHead` |

函数不写 guest RAM、不访问 filesystem/console/media，也不引入新的 host API。它的意义是
把 NTDOS 已拥有的数据地址登记给后续 DEM owner。

## 运行证据

default-disabled target `ntdos64-opennt-original-demsetdta-trace-fixture` 先调用原
`DemInit`，在 SAS RAM 中仅写入 `DOSWOWDATA::lpSftAddr` 这一读取前提，随后以原始
`BIOS[50h]` 调用 service `1Bh`。它退出 `0`，输出：

```text
demsetdta fixture: ip=0479 dta=02A7E030 pdb=02A7E040 error=02A7E050 sft=02A7E070
```

fixture 同时断言四个地址分别等于同一 SAS aperture 的 `DS:0010`、`DS:0020`、
`DS:0030`、`DS:0050`。`IP=0479` 证明原始 `MS_bop_0` 只消费 service byte。

这里的 segment、offset 和 RAM 内容只是有界 fixture 输入，用来证明原始转换及 owner
存储；它们不是 NTDOS 的真实 DOS data layout，更不应被提升为 reset、entry-plan 或
产品 profile。

## 结论

`1Bh` 没有暴露额外 modern host seam。它确认后续 software monitor 必须向原始 owner
提供受限、可验证的 real-mode `GetVDMAddr` 路由；不能以预先构造的 host pointer、DOS
data 替代物或自写 SVC handler 取代该链。

下一个固定 `DosInit` service 是 `50h/32h -> demSetHardErrorInfo`，同样只登记 guest
地址。应先以同一 caller-first 原则验证它，再进入 `54h/05h` COMMAND owner 或会写入
guest DPB 的 `50h/46h`。
