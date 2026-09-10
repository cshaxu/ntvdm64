# S5 NTDOS Image Address Map

状态：2026-08-09。本记录说明固定 NT4 source-built `NTDOS.SYS` 的文件偏移、
map 地址和 temporary DOS 逻辑地址之间的关系。它不授权直接从任意 map 地址开始
执行，也不定义 DOS 的最终驻留位置。

## 输入与转换规则

固定产物为：

```text
NTDOS.SYS
27858 bytes
SHA-256 957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84
```

`origin.asm` 的首字保存 `PARASTART`；本映像在 file offset `0x0003` 保存的
word 为 `0x2250`。`stripz.c` 的原始实现只丢弃 `NTDOS.BIN` 开头这 `0x2250`
个字节。因此，对 map 中属于 DOS image 的线性 offset `L`：

```text
final-NTDOS.SYS file offset = L - 0x2250
```

map 地址本身不等于 final-file offset。特别是不得把 map 的 `0000:4D47` 直接
当作文件 `0x4D47`；那会读取另一段代码。

## 已核验的 DEVIOCALL2 位置

`NTDOS.MAP` 给出：

```text
DEVIOCALL2 = 0000:4D47
CALLDEVAD  = 0794:037A
```

据此：

```text
DEVIOCALL2 file offset = 0x4D47 - 0x2250 = 0x2AF7
CALLDEVAD  file offset = (0x0794 << 4) + 0x037A - 0x2250 = 0x5A6A
```

固定 `NTDOS.SYS` 的实测 bytes 证实：

```text
0x2AF7  8B 44 06                mov ax,[si+6]       ; strategy
0x2AFA  36 A3 7A 03             mov ss:[CALLDEVAD],ax
0x2AFE  36 8C 1E 7C 03          mov ss:[CALLDEVAD+2],ds
0x2B03  36 FF 1E 7A 03          call dword ptr ss:[CALLDEVAD]
0x2B08  8B 44 08                mov ax,[si+8]       ; interrupt
0x2B0B  36 A3 7A 03             mov ss:[CALLDEVAD],ax
0x2B0F  36 FF 1E 7A 03          call dword ptr ss:[CALLDEVAD]
0x2B14  C3                      ret
```

`0x5A6A` 的初始四字节为零。这符合 `DEVIOCALL2` 的职责：它先从原始 DOS
device header 读取 strategy/interrupt，再在 DOS data 的 `CALLDEVAD` 临时槽写入
far pointer。该槽不能由 fixture、shim 或 profile 预填。

## 临时 DOS 映像的逻辑地址

`sysinit1.asm:903-928` 把整个加载映像移到临时段 `T`，读取 image offset `3`
的 `0x2250`，并把 `current_dos_location` 调整为：

```text
DOSCODE segment = T - (0x2250 >> 4) = T - 0x225
DOSDATA segment = DOSCODE segment + 0x0794
```

因此在该原始 `sysinit` 步骤真实完成后：

```text
DEVIOCALL2 = (T - 0x225):4D47
CALLDEVAD  = (T - 0x225 + 0x794):037A
```

`T` 由已运行的 `sysinit` 根据 memory/topology 选择；它不是 wrapper 可以猜测或
固定的数值。随后 `DosInit` 还会迁移 DOS data 到低内存，所以此公式仅适用于
`sysinit -> temporary DOSINIT -> charinit` 这一个时间窗口。

## 对组合切片的约束

要执行真正的 `NTDOS charinit -> DEVIOCALL2 -> NTIO strategy/interrupt`，下一
fixture 必须同时证明：

1. 原始 `sysinit` 选择的临时段 `T`；
2. `DosInit` 实际迁移后的 DOSDATA/SS，及 `CALLDEVAD` 对应的 slot；
3. `DS:SI` 仍指向源构建 NTIO 的 resident `CON` header；
4. `ES:BX` 是 `charinit` 在 DOS data 中实际建立的 request packet。

在四项齐备前，直接写 `CALLDEVAD`、把 map 地址硬编码为 segment:offset，或把
NTDOS `charinit` 与 NTIO `strategy` 人工接续，都会绕开原始 `sysinit/DosInit`
的地址和生命周期语义，因而禁止。
