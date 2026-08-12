# R2 Reconstructed Software Monitor Contract

状态：归档的早期 trace 合同，2026-08-09。该文件保留早期自写 R2 fixture 的解码和停止断言，
不能定义当前 SoftPC 重建实现路线，也不能作为 x64 用户态 monitor 的实现授权。
它不实现 DOS SVC，不替代 BIOS handler，不接入 `ntvdm64`、Bochs 或 PCjs 的代码。

## 范围

执行边界固定为：

```text
reconstructed monitor
  -> original OpenNT BIOS[selector]
  -> original BIOS[selector]
  -> original MS_bop_0
  -> original DemDispatch(service)
  -> original DEM
```

monitor 只拥有 CPU mechanics：寄存器、flags、段/线性/物理地址翻译、checked memory、
指令解码、受控 I/O、事件注入和一次执行切片的停止/恢复。任何 DOS 返回值、SVC 分派、
设备功能或 DOS 成功条件仍由原始 owner 决定。

## 原始锚点

- `base/ccpu386/c_main.c:3299-3352` 识别 C4 形式的 BOP，并调用原始 `bop()` macro。
- `base/ccpu386/c_main.c:3632-3645` 识别 D6 immediate BOP，先按原始规则推进 guest IP，再调用
  原始 `bop()`。
- `base/ccpu386/c_main.c:4831` 是 `c_cpu_simulate()` 的原始执行 loop 入口。
- `base/ccpu386/c_main.c:4864` 说明 BOP `FE` 是本次 CPU invocation 的退出点。
- `base/bios/bios.c` 拥有 selector 到 handler 的 `BIOS[]` 表；`host/src/nt_bop.c` 拥有
  service-byte fetch、`DemDispatch` 调用及其 IP 规则。

`base/inc/bios.h:179-183` 证明 `bop(n)` 是 `(*BIOS[n])()` macro，不是可链接函数 ABI。
因此历史闭包内部必须仍调用原始 `BIOS[]` 表（或一个只做 table invocation 的受控 adapter）；不得在
monitor 中复制 selector 50h 后的 service switch，也不得把 C4/C4 当成对外暴露的任意指令 override API。

归档 R2 的 `r2_monitor_call_bios_table()` 只是**同地址空间、同 calling convention 的单元测试
adapter**：它验证 selector 到 `BIOS[]` entry 的调用形状和 fail-closed 行为，并没有连接任何历史
OpenNT object。它不得被用于把 i386 历史函数指针传入 x64 monitor，也不得被重新接入 CMake；实际历史
连接仍须先满足 `R2-HISTORICAL-BRIDGE-BOUNDARY.md` 的窄 ABI 约束。

## `MonitorRunSlice` 合同

输入：一个已初始化的 session、CPU state、固定 memory topology、可用的原始 `bop()` 入口和
有限 instruction budget。输出只能是以下之一：

| disposition | 语义 |
| --- | --- |
| `budget_exhausted` | 正常可恢复暂停；寄存器和内存是已提交的 guest state |
| `bop_exit` | 原始 BOP `FE` 请求结束当前 invocation；不代表 DOS 完成 |
| `bop_unbound` | selector 在当前原始 `BIOS[]` table 中未绑定；fail-closed，不伪造 handler |
| `guest_fault` | monitor 已按 CPU 语义形成 fault state；不伪造 DOS error |
| `unsupported_instruction` | 未实现指令的 fail-closed 停止，记录 CS:EIP、opcode 和 mode |
| `host_stop` | 明确的取消、时间限制或已登记 host boundary |

monitor 不能返回“handled DOS service”或“device success”。BOP handler 返回后，执行循环从
原始 CCPU 所规定的 guest state 继续；只有 `FE` 才结束 invocation。

## Memory 与 CPU 规则

1. 所有 guest access 必须经过 `segment -> linear -> physical -> mapping`；real mode、V86、
   protected mode、分页、limit、权限和 fault 不能被 host pointer shortcut 绕过。
2. 普通 RAM、immutable ROM 与 device/I/O 是不同 mapping class。ROM 不可被 preload 或运行期
   guest write 修改；device callback 不能在事务失败时留下半提交状态。
3. A20、interrupt pending、PIC/IRQ 和 timer 是 session/machine state，而非 host-global state。
4. 尚无原始 caller 合同的 `c_sas_touch`、`c_VirtualiseInstruction` 保持 fail-closed，不得借
   此合同升级为泛化 hook。

## BOP 处理规则

1. 只解码原始 CCPU 已证明的 BOP encodings，且按该 encoding 的 IP 消费规则更新 state：
   D6 immediate BOP 先 `UPDATE_INTEL_IP(p)`，再 dispatch；C4 form 会读取其变长 immediate
   以取得正确 next EIP 后再 `UPDATE_INTEL_IP(p)`。
2. selector 由 guest instruction 得出；monitor 将其原样交给原始 `BIOS[selector]`。
3. `BIOS[]`、`MS_bop_0`、`DemDispatch` 的函数边界和数据流不在 monitor 中重写。
4. selector 对应的 `BIOS[]` entry 必须非空并实际被调用；未绑定 entry 返回 `bop_unbound`，不得
   将无 handler 记为 dispatch 成功。selector `FE` 是特殊的 invocation exit：C4 form 直接 `c_cpu_unsimulate()`；D6 form 也先
   调用 `c_cpu_unsimulate()`，其后源码的 `bop(FE)` 不得被误作正常可达 dispatch。其余未知/非法
   selector 保持原始 `illegal_bop` 行为或受控 fault stop；不自动忽略。
5. 自研 trace 可观察 instruction、BOP selector、前后寄存器与停止原因，但不可改变 handler
   返回、DOS 状态或设备响应。

## 首个实现切片与验收

当前第一切片只需在普通 RAM 上执行 NOP 与已证实的 BOP encoding，并用合成 `BIOS[]` table
验证 IP 消费、table invocation 和未绑定 selector 的停止语义。它不构成原始历史链已链接的证据。
下一切片才可在满足桥接边界后验证原始
`BIOS[50h] -> MS_bop_0 -> DemDispatch`，并在原始 `FE` 返回时输出 redacted trace。验收包括：

- 当前正例：D6、C4 及其变长 immediate 的 IP 前后值、合成 table entry 命中和 `FE` 退出均可记录；
- 后续链路正例：原始 BOP selector、service byte、IP 前后值和 `DemDispatch` 原始函数命中均可记录；
- 负例：未知 opcode、非法 BOP、不可访问 guest memory 与 budget exhaustion 均 fail-closed；
- containment：无自写 SVC handler、无 BIOS/IVT/BDA scaffold、无 host drive discovery、无 guest
  image 持久化。

达到该门槛只证明 monitor 与历史 dispatch 接缝成立；不证明 reset、NTIO、NTDOS 或
`COMMAND.COM` 已可运行。
