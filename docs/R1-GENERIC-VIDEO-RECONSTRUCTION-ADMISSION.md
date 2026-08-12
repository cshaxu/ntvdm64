# R1 Generic Video Reconstruction Admission

状态：受限准入，2026-08-09。

## 问题与决定

固定 NT4 C-VIDC 输入明确给出 81 槽 `VideoVector` ABI。原始 `vglfunc.c` 可无修改地
提供 0--77 的 typed generated-provider initializer；原始 `ev_glue.c` 则提供与 78--80
槽精确匹配的 `setWritePointers`、`setReadPointers(IUH)`、`setMarkPointers(IUH)`。

公开快照没有保留 `ccpu486.lib` 的 profile-aware storage owner。因此本准入**不声称**复原
该历史 archive composition，也不修改或取代 CCPU 的 5 槽 `Video`。它只准入一个新、可审计的
reconstruction provider，供将来独立 C-VIDC 闭包实验使用：

```text
original vglfunc.c, compiled with C_Video -> Video
  -> reconstructed tail initializer (78--80 only)
  -> original ev_glue setWritePointers/setReadPointers/setMarkPointers
```

## 严格边界

1. 0--77 槽只能来自原始 `vglfunc.c` 的第二次编译；不得 `memcpy`、alias 或重写该表。
2. 新代码只能赋值 78--80 槽，且三项必须分别等于原始、签名匹配的命名 glue 函数。
3. 新代码不定义 `Video` storage，不定义 `C_Video`，不包含 CCPU short header，不调用
   BIOS、BOP、DEM、DOS、host API 或设备代码。
4. 若任一尾槽已是非空且不是预期的原始函数，初始化返回失败；不得覆盖未知 owner。
5. target 必须 `EXCLUDE_FROM_ALL`，只能产出对象；不得链接入 `ntdos64-run`、未来
   `ntvdm.exe`、CCPU archive、BIOS table 或任何 release target。
6. 此 provider 仅是将已知的 C-VIDC 字段语义表达为新 reconstruction code。它不能作为
   NT4 `ccpu486.lib` 的 exact-binary、历史初始化时刻或 CCPU/C-VIDC 最终组合的证据。

## 验收

- i386 clang-cl object 同时保留 `_Video` import 与三个原始 glue import；不定义 `_Video`、
  `_C_Video` 或任何 CCPU symbol。
- 目标不引入 non-C-VIDC function pointer cast；编译器以 `evidgen.h` 字段类型检查赋值。
- 重复调用初始化是幂等的；冲突尾槽返回失败且不修改其余槽。
- 运行期闭包仍需另行证明原始 C-VIDC、video owner、session lifecycle 和 CCPU storage
  选择；本准入本身不授权启动 fixture。

## 首次对象验证

2026-08-09，使用既有 i386 `clang-cl` CMake island 显式构建
`ntdos64-opennt-reconstructed-cvidc-video-tail-objects`。构建成功；目标仍为
`EXCLUDE_FROM_ALL` 的 object-only target，未生成或链接任何 runtime/fixture。

对生成的 COFF-i386 object 使用 `llvm-nm` 与 `llvm-objdump -r` 复核：未定义符号恰为
`_Video`、`_setWritePointers`、`_setReadPointers`、`_setMarkPointers`；唯一业务定义符号为
`_ntdos64_reconstructed_cvidc_video_tail_initialize`。重定位也只指向这四项输入（另有该函数
自身的调试重定位），没有 `_C_Video`、`_Cpu`、`_Sas` 或 `_Gdp`。这验证了本文件的
storage-owner 禁止项，但不验证完整 C-VIDC 运行时闭包，更不构成历史 `ccpu486` 组合证据。

相关采证见 `research/softpc-source-matrix/NT4-GENERIC-VIDEO-RECOVERY-CANDIDATE.md`。

## 首次链接闭包审计

2026-08-09 对上述 object 做了进一步的原始 owner 审计。新代码只引入三个
`ev_glue.c` 函数的地址，但 COFF 的链接单位是整个 `ev_glue.c.obj`；因此任何试图生成
fixture EXE 的链接都会拉入其完整原始引用集。当前固定 NT4 C-VIDC archive 的
`ev_glue.c.obj` 除 `_Video` 外，还依赖：

- `EGA_CPU`：原始 `base/video/ega_prts.c` 的 EGA 全局状态 owner；
- `gvi_pc_low_regen`：原始 `base/video/gvi.c` 的再生区域 owner；
- `bios_ch2_byte_wrt_fn` 和 `bios_ch2_word_wrt_fn`：原始 `base/video/cga.c` 的字节/word
  写入 owner；
- `simple_evid`、`chain2_evid`、`chain4_evid`、`unchained_evid`、`dith_evid`、
  `gricvid_evid` 及其 read/mark 变体：原始 generated/video provider 集。

这些符号不能因为 fixture 不主动调用 handler 而使用本地零实现、空存储或
`/FORCE` 忽略。那样会把对 table tail 的机械验证伪装成完整 video
闭包。所以当前的下一步是为这些原始 owner 建立 default-disabled 的构建闭包和符号清单，
而不是新增一个依靠 shim 才能通过的 fixture。

本审计只证明 tail 对完整 C-VIDC video provider 组合的静态依赖；不证明 NTIO
启动动态路径已使用视频，也不准入任何 host shim。
