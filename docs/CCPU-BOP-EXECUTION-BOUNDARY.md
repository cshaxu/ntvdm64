# CCPU BOP 执行边界

状态：source-recovery evidence，2026-08-08。本记录只描述固定 NT4 OpenNT
`ccpu386` 的已见执行语义；不引入 BOP decoder、SVC dispatcher、BIOS handler 或
可运行 DOS 路径。

## 已证实的普通 BOP 编码与控制流

`base/mvdm/softpc.new/base/ccpu386/c_main.c` 在 opcode `0xC4`（通常为 `LES`）的
解码分支中，将 `modRM & 0xfc == 0xc4` 识别为 SoftPC BOP marker。`modRM & 3` 是
随后的 immediate 字节数；CCPU 读取这些字节形成 `immed`，并在进入 callback 前通过
`UPDATE_INTEL_IP(p)` 把 guest EIP 推进到 marker/immediate 之后。

非 PIG CCPU profile 的分流是：

| `immed` 的低字节 | CCPU 动作 | 结论 |
| --- | --- | --- |
| 不等于 `0xfe` | 设置 `in_C`，调用 `bop(ops[0].sng)`，再清除 `in_C` | `bop(n)` 是原始 `(*BIOS[n])()` 宏；selector 是 `immed` 的低字节。 |
| 等于 `0xfe` 且值为 `0xfe` | `c_cpu_unsimulate()` | monitor 控制退出，不是 BIOS/DEM 服务。 |
| 等于 `0xfe` 且其余值 | `EDL_fast_bop(immed)` | 独立 fast-BOP monitor contract。 |

因此普通 BOP 不是通过 `EDL_fast_bop` 进入，且 CCPU 在调用 `BIOS[]` 前已经消费完 BOP
marker。任何恢复实现必须保持这个 IP 时点；不得由 `BIOS[]`、`MS_bop_0` 或 DEM 再次消费
marker 字节。

## NTVDM 的原始 BIOS 表连接

`base/bios/bios.c` 在 `NTVDM` profile 将 `BIOS[0x50..0x5f]` 依次绑定至
`MS_bop_0` 至 `MS_bop_F`。这是原始表项，不是本地约定。

其中 `host/src/nt_bop.c:MS_bop_0`：

1. 以当前 `CS:IP` 调用 `Sim32GetVDMPointer(..., 1, FALSE)`；
2. 读取一个 DOS emulation service byte；
3. 调用原始 `DemDispatch(DemCmd)`；
4. 仅在返回后将 IP 再推进一字节；
5. 对非 `0x14` / `0x15` service 调用 `IDLE_disk()`。

两层消费规则因而不同：CCPU 消费 BOP marker，`MS_bop_0` 消费 BOP 后的 DEM service byte。
这证明完整的正常职责链是：

```text
guest C4/C4.. marker
  -> CCPU advances IP
  -> BIOS[0x50] / MS_bop_0
  -> read one service byte at current CS:IP
  -> DemDispatch(service)
  -> MS_bop_0 advances IP by one
```

`MS_bop_2`（XMS）、`MS_bop_4`（COMMAND/SCS）、`MS_bop_5`（VDD）等也各自读取
当前 IP 的一字节或按其原始 entry contract 转交，不能被合并为一个新 dispatcher。

## fast-BOP 的边界

`EDL_fast_bop` 只在上述 low-byte `0xfe` 分支调用；固定 NT4、NT4.5、XP 比较树都未给出
其定义或可归属 generator 输入。其 `immed` 保留完整宽度，而非普通 BOP 的低字节 selector。
`0x03fe`、`0x05fe`、`0x06fe` 仅在 `SFELLOW` build 下有专用分支，不能推广为 NTVDM contract。

结论：在取得 owner 或实测调用契约前，fast-BOP 必须保持未恢复、fail-closed。它不得承担
`MS_bop_0`、`DemDispatch`、BIOS、DOS SVC、IRQ 或设备响应；普通 BOP 是否可到达，应由
只读 bounded trace 记录 selector、进入时 CS:IP、退出时 CS:IP 与返回状态来证明。

## 下一项可验证工作

为 default-disabled CCPU fixture 增加无副作用的普通 BOP 观察点：只记录 marker、selector、
CS:IP 的前后值和是否到达 `BIOS[0x50]`，然后停止。该观察点必须调用原始表/函数边界，
不得填充 BIOS 表、伪造 DEM 返回、修改 guest memory 或把 fast-BOP 转译为普通 BOP。
