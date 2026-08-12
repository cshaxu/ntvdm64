# OpenNT `host_applInit` 前缀链路证据

状态：已验证，2026-08-08。

## 目的与边界

`ntdos64-opennt-host-applinit-prefix-trace-fixture` 是默认关闭的只读
instrumentation target。它只调用 OpenNT 原始 `host_applInit()`，并在历史函数
内部的 checkpoint 处以 `longjmp` 退出。它不是 `nvtdm.exe`、不启动 DOS，也不实现或
替换任何 BIOS、BOP、DEM、设备或 DOS 服务。默认检查点 `17` 在 UI 之前退出；显式的
`30` / `31` 检查点可分别观察原始 UI 完成和原始 event-thread 创建函数返回，但仍不恢复
event loop。

采用的输入是 `-f -o`：二者均为原始 `nt_reset.c` 已有的内部开关。`-f` 通过原始的
直接启动保护；`-o` 禁用原始的 stream-I/O 初始化。它不是本地行为覆盖，且只服务于
有界 prefix trace，不能作为未来产品 CLI 契约。

## 证据

原始 `host_applInit()` 的下列检查点均已返回零：

| 检查点 | 原始动作 | 结果 |
| --- | --- | --- |
| 10 | 函数入口 | 通过 |
| 11 | 赋值 `working_video_funcs`、`working_keybd_funcs`、`working_mouse_funcs` 后 | 通过 |
| 12 | 原始参数扫描与 `-f` 验证后 | 通过 |
| 13 | stream-I/O 分支后（`-o`） | 通过 |
| 14 | 原始 `DuplicateHandle` 后 | 通过 |
| 15 | `RtlInitializeCriticalSection(&IcaLock)` 前 | 通过 |
| 16 | 同一原始 RTL 调用返回后 | 通过 |
| 17 | 原始 `host_ica_lock()` 返回后 | 通过 |

函数表指针的 data owner 是原始 `base/bios/reset.c`，故夹具链接该原始 owner 的
checkpoint overlay；其 `reset()` 从未被调用。`nt_eoi.c` 也以 overlay 形式保留原始
`InitializeIcaLock()`，只在 native RTL 调用前后插入 trace checkpoint。

早期一次失败的原因已经排除：该最小 link profile 未导入 `ntdll.dll`，导致历史
`RtlInitializeCriticalSection` 在 `/FORCE:UNRESOLVED` 下成为无效陷阱。显式链接
`ntdll.lib` 后，PE import table 含 `ntdll.dll!RtlInitializeCriticalSection`，检查点 16
与 17 均通过。这是恢复原始 host dependency，不是现代 shim 或锁实现。

## 可交互 console 的延伸检查点

fixture 现提供检查点 `31`：它位于原始 `nt_start_event_thread()` 返回之后。原始
函数以 `CREATE_SUSPENDED` 创建 event thread，因此到达此点只证明原始创建与同步
对象初始化完成，**不**证明线程已恢复、event loop 已读取输入，或 DOS 已启动。

当前非交互式自动化 host 在 UI 的 `CONOUT$` 回退路径之前不能作为此检查点的运行
证据。需要在真实 Windows interactive console 中显式执行：

```powershell
artifacts/build/current/opennt-r5-clang-x86/ntdos64-opennt-host-applinit-prefix-trace-fixture.exe 31
```

成功时退出码为 `0`；任何异常、未解析 late dependency 或 console 失败均不是可接受
的替代成功结果，也不得通过添加本地 event-loop/console shim 消除。

## 结论

当前可以证明历史 host 生命周期在 UI/event-thread 之前需要：

1. SoftPC 的三张工作 function table 及其原始 data owner。
2. 原始的 process/thread handle 获取。
3. `ntdll` 的 RTL critical-section API。
4. 由原始路径管理的 stream-I/O policy。

这并未证明 console、事件线程、全屏图形、VDM API 或 DOS session 已能运行。下一阶段应
在保持原函数边界的前提下，分别审计 `init_host_uis()` 与 `nt_start_event_thread()` 的
历史依赖；不能用新的控制台循环、输入分派器或自写事件模型绕过它们。

## 复现

在 x86 `clang-cl` 环境配置并构建：

```powershell
cmake -S src/opennt -B artifacts/build/current/opennt-r5-clang-x86
cmake --build artifacts/build/current/opennt-r5-clang-x86 --target ntdos64-opennt-host-applinit-prefix-trace-fixture
```

检查点通过命令行参数选择，例如：

```powershell
artifacts/build/current/opennt-r5-clang-x86/ntdos64-opennt-host-applinit-prefix-trace-fixture.exe 17
```

该 target 仅为研究证据，始终 `EXCLUDE_FROM_ALL`。
