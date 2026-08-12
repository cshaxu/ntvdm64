# S3 Reconstructed Monitor Semantics

状态：S4 实现前字段级规格，2026-08-09。

## 适用范围

本规格定义未来自有 software monitor 的最小执行语义。它依据 OpenNT NT4 的
`c_main.c`、`sim32.c`、`ccpusas4.c`、`bios.c` 和 `nt_bop.c`，用于把 guest instruction
mechanics 接到 S2 的历史 i386 bridge。它不授权复用归档 R2/owned monitor 代码，不授权
重写 BIOS/BOP/DEM/DOS，也不要求或使用 V86。

首个实现 profile 仅为 real-mode、有限 ordinary RAM、无设备/IRQ 的 bounded slice。所有
未列出的 CPU mode、指令、I/O、映射或中断路径必须 typed fail-closed。

## 状态模型

每个 session 独占下列状态，不能是 host-global：

| 类别 | S4 必需字段 | 当前限制 |
| --- | --- | --- |
| visible general state | EAX、EBX、ECX、EDX、ESI、EDI、EBP、ESP | real-mode 初始 slice 可以只由所实现指令读写相应部分，但存储必须不丢失未使用高位。 |
| visible control state | CS、DS、ES、SS、EIP、EFLAGS | BOP/bridge v1 当前只交换 AX/CX/DX、CS/IP；其余字段仍由 monitor 所有。 |
| address/mapping state | A20 policy、ordinary RAM extent、immutable ROM/provider descriptors | S4 不开启 ROM/provider、A20 或设备；访问这些类别必须停止。 |
| execution state | invocation depth、budget、stop reason、last physical fetch address | `FE` 仅结束一次 invocation，不代表 session 或 DOS 完成。 |
| pending hardware state | IRQ/PIC/PIT/event queue | S4 不投递；任何要求该状态的路径停止。 |

CPU mode、CRx、descriptor cache、GDT/IDT、paging、V86 task state 不属于 S4 可写状态。
它们既不能由 bridge record 修改，也不能由 BOP handler 修改。

## 地址与内存

1. 所有 fetch、read、write 均经过同一条 `segment -> linear -> physical -> mapping` 路由；
   不允许 host pointer shortcut。
2. real mode 的 source reference 是 `sim32_effective_addr()`：`linear = (segment << 4) +
   offset`。该计算本身不处理 A20；A20 以后属于 linear-to-physical/mapping policy，不能
   偷偷混入 segment 计算。
3. 每次访问先验证长度非零、加法不溢出、mapping class 与权限。ordinary RAM 才允许 S4
   preload 或 write；ROM、provider、I/O 和未映射范围均返回 typed stop/fault。
4. S2 v1 的 service byte 是唯一允许的 bridge-side guest write，位置由输入 `CS:IP`
   计算，并在历史 i386 SAS aperture 内预检。它不是一般 guest-memory API。

## BOP 指令语义

OpenNT `c_main.c` 证明 BOP 是 CPU decoder 内的历史 instruction form，而不是公开的
“任意 opcode hook”。S4 仅可处理下列已证明形式：

| encoding | CPU 侧动作 | bridge 侧动作 |
| --- | --- | --- |
| `C4 C4 selector` 及原始 C4C? 变长 immediate | decoder 先读取足够字节得到完整 immediate，再更新 EIP 到 next instruction | 若 low byte 不是 `FE`，把 selector 原样交给 S2；不得在 monitor 内做 service switch。 |
| `D6 selector` | decoder 先消费 immediate 并更新 EIP | selector 原样交给 S2。 |
| D6 的 `FE`，或完整 C4 immediate 恰为 `0xFE` | 遵守各自历史 IP 消费规则后结束当前 invocation | 不访问 `BIOS[FE]`，不把它记录为 dispatch success。 |

对于 C4 form，`c_main.c` 在识别到 BOP 时会读取变长 immediate 以得到正确 next EIP；
S4 不得只匹配前两个字节。C4 immediate 的低字节虽为 `FE`、但完整 immediate 并非
`0xFE`（例如 `0x03FE`）时，原始路径进入 `EDL_fast_bop`/其历史 special case；S4
尚未恢复该语义，必须 typed stop。对于其余 immediate，原始代码调用 `bop(selector)`；
S4 只能通过 S2 historical bridge 保持该责任边界。

未知 opcode、无 S2 binding 的 selector、非法/未验证 C4 form、bridge refusal 和 bridge
ABI mismatch 都必须停止；不得退化为 NOP、success 或本地 DOS error。

## 历史 bridge 调用

S4 对 S2 的最小调用顺序为：

1. 取得 physical fetch address 与完整 BOP operand；
2. 按该 encoding 的规则提交 EIP advance；
3. 复制 bridge 所需 real-mode visible registers 到定宽 request；
4. 调用历史侧 record transport；
5. 仅当 response 是 `completed` 时，按 response 的明确字段回写 AX/CX/DX、CS/IP；
6. response 非完成或 transport failure 保留已提交的 CPU instruction state，停止并记录
   redacted event，不合成 DOS 结果。

S2 v1 只允许 selector `0x50`、service `0x14`。它是 trace profile data，不得成为
monitor 的 C4/50/14 专用 API。新增 selector、service、memory window 或 state field 前，
必须先获得相应原始 caller/owner trace。

## 明确排除

- `ccpusas4.c::biosDoInst` 的 V86/host-simulate 路径：它依赖 V86 fault/ROM segment
  machinery，在 x64 user mode 不可作为实现来源。
- synthetic IVT/BDA、BIOS ROM builder、PC/AT device success、DOS SVC table、文件系统、
  command loader、DPMI/XMS/EMS、keyboard/video/mouse/media policy。
- 外部 CPU backend、`ntvdm64`、Bochs、PCjs 或 NTVDMx64 代码；它们只可用于规格交叉审查。

## S4 最小验收

1. 一段 ordinary-RAM byte stream 的 NOP 能按 budget 前进，且 CS:EIP/physical fetch
   trace 一致。
2. 已验证 BOP byte stream 经 S2 record 到达原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch`，
   并正确合并 S2 response。
3. `FE` 停止当前 invocation；未绑定 selector、未知 opcode、I/O、mode change、IRQ 与
   mapping failure 都不产生 guest/DOS 成功。
4. 每个 stop 包含 mode、CS:EIP、physical fetch、opcode window 和 bridge disposition，
   但不记录 owner-supplied DOS binary 内容或 host-private path。

通过这些验收只说明受限 monitor/历史 host 接缝成立；不说明 reset、NTIO、NTDOS 或
`COMMAND.COM` 可运行。
