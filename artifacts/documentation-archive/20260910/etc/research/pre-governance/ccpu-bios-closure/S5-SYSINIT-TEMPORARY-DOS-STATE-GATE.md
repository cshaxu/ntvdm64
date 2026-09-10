# S5 Sysinit Temporary DOS State Gate

状态：2026-08-09。本记录界定原始 `sysinit` 进入 `DosInit` 前的 temporary DOS
state。它是 `NTDOS -> NTIO` device-call 组合切片的前置采证，不是新的 bootstrap
或设备实现方案。

## 为什么临时段不能静态指定

`sysinit1.asm:903-920` 先把由 `demLoadDos` 写入的 NTDOS image 移到临时段：

```text
T = CS(sysinit) + paragraphs(SI_end)
copy dossize bytes from current_dos_location to T:0000
current_dos_location = T
```

随后 `sysinit1.asm:922-928` 读取 image offset 3 的 `PARASTART`，记录 DOSINIT
entry offset，并把 `current_dos_location` 减去 `PARASTART >> 4`。固定输入的
`PARASTART` 是 `0x2250`，但 `T` 依赖已经运行到该点的 sysinit CS 与 SI_end，
不等于最初 `demLoadDos` 的 `DI`，也不等于一个可由 profile 写死的高内存段。

临时 DOSCODE/DOSDATA 的映射为：

```text
temporary DOSCODE = T - 0x225
temporary DOSDATA = temporary DOSCODE + 0x794
```

而 `DosInit` 随后把 DOS data 移往低内存。故 `DEVIOCALL2` 的调用时刻使用的
SS/DS 只能由原始执行所得，不能从载入时的 `DI=08B0h`、map 或最终 `NTDOS.SYS`
文件长度推导。

## 现有证据与缺口

已证实：

1. `demLoadDos` 原始读取把 final `NTDOS.SYS` 写到 live `DI:0000`；
2. 固定 image 的 `DosInit` byte slice 已能到达 `charinit` call target；
3. `NTIO` 的 first `CON` header、strategy 和 interrupt 是实际 resident bytes；
4. S4 已具备这条 caller 所需的受限 far call/return mechanics。

尚未证实：

1. 一次同一 session 中原始 sysinit 的 `T` 值、`dos_temp_location`、以及调整后
   DOSCODE/DOSDATA segments；
2. `DosInit` 后实际 SS 与 `CALLDEVAD` 所在 DOSDATA segment；
3. `charinit` 的 `ES:BX` request packet 与 `DS:SI` device-header 是否同时保留；
4. 从原始 `DEVIOCALL2` 开始到返回 charinit 的完整 live instruction trace。

## 准入规则

下一个 default-disabled instrumentation 只可观察和断言以下 original state：

```text
sysinit chooses T
  -> copies source-built NTDOS image
  -> adjusts current_dos_location using image header
  -> calls original DOSINIT
  -> reaches original charinit / DEVIOCALL2
```

它不得：

- 为了进入 `DEVIOCALL2` 而预设 `T`、DOSDATA、`CALLDEVAD`、SS 或 DOS request packet；
- 以 map address 直接跳入 NTDOS；
- 用虚构 device header、strategy/interrupt 或 request status 替代 NTIO bytes；
- 在遇到未覆盖的 interrupt、BOP、host event、A20 或 memory-topology dependency 时
  返回成功。

当原始 trace 首次抵达未支持的 CPU instruction 或未准入 machine dependency 时，
记录完整的 CS:IP、相关段寄存器、字节窗口和最小内存地址范围后停止。只有该记录
才能授权下一项 S4 mechanics 或历史 owner 恢复。
