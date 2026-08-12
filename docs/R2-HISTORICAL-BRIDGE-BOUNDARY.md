# R2 Historical Bridge Boundary

状态：设计约束，2026-08-09。

## 问题

R2 monitor 由现代 x64 MinGW runner 构建；已采集的历史 SoftPC/BIOS/DEM 输入按其原始 i386
工具链和 ABI 构建。`bios.h` 中的 `BIOS[]` 是进程内函数指针数组，不是稳定的跨模块协议。
因此不能把历史 `void (*)()` 指针或 C++/CRT 对象从 i386 闭包直接交给 x64 R2。

## 允许的连接形态

历史闭包与 R2 之间只能使用单独、版本化的 `extern "C"` bridge ABI。每个入口必须：

- 使用固定宽度整数、显式 calling convention 和无所有权歧义的 caller-owned buffer；
- 用 selector、寄存器快照、受检查 guest-memory request 或明确的 event record 表示数据；
- 由 bridge 自己在历史侧完成 `(*BIOS[selector])()`，保留 `BIOS[] -> MS_bop_0 -> DemDispatch -> DEM`
  的原始调用关系；
- 返回显式 disposition，而不是返回历史函数地址、CRT 指针、异常或 C++ object；
- 记录 ABI 版本、历史输入 manifest 和实际 target bitness。

在同一位宽、同一 ABI、并且已通过专门链接审计的测试闭包内，可以有一个 table-invocation adapter。
该 adapter 的唯一职责是调用 table entry；不能复制 BOP/SVC、DOS 或设备语义。

## 禁止项

- 不得把 `BIOS[]`、`Video`、SAS host pointer 或其他进程内数据结构当作 x64 ABI。
- 不得为跨位宽便利而把 `MS_bop_0`、`DemDispatch` 或 DEM service switch 重写到 R2。
- 不得通过 FFI 传递 C++ object、异常、`malloc`/CRT 所有权或未标注的可变长结构。
- 不得把当前合成 `BIOS[]` fixture 的成功写成“原始 BIOS/DEM 链已运行”。

## 下一项证据

先确定实际可构建的历史闭包 target、其位宽和入口 calling convention；然后为一个无副作用、可重复的
selector fixture 定义 bridge record。只有这个 fixture 同时证明 bridge 进入原始 `BIOS[selector]`
和原始 handler 返回，才允许继续向 `MS_bop_0`/DEM 扩展。

## 已复现的历史侧基线

2026-08-09 已在独立目录 `artifacts/build/current/opennt-r2-bridge-ninja-i686` 用
`clang-cl --target=i686-pc-windows-msvc`、VS x86 SDK/CRT 与 Ninja 重新配置并构建
`ntdos64-opennt-original-selector50-trace-fixture`。该 PE 经 `llvm-readobj --file-headers`
验证为 `COFF-i386` / `IMAGE_FILE_MACHINE_I386`，运行退出码为 0。

fixture 的受限 guest stream 是原始 CCPU 的 `C4 C4 50 14`（selector 50h，DEM date service）
和 `C4 C4 FE` invocation exit；其目标构建图保留完整原始 `bios.c` table、原始 `nt_bop.c` 与 DEM
objects。它因此是历史侧 `CCPU -> BIOS[50h] -> MS_bop_0 -> DemDispatch` 的可执行采证基线。

这**不是** R2 与历史闭包已经桥接的证据：该 fixture 仍使用
`local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c` 提供受控 RAM aperture 与未准入回调的
fail-closed 停止。因此它只能为后续 bridge 提供调用顺序、位宽和错误边界，绝不可进入默认 runtime。

为使该历史 target 在现代工具链中按原声明顺序编译，CMake 仅作两项 compiler-configuration 校正：

- `ntdos64-opennt-sim32-objects` 与 `ntdos64-opennt-ccpu-host-sas-objects` 不再被强制注入
  Win32-first `opennt_stage_preamble.h`；`sim32.c` 本身先包含 `nt.h` 再包含 `windows.h`。
- `ntdos64-opennt-sim32-objects` 显式定义原始平台宏 `_X86_`，使 `nt.h` 包含 `ntx86.h` 中的
  `CONTEXT`/`PCONTEXT` 声明。

两项均不添加 host、BIOS、BOP、DEM 或 DOS 行为。
