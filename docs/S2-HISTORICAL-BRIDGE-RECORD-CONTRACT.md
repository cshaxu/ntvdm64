# S2 Historical Bridge Record Contract

状态：v1 i386 trace fixture 已验证，2026-08-09。

## 目的与边界

S2 不是把 i386 `BIOS[]` 函数指针交给 x64，也不是将 `MS_bop_0`、`DemDispatch`
或 DOS service switch 重写到 monitor。它定义一个仅用于已验证 date trace 的、由
历史 i386 闭包实现的 C record ABI：

```text
modern-side record
  -> historical i386 bridge
  -> original BIOS[0x50]
  -> original MS_bop_0 -> DemDispatch(0x14)
  -> copied result record
```

历史函数、SAS allocation、CPU register storage、`BIOS[]`、`Video`、CRT allocation
和异常都不跨越 record 边界。v1 只是在同一 i386 fixture 内可执行；将来跨位宽/跨进程
transport 必须逐字节传输同一 record，而不能直接链接或调用此 C entry。

## v1 输入

输入 record 使用固定宽度类型，包含：

| 字段 | 规则 |
| --- | --- |
| `abi_version`、`struct_bytes` | 必须精确匹配 v1；不匹配时 bridge 不触碰历史状态。 |
| `selector` | v1 仅接受已闭包证明的 `0x50`。 |
| `service_byte` | v1 仅接受已实证且无 I/O 的 `0x14` date service。 |
| `ax/cx/dx` | 写入历史 register owner；未由 date service 改写的部分必须可回读。 |
| `cs/ip` | real-mode visible location；bridge 以 `cs << 4 + ip` 计算 service-byte 写入位置，拒绝溢出或超出已拥有 SAS RAM 的地址。 |

v1 没有 guest pointer、host handle、可变长 buffer、callback、C++ object、异常、
CRT allocation 或“任意 memory access”字段。它也不提供 CPU mode、segment cache、
A20、mapping、IRQ、device 或 DOS 状态的写权限。

## v1 输出与 disposition

输出 record 回传同一有限的 register subset、`cs/ip`、ABI/size 与下列 disposition：

- `completed`：历史 table entry 已返回；仅此时允许读取更新后的 registers。
- `invalid_record`：版本、大小、selector、service 或 real-mode SAS address 不合法；
  bridge 不调用历史函数。
- `not_initialized`：未建立历史 i386 session；bridge 不调用历史函数。
- `unsupported`：为未来版本保留；v1 不用它把未知 selector/service 伪装为成功。

`completed` 不表示 DOS 成功、NTIO 启动、设备成功或完整 selector closure；它只表示
该次历史 date handler 返回。

## 生命周期与原子性

v1 分为 `initialize -> invoke -> terminate` 三步。`initialize` 在历史 i386 侧调用
原始 SAS/CCPU lifecycle owner；它不能替换 CPU execution。`invoke` 先完整验证 record，
再写入寄存器和一个 service byte，最后调用原始 `BIOS[0x50]`。任何验证失败不得部分
写入历史 registers 或 SAS RAM。`terminate` 只释放同一历史 session。

当前 bridge 仍使用归档 foundation 提供的受控 SAS aperture，且只作为 default-disabled
trace fixture。它绝不进入 `ntdos64-run`，也不因 fixture 成功而成为 runtime host shim。

## 验收夹具

1. 合法 date record：返回 `completed`，`IP = input.IP + 1`，并验证原始 date registers。
2. 错误 ABI version/size：返回 `invalid_record`，无历史函数调用、无 register/RAM 修改。
3. 非 `0x50` selector 或非 `0x14` service：返回 `unsupported`，无历史函数调用。
4. 未 initialize 调用：返回 `not_initialized`。
5. fixture 的 `unexpected_calls` 必须为零。

v1 验收后下一项仍是扩展 record 的实证，不是直接接入 NTIO，也不是允许任意 selector。

## 验证记录

`ntdos64-opennt-historical-bios-bridge-v1-fixture` 已使用独立
`clang-cl --target=i686-pc-windows-msvc` 构建为 `COFF-i386` 并以退出码 `0` 运行。
fixture 已覆盖：未 initialize 的 typed refusal、错误 ABI version 且 AX 不变、未支持
selector 且 AX 不变、以及唯一合法 record 的 original date result/IP increment。它还
断言归档 foundation 的 `unexpected_calls` 为零。

该验证只证明 i386 历史侧 record adapter 的边界正确；它尚未提供 x64 transport、
跨进程 session、一般 guest-memory window 或额外 selector/service。
