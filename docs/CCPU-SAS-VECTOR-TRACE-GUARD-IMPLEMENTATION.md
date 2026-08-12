# CCPU SAS Vector Trace Guard 实施记录

状态：已实施并完成编译验证，2026-08-08。

## 目的

为 OpenNT CCPU `SasVector` 中尚未恢复语义的两个 slot 提供默认关闭、
fail-closed 的调用边界记录：

```text
c_sas_touch(address, length)
c_VirtualiseInstruction(eipInRom, size, linearAddrOrPort, dataIn)
```

本次工作不为任一 slot 提供成功语义。它们不能返回可用 guest pointer、
instruction result、BOP/SVC result 或 device response。

## 实现位置

- 自研 trace guard：`src/opennt/local/archived/legacy-adapters/ccpu_unresolved_vector_guard.c`
- 默认关闭的唯一链接消费者：
  `ntdos64-opennt-ccpu-cvidc-register-bridge-fixture`
  （`src/opennt/CMakeLists.txt`）

该 fixture 只用于 CCPU 加原始 CVIDC archive 的 link diagnostic；它不进入
`ntdos64-run`、不运行 CPU simulation，也不进入 BIOS、BOP、DEM 或 DOS。

## Guard 合同

每次 callback 进入时，guard 记录 sequence number、slot kind、原 ABI 的全部值参数，
以及从实际 ABI entry point 取得的 caller return address。随后立即调用 `abort()`。
即使未来某个 bounded historical experiment 实际抵达该 slot，也只能得到调用证据，
不能把未恢复的语义伪装成 guest-visible 成功。

guard 不声称能在此处取得 CPU mode、CPL、CS:EIP 或 EFLAGS：该 ABI 没有传入这些
状态。后续若定位了真实 caller，应在 caller 所属的历史 module 边界补充 snapshot，
而不是在此处猜测全局 CCPU state。

## 构建验证

在既有独立 x86 历史工具链目录中，使用 `clang-cl` 和 `lld-link` 重新配置并构建：

```text
cmake --build artifacts/build/current/opennt-r5-clang-x86 \
  --target ntdos64-opennt-ccpu-cvidc-register-bridge-fixture
```

`ccpu_unresolved_vector_guard.c` 已成功编译。最终链接按既有诊断预期失败，
剩余未定义符号为：

```text
sprintf
read_pointers
LIM_b_write
LIM_w_write
config_inquire
host_read_resource
c_effective_addr
```

其中没有新增的 trace-guard 未定义符号。`c_effective_addr` 未出现在该 CCPU/CVIDC
baseline fixture 的链接列表中；它已在其他更高层的 default-disabled historical fixture
中通过 `overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c` 准入，不能据此宣称本 fixture 已完成整体
host closure。

## 后续恢复边界

1. `c_effective_addr` 已有独立 completion 和 fixture；下一步只需将其证据状态与
   实际使用它的 link diagnostic 对齐。
2. `c_sas_touch` 与 `c_VirtualiseInstruction` 经来源审计确认属于 NTVdmx64 后代
   `sas4gen.patch` ABI，不是 NT4 SoftPC 缺失实现。它们永久保持 fail-closed trace-only；
   不得以 caller trace、后代 stub 或自研兼容版本把它们升级进 NT4 runtime。
3. `GDP_PTR`、`ConstraintR*` 属于不兼容的非 x86 `CPU_40_STYLE` profile 生成 ABI；
   不能把它们当作本 x86 CCPU fixture 的下一个 host shim 工作项。
