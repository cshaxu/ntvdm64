# M92 OpenNT Overlay 审计

状态：M92 第二阶段逐文件 delta 审计完成，2026-08-08。

## 方法与判定

基线是 `src/opennt/` 中固定导入的 OpenNT NT4 快照。每一项均比较对应的
`src/opennt/overlay/` 文件；分类只描述本地差异，不把整个 OpenNT 原文件误称为
自写代码。`overlay/reconstruction` 必须有原始 caller 证据；`compat/compiler` 不得
改变运行期数据流；`overlay/instrumentation` 必须受默认关闭宏控制；含本地 host
行为的项在取得 live caller 准入前只能是 `compat/host candidate` 或归档候选。

## 审计结果

| Overlay | 本地差异 | 分类 | 处置 |
| --- | --- | --- | --- |
| `ccpu386/c_effective_addr.c` | 上游缺失，按 selector/descriptor caller 复原 effective address | `overlay/reconstruction` | 保留；唯一已确认的 SoftPC 语义恢复。 |
| `support/main.c` | `NTDOS64_HOST_MAIN_TRACE` checkpoint 60--65 | `overlay/instrumentation` | 保留，默认关闭。 |
| `keymouse/mouse_io.c` | `NTDOS64_MOUSE_INIT_TRACE` checkpoint 631--633 | `overlay/instrumentation` | 保留，默认关闭。 |
| `ccpu386/c_main.c` | `NTDOS64_CPU_INIT_TRACE` checkpoint | `overlay/instrumentation` | 保留，默认关闭。 |
| `bios/reset.c` | `NTDOS64_RESET_TRACE` checkpoint 1--12 | `overlay/instrumentation` | 保留，默认关闭。 |
| `host/src/nt_eoi.c` | host-applInit checkpoint 15--16 | `overlay/instrumentation` | 保留，默认关闭。 |
| `host/src/nt_hosts.c` | host UI checkpoint 20--36 | `overlay/instrumentation` | 保留，默认关闭。 |
| `host/src/nt_reset.c` | host-applInit checkpoint 10--31 | `overlay/instrumentation` | 保留，默认关闭；不得以 checkpoint 取代原生命周期。 |
| `host/src/nt_event.c` | checkpoint 40--50，及为 clang-cl 的 `BOOL`/`int` 和字符赋值修正 | `overlay/instrumentation` + `compat/compiler` | 保留，默认关闭；不准入事件队列实现。 |
| `host/src/config.c` | PEB `ConsoleHandle` 换为 `ntdos64_legacy_console_identity()`，以及 config checkpoint | `compat/host candidate` + `overlay/instrumentation` | 仅限目前 default-disabled trace；普通 runtime 不得链接该 seam，待真实 config caller 的失败/成功合同。 |
| `host/src/nt_msscs.c` | 可选 BYOB NTIO 路径和 IO/reset checkpoint | `compat/host candidate` + `overlay/instrumentation` | BYOB 路径改变原 host 输入，保持未准入；不得作为 loader/runtime 证据。 |
| `host/src/nt_bop.c` | `GetProcAddress` 到 historical FARPROC 的显式转换 | `compat/compiler` | 保留；不增加 selector、BOP 或 SVC 语义。 |
| `host/src/copy_fnc.c` | `windows.h` 后解除现代 winsock `h_addr` 宏，保留原始同名私有局部变量 | `compat/compiler` | 保留；不改变复制算法或 host 文件策略。 |
| `host/src/nt_sound.c` | 将历史一参 `PlaySound` helper 限于本翻译单元，避开现代 WinMM 同名 API | `compat/compiler` | 保留；不改变 tone/alarm 行为。 |
| `host/src/nt_error.c` | `stdlib.h` 后解除 UCRT `errno` 宏，保留历史局部 DOS error 值 | `compat/compiler` | 保留；不改变错误转换、对话框或错误码策略。 |
| `dos/dem/demfile.c` | K&R 声明转原型、`PVOID` 中转 | `compat/compiler` | 保留；不重写 DEM 文件语义。 |
| `dos/dem/demhndl.c` | `STOREWORD` 改为字段赋值 | `compat/compiler` | 保留；等价窄修正。 |
| `dpmi32/i386/dpmimem.c` | 合法的指针算术转换 | `compat/compiler` | 保留；不证明 DPMI 可运行。 |
| `ccpu386/c_xcptn.c` | `host_getenv` const 限定 | `compat/compiler` | 保留。 |
| `host/src/nt_cpu.c` | 仅 CCPU profile 排除未使用、且与 CCPU SAS provider 冲突的旧 `Start_of_M_area` / `IHPE Length_of_M_area` 声明 | `compat/compiler` | 默认关闭 object/link-diagnostic closure；不改变 lifecycle wrapper，不进入 runtime。 |
| `ccpu386/ccpusas4.c`、`zfrsrvd.c` | `Length_of_M_area` 的一致类型 | `compat/compiler` | 保留；不恢复 SAS 缺失接口。 |
| `base/dos/emm_mngr.c` | `unsigned short` 参数与历史声明对齐 | `compat/compiler` | 保留；EMS 仍未准入。 |
| `base/inc/host.h` | `memset4` 原型的现代 C 一致化 | `compat/compiler` | 保留。 |
| `support/ios.c` | `BOOL` 改为历史 `IBOOL` | `compat/compiler` | 保留；不定义 I/O 设备策略。 |
| `system/illegalp.c` | 预处理器 `defined()` 拼写 | `compat/compiler` | 保留。 |
| `host/inc/x86/prod/sas4gen.h` | 固定的历史生成 ABI 头，固定快照无同路径 | `overlay/reconstruction candidate` | 保留为研究输入；不得作为新 SAS 语义来源或进入 runtime。 |
| `xms.486/xmsblock.c` | 指针递减/转换改为可编译形式 | `compat/compiler` | 保留；不证明 XMS 可运行。 |
| `base/mvdm/inc/mvdm.h` | clang 专用 `STOREWORD`/`STOREDWORD` 宏 | `compat/compiler` | 保留；仅为编译器表达式兼容。 |

## 结论

1. 没有发现第二个可称为 SoftPC 语义恢复的 overlay。
2. 8 个纯 instrumentation 文件均为默认关闭的 trace，不得进入研究型
   `ntvdm.exe` 正常路径。
3. 17 个纯 compiler compatibility 文件必须维持窄差异；它们不是对应 DEM、DPMI、
   EMS、XMS、BOP 或设备功能的验收。
4. `host/inc/x86/prod/sas4gen.h` 在固定 OpenNT 快照及所有比较树中均无 x86 同路径
   生成输入。它不是 compiler compatibility，而是未准入的 `overlay/reconstruction`
   candidate；不得进入任何 runtime 或被用于证明 SAS/CCPU 已恢复。
5. `config.c`、`nt_msscs.c` 的 host seam/BYOB 差异尚未获 runtime 准入。后续先由
   已抵达的原始 caller 给出输入、失败和返回合同；否则应从所有正常组合 target
   移出，仅保留 trace fixture。

此报告不宣布 `config()`、NTIO、NTDOS、DEM 或 DOS runtime 已恢复。

## 第二阶段：逐项 delta 与来源闭合

本阶段以 `src/opennt/<relative path>` 中固定导入的 OpenNT 原文件为逐字节基线，对
`src/opennt/overlay/` 的全部 29 个文件运行
`git diff --no-index --unified=0 -- src/opennt/<relative path> src/opennt/overlay/<relative path>`，并核验
每个文件在 `SOURCE-MANIFEST.json` 中都有 hash、字节数、分类、准入状态和（适用时）
上游路径与基线 hash。结果如下。

| 集合 | 文件数 | 基线结论 | 分类与准入 |
| --- | ---: | --- | --- |
| 有同路径上游的窄 compiler delta | 17 | 每项为 1--22 行的声明、类型、宏或旧 C 表达式改写 | `compat/compiler`；只能参与 default-disabled historical object gate，不能构成运行能力证据。 |
| 默认关闭的观察插桩 | 7 | 仅增加 `NTDOS64_*_TRACE` 宏包裹的 checkpoint；宏未定义时预处理结果回到上游主体 | `overlay/instrumentation`；只允许 fixture 链接，不能承担任何推进 DOS 的职责。 |
| host seam 与 trace 混合项 | 2 | `config.c` 替换已失效 PEB console identity；`nt_msscs.c` 增加 BYOB NTIO 路径并可跳过首次系统文件写入 | `compat/host candidate` 加 instrumentation；仅 isolated/trace target，未准入 runtime。 |
| 已确认缺失语义恢复 | 1 | 上游及比较树均无 `c_effective_addr.c`；已有 caller-first 研究记录 | `overlay/reconstruction`；默认关闭，仍需持续验证。 |
| 未准入缺失生成输入 | 1 | 上游及比较树均无 x86 `host/inc/x86/prod/sas4gen.h` | `overlay/reconstruction candidate`；默认关闭，未证明语义等价。 |

### 逐项风险判定

| Overlay | 相对上游 delta | 最终分类 | 正常 runtime |
| --- | --- | --- | --- |
| `dos/dem/demfile.c` | 4 个 K&R 定义改为带原型的等宽指针签名，局部 `PDOSSFT` 转换 | `compat/compiler` | 禁止 |
| `dos/dem/demhndl.c` | `STOREWORD` 改字段赋值 | `compat/compiler` | 禁止 |
| `dpmi32/i386/dpmimem.c` | 旧 cast-lvalue 指针递增改显式赋值 | `compat/compiler` | 禁止 |
| `inc/mvdm.h` | clang 专用 `STOREWORD`/`STOREDWORD` 表达式 | `compat/compiler` | 禁止 |
| `ccpu386/c_xcptn.c` | `host_getenv` 参数加 `const` | `compat/compiler` | 禁止 |
| `host/src/nt_cpu.c` | `CCPU` 下排除未使用且与 CCPU SAS owner 冲突的 `Start_of_M_area` / `IHPE Length_of_M_area` 声明；非 CCPU branch 原样保留 | `compat/compiler` | 禁止 |
| `ccpu386/ccpusas4.c`、`zfrsrvd.c` | 同一 `Length_of_M_area` 在 x86 下均为 32-bit 的声明类型对齐 | `compat/compiler` | 禁止 |
| `base/dos/emm_mngr.c` | `deallocate_altreg_set` 声明与 header 对齐 | `compat/compiler` | 禁止 |
| `base/inc/host.h` | `memset4` 原型改为现有实现使用的 32-bit unsigned 类型 | `compat/compiler` | 禁止 |
| `support/ios.c` | 返回类型与历史 `IBOOL` ABI 对齐 | `compat/compiler` | 禁止 |
| `system/illegalp.c` | 预处理器 `defined` 拼写修复 | `compat/compiler` | 禁止 |
| `host/src/nt_bop.c` | `GetProcAddress` 到已有 FARPROC typedef 的显式转换 | `compat/compiler` | 禁止 |
| `xms.486/xmsblock.c` | 5 个旧 cast-lvalue 指针操作改显式指针赋值 | `compat/compiler` | 禁止 |
| `bios/reset.c`、`ccpu386/c_main.c`、`keymouse/mouse_io.c`、`support/main.c`、`host/src/nt_eoi.c`、`nt_hosts.c`、`nt_reset.c`、`nt_event.c` | 仅默认关闭 checkpoint；`nt_event.c` 另有两项 compiler delta | `overlay/instrumentation`（`nt_event.c` 同时为 `compat/compiler`） | 禁止 |
| `host/src/config.c` | console identity seam 加 trace；非 trace 分支也调用外部 seam | `compat/host candidate` + instrumentation | 禁止 |
| `host/src/nt_msscs.c` | BYOB NTIO 输入、首次写入抑制、`io_init`/`reset` checkpoint | `compat/host candidate` + instrumentation | 禁止 |
| `ccpu386/c_effective_addr.c` | 完全缺失的 CCPU vector source | `overlay/reconstruction` | 禁止 |
| `host/inc/x86/prod/sas4gen.h` | 完全缺失的 x86 generated vector header | `overlay/reconstruction candidate` | 禁止 |

`host/inc/x86/prod/sas4gen.h` 仅可作为缺失生成输入的研究证据。MIPS/PPC/`cvidc`
变体可辅助辨认生成格式，但不是 x86 语义来源，不能通过复制或“形状相似”获得 runtime
准入。

### 构建图复核

根 `CMakeLists.txt` 不含 `add_subdirectory(src/opennt)`；`src/opennt` 是独立的
historical-input CMake island。其 124 个 `add_library`/`add_executable` 声明均带
`EXCLUDE_FROM_ALL`，唯一 aggregate custom target 也显式设置 `EXCLUDE_FROM_ALL TRUE`。
因此本阶段没有把 overlay、candidate 或 trace 代码加入默认 `ntdos64-run` 构建图。

## 独立复核记录

2026-08-08 以工作树而非本文档为输入重新执行了第二阶段验收：递归枚举
`src/opennt/overlay/`，逐项与 `SOURCE-MANIFEST.json` 对照，并对有同路径基线的
文件执行 `git diff --no-index --numstat`。结果如下：

| 检查项 | 结果 |
| --- | --- |
| overlay 物理文件 / manifest 条目 | 29 / 29；无漏登或多登 |
| hash 与字节数 | 29 / 29 匹配 |
| 有同路径 OpenNT 基线 | 27；均可产生独立 delta |
| 已声明无上游同路径 | 2：`c_effective_addr.c` 与 x86 `sas4gen.h` |
| trace/instrumentation 分类文件 | 10 / 10 含 `NTDOS64_*_TRACE` 默认关闭守卫 |
| 根 CMake 直接加入 `src/opennt` | 0 |

27 个有同路径基线的 delta 总量按最终类别汇总为：纯
`compat/compiler` 17 个文件、63 行增加/49 行删除；纯
`overlay/instrumentation` 7 个文件、123 行增加/1 行删除；两个 host candidate
混合文件合计 86 行增加/4 行删除；`nt_event.c` 的混合项为 22 行增加/2 行删除。
行数只用于发现异常扩大，不替代逐项语义判定。

## 审计后的编译边界复核

2026-08-08 的默认关闭 trace 复核发现，`nt_eoi.c`、`nt_event.c`、`config.c`、
`qevnt.c` 和 `nt_umb.c` 自身均以 `<nt.h>` 开始；对它们强制注入 Win32-first
preamble 会让现代 `winnt.h` 先于 OpenNT `ntdef.h` 定义同名 native 类型。CMake
因此只对相应的 isolated target 取消该 preamble，并恢复其历史 `_X86_` 配置。
`nt_event.c` 仍只使用既有的声明级 `try/finally/except` compiler compatibility。
`mouse_io.c` 以及 CCPU/platform-provider 的 `ica.h` consumers 另补入已受
manifest 管理的 `public/internal/base/inc`，使原始 `ica.h -> vdm.h` 包含链可见。

重新构建的 `ntdos64-opennt-config-x86-prefix-trace-fixture` 和
`ntdos64-opennt-config-x86-umb-trace-fixture` 分别只到达原始 `InitUMBList` 前和
原始 `gfi_floppy_active` 调用点的 fail-closed stop。它们不读取 host A:/B:、不
加载媒体、不启动 guest，也不准入 UMB、介质或 DOS runtime 行为。

这些调整没有添加、替换或调用任何 DOS、BOP、设备、输入、文件或 host runtime
行为。`ntdos64-opennt-host-main-applinit-trace-fixture` 已以默认 stop stage `17`
构建并执行成功；该结果只证明原始 `host_main -> host_applInit` 前缀到达已有的
instrumentation checkpoint。它不构成 `config()`、事件循环、鼠标、DEM、NTIO、
NTDOS 或完整 SoftPC runtime 的可运行性证据。

该 fixture 的未处理异常 snapshot 仅记录异常码、指令地址、模块相对地址、访问目标，
`VirtualQuery` 返回的分配区和模块身份，以及只读栈顶返回地址；它不改变异常控制流，
不得被当作 host recovery 或运行时兼容行为。
