# BYOB NTIO Loader Seam

状态：原始 session loader 的 BYOB 编译变体已通过，2026-08-08。

## 历史入口

OpenNT 的 `base/mvdm/softpc.new/base/support/main.c` 在 machine/config 之后调用：

```text
InitialiseDosEmulation(argc, argv)
```

其原始实现位于 `host/src/nt_msscs.c`。它的不可替代顺序是：

1. 初始化固定 DOS arena 中的 virtual-interrupt state；
2. `io_init()` 与 TLS slot；
3. 原始 `reset()`；
4. `SetupInstallableVDD()` 与可选 LIM page frame；
5. `scs_init()`，即 `CMDInit -> DemInit -> XMSInit -> DBGInit`；
6. 把 NTIO 文件读到 `NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET`；
7. 恢复 virtual-interrupt state，并将 CPU 设置为该 CS:IP。

因此它是外部 NTDOS wrapper 应当保留的 guest 装配职责链，而不是重新定义 DOS
启动协议的地方。

## 历史策略中不适合 BYOB 的两项

原件在第 5 步的首次 session 中调用 `AddSystemFiles()`，可能向 host `C:` 创建
零长度 `IO.SYS` / `MSDOS.SYS` 兼容文件；随后使用 `GetSystemDirectory()` 并强制
追加 `\\ntio.sys`。这两项都是 Windows NT 产品布局策略，不是 NTIO 的二进制 ABI。

## Overlay 变体

`src/opennt/overlay/base/mvdm/softpc.new/host/src/nt_msscs.c` 保留原目录、原函数
和所有上述顺序，仅在 `NTDOS64_BYOB_NTIO` 编译宏下：

- 跳过首次 session 的 `AddSystemFiles()` host 写入；
- 通过 `ntdos64_byob_ntio_path()` 取得完整 NTIO 文件路径，不调用
  `GetSystemDirectory()`，也不追加文件名；
- 找不到路径时沿原始错误返回通道返回 `-1`，不合成 guest 状态。

默认历史 `ntdos64-opennt-session-host-objects` 仍使用未修改原件；新增
`ntdos64-opennt-session-byob-objects` 是独立、默认关闭的对象目标，不能进入
runtime，直到其 VDD、DEM、XMS 和 reset closure 逐项实测。

## 当前 path provider

`local/shim/byob_ntio_path.c` 只读取明确设置的 `NTDOS64_NTIO_PATH`，并拒绝空值
或截断值。它不读取 NTIO 内容，不扫描目录，不缓存二进制，也不收集路径以外的
guest 信息。

`ntdos64-opennt-byob-ntio-path-fixture` 已验证环境变量缺失时失败、设置完整路径时
成功。`ntdos64-opennt-session-byob-objects` 也已在原 session 单元相同的编译环境
中成功构建。

## Session 前缀 Trace

默认关闭目标 `ntdos64-opennt-session-byob-prefix-trace-fixture` 将已验证的
`config -> SAS -> UMB -> GFI(no-media)` 链与原始 BYOB session loader 相连。它让
`InitialiseDosEmulation()` 执行 virtual-interrupt state 的原始 SAS 读写，并在
原始 `io_init()` 调用点之前停止，随后调用原始 `sas_term` 回收 aperture；执行
退出码为 `0`。

trace 使用单独的 `NTDOS64_SESSION_TRACE_IO_INIT` overlay 变体。该变体只在调用点
插入一个非返回记录 hook；正常 `ntdos64-opennt-session-byob-objects` 仍调用原始
`io_init`。为使完整 `nt_msscs.c` 翻译单元可链接，后续未执行的 CMD、DEM、XMS、
DBG、VDD/reset 依赖均是 fail-closed guard，不能当作运行时实现或兼容性证据。

第二个默认关闭目标 `ntdos64-opennt-session-byob-io-trace-fixture` 改用
`NTDOS64_SESSION_TRACE_AFTER_IO_INIT` 变体：原始 `io_init()` 完整执行后才停止。
该原件只注册 empty I/O adapter，并把每个端口的 in/out 路由表初始化为该 adapter；
它不打开物理设备、没有 BIOS/BOP/DEM 行为。该目标执行退出码为 `0`，从而证明
session loader 可以在 BYOB profile 中实际完成 I/O 路由基线建立。

### 2026-08-08 编译图复核

重建 `ntdos64-opennt-session-byob-io-trace-fixture` 时发现，`nt_msscs.c` 本身以
OpenNT `nt.h -> ntrtl.h -> nturtl.h -> windows.h` 开始，但 session object target 曾被
强制注入一个 Win32-first native-I/O compatibility header，造成 `STRING`、`WAIT_TYPE`
等 OpenNT native 类型重定义。移除该错误 preamble 后，编译又暴露其 target 漏传 `_X86_`：
`nt.h` 因而未包含原始 `ntx86.h`，使 `vdm.h` 所需的 `CONTEXT` 未定义。

修正只限于 `src/opennt/CMakeLists.txt` 的默认关闭 session object targets：恢复原始
native-header 顺序，并传入 `_X86_`。没有修改 `nt_msscs.c` 的运行时代码、BYOB 输入、
`io_init`、`reset`、BOP、DEM 或设备语义。以 x86 clang-cl 重新 configure/build 后，
该 fixture 构建成功并返回 `0`。它仍只证明原始 `io_init` 后的 trace stop；不构成
`reset`、VDD、DEM、XMS、DBG、NTIO 装载或 guest 执行的准入。

## 尚未证明的部分

本记录不意味着 `InitialiseDosEmulation()` 已能端到端运行。`reset()`、
VDD 安装、DEM、XMS、DBG 和 CPU 执行入口仍需以原始函数边界分别接入与验证。
特别是后续不得用手写 BOP、DEM 或 BIOS handler 来替代上述链条。
