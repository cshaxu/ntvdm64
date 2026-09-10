# M91 当前 Core Backend Gate 复核

状态：只读复核，2026-08-08。

## 问题

在 `ntvdm64` 当前 M5 设备迁移之后，是否已经具备未来外部 NT4 EN-US wrapper
选择该 core 时所需的两项中性 backend 能力？

1. 配置期注册、冻结后不可变的 instruction/decode observation；它必须在 core 判定
   前缀命中后，向外部 adapter 交付受限的 copied-value context，而不是把任意 `#UD`
   fault 交给 handler。
2. paused/STOPPED 边界上的 ordinary-RAM multi-span transaction；它必须先验证所有
   span 都是普通 RAM，随后一次性提交，失败时不留下部分写入。

这两项是 M70/M83 的 engine admission 条件。它们不是 CMOS、RTC、FDC、HDC、媒体或
ROM mapping 的同义词。

## 检查基线

只读工作树：`D:\home\repos.hobby\ntvdm64`。

```text
HEAD: da152c9 M5 T295 S3 P3: close CPU PIC lifecycle authority
git status --short: no output
```

本记录没有修改该工作树。

## 当前公开 memory surface

`src/core/machine/memory_interface.h` 仍只声明：

```text
core_machine_memory_read(machine, physical, data, size)
core_machine_memory_write(machine, physical, data, size)
core_machine_memory_query(machine, physical, size, access, route)
core_machine_set_a20(machine, enabled)
```

每项只表达一个连续 physical range。`query` 可报告 ordinary RAM 或 provider，但没有
span array、prepare/commit、transaction identifier、回滚保证或“全部 ordinary RAM”的
原子验证接口。故 entry plan 的预加载原子性不能替代一个运行期 multi-span transaction：
前者是受限 initial-state 操作，后者必须服务已暂停的外部 adapter 请求。

对 `core_machine_memory_transaction`、`memory_transaction`、`multi_span` 与
`multispan` 的当前 `src/tests/docs` 搜索均无结果。

## 当前 instruction/fault surface

对 `transition_registry`、`configured_decode`、`decode_observation`、
`instruction_transition` 和 `transition_handler` 的当前 `src/tests/docs` 搜索均无结果。

现有 CPU 路径在 `src/core/machine/cpu_instructions.c` 通过 `_SetExcept_UD` 设置
`VCPUINS_EXCEPT_UD`。随后 `src/core/machine/machine.c` 的 fault callback 将
`instructions->data.except`、`excode` 和 copied instruction point 写入 first-fault
diagnostic。这是有价值的受控 fault observation，但不是注册 pattern、在配置 decode
点观察、返回 `unhandled/handled-resume/stop/fault` 的 transition contract。

因此不能把一个未知 BOP marker 故意变成 `#UD` 后“恢复执行”；那会绕开 CPU 的正常
fault、IP 和 mode 语义，也不满足 M83 的 constrained outcome 要求。

## M5 设备迁移的关系

当前 core 已拥有更广的 PIC/PIT/DMA/CMOS/RTC/FDC/HDC/media/ROM 机制。这推进了
NXVM 的产品中性 machine ownership，也为后续 mantle 提供真实基础；但没有为外部
wrapper 提供上述 decode observation 或 multi-span transaction。把设备移动误认为
wrapper 可执行 backend 会导致 adapter 反向依赖 VM/private borrows，或迫使本仓库
自写 executor/dispatcher，两者都违反既定边界。

## 结论与未来集成条件

若未来 wrapper 选择 `ntvdm64/core`，M70 backend gate **仍未满足**。这不阻止
`ntdos64` 修复和组合其自身的历史 SoftPC/CCPU/host 路径；它只禁止把当前 core
误称为已满足该另一条集成路线的 backend。

可解除条件仍是：一个真实的独立 first-party consumer 证明这两项通用 core 能力的需要；
core owner 在 `ntvdm64` 以 focused tests 提供窄 copied-value ABI；届时本仓库可另行
评估该 integration option。当前主线不依赖此条件。

## 复现

```powershell
git -C D:\home\repos.hobby\ntvdm64 status --short
git -C D:\home\repos.hobby\ntvdm64 log -1 --oneline
rg -n "core_machine_memory_transaction|memory_transaction|transition_registry|configured_decode|decode_observation|instruction_transition|transition_handler|multi_span|multispan" D:\home\repos.hobby\ntvdm64\src D:\home\repos.hobby\ntvdm64\tests D:\home\repos.hobby\ntvdm64\docs
```
