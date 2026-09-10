# OpenNT Event Thread Host Boundary

## 结论

OpenNT NT4 的 `nt_start_event_thread()` 不是一个可以由现代 runner 用“读取
stdin 的循环”替换的小型帮助函数。它是历史 NT console、DOS 键盘硬件缓冲、BIOS
键盘队列、Ctrl 事件和 VDM 生命周期之间的状态所有者。研究型 runtime 如要保留
这条历史路径，必须保留它的函数边界与数据流；在此之前，不能把自写输入循环接入
正常启动链。

本记录只审计源级依赖，并不声称事件线程已经在现代交互式 console 下运行。

## 原始职责链

原始定义在
`src/opennt/base/mvdm/softpc.new/host/src/nt_event.c`：

```text
host_applInit()
  -> nt_start_event_thread()
       -> CreateMutex(hKbdHdwMutex)
       -> SetConsoleCtrlHandler(CntrlHandler)
       -> CreateThread(ConsoleEventThread, CREATE_SUSPENDED)
       -> CreateEvent(hConsoleWait / hConsoleWaitStall / hConsoleSuspend)
       -> InitQueue()
       -> allocate + InitKeyHistory()

nt_init_event_thread()
  -> ResumeThread(ThreadInfo.EventMgr.Handle)

ConsoleEventThread()
  -> nt_event_loop()
       -> GetConsoleInputWaitHandle()
       -> NtWaitForMultipleObjects(...)
       -> ReadConsoleInputExW(sc.InputHandle, ...)
       -> update_key_history()
       -> keyboard / mouse / menu processing
```

创建时使用 `CREATE_SUSPENDED` 是一个重要的历史不变量：DOS 尚未准备好时，线程
不得消费输入。该事实允许对“创建前缀”做 bounded trace，但不证明恢复后的 event
loop 可运行。

## 所有权与不可拆分状态

| 状态或行为 | 原始 owner | 现代适配器的约束 |
| --- | --- | --- |
| `hKbdHdwMutex`、`KbdHdwFull` | `nt_event.c` | 不得由外部 runner 另建一套键盘锁并与历史队列并存。 |
| `ThreadInfo.EventMgr.Handle/ID` | `nt_event.c` + `nt_thred.h` | 线程的创建、恢复、alert 与关闭必须使用同一 handle。 |
| `hConsoleWait`、`hConsoleWaitStall`、`hConsoleSuspend` | `nt_event.c` | suspend/resume 是协作协议，不能以强制终止线程代替。 |
| `key_history` 和 `KeyQueue` | `nt_event.c` | 历史回看和 BIOS 键盘交付依赖其顺序与溢出规则。 |
| `CntrlHandlerState` | `nt_event.c`、`config.c` | PIF/close/blocked 状态与 Ctrl/Ctrl-Break 的异步处理共享该字。 |
| console `sc.InputHandle` | `nt_hosts.c` / `nt_graph.c` | 输入 handle 必须是可等待的 console 输入对象，不是任意 pipe。 |

## 需要的现代 host capability

正式 shim 的最小合同应是 capability，而不是把历史私有 API 暴露给上层：

- `console-input`：稳定、可等待的 Windows console input handle；支持 nonblocking
  record read，并能报告无交互式 console。
- `console-control-events`：注册和撤销 process console control handler，保持 Ctrl-C、
  Ctrl-Break、close 的离散原因。
- `event-thread`：创建、挂起、恢复、alert、等待和有序关闭；线程资源归历史 host
  状态所有。
- `keyboard-event-transport`：向已有历史 `KeyQueue` / BIOS 路径交付事件；不能另定义
  DOS keycode、IRQ 或 BIOS buffer 语义。
- `terminal-policy`：当标准输入是 pipe、文件或没有 console 时，明确选择“无交互式
  输入”或另一条专门 stream 路径，不能伪造 `CONIN$` 成功。

这些是 future modern-host shim 的需求，不是 `core` 机器能力，更不是向 NTDOS
二进制暴露的 ABI。

## 已知现代差异与未证实点

`nt_event_loop()` 使用 `GetConsoleInputWaitHandle()`、`ReadConsoleInputExW()`、
`NtWaitForMultipleObjects()` 与 `NtAlertThread()`。其中前两者/后两者含有历史或
native console 依赖，不能仅凭对象编译成功推定现代 Windows 的运行等价性。

在当前非交互式自动化环境中，stdout/stderr 是 pipe，原始 UI 初始化在获取
`CONOUT$` 的路径前无法完成可验证的交互式 console 设置。因此下一项证据必须来自
真正的 Windows interactive console，并至少记录：

1. 原始创建前缀是否完成且 event thread 仍为 suspended；
2. `nt_init_event_thread()` 后是否只消费一次输入记录；
3. Ctrl-C、suspend/resume、`nt_remove_event_thread()` 的有序清理；
4. 无 console 与重定向输入时的显式失败/降级，而不是 silent fallback。

## 当前证据

- CMake 默认关闭对象 target `ntdos64-opennt-event-host-objects` 只编译具有可追溯
  compiler-compatibility overlay 的历史 `nt_event.c` 输入，不是 runtime；overlay
  不含 `NTDOS64_*` 行为或 trace hook。
- 该 target 已以当前 OpenNT x86 toolchain 构建通过。
- 没有增加自写 BOP/DEM/BIOS handler、输入队列或模拟 console 来推进 DOS。
