# NT4 CCPU/C-VIDC 产品路径可达性记录

状态：采证完成，2026-08-09。本文只记录固定 NT4 OpenNT 输入中的条件编译与调用可达性；不授予
任何 runtime、host shim、overlay 或 C-VIDC 向量恢复的实现权限。

## 结论

`ccpu386/c_getset.c` 中的 `a3cpu_get_state()` 与 `a3cpu_set_state()` 的确分别调用
`Cpu.Video->GetVideolatches()` 和 `Cpu.Video->SetVideolatches(...)`。但整个 translation unit
受 `#if defined(PIG)` 包住，文件头明确说明其用途是将 C CPU 与 assembler CPU 对照测试。

固定 NT4 CCPU 产品编译记录
`artifacts/build/current/opennt-r5-clang-x86/CMakeFiles/ntdos64-opennt-ccpu-objects.dir/flags.make`
使用：

```text
CCPU CPU_30_STYLE CPU_40_STYLE NTVDM PROD SPC386 i386
```

其中没有 `PIG`。故该 `Cpu.Video` 访问是已知的历史 ABI/composition 旁证，不是当前固定 NT4
产品 CCPU 的启动、reset、BIOS、BOP、DEM 或 NTDOS 路径的动态 blocker。

对象级复核同样成立：对该 profile 已生成的
`artifacts/build/current/opennt-r5-clang-x86/ntdos64-opennt-ccpu-archive.lib` 运行
`llvm-nm`，没有导出 `a3cpu_*`、`c_getCpuState`、`c_setCpuState` 或 `ccpu_pig` 符号。
这证明当前 archive 未把 PIG-only translation units 的实现带入产品 CCPU 对象集合。

## 原始证据

| 问题 | 原始输入 | 观察 |
| --- | --- | --- |
| 短 video ABI | `base/ccpu386/evidgen.h` | 5 槽：latch get/set 与三种 pointer selector |
| CCPU state vector | `base/ccpu386/cpu4gen.h` | `CpuVector` 包含 `struct VideoVector *Video` |
| PIG state exchange | `base/ccpu386/c_getset.c` | 仅在 `#if defined(PIG)` 中读/写 `Cpu.Video` 的 latch slots |
| CCPU product flags | 构建记录 `flags.make` | `PROD`，无 `PIG` |
| CCPU product archive | `ntdos64-opennt-ccpu-archive.lib` | `llvm-nm` 无 PIG state-exchange / pigger 符号 |
| 完整 C-VIDC vector | `base/cvidc/evidgen.h`、`base/cvidc/vglfunc.c` | 81 槽 `C_Video`，0--77 有 generated provider，78--80 为零 |
| 已知产品侧 reset caller | `base/video/ega_prts.c`、`base/cvidc/ev_glue.c` | `init_vga_globals()` 经 `setMarkPointers(0)` 写完整表的 58--60 槽 |

对固定 NT4、NT4.5、NT5 可见 C/H 输入的定向搜索没有发现 `Cpu.Video = ...`、
`Cpu.Video = &Video` 或 `Cpu.Video = &C_Video`。也没有发现固定产品 profile 中短表 5 个宏的
直接 C caller。缺失的 `ccpu486.lib` 或其生成/组合步骤仍是这层 lifecycle 的唯一候选历史 owner。

## 已确认但不能越界使用的旁证

OpenNT 的仓库外只读路径 `base/cpu/src/evid/Vglob.c` 是从 `Vglob.edl` 派生的原始 C 文件；
NT4、NT4.5、XP 三份文件的 SHA-256 都是
`E695A9807285703119CFFB0C3BC8A3AA6433EDE43EFA499F21A6E736F94395D8`。它定义 `setVideomark_byte`、`setVideomark_word` 和
`setVideomark_string`，直接写入 `GLOBAL_VGAGlobals` 对应字段。这解释了
`ev_glue.c::setMarkPointers()` 写入完整表 58--60 槽之后的最终语义。

同一文件还在原始注释中明确说明 `getVideolatches`/`setVideolatches` 仍保留在
`EvPtrs.edl`，因为它们供 pigging 使用。逐符号审计确认它不定义短 ABI 的 latch 或
`setWritePointers`/`setReadPointers`/`setMarkPointers` 入口，亦不设置 `Cpu.Video`。
所以这不是漏收进 CCPU archive 的短表 provider；它只能缩小候选范围，不能授权以
local code 补写短表或生成 EDL 等价物。

不过，当前可见的 `sources`/makefile 没有把 `base/cpu/src/evid` 任何 C 文件列入构建；
可见的 `base/cvidc/sources` 只列出同名但独立的 `base/cvidc/ev_glue.c`，且
`base/cvidc/vglob.c` 为空文件。该旁证没有定义完整 `struct VideoVector Video`，没有提供其
initializer，也没有说明 `ccpu486.lib` 的成员组成。因此它只能用于解释已调用函数的字段语义，
不得复制、链接或据此合成 `Video` 表。

现有 object gate 也独立验证了这一点：对
`artifacts/build/current/opennt-r5-clang-x86/ntdos64-opennt-cvidc-archive.lib` 运行
`llvm-nm` 的结果为 `_C_Video` 已定义、`_setMarkPointers` 已定义、`_Video` 未解析。
这不是缺一个可以用 alias 消掉的符号，而是原始 C-VIDC archive 外仍缺完整 `Video` storage
及其 initialization/composition owner 的直接链接证据。

## 恢复影响

1. 不得以 PIG-only 的 `c_getset.c` 为理由，在产品 profile 初始化 `Cpu.Video`。
2. 不得将短 `Video`、完整 `Video` 与 `C_Video` alias、复制或通过不匹配 header 互用。
3. 当前应继续以产品侧 `init_vga_globals()` 到完整 `Video` 槽 58--60 的原始调用链为采证对象。
4. 在取得 historical composition 输入，或固定产品 profile 的两个独立 caller trace 前，不准为
   `Cpu.Video` 新增 reconstruction provider，也不准将任何候选接入 runtime。

## 与其他记录的关系

- ABI 与 storage 边界：`docs/R1-CCPU-CVIDC-ABI-MATRIX.md`。
- composition 恢复准入：`docs/R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md`。
- 跨版本 source matrix：`docs/research/softpc-source-matrix/NT4-NT5-SOFTPC-SOURCE-MATRIX.md`。
