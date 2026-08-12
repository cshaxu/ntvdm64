# S5 NTIO CON Device Initialization Trace

状态：2026-08-09。本记录只采证固定 NT4、由源码构建的 `NTIO.SYS` 中
第一个常驻 `CON` 设备的初始化控制流。它不新增 runtime、host shim、BOP/DEM
handler 或设备实现。

## 已锁定的实际输入

构建产物为
`artifacts/toolchain-runs/ntio-tools16-opennt-v1/base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS`：

- SHA-256：`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`；
- `ntio.map` 给出 `res_dev_list = 0000:0024`、`strategy = 0000:01EC`、
  `cdev = 0000:017D`、`con_table = 0049:00E4`；
- `msbio1.asm:80` 的第一个 header 是 `CON     `，其 strategy 是 `strategy`，
  interrupt 是 `con_entry`；对应 image offset `0x24` 的 header 指向
  `0070:01EC` 与 `0070:01F7`。

`NTDOS.SYS` 的 `charinit` 不构造 host 事件。它建立标准 DOS device request
packet 后，通过 `DEVIOCALL2` 先调用 header 的 strategy、后调用 interrupt。
这条责任链在 `dos/msinit.asm:788-804` 和 `dos/dev.asm:525-542` 中完整可见。

```text
NTDOS charinit
  -> DEVIOCALL2
  -> NTIO CON strategy (0070:01EC)
  -> NTIO CON interrupt / con_entry (0070:01F7)
  -> cdev_entry
  -> far code pointer cdev (patched by seg_reinit)
  -> chardev_entry
  -> original con_table command routine
```

## 字节与源码的交叉核对

`strategy` 的真实 image bytes 为：

```text
0070:01EC  2E 89 1E 18 00       mov cs:[0018], bx
0070:01F1  2E 8C 06 1A 00       mov cs:[001A], es
0070:01F6  CB                   retf
```

它只保存 request packet 的 `ES:BX` 到 NTIO 的 `ptrsav`，没有 host call、BOP
或合成的成功状态。

`con_entry` 的真实首字节为：

```text
0070:01F7  E8 3B 00             call cdev_entry
0070:01FA  E4 00                dw con_table
```

第二行是 call 后的内联 dispatch-table 指针，不是 `IN AL,00h` 指令。这个区分
很重要：不能把线性扫到的 `E4 00` 误报为端口依赖。

`msbio1.asm:363-385` 的 `cdev_entry` 只在必要时执行 `EnsureA20On`，随后经
`cdev` 的 far pointer 进入 `Bios_Code`。`msbio1.asm:779-793` 的 `seg_reinit`
负责把这类 code-segment pointer 的 segment 字写入 BIOS data；因此 prepared
entry 必须包含真实 NTIO 初始化已经完成的 `seg_reinit` 结果，不能在 monitor 或
现代 shim 中硬编码 `0049`。

`chardev_entry`（`msbio1.asm:807-879`）保存寄存器，读取内联 table、`ptrsav`
和 request packet，并按 `cmd` 选择 `con_table`。`mschar.asm:73-87` 证明
`CON` command 0 是 `bc_exvec`（初始化），而读/写等 command 指向各自原始
guest routine。它最终将 status 写回原 request packet，再按原调用栈返回。

`bc_exvec` 的定义在 `mschar.asm:359-361`，只有 `clc; ret`。因此 `CON`
command 0 的成功不是 host console 的成功回报：它是原始 resident driver 对
初始化 request 的正常 guest 返回。`chardev_entry` 随后自行写入 packet status
并回到 `DEVIOCALL2`。这条最短路径没有触达 keyboard、video、time、port I/O 或
BOP。

## 由此得出的 monitor 合同

在第一个真实 `CON` 初始化前，软件 monitor 所需的是普通 16 位 guest 语义：

1. checked real-mode memory read/write，含 `CS:` 覆盖、`ES:BX` request packet
   和由 `ptrsav` 取得的 far pointer；
2. near/far call、near/far return、固定宽度 stack push/pop，以及由原始代码
   维护的 fake return address；
3. 段寄存器与 `seg_reinit` 产生的实际 far pointers；
4. A20 状态及 `EnsureA20On` 的原始 guest 路径。

这不是 console、keyboard、video、disk 或 BOP capability contract。只有当
`con_table` 实际选中的原始 command routine 抵达一个有来源的 host seam，才允许
为那个具体 caller 建立最窄 shim。`CON` 的初始化本身不得由预写 packet status、
假 strategy/interrupt、固定 console 成功值或任意 BOP handler 代替。

## 下一项高 ROI 工作

在不改变正常 runtime 的前提下，下一步是以实际 `NTIO.SYS` 建立 `strategy ->
con_entry -> cdev -> chardev_entry -> bc_exvec` 的可执行、default-disabled monitor
fixture，并枚举该小切片首次遇到的尚未支持指令。它必须只运行原始 image bytes、
request packet 和 `seg_reinit` 后的 pointer；不得以本地 device handler 代替。
若路径抵达原始 `EnsureA20On` 或真实 host seam，记录那个准确 caller；若未抵达，
不提前引入任何 host capability。该 fixture 仍不构成 DOS 运行能力。
