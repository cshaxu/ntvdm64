# 历史 Reset Trace 状态

## 2026-08-08 R1 更正

已排除 SAS 未初始化与 `c_sas_overwrite_memory()` 为本次异常根因：trace
在 `config` 和 `c_cpu_init` 后均观察到有效 16 MiB SAS aperture，低内存
类型为 RAM。第一处异常是错误 CCPU/C-VID profile 使 `phy_r8()` 的
`read_pointers` 保持未解析零地址；修正 profile 后该异常消失。

当前 trace 在原始 `init_vga_globals -> setMarkPointers` 停止，原因是
`Video.setMarkPointers` 为零，而非 DOS、BIOS、BOP、DEM 或现代 host
dependency。此目标继续是只记录的 bounded trace；在确认历史 C-VID
向量绑定来源前，不得以本地替代实现推进启动。

2026-08-09 按 NT4 CCPU/C-VIDC 各自的 `ccpudefs.inc` 重新构建后，停点
完全相同。该重复结果说明 profile 校正未掩盖问题；当前待恢复的是完整
`CpuVector` 的历史装配者，而非单个视频回调。

同日的受限 C-VIDC 全表复制实验已撤回，并作为 ABI 证据保留在
`R1-CCPU-HOST-BRIDGE-NEGATIVE-AUDIT.md`。完整 `C_Video` 的 mark 槽确实
非零，但 `ntstubs.c` 的 `Video` 存储来自较短的同名 generated header；完整
复制会越界破坏相邻状态，短表解释则按错误 offset 调用完整表。实验源码、CMake
目标、fixture 接线和 manifest 条目均已移除；reset trace 已重建并回到本节所述
的原始空分派停止点。

状态：已重新建立 `InitialiseDosEmulation()` 的 session/SAS 精确边界，2026-08-08。

## 目的与边界

默认关闭目标 `ntdos64-opennt-session-byob-reset-trace-fixture` 从原始
`InitialiseDosEmulation()` 调用原始 `reset()`，其返回后立即通过 trace harness
停止。它不是 DOS runtime，也不允许本地 BIOS、BOP、DEM 或设备 handler 让 DOS
继续前进。

目标链接原始 CCPU archive、原始 CCPU platform provider 以及历史 provider archive。
为避免未恢复的后续 session callbacks 被自写代码替代，目标以
`/FORCE:UNRESOLVED` 链接：任何这类符号一旦在执行路径上被调用，都不是成功路径，
而是无效的链接陷阱。

## 已获得的证据

构建已通过历史 `reset.c`、BIOS、CMOS、PIC、PPI、timer、keyboard、video、disk、
XMS 等 provider 的对象闭包。为消除重复 owner，仅在该 trace profile 下关闭了原先
为较浅 trace 设置的 fail-closed fallback；对应符号改由其历史对象唯一拥有。

原始 `reset()` 的入口 checkpoint 已以退出码 `0` 到达。最初将停止点放在
`sas_fills(0, '\0', 640L * 1024L)` 之后时，进程未到达该 checkpoint，并以
`0xC0000005` 退出。

随后审计原始 `base/support/main.c` 发现夹具遗漏了历史顺序中的 `cpu_init()`：它位于
`config()` 与 `InitialiseDosEmulation()` 之间。以独立 trace overlay 分别验证
`ccpu386InitThreadStuff()` 和 `c_cpu_reset()` 后，两者都以退出码 `0` 到达。

恢复正确的 `config() -> c_cpu_init() -> InitialiseDosEmulation() -> reset()` 顺序后，
该异常被精确归因：PDB 和反汇编将返回地址映射到 trace harness 中
`InitializeIcaLock()` 调用之后，而 `/FORCE:UNRESOLVED` 将这个未链接的原始调用
变成了无效跳转；它不是 `sas_fills` 的数据访问失败。无 `/FORCE` 的临时链接诊断同时
确认该对象组合遗漏了原始 `gfi_mpty.c::gfi_empty_active()`。

reset trace 现显式组合原始 `nt_eoi.c::InitializeIcaLock()`、其
`ntdll!RtlInitializeCriticalSection` 导入、原始 `gfi.c` 与原始
`gfi_mpty.c`。同时，它选择原始非 `CPU_40_STYLE` `sim32.c` 的
`host_sas_init`/`host_sas_term` 作为 CCPU SAS aperture owner；仅以一个声明式
兼容头把旧 V86 名称 `NtGetPtrToLinAddrByte` 指向已有的
`ccpusas4.c::c_GetLinAdd` 合同，不含分配、转换或设备实现。

此前的构建将 `NTDOS64_RESET_TRACE_STOP_STAGE` 仅传给 executable harness，遗漏了
独立编译的 `reset.c` object。故该 object 采用默认值 `1`，而 harness 等待配置值，
checkpoint 没有停止；此前的退出码 `0` 或后续 COM 初始化访问异常只能作为“执行曾越过
前段 reset”的弱证据，不能作为各 stage 精确停止或返回的验收。该配置错误已修正：同一
CMake 变量同时传给 reset object 与 harness。所有 stage 结论将从 stage 8 的重新执行起
重新记录。

## 已重新验证的 session/SAS 边界

此前 reset stage 的精确结论因 object/harness 宏作用域不一致而全部降级为弱
reachability 证据；本记录不再将它们用作 reset stage 的验收。修正后，
`NTDOS64_SESSION_TRACE_VINT_STOP_STAGE` 同时传入独立编译的历史
`nt_msscs.c` object 与 harness，并以三个默认关闭的 checkpoint 重新执行：

| checkpoint | 历史语句位置 | 结果 |
| --- | --- | --- |
| 1 | `sas_loads(FIXED_NTVDMSTATE_LINEAR, ...)` 之前 | 退出码 `0` |
| 2 | 同一 `sas_loads` 返回之后 | 退出码 `0` |
| 3 | `sas_storedw(FIXED_NTVDMSTATE_LINEAR, fVirtualInt)` 返回之后 | 退出码 `0` |

这证明原始 `InitialiseDosEmulation()` 已通过 CCPU 的既有 SAS 路由完成固定
NTVDM virtual-interrupt 状态字的读取与写回；没有替代 SAS、BOP、BIOS、设备或 DOS
语义。随后，独立的默认关闭目标
`ntdos64-opennt-session-byob-io-trace-fixture` 调用原始
`base/support/ios.c::io_init()` 并在其返回后立即停止，退出码同为 `0`。这确认 SoftPC
I/O 路由表初始化已走原始 owner；没有本地端口表、设备或 BOP 替代物参与。

随后对 TLS 后进入的原始 `reset()` 重新建立了精确边界。此前该 trace 已接入原始
`cmosnt.c`，但尚未选择其 `rtc_init()` 所需的历史 host owner
`host/src/nt_timer.c::host_GetSysTime()`，因而 `/FORCE:UNRESOLVED` 陷阱曾落在
`cmos_init()` 的返回地址附近。该地址起初被错误地关联到 DMA 的 PDB 行表；经反汇编、
COFF relocation 和 frame return-address 映射确认，真实顺序是：

```text
InitialiseDosEmulation -> reset -> cmos_init -> rtc_init -> host_GetSysTime
```

`host_GetSysTime` 的原始 owner `nt_timer.c` 已被加入该 default-disabled composition；没有
新增 RTC、DMA 或时钟 shim。首次接回后发生的 `0xC0000094` 除零同样不是 host API 缺失：
原始 `obj.vdm/ntvdm.c::main()` 在调用 `host_main()` 前先调用 `TimerInit()`，而该步骤初始化
性能计数器频率和两个尚未启动的同步事件。trace harness 现仅在 reset profile 按原始顺序调用
`TimerInit()`，不会创建 heartbeat thread。

在修正一个纯 instrumentation 验收顺序问题后，以下 stop 已重新构建并运行：

| reset stage | 原始边界 | 结果 |
| --- | --- | --- |
| 8 | `cmos_init(); cmos_post();` 返回之后 | `trace-stop result=108 reason=108`，退出码 `0` |
| 9 | `ppi_init(); ppi_get_switches(...);` 返回之后 | `trace-stop result=109 reason=109`，退出码 `0` |

因此，当前精确证明的是：在原始 `TimerInit -> InitialiseDosEmulation -> reset` 顺序下，
CMOS/RTC POST 与 PPI/开关读取均由历史 owner 完成。随后将同一 trace 的停止边界推进到
stage 10（视频初始化之后）时，未到达 checkpoint，而是在原始
`vga_init() -> init_vga_globals()` 的第一组 video-global 初始化调用中进入
`/FORCE:UNRESOLVED` 跳转。PDB、反汇编和源代码共同将首个返回地址归因到
`base/video/ega_prts.c::init_vga_globals()`，而不是键盘、PIC、CMOS 或某个现代宿主 API。

该陷阱揭示的是一个已知、但现在被 live caller 证实的 CCPU/C_VIDC **历史构建 profile
缺口**：reset trace 选择了 `vga_init()` 所在的原始 video provider，却没有同时选择
`ccpu386/vglob.c` 的 direct-global accessor 及其同 profile 的 storage/generated-ABI
上下文。`cvidc/vglfunc.c` 提供的 `C_Video` vector 不等价于 `vglob.c` 提供的
`get/setVideo*` owner；以旧的 `ccpu_vidc_global_storage.c` adapter、零表、手写 getter/
setter 或 video shim 让它继续，都会掩盖这一事实，均被禁止。

下一步不是新增设备语义，而是 caller-first 复核 `obj.vdm/cdefine.inc` 的
`MONITOR/C_VID/X86GFX` 与非 x86 `CPU_40_STYLE + CCPU + C_VID` profile 分支，确定软件
CCPU reconstruction 所需的完整、可复现的 video/global-data 编译组合。只有在原始 source
输入或可追溯生成输入足以闭合该组合后，才允许把相应原始对象加入 reset trace。任何后续依赖
仍须先由原始 source owner 闭合；只有已确认在现代宿主缺失的 OS seam 才可提出窄 shim，且不得
把异常或 `/FORCE:UNRESOLVED` 陷阱解释为成功。

后续 profile 复核已得到两项正面、但仍不足以宣布 stage 10 成功的结果：

1. `init_vga_globals()` 的第一批 `get/setVideo*` 调用由原始
   `ccpu386/vglob.c` 提供；该 object 可以直接以固定 clang-cl island 编译。它最初因
   `Gdp` 尚未建立而写入低地址。原始 `base/support/main.c` 显示，在 `cpu_init()` 后、
   `host_main()` 前必须调用 `setup_global_data_ptr()` 和 `setup_vga_globals()`；trace 现按该
   原始顺序调用 C_VIDC `ev_glue.c` 的两个 owner，未分配或伪造本地 workspace。
2. `ev_glue.c` 与 `ntstubs.c` 都定义 `Gdp`，不能共同拥有 live storage。固定源树中
   `Cpu_define_outb` 仅由 `ntstubs.c` 提供，且该函数不读取 `Gdp`。reset trace 因而仅将
   `ntstubs.c` 的重复 storage 定义改名，保留其原始函数体，而 C_VIDC `ev_glue.c` 成为唯一
   live `Gdp` owner。video provider 也按历史 `C_VID` 条件重编译，消除了先前对零初始化
   `Video` table 的间接跳转。

当前新的停止点位于原始 `ev_glue.c::setMarkPointers()` 进入 generated C_VIDC
`S_2126_SimpleMark` 的路径。它随后以零目标跳转而停止；这不是可以用 mark callback 或
显示 shim 填补的接口。下一项调查必须还原该 generated C_VIDC path 的初始化/数据合同，并用
原始 caller 证明需要哪一段生成输入或初始化顺序。stage 10 至今仍是未通过状态。

## Reset 组合的 no-force 诊断

为避免把某一个 `/FORCE:UNRESOLVED` 陷阱误认为唯一阻塞，当前 reset composition 另以
相同 object 集合进行一次不生成可运行产物的 no-force 链接诊断。它确认的未闭合集合分为：

| 类别 | 当前历史调用者 | 首选处理 |
| --- | --- | --- |
| GFI 软盘存在性/变更 | `gfi.c` | 先审计原始 host GFI owner；现代介质策略只能在其后作为显式 seam |
| COM 轮询、idle、错误显示 | `nt_com.c`、`nt_eoi.c` | 保留原始 Windows COM owner，逐项审计其现代宿主依赖 |
| 磁盘读写/seek | `diskbios.c`、`fdisk.c` | 先接入原始 NT disk backend，不以虚拟介质替代物闭合 |
| 版本、版权、reset 宿主钩子 | `reset.c` | 单独归类为 host policy seam，不能与 BIOS/CMOS 语义混合 |
| Sim32 SAS facade | `sim32.c` | 由原始 CCPU/SAS owner 与历史 ABI 路由闭合，不新增指针或内存替代层 |

此前已将原始 `nt_com.c`、`ccpusas4.c` 与窄 `diskbios.c` source owner 纳入该 default-disabled
trace；随后又按实际 caller 接入了原始 `nt_timer.c`。旧 `/FORCE` 陷阱与其 PDB 误映射均已被
上述 RTC 调用链归因取代。后续仍以 no-force 清单和原始调用顺序决定最小下一项，不得为追求
更高 reset stage 而批量加入或自写这些能力。

随后已将原始 `nt_fdisk.c` 与 `nt_rflop.c` 纳入该 trace，作为 GFI 和固定盘回调的唯一
owner；为保持单一职责，reset trace 已移除 trace-only 的固定“无软盘”函数，并将旧
`host_*fdisk*`/`*TerminatePDB` 占位定义改名为未使用的 trace 符号。构建通过，运行期
陷阱已前移至 `RVA 0x56cde`。这只是 source-owner 闭合证据，不意味着已安全地执行任何
DOS 磁盘工作负载或到达 reset stage 8。

`RVA 0x56cde` 的 PDB 行表落在 `at_dma.c::dma_page_outb()` 的纯 page-register 赋值分支，
但该原始 object 的重定位表在该函数中不含外部调用；因此该映射本身不能作为 DMA 缺失或
DMA shim 的依据。已用 `/INCREMENTAL:NO` 重建，地址仍相同，排除了增量 PDB 残留但没有
消除该语义歧义。下一步须从 no-force unresolved 集合及实际调用入口建立显式归因，不得
按此地址猜测、重写或替代 DMA 语义。

## 后续 reset 调查

overlay 已位于
`src/opennt/overlay/base/mvdm/softpc.new/base/bios/reset.c`，保持原始主体，仅以专用
trace compile profile 在低内存清理、IVT、双 PIC、CMOS、PPI、timer、keyboard、video、
disk POST 与 `host_reset` 后记录。该对象只服务 reset trace target，不能取代正式
historical provider archive。在 `io_init()` 的原始职责链可观测且停止边界重新建立之前，
不重新宣称任何 reset stage 的精确返回。之后才按原始调用顺序从 stage 1 至 12 逐项记录。
不得以本地 IVT/PIC/CMOS 写入、自写设备语义或替代 BIOS/CPU reset 绕过它。

复现命令：

```powershell
cmake -S src/opennt -B artifacts/build/current/opennt-r5-clang-x86
cmake --build artifacts/build/current/opennt-r5-clang-x86 --target ntdos64-opennt-session-byob-reset-trace-fixture
artifacts/build/current/opennt-r5-clang-x86/ntdos64-opennt-session-byob-reset-trace-fixture.exe
```

## 2026-08-08 R1 Stop-Point Correction

当前 reset trace 的最新构建不再以此前记录的 C_VIDC generated-mark 跳转作为活动停止点。
`obj.vdm/CDEFINE.INC` 与 `ccpu386/ccpudefs.inc` 证明，历史 x86
`MONITOR/C_VID/X86GFX` 路径和 CCPU `CPU_40_STYLE` 软件解释器是不同的 profile；
不能把它们混合为一个“x86 CCPU”对象集合。一次这样的 build probe 已在 CCPU 自身的
interrupt ABI 定义上编译失败，且已完全撤回。

在恢复一致的 CCPU profile 后，default-disabled fixture 的可复现观察为：

```text
after-config:     Start_of_M_area != NULL, size = 0x01000000
after-cpu-init:   Start_of_M_area != NULL, size = 0x01000000
SAS types:        0x00000, 0x00714, 0x9ffff, 0xa0000 are all SAS_RAM
first exception:  ccpusas4.c::c_sas_overwrite_memory, RVA 0x39399
caller chain:     scs_init -> bios_write_double -> c_sas_PRS ->
                  c_sas_PRS_no_check
```

`c_sas_overwrite_memory` 在原始 CCPU source 中是“invalidate compiled code”的空实现；
因此此处的 null-target access 不能被解释为缺少 SAS RAM、video callback、DOS service
或 host capability。当前要审计的是该 call/return 的 ABI、对象组成及 stack layout。
不得以 local no-op、video fallback、SAS pointer replacement 或 V86 profile 切换绕过它。
`docs/R1-CCPU-PROFILE-SEPARATION.md` 是此 profile 边界的依据。
