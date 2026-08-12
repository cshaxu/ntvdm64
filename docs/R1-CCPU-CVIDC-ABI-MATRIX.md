# R1 CCPU C-VIDC ABI Matrix

状态：进行中，2026-08-09。

## 结论

固定 NT4、OpenNT 4.5 和本地 XP 源码树中的 CCPU/C-VIDC generated-header
组合完全一致；它们不是不同版本之间偶然不兼容的输入。现有源码快照缺少的
是 `ccpu486.lib` 的历史构建/生成输入及其 composition 证据。当前可证实的是
完整 C-VIDC 表不能按布局复制到 CCPU 短表；尚不能据此断言历史组合一定保存了
完整表 storage，或一定以某一种 bridge 形式实现。

## 同名 `VideoVector` 的布局

| 提供者 | header | 函数指针字段数 | `Video` 存储 owner | 角色 |
| --- | --- | ---: | --- | --- |
| CCPU | `base/ccpu386/evidgen.h` | 5 | `base/ccpu386/ntstubs.c`（非 `PIG`） | 旧 generic forwarding ABI |
| C-VIDC | `base/cvidc/evidgen.h` | 81 | 只声明 `extern Video` | generated video execution ABI |
| C-VIDC | `base/cvidc/vglfunc.c` | 81-slot typed storage，78 项 explicit initializer | `C_Video` | generated provider table（尾 3 slots 为零） |

在 x86 32-bit 目标中，二者分别至少占 20 和 324 字节的函数指针槽位空间。
这不是只从 header 推断的结论：当前 x86 `clang-cl` 对原始
`ccpu386/ntstubs.c` 的 COFF 输出中，`_Video` 位于其数据段 offset
`0x288`，紧随其后的 `_Cpu_outb_function` 位于 `0x29c`。两者相差
`0x14`（20）字节，恰好是 5 个 32-bit 指针槽位；当前实际 CCPU 对象确实
只为短 ABI 分配了 storage。

更关键的是，二者**不是可安全复制的共同前缀**：

| ordinal | CCPU 短 ABI | C-VIDC 完整 ABI |
| ---: | --- | --- |
| 0 | `GetVideolatches` | `GetVideolatches` |
| 1 | `SetVideolatches` | `GetVideorplane` |
| 2 | `setWritePointers` | `GetVideowplane` |
| 3 | `setReadPointers` | `GetVideoscratch` |
| 4 | `setMarkPointers` | `GetVideosr_masked_val` |

因此短 ABI 的 `setMarkPointers` offset 并不指向完整表中的
`setMarkPointers`。任何在短 ABI 编译单元中读取 `C_Video`、复制其“前缀”或
比较同名字段的观察，都会按错误 offset 解释指针。

这不等于 CCPU 与 C-VIDC 没有有效结合方式。顶层 `obj.vdm/cdefine.inc` 的非 x86
`CCPU` product profile 定义 `CPU_40_STYLE`、`CCPU` 和 `C_VID`；`egacpu.h` 在
`C_VID` 下将 video macros 直接重定向到命名的 `C_Video` fields 或 C-VIDC 的
`set*Pointers` functions。相反，`ccpu386/ccpudefs.inc` 自己不定义 `C_VID`，其
内部调用仍使用 5-slot `Video` forwarding ABI。待恢复的事实正是这两条路径在
历史 `ccpu486.lib` composition 中的选择与初始化关系。

`vglfunc.c` 的 initializer 本身也不能被误称为“全部 81 项已填”：它从
`S_0363..._00000000` 连续列至 `S_0440..._0000004d`，正好是 ordinals 0--77；
header 的 78--80 三个 fields `setWritePointers`、`setReadPointers`、
`setMarkPointers` 没有 initializer，按 C 规则保持零。这与 C-VIDC 中对应的
free functions 位于 `ev_glue.c` 的事实一致，但仍不能推出某个历史 owner 如何
初始化 `Video` 或 CCPU 短表。

## 已区分的三个对象与两条未闭合边

当前 COFF 与源码可分别证明以下对象，而不能把它们合并为同一个 global：

| 对象/边 | 已知 owner 或引用者 | 当前事实 | 未知部分 |
| --- | --- | --- | --- |
| CCPU 短 `Video`（5 slots） | `ccpu386/ntstubs.c` | 已定义为 20-byte BSS；CCPU 内部 header 使用它 | 五个命名 slot 的历史初始化 owner 与时序 |
| C-VIDC 完整 `Video`（81 slots） | `cvidc/ev_glue.c` | `llvm-nm -u` 证明该原始 object 引用 `_Video` | 81-slot storage owner 与初始化 owner |
| C-VIDC `C_Video`（81 slots） | `cvidc/vglfunc.c` | 78 个 nonzero generated entries，尾 3 slots 为零 | 是否、何时、由谁用于初始化完整 `Video` |
| `C_VID` product-side caller | `video/gfx_updt.c` | 当前原始 object 引用 `_setMarkPointers`，不引用 `_Video` | 它与上两条 internal 边的生命周期组合 |

因此，`C_VID` 能解释 product-side direct-call path，却不能消除 `ev_glue.c` 对
完整 `Video` storage 的真实引用。此前归档的
`local/archived/legacy-adapters/ccpu_vidc_global_storage.c` 只复制了
`localfm.c` 风格的 storage 声明，未提供 initialization；它是历史探索遗留物，
不是原始 owner 或可采纳恢复。

当前固定 CCPU C/H 输入中，短表五个 macro 没有找到实际 caller；它们不是目前
reset/startup 的第一 blocker。产品侧的 `ega_prts.c::init_vga_globals` 经 C_VID macro
直接调用 `ev_glue.c::setMarkPointers(0)`；后者才依次使用完整
`Video` 的 `SetVideomark_byte`、`SetVideomark_word` 与 `SetVideomark_string`
（ordinals 58--60）。这把 R1 的第一恢复目标收敛为完整 `Video` 的 storage 与
initializer owner。

## 已执行的受限实验及撤回

默认关闭 reset trace 曾在 `setup_vga_globals()` 后尝试把 `C_Video` 复制到
`ntstubs.c::Video`，只为验证组合假设。实验的两种 header 选择均被否定：

| 编译时选择 | 观察 | 结论 |
| --- | --- | --- |
| `ccpu386/evidgen.h` | 只能按短 layout 读取 5 个槽；所谓绑定检查只是同一错误 offset 的自比较 | 不构成 ABI 绑定证据 |
| `cvidc/evidgen.h` | 完整 `C_Video` 的 mark 槽均非零，但完整复制越过 `ntstubs.c::Video` 短存储并破坏相邻状态 | 不可进入 trace 或 runtime |

实验源码、CMake target、fixture 接线和 manifest 条目均已移除。清理后只保留原始
`init_vga_globals -> setMarkPointers` 命名调用边与其内部完整 `Video` 需求的证据。

该边界有严格的 profile 限制：当前实际 build directory 的 reset-overlay
`flags.make` 选择 `CPU_40_STYLE` 与 `CCPU`，但未定义 `C_VID`；而同一 build 中
video-provider 的原始 object 选择 `CPU_40_STYLE`、`CCPU` 和 `C_VID`。因此 reset
trace 是刻意混合的 portable-CCPU source-closure diagnostic，不是 NT4 x86 的
`MONITOR/C_VID/X86GFX` product profile，也不是完整 CCPU/C-VID product profile。
它证明的是该受控 diagnostic 中短 `Video` storage 未初始化，不能单独证明历史
CCPU runtime 需要完整表复制，或已经证明缺失 bridge 的具体形式。

## 跨树一致性

下列文件在 NT4、OpenNT 4.5、XP 三棵本地树中 hash 完全相同：

* `base/ccpu386/evidgen.h`
* `base/cvidc/evidgen.h`
* `base/ccpu386/ntstubs.c`
* `base/cvidc/vglfunc.c`

同时，三棵树均让 `obj.vdm/sources` 的 CCPU 分支链接命名为
`obj.vdm/obj/*/ccpu486.lib` 的产物；可用快照中均没有该库、其 packaging rule
或 `host/genPg` 生成目录。

这个名称不是当前两个 library 目标的普通别名：原始
`base/ccpu386/sources` 声明 `TARGETNAME=ccpu386`，而原始
`base/cvidc/sources` 声明 `TARGETNAME=Cvidc`；二者各自都是 `TARGETTYPE=LIBRARY`。
顶层却只在 `CCPU` 分支中消费 `ccpu486.lib`。因此，`ccpu486.lib` 必须是快照外的
后处理、组合或配置特定产物；不能由现有 `ccpu386.lib` 与 `Cvidc.lib` 的并列链接
自动推出。

`base/cvidc/ev_stubs.c` 是已审计但已排除的相邻候选。其原始注释说它为
`a4+evid` 与 `ccpu+cevid` 提供相同接口，但文件只提供空的
`setup_vga_globals()` 和 `setup_global_data_ptr()`，既不定义 `Video`，也不初始化
`C_Video`，并且不在原始 `cvidc/sources` 选择列表中。它不能作为 composition
storage、initializer 或运行期绕过输入。

本地文件名和构建文本搜索还找到完整 C-VIDC 生成输出
`SINIT011..013.c`、`SEVID000..030.c`、`vglfunc.c`、`cpuint_c.h` 和
`evidgen.h`，但没有 `MkCpuInt`、等价生成器可执行文件或 C-VIDC/CCPU merge
脚本。这说明“video provider 已生成”不能推出“CCPU composition 已生成”；后者
仍是待找回的独立输入。

此外，`cpuint_c.h` 本身也不是可在两个子树间任意混用的公共输入。CCPU
版本的 `InterruptREC` 比 C-VIDC 版本额外包含 `PoolsGettingTooBig` 字段；
二者都包含 `Reset`。这个差异与 `VideoVector` 的 layout 差异共同说明：
任何候选恢复必须把 CCPU、C-VIDC、SAS/interrupt generated inputs 与其
storage/initializer 视为同一版本化 composition，不能只抽取一张 video 表。

### 已检查的输入边界

在本地 NT4、OpenNT 4.5 与 XP 的各自 `softpc.new` 子树，以及本项目固定的
`src/ntvdmx64` 输入中，未找到以下任一种完整-table initialization 形式：

* `struct VideoVector Video = { ... }`；
* `Video = C_Video` 或其地址变体；
* 以 `memcpy` 写入 `Video` 的初始化；
* EDL 输入中对 `VideoVector`、`C_Video`、`SetVideomark_byte` 或
  `setMarkPointers` 的匹配定义。

本地 Win2000 `private` 快照没有 `softpc.new` 目录，也没有命名为
`ccpu486.lib`/`cvidc.lib` 的 composition artifact。这是已检查快照中的
negative evidence，不是对未持有历史 build drop、内部生成器或其他发布介质的
全局不存在声明。

## 准入规则

在找到完整历史组合输入前：

1. 禁止以 `Video = C_Video`、`memcpy`、alias、短表 adapter 或单字段 callback
   处理该边界。
2. 禁止让 `ntstubs.c::Video` 被 C-VIDC 完整 ABI 解释，或反向解释。
3. 下一候选必须提供 `ccpu486.lib` 的符号/链接证据，或同时提供经 source-level
   profile 证明的短表 storage owner、五个 named-field/function mapping、初始化
   时序和字段级 fixture；仅有 `C_Video` initializer 不够。
4. `CpuVector`、SAS、BIOS、BOP、DEM 和 DOS 不得被用作绕过此 ABI 闭合的
   替代执行路径。

## 下一步

1. 搜索历史 build output、symbol archive、SDK 安装和构建日志中的
   `ccpu486.lib`、`MkCpuInt`、`host/genPg`、C-VIDC library merge 或 storage
   generator 证据。
2. 对 `ccpu386`、`cvidc` 和任何找回的库生成 COFF symbol/member 清单，确认
   `Video` 的真正定义对象及其编译 ABI。
3. 在不添加 bridge、shim 或 synthetic handler 的前提下，建立 profile-aware 的
   original-caller 对照：分别观察 `C_VID` product-side video call 与 CCPU 内部短表
   call 到达的第一个历史边界。
4. 找不到历史输入时，先写出 profile-aware 的短表 bridge/storage/initializer
   reconstruction specification 和字段级 fixture，再决定是否实施；不得直接恢复
   reset execution。

恢复门槛与顺序已在
`R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md` 固化。
