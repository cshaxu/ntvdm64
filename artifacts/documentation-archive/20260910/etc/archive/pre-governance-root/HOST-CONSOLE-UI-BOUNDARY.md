# OpenNT Console/UI 宿主边界

状态：已完成静态审计与部分动态前缀验证，2026-08-08。

## 历史职责

OpenNT 的 `host_applInit()` 在取得 ICA lock 后调用 `init_host_uis()`，其原始实现位于
`base/mvdm/softpc.new/host/src/nt_hosts.c`。它不是 DOS、BIOS 或 BOP 层，而是历史
NTVDM 的 Windows console host 适配层。

`InitScreenDesc()` 的顺序为：

1. 取得 `STD_OUTPUT_HANDLE` 与 `STD_INPUT_HANDLE`。
2. 若某个句柄不是 `FILE_TYPE_CHAR`，尝试通过 `CONIN$` 或 `CONOUT$` 获得 console
   句柄，并替换相应标准句柄。
3. 把句柄保存进 `sc` 和 SCS 的标准句柄槽。
4. 以 `GetConsoleMode`、`SetConsoleMode`、`GetConsoleCursorInfo` 与
   `GetConsoleScreenBufferInfo` 初始化 console 状态。
5. 非 WOW、非 session-id 路径登记 console close notification。
6. `init_host_uis()` 之后创建显示 palette；随后原始 `host_applInit()` 才启动事件线程。

## 动态证据与限制

默认关闭的 `ntdos64-opennt-host-applinit-prefix-trace-fixture` 已到达：

- UI 函数入口（20）。
- `InitScreenDesc` 入口（21）。
- 取得原始标准输入/输出句柄后（22）。
- 输入句柄重定向处理后（23）。

2026-08-08 的同栈复验到达并受控停止于 UI/palette 的 checkpoint `29`，继而到达
原始 `host_applInit()` 中 `init_host_uis()` 返回后的 checkpoint `30`。这说明当前
fixture host 已能走过这段原始 console/palette 前缀；它不代表完整 console 事件模型
已经恢复。此前把此环境描述成固定在 UI 回退超时的结论已失效。

`SetLastConsoleEventActive()` 的历史 x86 调用符号为
`_SetLastConsoleEventActive@0`。默认关闭 trace target 仅用同调用约定的无输出
acknowledgement 让原始 caller 返回，且不会进入 runtime；它没有定义 close-event
订阅、键盘输入或 console-server 行为。之后第一个不可有界的点是原始
`nt_start_event_thread()` 的 `CreateThread(..., CREATE_SUSPENDED, ...)` 后，不能以
伪造 console handle、自写 event loop 或替代键盘队列绕过。

## 对现代 host shim 的约束

未来的现代 host compatibility shim 需要明确提供或拒绝以下 capability，不能静默把
pipe 当作 console：

- `console-input`: 可读的 character console handle 与输入 mode 设置。
- `console-output`: 可写的 character console handle、buffer/cursor 查询与 output mode。
- `console-close-notification`: console control/close event 的订阅或明确的不支持结果。
- `console-stream-policy`: 历史 `-o` / stream-I/O 选择由产品启动 profile 决定，不能由
  SoftPC 设备层猜测。

这些是 host capability，不属于 CCPU、guest RAM、PC/AT BIOS 或 DOS SVC 语义。当前
研究代码不得用自行实现的事件循环、键盘队列或屏幕设备来绕过历史 `nt_hosts.c` /
`nt_event.c`。

## 未完成项

仍需在真实可交互 Windows console 中验证原始 `SetupConsoleMode`、console close event、
palette 与 `nt_start_event_thread()`。默认关闭 fixture 的检查点 `31` 已位于原始
`nt_start_event_thread()` 返回之后，且仍在历史 `CREATE_SUSPENDED` 语义内；它可作为
这项验证的受限入口，但不运行 event loop。之后才能定义现代 console/event shim 的窄 ABI；
当前没有声明这些行为已经可用于 `nvtdm.exe`。
