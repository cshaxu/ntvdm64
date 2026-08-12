# CCPU X86 配置 Profile Trace

状态：已验证的默认关闭研究目标，2026-08-08。

## 目的

本记录隔离一个容易误判的历史构建差异：`CPU_40_STYLE` 使原始
`config.c` 在 `sas_init` 后调用 `InitNtCpuInfo`。该函数服务于 NTSD 的寄存器
观察，不是 DOS 启动、BOP、DEM、BIOS 或 guest 文件加载的服务。

探针不修改历史 `config.c`，也不以自写 DOS 行为绕过它；它仅建立一个不定义
`CPU_40_STYLE` 的 CCPU 软件执行研究 profile，让原始条件编译自然选择其另一
分支。

## 输入与构建边界

- 原始输入：`src/opennt/overlay/base/mvdm/softpc.new/host/src/config.c`。
  overlay 只保留已登记的可构建性变更；本次没有改动该文件。
- 新对象目标：`ntdos64-opennt-config-x86-prefix-objects`。
- 新 trace 可执行目标：`ntdos64-opennt-config-x86-prefix-trace-fixture`。
- CMake 选项：`WITHOUT_CPU_40_STYLE`。其余 CCPU 研究宏保持为
  `NTVDM CCPU PROD DEVL=1 SPC386 NEW_CPU ANSI i386`。
- 范围：均为 `EXCLUDE_FROM_ALL` 的历史输入/trace 目标，不连接
  `ntdos64-run.exe`，不构成默认 engine 或运行时组件。

## 执行合同

1. 通过原始 `config()` 进入。
2. 原始 `config.c` 调用原始 `sas_init`；SM-0 foundation 仅提供有界、零初始化
   的 RAM aperture 和 `host_sas_init` / `host_sas_term` 所需 host 接缝。
3. 因未定义 `CPU_40_STYLE`，原始预处理条件排除 `InitNtCpuInfo`。
4. 首个探针以 `InitUMBList` 作为下一道原始 host 生命周期边界，由 trace guard
   记录后非返回停止。
5. 后续 UMB 探针改为连接原始 `nt_umb.c`。在 CCPU 的非 `MONITOR` 分支中，
   原始 `InitUMBList` 建立 `C0000-100000` UMB 所有权链，其中 EGA/BIOS ROM
   区被标记为 `UMB_OWNER_ROM`；其后原始 `host_runtime_init` 正常返回。
6. 两个夹具都会调用原始 `sas_term`，确认 RAM aperture 已释放，并验证 console title 与
   临时 PIF 均已恢复/删除。

trace guard 只能记录、断言、停止；它不能提供 UMB、设备、BIOS、BOP、DEM 或 DOS
响应。

## 验证证据

在 `artifacts/build/current/opennt-r5-clang-x86` 中，使用 x86 MSVC 环境运行：

```text
cmake -S src/opennt -B artifacts/build/current/opennt-r5-clang-x86
cmake --build artifacts/build/current/opennt-r5-clang-x86 --target \
  ntdos64-opennt-config-x86-prefix-trace-fixture
artifacts/build/current/opennt-r5-clang-x86/\
  ntdos64-opennt-config-x86-prefix-trace-fixture.exe
```

两个目标均构建成功且退出码为 `0`：

- `ntdos64-opennt-config-x86-prefix-trace-fixture`：在 `InitUMBList` 前停止；
- `ntdos64-opennt-config-x86-umb-trace-fixture`：通过原始 `InitUMBList` 与
  `host_runtime_init`，在原始 `gfi_floppy_active` 调用点停止。

后者为使 `nt_umb.c` 的未执行 `ReleaseUMB` 分支可以链接，保留一个
`xmsReleaseUMBNotify` fail-closed guard。该 guard 不提供 XMS 行为；若被命中即
停止，因此不能被当作 XMS 兼容性证据。

这些结果只证明上述有限控制流，不证明 DOS 程序、NTIO.SYS、NTDOS.SYS 或完整
VDM 已经运行。

## 后续问题

下一项研究对象是原始 `gfi_floppy_active` 的实现和它所需的配置/媒体 host
契约。若没有完整证据，不允许将其改写成自定义 UMB、磁盘或借自写 BOP/DEM
handler 跳过。
