# OpenNT `host_main` 前缀链路证据

状态：已建立默认关闭的入口观察夹具，2026-08-08。

## 目的与边界

`ntdos64-opennt-host-main-prefix-trace-fixture` 链接 OpenNT 原始
`base/mvdm/softpc.new/base/support/main.c` 中的 `host_main()`。它不手排
`gfi_init()`、`config()` 或 `cpu_init()`；而是让原始入口先保存 `pargc`/`pargv`，
并在其第一次历史宿主调用 `host_applInit(argc, argv)` 处停止。

夹具中的同名函数是 fail-closed observation stop：它只验证传入的参数与原始
`main.c` 保存的全局指针一致，随后立即 `longjmp` 返回。它不扫描参数、不设置函数表、
不初始化 UI、也不返回使启动继续。因此它不是 `host_applInit` 的现代 shim，更不是
runtime 路径。

## 已验证的原始顺序

在 OpenNT 的 `NTVDM`/`PROD` 编译 profile 下，原始 `host_main()` 的可观察前缀为：

```text
host_main(argc, argv)
  -> pargc = &argc
  -> pargv = argv
  -> host_applInit(argc, argv)
  -> bounded stop
```

夹具传入原始内部启动标志 `-f`，并断言 `argc == 2`、`argv[1] == "-f"`、`pargv == argv`
及 `*pargc == argc`。退出码 `0` 仅证明这个入口前缀；它不证明 `host_applInit`、
`gfi_init`、`config`、CPU、BIOS、BOP、DEM 或 guest 已运行。

## 复现

在 x86 `clang-cl` 历史工具链环境中：

```powershell
cmake -S src/opennt -B artifacts/build/current/opennt-r5-clang-x86
cmake --build artifacts/build/current/opennt-r5-clang-x86 --target ntdos64-opennt-host-main-prefix-trace-fixture
artifacts/build/current/opennt-r5-clang-x86/ntdos64-opennt-host-main-prefix-trace-fixture.exe
```

该 target 始终 `EXCLUDE_FROM_ALL`，不得被普通 CLI 或未来 `ntvdm.exe` 链接。

## 原始函数串接

`ntdos64-opennt-host-main-applinit-trace-fixture` 进一步把原始
`host_main()` 和带 checkpoint 的原始 `host_applInit()` 置于同一个调用栈。它以
`-f -o` 调用 `host_main()`，在 checkpoint `17`（原始 `host_ica_lock()` 返回后）
停止。这里不存在本地 `host_applInit` 定义；因此该夹具用于证明原始调用链，而不是
把两个分开的 fixture 拼成推论。

夹具接受原始 overlay 已定义的 checkpoint `10`--`50`；未知编号保留默认的 `17`。
每次扩展检查点后必须核对该参数范围，不能把默认 checkpoint 的成功误记为更深层
生命周期已到达。

2026-08-08 的复验已使此同一原始栈越过 `InitScreenDesc()`、palette 及
`init_host_uis()`，到达原始 `nt_start_event_thread()` 的入口（40）、键盘硬件 mutex
创建后（41）以及 Console Ctrl handler 登记后（42）。第一次尝试在
`CreateThread(..., CREATE_SUSPENDED, ...)` 之后无法有界返回，随后审计发现
`ThreadInfo` 的原始 storage owner 是 `host/src/nt_timer.c`，而非 `nt_event.c`。
把该既有历史 owner object 加入同一默认关闭 target 后，checkpoint `43`--`50` 与
原始 `host_applInit()` 返回后的 checkpoint `31` 均以退出码 `0` 到达。trace 不调用
`host_timer_init()`、不启动 heartbeat、也不 resume event thread。

它同样是默认关闭、只读的 trace target。上述到达点仍不证明 event loop、
`gfi_init`、`config`、CPU 或 DOS runtime 已可用。

## 原始 `main.c` 后续前缀

同一 target 的另一份 default-disabled overlay 保留原始 `main.c`，只在
`host_applInit()` 返回、`gfi_init()` 前后、`init_virtual_drivers()` 返回、
`config()` 返回和 `c_cpu_init()` 返回处加入 stop。它使下列原始顺序可直接观察：

```text
host_applInit return (60)
  -> verbose = FALSE
  -> gfi_init entry (61)
  -> gfi_init return (62)
  -> init_virtual_drivers()
```

`verbose` 的 owner 是原始 `base/support/xt.c`；它必须以独立 object 进入此 target，
不能链接包含第二个 `main.c` 的 broad support group。`gfi_init()` 则需要原始
`config.c::host_runtime_set()`、`gfi.c` 和 `gfi_mpty.c`：加入这些已有 owners 后
checkpoint 62 正常返回，且没有任何 soft-drive、image 或 host A:/B: backend。

下一次未到达的是 `init_virtual_drivers()` 返回后的 checkpoint 63。该函数的原始
body 进入 `mouse_driver_initialisation()`；在未完成 caller-first mouse/keyboard
审计前，不得为了通过它引入 input queue、mouse emulation 或 console substitute。

后续以同一默认关闭 target 复验：`mouse_driver_initialisation()` 已到达内部
checkpoint 631（原始 `NIDDB_Allocate_Instance_Data` 前）、632（其后）、6321（原始
`host_memset` 后），四次 HERC instance-field 写入，以及函数尾部 633；随后已返回 `main.c`
checkpoint 63。`host_memset` 的历史 owner 是既有的 `host/src/nt_unix.c` object，它采用项目内
既有的旧 CRT declaration-order 兼容编译配置，并未改为本地或 CRT 替代实现。该组合没有调用
host mouse backend，也没有创建输入队列。

原始 `config()` 随后被调用，但尚未返回 checkpoint 64，而是以 `0xC0000005` 退出。因此当前
证据仅说明调用链已进入历史配置边界；不能据此宣称配置、CPU、BIOS、BOP、DEM 或 guest 已可用。
下一步必须在原始 `config.c` 调用顺序中定位缺失的历史状态 owner 或现代 host dependency；不得以
自写默认配置、介质策略或 DOS service 让该路径继续。

进一步的 default-disabled `config.c` trace 证明 checkpoint 701（函数入口）、711--713
（原始 title buffer、`VDMForWOW`/`DosSessionId` 读取和 `GetConsoleTitle` 后）以及 7131--7133
（过期 PEB console-identity seam、线程 ID、进程 ID 后）均可达。该 target 显式链接的
`direct_cli_console_identity.c` 仅提供原始临时标题唯一 token 的现代 host-context seam；它不参与
任何 normal runtime target。下一步的原始 `sprintf` 尚以 `0xC0000005` 失败，未到达 714。

对照重建的独立 `ntdos64-opennt-config-prefix-trace-fixture` 当前仍返回 `0`，并到达其原始
`sas_term` stop。因此该失败暂定为 host-main composition 的 CRT/link closure 差异，而非 `config.c`
已证实的 SoftPC/DOS 语义缺失。不得以本地格式化函数或跳过 title path 掩盖它。

2026-08-09 的只读异常快照进一步显示，重复构建后的故障 IP 会落在不同的未映射低地址
（例如 `0x00090000`、`0x00050000`、`0x00170000`），而同一故障的栈顶返回地址在研究 EXE
映像范围之外。研究 EXE 的 `__stdio_common_vsprintf` UCRT import 仍存在；这与
`/FORCE:UNRESOLVED` 下的链接/加载组合污染一致，不是可归因于 `config()` 的历史 contract。
该 trace 因此冻结在 7133；除非能以不带 `/FORCE` 的历史 owner closure 到达 714，否则不得为
这个 fixture 新增格式化、title 或 DOS/SoftPC 替代行为。
