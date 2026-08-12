# R1 CCPU C-VIDC Composition Recovery Specification

状态：草案，2026-08-09。本文是 R1 的 evidence-backed 恢复准入规格，不是
runtime 设计、host shim 授权或 local overlay 的实现授权。

## 问题

当前固定 NT4 输入能独立编译 CCPU 与 C-VIDC，却不能从现有 `sources`、生成器或
library 产物重建顶层所引用的 `ccpu486.lib`。对当前对象的审计已证明存在三个不同的
对象：

1. `ccpu386/ntstubs.c` 定义 5-slot 的 CCPU `Video` storage；
2. `cvidc/ev_glue.c` 以 81-slot C-VIDC header 引用完整 `Video` storage；
3. `cvidc/vglfunc.c` 定义 81-slot typed `C_Video` storage，其中 0--77 为
   generated provider entries，78--80 保持零。

这些不是同一个 C type。不能以 global alias、前缀复制、错误 header 的 `memcpy` 或
单个 callback 代替未知的 composition。

权威事实与原始命令/对象证据见
`R1-CCPU-CVIDC-ABI-MATRIX.md`。

## 缺失的 composite library 边界

`obj.vdm/sources:132-136` 在 `CCPU` 非空时只选择
`$(SOFTPC_TREE)\obj.vdm\obj\*\ccpu486.lib`；它没有在产品 link line 上单独列出
`ccpu386.lib` 或 `cvidc.lib`。而现存 `base/ccpu386/sources` 声明的 library 只包含
CCPU 指令实现和 `ntstubs.c`，不包含 `vglob.c`；`base/cvidc/sources` 则独立声明
`Cvidc` library。

这证明 `ccpu486.lib` 是一个缺失的、比两个可见 source library 更高层的历史 composite
产物，或由未保存的 build step 组合出的等价物。当前将独立编译的 CCPU 与 C-VIDC object
archive 直接拼入 fixture，能定位 ABI 和 host dependency，却不能证明它们就是原始 link
composition，更不能靠 link order 或 duplicate-symbol 选择恢复该 composition。

`vglob.c` 是这一结论的实物佐证：它实现完整 C-VIDC 的 direct-global accessor，却未列入
固定 `ccpu386/sources`。因此其历史进入 `ccpu486.lib` 的方式和 `Video` 的 complete-table
storage/initializer owner 同属缺失 composite input，必须一起采证。

后续的 source-level closure 审计补充了一个受限事实：把原始 `vglob.c` 单独编译后，
它会解析 video provider 对命名 `getVideo*`/`setVideo*` accessor 的全部导入。这说明
`vglob.c` 是可见 source 中该 accessor 语义的 owner，但不改变它不在公开
`ccpu386/sources`、不定义 complete `Video` storage、也不证明其进入历史
`ccpu486.lib` 的结论。完整计数、原始 owner 和禁止项见
`research/softpc-source-matrix/NT4-CVID-PROVIDER-CLOSURE-AUDIT.md`。

`ccpu386/localfm.c` 不是可替代的 owner。它的源文本确实定义 `Gdp`、`Cpu`、`Sas` 与短
`Video` storage，但固定 NT4、NT4.5 与 XP 的 `ccpu386/sources` 均未把该文件列入
`SOURCES`；实际列入的是 `ntstubs.c`。因此，不能通过把 `localfm.c` 添回当前构建来消除
重复/缺失符号：那会人为合成一个没有历史 product-link 证明的第三种 composition，而非恢复
`ccpu486.lib`。

2026-08-09 的本地文件名级检索覆盖 `D:\home\repos\ntdos64`、
`D:\home\repos.hobby\opennt`、`opennt-4.5`、`Win2000SRC`、`winxpscodes` 与
`winxpsrc` 的可见普通文件，查找 `ccpu486*`、`cvidc.lib`、`host/genPg`、CCPU map、
listing 与 build log。结果没有历史 library、producer rule、map/listing 或生成目录；唯一
命中是本项目此前的 trace/link diagnostics。该结果不声称外部发行介质或未采集 build drop
不存在，但已足以禁止继续把当前 snapshot 当作可自行重产原始 composite library 的完整输入。

本地 OpenNT-4.5 i386 `ntvdm.exe` 虽保留有用的 COFF symbol table，却是 V86 `monitor.lib`
产品，不含 CCPU/C-VIDC symbols；它只能作为 host/BOP/DEM oracle，不能补充本节的
composite 输入。详见 `R1-CVIDC-REFERENCE-COMPARISON.md`。

## 已知映射边界

| consumer | 编译 ABI | 所需对象 | 已知安全事实 |
| --- | --- | --- | --- |
| CCPU internal | 5-slot `ccpu386/evidgen.h` | 短 `Video` | storage 真实存在且为 20 bytes |
| C-VIDC glue | 81-slot `cvidc/evidgen.h` | 完整 `Video` | 原始 `ev_glue.c` 未解析引用它 |
| C-VIDC generated provider | 81-slot `cvidc/evidgen.h` | `C_Video` | `vglfunc.c` 初始化 0--77；尾 3 slots 为零 |
| C_VID product caller | `C_VID` macro path | named C-VIDC functions / `C_Video` fields | `gfx_updt.c.obj` 引用 `_setMarkPointers`，不引用 `_Video` |

CCPU short table 的字段名与潜在 provider 名称可由源码对应，但这只是候选映射，尚非
历史初始化证据：`GetVideolatches`、`SetVideolatches`、`setWritePointers`、
`setReadPointers`、`setMarkPointers`。

`C_Video` 的显式 provider 范围止于 ordinal 77；78--80 分别是
`setWritePointers`、`setReadPointers` 与 `setMarkPointers`，由 `ev_glue.c` 定义为
独立函数而非 `C_Video` initializer 项。因而任何 future reconstruction 必须把
0--77 provider initialization 与 78--80 owner 分开证明。

## CCPU 状态向量绑定

2026-08-09 的进一步 source audit 证明，`ntstubs.c` 中的短 `Video` global 并不是
当前已见 CCPU state save/restore 的直接 consumer。`ccpu386/c_getset.c` 的
`a3cpu_get_state()` 与 `a3cpu_set_state()` 分别通过
`Cpu.Video->GetVideolatches()` 和 `Cpu.Video->SetVideolatches(...)` 访问 video
latch；但整个文件由 `#if defined(PIG)` 包住。它是 CCPU 与 assembler CPU 对照测试器的
状态交换路径，而固定 NT4 CCPU 产品 profile 使用 `CCPU`、`CPU_40_STYLE`、`PROD`，不定义
`PIG`。因此此处只构成历史 ABI/composition 证据，不能作为当前 NT4 产品启动、reset 或 NTDOS
执行路径已到达的证明。

`Cpu` storage 虽由 `ccpu386/ntstubs.c` 定义，但对 NT4、NT4.5、Win2000/XP 可见
SoftPC C/H source 的定向搜索未发现 `Cpu.Video = ...`、`Cpu.Video = &Video` 或
`Cpu.Video = &C_Video` 的赋值。`ccpu386/vglob.c` 是 `Vglob.edl` 的 generated
accessor 输出，提供命名的 VGA-global getter/setter，亦不为 `Cpu.Video` 赋值。

2026-08-09 对 `ccpu386/vglob.c` 的逐符号复核进一步排除了另一条容易误判的恢复路径。
该文件在注释中明确写明 `getVideolatches`/`setVideolatches` 仍在 `EvPtrs.edl`，仅为
`GLOBAL_VGAGlobals` 生成按名称访问器；它没有短 ABI 的 `GetVideolatches`、
`SetVideolatches`、`setWritePointers`、`setReadPointers` 或 `setMarkPointers`，也没有
`Cpu.Video` 绑定。其与只读 EVID 版本的 SHA-256 相同，均为
`E695A9807285703119CFFB0C3BC8A3AA6433EDE43EFA499F21A6E736F94395D8`。因此，
不得把 `vglob.c` 当作短 `Video` 表 initializer、缺失 EDL 的替代，或 CCPU/C-VIDC
composition 的来源；它只为已存在的命名 VGA-global 调用提供字段语义旁证。

这增加一个不可省略的 composition 事实：缺失的 `ccpu486.lib`/生成步骤不但要说明
完整 C-VIDC `Video` storage 与 initializer，还必须说明 CCPU state-vector 的
`Cpu.Video` binding 在何时、以哪个 ABI 完成。不能把 `Cpu.Video` 默认为短全局
`Video`，也不能把它直接指向 `C_Video`；两种候选都缺少 profile/lifecycle 证据。

现有固定快照中，除了 `evidgen.h` 的宏定义和上述 PIG-only state exchange 外，短 ABI 的
`SetWritePointers`/`SetReadPointers`/`SetMarkPointers` 没有被发现为 CCPU C source
caller。此项 negative evidence 仅表示它们不是当前发现的第一个 CCPU state blocker，
不能证明它们从未在缺失的组合物、汇编或历史对象中被使用。

## 当前可达性排序

对固定 NT4 CCPU 源目录的所有 C/H 输入搜索表明，上述五个 short-table macro 只有
`ccpu386/evidgen.h` 中的定义，没有任何当前 CCPU C caller。因此它们仍需保留为
composition inventory，但不是当前 reset/startup 的第一 blocker。

相反，当前已知的产品侧原始调用链已精确到 C-VID product-side 的命名入口，以及该入口内部的三个完整表写 slot：

```text
video/ega_prts.c::init_vga_globals()
  -> SetMarkPointers(0)
  -> C_VID macro rewrite: cvidc/ev_glue.c::setMarkPointers(0)
  -> Video.SetVideomark_byte   (full table ordinal 58)
  -> Video.SetVideomark_word   (full table ordinal 59)
  -> Video.SetVideomark_string (full table ordinal 60)
```

`ega_prts.c.obj` 在 `C_VID` profile 下未解析 `_setMarkPointers`，不读取 `_Video`；
`ev_glue.c.obj` 才未解析完整 `_Video`。因此当前恢复顺序先追索/验证完整 81-slot
`Video` 的 historical storage 与 initializer；短表只在出现真实 CCPU caller 后
再提升。

## 恢复前置条件

任何新 overlay 或构建组合在进入 default-disabled fixture 前，必须同时满足：

1. **Profile proof**：记录每个 translation unit 的实际 `flags.make`，不得把
   `CPU_40_STYLE`、`C_VID`、`MONITOR` 与 `X86GFX` 混为一个 profile。
2. **Single-definition proof**：完整 `Video` 和短 `Video` 的 storage owner 分开列出；
   不能由 weak/common 或链接器选择规则悄悄决定。
3. **Initialization proof**：对每个 table 指出初始化 owner、发生时刻、所有写入项，
   并证明初始化以及 `Cpu.Video` binding 均在第一个 original caller 前完成。
4. **ABI proof**：完整表只以 C-VIDC header 编译/访问；短表只以 CCPU header
   编译/访问；跨边界只能使用按名称证明的函数入口，不传递 struct 指针。
5. **Fixture proof**：fixture 分别验证完整表和短表的每个必需 slot，包含未初始化
   负例；失败不得继续 reset、BIOS、BOP、DEM 或 DOS。

## 已撤回的候选与后续受限采证

此前将 0--77 provider 与 78--80 glue tail 当作可直接恢复完整 `Video` 的候选已经撤回：
它们没有证明 historical initializer/lifecycle owner；更关键的是，`C_VID` 产品侧并不通过
表尾调用命名 `set*Pointers`。该撤回不改变 `ev_glue.c` 对完整 `Video` storage 的真实引用，
也不授权用本地 stub、假 handler 或 `/FORCE` 通过该边界。

后续仅新增了一个**更窄、object-only 的字段级采证例外**，并未恢复或重新准入该候选：

1. 原始 `vglfunc.c` 的第二次编译可在 `C_Video -> Video` 的预处理 rename 下产出
   typed 0--77 initializer object；
2. 本地 reconstruction 仅以原始签名把三个命名 glue 函数写入 78--80，且不定义 storage、
   不复制、别名或链接 0--77；
3. 两个 target 都是 `EXCLUDE_FROM_ALL` object-only evidence，不能链接 CCPU archive、
   BIOS table、fixture、runner 或任何 runtime。

该例外的边界、COFF 验证和禁止项由
`R1-GENERIC-VIDEO-RECONSTRUCTION-ADMISSION.md` 单独约束。它只能验证字段类型、导入和
single-storage 禁止项；不能证明 `ccpu486.lib` historical composition、初始化时刻、
`Cpu.Video` binding 或 C-VIDC runtime closure。

本地 NTVDMx64 对照 patch 的确采用了“完整 `Video` 由 `vglfunc.c` initializer 定义、
注释 CCPU short `Video`”的后代兼容策略；它证明该断裂曾被其他项目处理过，但不是 NT4
历史构建证据，也不能作为本规格的实现授权。详见
`R1-CVIDC-REFERENCE-COMPARISON.md`。

## 允许的恢复顺序

任何候选 archive、map/listing 或 generator 输入的接收、隔离、hash/member 审计和准入前限制必须
先满足 `R1-CCPU-CVIDC-INPUT-ACQUISITION-CONTRACT.md`；在其 provenance/profile 审计完成前，
不得把新材料迁入 `src/opennt` 或添加 runtime link edge。

1. 继续查找 `ccpu486.lib`、其 map/listing、`host/genPg` 或 historical build log；
   若找回，先做 COFF member/symbol 对照，不直接链接进 runtime。
2. 以原始 `ev_glue.c` 的第一个完整-table caller 做 bounded trace，记录 ordinal
   58--60 的 storage、initializer 与 workspace state。
3. 只在固定产品 profile 出现真实 CCPU internal caller 后，才对五个短 slots 做独立 bounded trace；
   PIG-only caller 不能满足此准入条件。
4. 仅在完整-table caller evidence 与 historical source/build evidence 都成立时，提出
   最小 composition reconstruction；
   它必须是新的 manifest-tracked `overlay/reconstruction`，并有一正一负 fixture。
5. reconstruction fixture 成功后，仍只解除下一条 original boundary；不因此宣称
   reset、guest execution 或 NTIO bootstrap 已恢复。

## 明确禁止

- `Video = C_Video`、二进制 `memcpy`、linker alias 或把一种 header 用于另一种 storage；
- 通过 local host/video stub、空 handler、BOP/DEM 替代物让初始化看似通过；
- 把 archived `ccpu_vidc_global_storage.c` 升级为 runtime input；
- 将 NT5/XP 的未采集代码或外部 emulator 当作 NT4 composition 的实现来源。

## 退出条件

本规格在以下两种情况下更新：找到可审计的历史 composition 输入，或完整两个 caller
trace 共同定义了最小 reconstruction 的字段级行为。二者之前，R1 继续是 source/build
recovery，不进行 runtime promotion。
