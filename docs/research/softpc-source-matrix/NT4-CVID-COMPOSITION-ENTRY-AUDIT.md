# NT4 C-VID Composition Entry Audit

状态：`unresolved`。本文只记录固定 NT4 OpenNT 源码中 C-VIDC 与 CCPU 的
启动/组合证据；不增加 overlay、shim、CMake target 或运行时行为。

## 问题

此前已经确认：`base/cvidc/vglfunc.c` 初始化 `struct VideoVector C_Video`
的 0--77 provider 槽位，而 81-slot 结构的 78--80 保持 C aggregate 的零值；
`base/ccpu386/ntstubs.c` 则在非 `PIG` 分支定义零初始化的全局
`struct VideoVector Video`。本审计的问题是：

```text
在 CPU_40_STYLE + CCPU + C_VID 组合中，谁在何时为通用 Video 表提供
完整的历史绑定，尤其是 ev_glue.c 仍使用的 mark-provider 三个槽位？
```

结论不能由“两个结构都有”或“VGA globals 已初始化”推导出来。

## 已证实的构建组成

- `base/cvidc/sources` 将 `vglfunc.c`、`ev_glue.c`、`sinit011.c` 至
  `sinit013.c`、`sevid000.c` 至 `sevid030.c` 等列为 `Cvidc` library
  输入。因此，`C_Video` 及 C-VID glue 均是同一历史源组的正常输入，
  不是本项目漏迁的单个 `.c` 文件。`vglfunc.c` 明确只列至
  `S_0440_CiSetVideowrstate_0000004d`；生成的 `sinit013.c` 另有
  `S_0441..0443` wrapper，但它们并未进入该 initializer。
- `base/ccpu386/sources` 将 `ntstubs.c` 列为 `ccpu386` library 输入。
  它不列 `localfm.c`；后者的同名 `Video` storage 不能被当作该 profile
  的额外历史 owner。
- `obj.vdm/sources` 的 CCPU 分支仍只引用缺失的 `ccpu486.lib`。现有
  tree 没有与该库匹配的 member 清单、link map、listing 或 generator
  输出。因此不能据当前两份 component `sources` 反推最终产品的 archive
  composition。

## Vglob 生成输入的边界

固定树还存在两个同名但不能直接采用的文件：

- `base/ccpu386/vglob.c` 为 8,082 bytes，文件头明确写明由 `Vglob.edl`
  生成。它实现 `GLOBAL_VGAGlobals` 的命名 get/set accessor，包括三个
  mark provider setter；但不定义 `VideoVector` storage 或 initializer，
  且不在 `ccpu386/sources` 的输入列表中。
- `base/cvidc/vglob.c` 是零字节占位文件。NT4.5 与 XP 的同路径文件也同为
  零字节，故不能把它当作遗漏时误清空的 NT4 单文件。

NT4、NT4.5 与 XP 的 `ccpu386/vglob.c` SHA-256 均为
`E695A9807285703119CFFB0C3BC8A3AA6433EDE43EFA499F21A6E736F94395D8`；三份
`cvidc/vglob.c` 均为零字节 SHA-256
`E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855`。
三份可见树中均未找到 `Vglob.edl`、`EvPtrs.edl` 或其生成器/构建规则。

这说明可恢复的语义碎片已经存在，但其如何进入 `ccpu486.lib`、如何与
81-slot `Video` storage 合成仍未证实。`vglob.c` 只能作为 source evidence；
不得因它碰巧提供若干 setter 就把它手工加入 CMake、重写 `sources` 或当作
vector binding 的替代物。

## 已证实的启动顺序及其边界

原始 `base/support/main.c` 在 `cpu_init()` 之后、
`InitialiseDosEmulation(argc, argv)` 之前调用：

```text
setup_global_data_ptr();
setup_vga_globals();
```

在 `CCPU + C_VID` 编译条件下，这两个符号的原始实现位于
`base/cvidc/ev_glue.c`：

- `setup_global_data_ptr()` 分配 64 KiB `Gdp`，并写入 `j_Gdp`；
- `setup_vga_globals()` 将 `EGA_CPU.globals` 指向
  `GLOBAL_VGAGlobals`，并初始化 C-VID subroutine ring。

这证明 C-VID 的 GDP/VGA-global 初始化者和历史调用时机均在源码中可见。
但这两个函数没有读写 `Video`，也没有读写 `C_Video`。它们不能作为
`Video` vector binding 的证据，更不能被改造成该绑定的本地 shim。

## 精确的未闭合调用面

`base/inc/egacpu.h` 在 `CPU_40_STYLE + C_VID` 下把：

- `getVideolatches`、`setVideolatches` 直接改写为 `C_Video` 的字段调用；
- `SetWritePointers`、`SetReadPointers`、`SetMarkPointers` 改写为
  命名的 C-VID glue 函数。

其中 `base/cvidc/ev_glue.c::setMarkPointers()` 仍调用由其包含的
`evidgen.h` 定义的：

```text
setVideomark_byte(...)
setVideomark_word(...)
setVideomark_string(...)
```

这些宏解引用的是通用 `Video` 的 `SetVideomark_byte`、
`SetVideomark_word`、`SetVideomark_string` 字段，而不是 `C_Video`。
固定 NT4 `ntstubs.c` 只声明 `extern C_Video`，随后定义零初始化
`Video`；本 translation unit 内没有赋值、复制或别名操作。对整个固定
`softpc.new` tree 的 C/H 源搜索也未发现 `Video = C_Video` 或同等的
结构复制。

因此，`C_Video` 本身既不是完整 81-slot provider，也不是通用 `Video`
的历史 initializer；已证实的 C-VID accessor 改写只覆盖一部分表面。mark-provider
调用仍要求未找到的通用 `Video` 初始化/组合合同。

## Reset-time 动态可达性

该缺口并非只能在事后绘图中触发。固定 NT4 源码给出如下原始调用顺序：

```text
support/main.c
  -> InitialiseDosEmulation(...)
     -> host/src/nt_msscs.c::reset()
        -> bios/reset.c::gvi_init(gfxAdapt)
           -> gvi.c::ega_init()          (EGA profile)
              -> ega_prts.c::init_vga_globals()
                 -> SetMarkPointers(0)   (CPU_40_STYLE)
                    -> ev_glue.c::setMarkPointers(0)
                       -> Video.SetVideomark_{byte,word,string}
```

`nt_msscs.c` 的 `reset()` 调用位于 `InitialiseDosEmulation()` 内，且早于
主程序返回后开始 CPU simulate 的阶段。`ega_prts.c::init_vga_globals()`
在固定的 `NTVDM && !X86GFX` 条件下，对 `CPU_40_STYLE` 走
`SetMarkPointers(0)`；`egacpu.h` 又将该名字重写为
`ev_glue.c::setMarkPointers`。后者最终仍通过通用 `Video` 的三个
setter 字段写入 mark provider。

因此有两个不同结论：

- 对 NT4 x86 `MONITOR + X86GFX` 历史产品路径，CCPU/C-VIDC 仍是旁证，
  不能拿它阻塞当前 S5 x86 host-chain 采证；
- 对拟以 `CPU_40_STYLE + CCPU + C_VID` 作为 x64 软件 monitor 的恢复路径，
  此缺口是 EGA/VGA reset-time blocker，发生在 NTIO 进入之前，不能靠
  “稍后再处理 graphics”绕过。

这仍不授权本地表复制或假 handler。它只把后续证据搜寻的优先级从一般
link-composition 考古提升为 CCPU reset 组成的必要前置条件。

## 排除的错误修复

以下动作均不被本文授权：

- 写入 `Video = C_Video`、`memcpy` 或字段级复制；
- 把 `setup_global_data_ptr()` 或 `setup_vga_globals()` 扩展为绑定器；
- 用假 `setVideomark_*`、fake handler 或 host shim 让 C-VID fixture
  继续；
- 从 NT4.5、Windows 2000 或 XP 复制实现到 NT4 输入树；
- 因为一个 function-level object link closure 成功，就宣称完整
  `Video` composition 已恢复。

这些做法都会改变历史函数所有权，或把未证实的 composition 伪装成
现代 host seam。

## 后续受限 object-only 采证

本记录早期的“已撤回候选”结论仍适用于任何会定义完整 `Video` storage、链接 CCPU、运行
reset 或让 BIOS/BOP/DEM 前进的候选。2026-08-09 之后新增的
`ntdos64-opennt-cvidc-generic-video-vector-objects` 与
`ntdos64-opennt-reconstructed-cvidc-video-tail-objects` 属于更窄的例外：

- 前者仅将未修改的 `vglfunc.c` 第二次编译，使其原始 0--77 initializer 的符号名成为
  `Video`；后者只把原始、签名相符的 `setWritePointers`、`setReadPointers`、
  `setMarkPointers` 写入 78--80。
- 两者均为 `EXCLUDE_FROM_ALL` object target；不定义 CCPU short `Video`，不与 CCPU
  archive、BIOS、fixture、runner 或 runtime 链接，也没有 caller 可调用它们。
- 此操作只验证可审计的 full-vector 字段语义和符号边界，**不**满足本记录的
  profile proof、single-definition proof、初始化时机 proof 或 reset-time composition proof。

因此它们不是已恢复的 historical composition，亦不改变本节所列禁止项。其单独准入记录是
`docs/R1-GENERIC-VIDEO-RECONSTRUCTION-ADMISSION.md`；任何将它们提升为链接或运行 target 的
提案仍必须满足本记录的后续证据门槛。

## 后续证据门槛

要把本项从 `unresolved` 提升，至少需要以下任一可追溯输入：

1. 与 NT4 CCPU profile 匹配的 `ccpu486.lib` archive member 与其符号；
2. 相应产品的 link map 或完整 build log；
3. `host/genPg` 的生成输入和能说明 vector storage/initializer 的输出；
4. 一个同版本原始 caller，在可审计的历史对象组合中实际初始化
   `Video`，并可定位其写入顺序。

在获得上述证据前，C-VIDC 保持 source-evidence branch。它不构成 NT4 x86
V86 产品路径或当前 S5 历史 host 链的运行时 gate；但若选择 CCPU 软件
monitor 路线，它是 reset-time 的运行前置条件。

## 固定输入

- `src/opennt/base/mvdm/softpc.new/base/ccpu386/ntstubs.c`
  (`E25542825EB5EFB323DEAE41E8D8368E75E5DCC2ACABAD02A974F602E67641E8`)
- `src/opennt/base/mvdm/softpc.new/base/cvidc/vglfunc.c`
  (`22385F8A0F196C79B2F27B89EE33F7AE9D955304385D226522E2C06FA8726805`)
- `src/opennt/base/mvdm/softpc.new/base/cvidc/ev_glue.c`
  (`A12DDFD55DBD3B4426FF20075C46A665C6A30C6A955F24F6AB4872FB4895E935`)

关联台账：`NT4-NT5-SOFTPC-EVIDENCE-LEDGER.md` 的 `E-VEC-001`。
