# CCPU 原始 Selector-50 职责链复验

状态：已构建并运行通过，2026-08-08。

## 验证对象

默认关闭的 `ntdos64-opennt-original-selector50-trace-fixture` 只执行以下受限
历史职责链：

```text
CCPU
-> original BIOS[50h]
-> original MS_bop_0
-> original DemDispatch
-> original DEM service 14h (demQueryDate)
-> historical CCPU BOP FE return
```

fixture 的 guest stream 仅为 `C4 C4 50 14 C4 C4 FE`。它不加载 NTIO、NTDOS 或其他
BYOB 文件；不访问 DOS namespace、文件、磁盘、控制台、计时器或 host process handoff。

## 运行证据

在 `artifacts/build/current/opennt-r5-clang-x86` 的独立 x86 historical toolchain
中执行：

```text
cmake --build artifacts/build/current/opennt-r5-clang-x86 \
  --target ntdos64-opennt-original-selector50-trace-fixture
artifacts/build/current/opennt-r5-clang-x86\
  ntdos64-opennt-original-selector50-trace-fixture.exe
```

结果：构建成功，进程退出码为 `0`。

fixture 在退出前验证：

- `MS_bop_0` 精确消费 service byte，最终 `IP == 7`；
- 原始 `demQueryDate` 返回有效 weekday/month/day/year；
- `ntdos64_ccpu_sm0_unexpected_calls() == 0`。

因此，该结果证明受限 selector-50 调用没有由自写 BOP、SVC、DEM handler 或设备
响应推动。它经过的是保留的原始 BIOS table、`nt_bop` 和 DEM dispatcher。

## 边界

该结果不证明完整 NTVDM 或 NTDOS startup 已可运行：

- `ccpu_monitor_sm0_foundation.c` 仍是仅用于受限 fixture 的 RAM/stop foundation；
- 完整 `config -> sas_init -> rom_init -> cpu_init` 前缀仍在
  `InitNtCpuInfo` 的不兼容非 x86 generated monitor ABI 处停止；
- CCPU/CVIDC 基线 link diagnostic 仍有 video、EMS、resource 和 CRT closure 项；
- 两个 SAS vector slot 的 trace guard 仍不能提供成功语义。

此复验的价值在于：原始 BOP/DEM 协议链可以由 CCPU 软件解释器实际执行。接下来
应将关注点移到原始 startup prefix 的第一个可证实 host/monitor seam，而不是继续
扩展 selector 级别的自写 handler。
