# R3 Direct CCPU Linker Unresolved Inventory

状态：进行中，2026-08-08。

## 范围

本文记录默认关闭的
`ntdos64-opennt-direct-ccpu-full-source-closure-fixture` 的诊断边界。该 target
只用于把原始 OpenNT CCPU/SoftPC/BIOS/BOP/DEM 组合推进到编译和链接诊断；不执行
guest，不是 `ntdos64-run` 的依赖，也不构成研究型 `ntvdm.exe` 的 runtime 路径。

## 已冻结的链接快照

在尚未接入 `nt_sound.c`、`copy_fnc.c`、`nt_error.c` 三个已有 OpenNT host owner 的
快照中，链接器报告 146 个唯一未解析符号。接入这三个 owner、并以受管
compiler overlay 排除现代 SDK/CRT 名称冲突后，再接入原始 `nt_keycd.c` 与
`nt_nls.c` source owner，随后又将原始 `nt_cpu.c` 的 CCPU wrapper profile 和
原始 `nt_inthk.c` 的 CCPU interrupt-hook owner 及 `sim32.c + nt_mem.c` 的 SAS
allocation owner 接入 closure，当前链接器报告 117 项。当前原始文本
保存在 `artifacts/build/current/direct-ccpu-linker-unresolved.txt`。该清单不能被解释为
117 个缺失实现，初步按所有权分为：

| 类别 | 代表符号 | 当前结论 |
| --- | --- | --- |
| 已知缺失 CCPU vector | `c_sas_touch`、`c_VirtualiseInstruction` | 仍是缺失语义候选；不得用 archived guard 或自写 monitor 充当实现。 |
| 原始 SoftPC host owner | `host_clear_hw_int`、`host_alarm`、`host_direct_access_error` | 先确认其原始 source/profile；不能误标为 core 或 shim 缺失。 |
| C_VIDC/生成 ABI | `getVideo*`、`setVideo*`、`setup_vga_globals`、`ega_colour_hi_munge*` | 需要匹配的历史生成 header/object profile；不是可凭命名手写的普通函数。 |
| 历史私有 console/VDM API | `VDMConsoleOperation`、`RegisterConsoleVDM`、`ReadConsoleInputExW`、`ExitVDM` 等 | 现代 host seam 候选；必须逐项以真实 caller、输入、失败和返回合同审计，当前不得实现。 |
| NT native/CRT/import 边界 | `Rtl*`、`Nt*`、`DbgPrompt`、`__sys_errlist` | 先确定历史 import library、SDK 声明与现代导出可用性；不能改写为 DOS 行为。 |
| VDM physical-page record bridge | `VdmSetPhysRecStructs` | `nt_mem.c` 在 host-backed remap/remove 时替换 physical-page translation records；现代 host capability 候选，不能以空回调通过。 |
| 历史辅助 owner | `DisplayErrorTerm`、`BiosKeyToInputRecord`、`ActivityCheckAfterTimeSlice` 等 | 从原始 build 图定位 source owner 后才可加入 closure。 |

### 2026-08-08 快照的机械分组

对 `direct-ccpu-linker-unresolved.txt` 的 117 个唯一符号按符号族重跑分类，结果如下。
这一步只缩小审计面，不授予任何实现或 runtime 准入；真正的 live reachability 仍需由
`io_init -> reset -> scs_init -> NTIO` 的原始 caller trace 决定。

| 符号族 | 数量 | 已知边界 | 当前处置 |
| --- | ---: | --- | --- |
| C_VIDC direct-global ABI（`getVideo*`、`setVideo*`、`setup_*`、`ega_colour_*`） | 47 | `vglob.c` 已定位，但依赖未复原的 `localfm.c`/generated storage profile | profile gap；不手写 accessor，不进启动闭合。 |
| 历史 console/session API（含 `GetNextVDMCommand`、console input/display、`SetVDMCurrentDirectories`） | 18 | 现代 Windows 已不提供同一私有 ABI | 仅当 live caller 抵达时逐项审计为候选 host seam。 |
| monitor/debugger generated ABI（`InitNtCpuInfo`、`VdmTib`、`get/setE*`、debug context） | 21 | 当前 direct-CCPU profile 与 RISC/debugger generated layout 不匹配 | 不能用本地 register storage 或 mode logic 伪造。 |
| NT native API/import（`Nt*`、`Rtl*`、`DbgPrompt`） | 14 | historical import/SDK 和现代导出边界尚未逐项比对 | 先追 original caller/import library；不是 DOS 服务缺失。 |
| 缺失或异 profile monitor 语义（`c_sas_touch`、`c_VirtualiseInstruction`、fast BOP、EMS/time-slice slots） | 9 | 有的无同代实现，有的只见 V86 owner | caller-first 缺失语义调查；未知输入一律 fail-closed。 |
| VDM memory/context host ABI（`VdmSetPhysRecStructs`、`ClearInstanceDataMarking`） | 2 | host-backed remap/context 生命周期 | 仅在原始 caller 实际抵达时设计原函数边界的 capability。 |
| CRT/diagnostic import（`trace_file`、`sys_errlist`/`sys_nerr`） | 4 | 旧 CRT object/import 边界 | toolchain/import 审计，不是 runtime 功能补件。 |
| keyboard generated table（`nt_keybd_funcs`） | 1 | 键盘 provider/generated-table 边界 | 不因 link closure 直接引入输入行为。 |
| native list macro ABI（`InitializeListHead`） | 1 | historical native declaration/import 形状 | 在相关 caller reachability 前不单独补入。 |

该分组共计 117 项。它将当前下一步限制为：先从实际启动 trace 判断哪些簇被触及；
不得把 C_VIDC、debugger、私有 console 或 V86-only 残留当作“缺几个 shim”而整体补齐。

## 当前编译边界

为区分“漏接原始 owner”和“真正缺失”，fixture 已显式引用四个原始 OpenNT object
group。`nt_cpu.c` 首先暴露的不是链接错误，而是历史编译 profile：

1. 它以 OpenNT `nt.h` 开始，故必须避免 Win32-first preamble 与 `ntdef.h` 冲突。
2. 它在 `CPU_40_STYLE` 下使用 `Constraint*` 与 `GLOBAL_*` C_VIDC/CCPU 生成 ABI。
3. `base/mvdm/softpc.new/obj.vdm/cdefine.inc` 说明历史 x86 build 使用
   `MONITOR`、`C_VID`、`X86GFX`；通用 non-V86 closure profile 则使用
   `CPU_40_STYLE` 与架构特定生成 headers。
4. 因此，当前阶段不能把 `nt_cpu.c` 当作普通 host callback 文件接进来。必须先恢复
   它所要求的完整、同代 generated ABI profile，或证明 direct-CCPU profile 不应消费
   这个 RISC-oriented owner。

这是此前的 fail-closed 编译结果；随后依据历史 build 宏将 `nt_cpu.c` 排除出 direct
closure，构建已重新到达链接器。整个过程中没有采用旧的 `ccpu_host_*`、
`ccpu_monitor_*`、BOP/DEM handler 或任何自写 console/VDM replacement 来压过错误。

### Generated profile 追溯

`nt_cpu.c` 的实际 include trace 为 CCPU `cpu4gen.h/gdpvar.h` 与
`host/inc/mips/prod/cpu_c.h` 的组合。后者的首行条件是
`#if !defined(CCPU) || defined(PIG)`，故 retail `CCPU + PROD` profile 不会导出
`ConstraintR*` 枚举；而 `nt_cpu.c` 的 `CPU_40_STYLE` 分支又直接使用这些枚举。

这不是可以定义 11 个整数来“修好”的问题：这些 bit 编号属于同一生成 ABI，必须与
`cpu4gen.h`、`gdpvar.h`、对象侧的 C_VIDC data layout 一起匹配。将
`base/cvidc` 放到 include 前端会改为另一套 `gdpvar.h` offset，已被撤销。

对本地比较树的核验结果：OpenNT 4.5 的
`host/inc/mips/prod/cpu_c.h` 与固定 NT4 输入 SHA-256 相同
(`03AA7B45CD9DA10D327F3393D3D8BFB99FC2D30070091CB36CE12AAAE52A0A36`)；
XP 树没有该 RISC generated-header 集合。因此比较树目前不能提供一个可直接、可信
导入的替代输入。

随后对 `obj.vdm/cdefine.inc` 和 `host/src/sources` 的复核给出最终归属：虽然
`nt_cpu.c` 位于共同 host source list，NT4 x86 build 的宏为
`MONITOR + C_VID + X86GFX`，而非 `CPU_40_STYLE + CCPU`。后者是 RISC software
profile。由于 direct closure 的研究对象是无 V86 的 x86 CCPU 语义恢复，不能混用
这两条历史 ABI；`nt_cpu.c` 当时曾从 direct closure fixture 移出，作为独立 historical
reference compile gate。当时 direct closure 的有效链接基线为 133 项；后续已按本报告
记录的非 `CPU_40_STYLE` CCPU wrapper profile 重新受限接入。

键盘/NLS source owner 曾使该基线由 133 降至 125；这验证了
`KeyMsgToKeyCode`、`BiosKeyToInputRecord`、`aNumPadSCode`、`host_nls_get_msg`
及其 message string 的历史所有权。已将原始 `ntdos64-opennt-cvidc-archive` 加入
同一诊断链接，但 `get/setVideo*`、`setup_vga_globals` 等符号仍未消失。因此它们不是
遗漏该 archive，而是必须按 historical generated vector/bridge 输入继续审计；不得以
自写视频访问器替代。

### C_VIDC 实物 archive 复核

本阶段直接检查已构建的 `ntdos64-opennt-cvidc-archive.lib` 与其
`ev_glue.c.obj`。archive 的确被 direct-closure target 的 link line 引用，且导出
`_C_Video` 和成组的生成入口，例如
`_S_0383_CiGetVideomark_word_00000014`、
`_S_0415_CiSetVideofwd_str_read_addr_00000034`。它不导出 literal
`_getVideo*` / `_setVideo*` / `_setup_vga_globals` 名称。源头也印证这一点：
`base/cvidc/evidgen.h` 把 video access 表达为 `VideoVector` 的函数指针，
`base/cvidc/vglfunc.c` 提供 `C_Video`；这是生成 C_VIDC ABI，而当前未解析项属于
另一套 direct-global ABI。

后者的原始 source owner 已定位为
`base/mvdm/softpc.new/base/ccpu386/vglob.c`：该文件逐一给出所有
`getVideo*` / `setVideo*`，并在 `CCPU + C_VID` 条件下给出
`setup_vga_globals` 与 `setup_global_data_ptr`。但它读取 `GLOBAL_VGAGlobals`/
`Gdp` workspace；对应的 `Gdp`、`Cpu`、`Video` storage 原本由同 profile 的
`ccpu386/localfm.c` 组合提供。两个 unit 都不在固定 archived CCPU response-file
profile 内。

仓库现有的 video-closure fixture 因此把这作为单独 profile increment 研究，其中
`local/archived/legacy-adapters/ccpu_vidc_global_storage.c` 只为 clang-cl 复现
`localfm.c` 缺失的 storage 声明。依据 M92，该文件是 archived adapter，不能加入
direct full-source closure，更不能进入 runtime。结论是当前 117 项诊断中这组符号的
所有权已经闭合，但其原始构建 profile 尚未闭合；下一步必须寻找或重建有来源证据的
`localfm.c` 编译上下文，不能以这份 adapter 或手写 getter/setter 取得表面链接成功。

### `localfm.c` 原始编译上下文复核

`localfm.c` 在固定 NT4、OpenNT 4.5 与 XP 比较树中均为完全相同的 169-byte 文件，
SHA-256 均为
`5DBB2BD74E82D5B87910DB81F1411610C84A2D57A553E8CB78080871D19835DF`。四份树的
可见 `sources`、makefile、response-file 与 command 文件均没有列出该 unit 或其专属
`CPU_PRIVATE`/PCH 组合；固定 NT4 CCPU response-file 也明确不包含它。因此后续版本
不能提供可导入的编译期补件。

已记录两类不链接、不运行的 x86 clang-cl 探针，输出在
`artifacts/build/current/localfm-original-compile-probe.txt` 和
`artifacts/build/current/localfm-sascdef-context-probe.txt`：

1. 按 current CCPU profile 原样编译，以及仅增加 `CPU_PRIVATE`，均只报
   `struct SasVector` 与 `struct VideoVector` 未完成。这证明局部宏不是充分历史上下文。
2. 试图把同目录原始 `sascdef.c` 的完整 include 顺序预注入时，`localfm.c` 自身再次
   include 无 guard 的 `host_def.h`，产生 `quick_event_delays` 重定义。该结果不能被
   解释为 source 缺失，也不能以任意重排/自制 PCH 变成历史 profile 证据。

故本轮不新增 `localfm.c` overlay，也不改变 archive 选择。它仍是“已定位、未能以
固定输入复原其历史编译上下文”的 profile gap；`ccpu_vidc_global_storage.c` 继续仅作
archived fixture 证据。要推进此项，需找到原始 PCH/编译命令或从相同生成系统恢复它，
并证明其与 `sascdef.c` 的 `Sas` 所有权不发生重复定义，然后才可考虑替换 adapter。

### 同名 host CPU wrapper 的受限接入

`host/src/nt_cpu.c` 的 `host_start_cpu`、`host_simulate` 与 `host_set_hw_int`
在排除 RISC `CPU_40_STYLE` register facade 后，是原始 CCPU 的窄 wrapper：前两者
调用 `cpu_simulate()`（CCPU profile 中为 `c_cpu_simulate()`），后者调用
`cpu_interrupt(CPU_HW_INT, 0)`。其 CCPU branch 不读取 `Start_of_M_area` 或
`Length_of_M_area`；同名状态已由 CCPU SAS archive 所有。

受管同路径 overlay 只在 `CCPU` 下排除这两个重复 host-memory 声明，随后 object
gate 通过，并被加入 link-only closure。链接器从 125 项降至 120 项，且
`host_start_cpu`、`host_simulate`、`host_set_hw_int` 已从 unresolved inventory 消失。
这只恢复 wrapper 的 source ownership，不能证明递归调用可安全执行：终止、重入、
ROM/SAS 状态与 interrupt delivery 仍须由独立 bounded fixture 验证。

### 原始 CCPU interrupt-hook owner

`host/src/nt_inthk.c` 已存在于固定 OpenNT host `sources` 清单中。其
`host_swint_hook` 与 `host_exint_hook` 只在 `CCPU` 编译条件下读取由
`VdmInstallSoftwareIntHandler` / `VdmInstallFaultHandler` 保存的历史函数指针；没有
安装 handler 时二者均返回 `FALSE`。因此 CCPU 的 `intx.c`、`into.c`、`c_xcptn.c`
会继续其原有的软件中断或异常路径，而不会由本项目伪造处理结果。

该源以 `_X86_`、`CCPU` 和原始 `nt.h -> ntrtl.h -> nturtl.h -> windows.h` 声明顺序
作为独立 `EXCLUDE_FROM_ALL` object gate 编译；它只被 link-diagnostic closure 引用。
2026-08-08 的重建通过，随后 closure 的未解析集合从 120 降至 118，两个 hook 符号
均从清单消失。没有新增 overlay、shim、BOP/DEM handler 或执行目标。CPU mode、
handler 安装的实际 host contract 和异常恢复仍未获 runtime 准入。

### 原始 SAS allocation owner

`host/src/sim32.c` 在 `CPU_40_STYLE + SIM32` profile 中实现
`host_sas_init` / `host_sas_term`，并把 Intel-address-space reserve/commit 的具体工作
交给同一 host source list 的 `nt_mem.c`（`InitIntelMemory` / `FreeIntelMemory`）。该 pair
与 `ccpu386/ccpusas4.c` 的既有调用顺序一致：CCPU 仍设置 `Start_of_M_area`、SAS vector
和 ROM 状态；host pair 只管理预留、提交和释放的 Intel memory region。

`sim32.c` 还导出两项 Sim32 pointer bridge，但 active closure 已由原始
`dpmi32/i386/dpmi386.c` 提供它们。为避免两个历史 provider 同时定义一个 ABI，新 SAS
object gate 仅将这两个未选中导出重命名为诊断私有名；SAS allocation 代码本体未改。该
gate 和 `nt_mem.c` 均为 `EXCLUDE_FROM_ALL`，仅服务 link-diagnostic closure。

2026-08-08 的对象构建通过，closure 中 `host_sas_init` / `host_sas_term` 不再未解析，
集合由 118 变为 117。新出现的 `VdmSetPhysRecStructs` 来自 `nt_mem.c` 的
`VdmAddVirtualMemory` / `VdmRemoveVirtualMemory` 路径：在映射变化后替换 physical-page
translation record。它必须保留为明确的现代 host ABI seam，不能由 no-op shim、fixture
或 CCPU SAS 代码取代。

#### `VdmSetPhysRecStructs` 的最小合同证据

固定 OpenNT `nt_mem.c` 给出了调用签名和严格顺序：

```c
VdmSetPhysRecStructs(ULONG host_address, ULONG intel_address, ULONG size);
```

`VdmAddVirtualMemory` 先将 host address 向下对齐到 DWORD、将 size 向上对齐到页，成功
预留 Intel address range 后调用 `sas_overwrite_memory`，随后以
`(host_address, intel_address, page_rounded_size)` 调用该接口，最后才置
`HDR_REMAP_FLAG`。`VdmRemoveVirtualMemory` 则先 flush cache，再以
`(intel_base + intelMem, intel_base, recorded_size)` 调用同一接口，清除 flag 后才释放
Intel range。故未来 capability 的实现至少必须保证：对齐后的整个 range 一次替换、失败不
留半更新 translation record、移除以原 Intel range 恢复指向 Intel backing memory。

该接口未在固定 OpenNT、OpenNT 4.5 或本地 NT5 比较树中找到 source definition；它不是
`VdmAddVirtualMemory`/`VdmRemoveVirtualMemory` 的一部分，也不应被混同为 DOS/BOP/BIOS
服务。它目前只服务 RISC/WOW host-backed DIB/remap 场景，尚无 NT4 x86 CCPU normal-runtime
准入证据。

### `ActivityCheckAfterTimeSlice`：缺失的 CPU activity 语义

该符号是 CPU generated vector 的无参 slot，而非普通 console 或 sleep helper。固定 OpenNT、
OpenNT 4.5、NTVDMx64、本地 NT5 比较树均只保留声明、slot 名和调用点，未找到 source
definition。已知调用时序有三类：`nt_unix.c` 在 `idletime` 分支完成等待或 `Sleep(0)` 后
调用；`nt_eoi.c` 在 heartbeat 阈值到达时先释放 ICA lock，再调用并立即返回。因而实现若
存在，至少处于 CPU activity accounting、time-slice 结束和 interrupt/idle 交接边界。

这不足以重建其状态、重入或 IRQ 可见性合同。特别是将其替换为 no-op、`Sleep(0)`、直接
timer tick 或 BOP handler 都会改变上述调用后的调度/中断时序。它保持 unresolved，直至
取得同代生成输入、可观察的行为证据，或通过独立的 caller-first trace 证明所需语义。

### Opaque SAS slot 的来源恢复结论

`c_sas_touch` 与 `c_VirtualiseInstruction` 不是任意 host callback。`ccpu386/sascdef.c`
将它们固定放在 `struct SasVector cSasPtrs` 的最后三个槽中，顺序为
`c_sas_touch`、`c_IOVirtualised`、`c_VirtualiseInstruction`；`ccpusas4.c` 的
`SasSetPointers()` 整体复制该 vector。因此，任何补全都必须保持这一个既有 ABI，
不能新增平行接口、调换顺序或把其中任意一个改成 DOS/BIOS dispatcher。

非 x86 的保留 generated header 给出了两个缺失槽的准确函数类型：

```c
IU8 *TYPE_sas_touch(IU32 addr, IU32 length);
IU32 TYPE_VirtualiseInstruction(
    IU32 eipInRom, IUH size, IU32 linearAddrOrPort, IU32 dataIn);
```

其中 `sas_touch` 的返回指针、以及 `VirtualiseInstruction` 的四个标量输入可作为 ABI
证据，不能被误读为已知语义。x86 的 `sas4gen.h` overlay 同样保留了相同的 macro
调用形状。对固定 NT4、NT4.5、XP 和本地导入树的精确检索显示：除 generated header、
`sascdef.c` 的 extern/table entry 外，没有任何 C 调用点、实现、测试或可归属 generator
输入；V86 `monitor/i386` 树也没有这些符号的文字实现。故 V86 monitor 不能为它们提供
可直接迁移的实现。

这轮 source recovery 已不能从可用本地材料推出两个 slot 的副作用、错误、重入或 ROM/SAS
一致性合同。它们维持 default-disabled fail-closed guard；正常启动链应先以 trace 证明
没有到达这两个 slot。只有取得原始 generator 产物，或合法 trace 给出完整输入/输出与
状态副作用合同后，才可以开始重建。

### `localfm.c` 生成上下文的追加跨树核验

在既有固定 OpenNT、OpenNT-4.5 与 XP 文本比对之外，本轮又对
`D:\home\repos.hobby\opennt`、`D:\home\repos.hobby\opennt-4.5`、
`D:\home\repos.hobby\win2000src\private`、`D:\home\repos.hobby\winxpscodes` 和
本仓库固定 `src/opennt` 递归检索了 `localfm`/`sascdef` 以及它们可能留下的
`.obj`、`.pch`、`.pdb`、`.lib` 输入。三个可比较源码树仍只给出同名
`ccpu386/localfm.c`、`vglob.c`、`sascdef.c` 与非 x86/cvidc generated header；五个树均无
`localfm.obj`、`localfm.pch`、`localfm.pdb`、`localfm.lib` 或相应 `sascdef` 构建产物。

这不是“已找到可接入 object”的证据，反而排除了当前本地输入中存在可复用历史生成/PCH
产物的可能。因此 C_VIDC direct-global 簇继续停留在已定位 owner、未闭合 profile 的状态；
不得用 `ccpu_vidc_global_storage.c` 或手写 `GLOBAL_*` offset 代替该缺失输入。

### LIM write-fault 入口是跨 profile 残留

`LIM_b_write` 与 `LIM_w_write` 的文本 owner 是
`base/mvdm/softpc.new/base/dos/emm_mngr.c`，该 object 已经进入 direct-CCPU closure
fixture。对实际编译 object 的符号表复核表明它只导出 `lim_page_frame_init` 和
`init_expanded_memory`，没有导出两个 `LIM_*_write`。原因在原始 source：这三个
write-fault handler 位于 `#ifndef NTVDM` 区段，而 closure profile 是 `NTVDM`。

因此这两个 unresolved 不是漏接 EMS source，也不能以重编译宏、手写 write handler 或现代
media/EMS shim 消除。它们属于 CCPU non-NTVDM multi-mapped-page write-fault ABI 残留；只有
恢复与 active profile 一致的 CPU/SAS 写入路径后，才可以判定该引用应被哪个原始路径取代。

### 三个 monitor 边界符号不可混为普通 host 缺失

本轮对 NT4、NT4.5、XP 的 MVDM 子树进行了精确的实现/调用比对：

| 符号 | 可得证据 | 当前处置 |
| --- | --- | --- |
| `EDL_fast_bop` | 三个树均仅在 `ccpu386/c_main.c` 的 `C4 C4...FE` 快速 BOP 分支出现调用；未发现定义或生成输入。普通 BOP 仍走 `bop(ops[0].sng)`。 | 作为缺失的 fast-BOP monitor contract；不得把普通 `MS_bop_0` 或自写 SVC dispatcher 接到该入口。 |
| `ActivityCheckAfterTimeSlice` | 三个树均只有 `cpu4gen.h` 的 generated `Cpu` callback slot，以及 `nt_eoi.c` / `nt_unix.c` 调用；未发现 source owner。 | 作为缺失的 generated host callback；不得以 sleep、yield 或事件循环替代其语义。 |
| `HoldEMMBackFillMemory` | XP 与历史树的唯一实现位于 `base/mvdm/v86/monitor/i386/sas.c`；`host/src/x86_emm.c` 只是外部调用者。 | V86 monitor 实现仅作历史说明，依据项目的非 V86 研究路线不得迁入或链接；在软件 CCPU 获得独立 SAS/EMS 合同前保持未闭合。 |

这些结论不阻止普通 BIOS BOP、DEM 或 DOS 链的后续 source-owner 审计；它们只禁止用不相干的
V86、事件或 DOS 替代物伪造 monitor 语义。

### NT4.5 发布 `ntvdm.exe` 证明 x86 原路径是 V86 monitor，不是 CCPU

本地存在同代发布二进制
`D:\home\repos.hobby\opennt-4.5\nt\public\sdk\lib\i386\ntvdm.exe`
（SHA-256 `06618D9788036AC59EF6C37FF4000DF9EF39C58521C595DB875A7E2ABD84C231`，
I386，image base `0x0f000000`）。它的嵌入式 linker record 明确列出历史
`SYSTEM.LIB`、`BIOS.LIB`、`VIDEO.LIB`、`DOS.LIB`、`COMMS.LIB`、`DEBUG.LIB`、
`SUPPORT.LIB`、`DISKS.LIB`、`KEYMOUSE.LIB`、`SRC.LIB`，以及
`monitor.lib`、`dem.lib`、`command.lib`、`dpmi32.lib`、`xms486.lib`、`suballoc.lib`
和 `oemuni.lib`；其中没有 CCPU archive。

同一源码树的 `monitor.lib` 是 6-member x86 V86 archive（`int.obj`、`thread.obj`、
`spcstubs.obj`、`sas.obj`、`monitor.obj`、`fastpm.obj`），包含 V86 SAS、
`GetFastBopEntryAddress@4` 和 `HoldEMMBackFillMemory@8` 等接口，但不含
`c_sas_touch`、`c_VirtualiseInstruction`、`EDL_fast_bop` 或 CCPU execution。它只能
作为历史 ABI/行为证据，不能链接入本仓库的软件 CCPU target。

故结论是：NT4 x86 原版可证明完整的 V86 host/guest 职责链，但不能作为 CCPU 的现成
software-execution backend。direct-CCPU 的目标仍是以 `ccpu386` caller、generated ABI
残片与独立验证重建非 V86 语义；不得用发布 `ntvdm.exe`、`monitor.lib` 或其 V86 source
声称该后端已恢复。

## 下一步

1. 保持 `nt_cpu.c`、`nt_inthk.c` 及 SAS allocation pair 为 default-disabled 的 direct-CCPU closure input；继续对 117 项
   链接快照按原始 owner 收敛，且不进入 runtime。
2. 对仍存在的 C_VIDC getter/setter 簇追溯同代 generated vector/bridge 输入；不得复制
   MIPS/PPC 语义或手写 `GLOBAL_*` offset。
3. 任何最终留下的私有 console/VDM API 才进入现代 host seam 的 caller-first 合同
   阶段，且不得重定义 BIOS、BOP、SVC、DOS 或设备语义。
