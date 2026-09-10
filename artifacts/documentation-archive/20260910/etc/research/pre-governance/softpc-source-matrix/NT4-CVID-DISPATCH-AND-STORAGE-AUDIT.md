# NT4 C-VID Dispatch and Storage Audit

状态：采证完成，2026-08-09。本报告只记录固定 NT4 输入的预处理、对象与链接事实；不授权
新增 overlay、host shim、video handler、runtime link 或 CMake target。

## 问题

`C_Video` 已由 `cvidc/vglfunc.c` 定义，为什么同一 C-VIDC archive 的
`ev_glue.c` 仍保留对 generic `Video` 的未解析引用？这两个向量能否以 alias 或 copy
合并？

## 证据

### 产品视频代码的第一跳

`base/inc/egacpu.h:316-331` 先包含当前 profile 的 `evidgen.h`，然后在 `C_VID` 下：

- 把 `getVideolatches()` 重定义为 `C_Video.GetVideolatches`；
- 把 `setVideolatches(value)` 重定义为 `C_Video.SetVideolatches`；
- 把 `SetWritePointers`、`SetReadPointers`、`SetMarkPointers` 重定义为同名的 C 函数
  `setWritePointers`、`setReadPointers`、`setMarkPointers`。

因此，`ega_read.c`、`ega_writ.c`、`ega_prts.c` 与 `gfx_updt.c` 在
`CPU_40_STYLE + C_VID` 产品 profile 下并不通过 CCPU 的五槽 `Video` ABI 访问 latch。
例如 `ega_prts.c:1882` 的 `SetMarkPointers(0)` 会直接进入
`cvidc/ev_glue.c::setMarkPointers(0)`。

### glue 内部的第二跳

`ev_glue.c` 通过 `Evid_c.h`（原始 EDL 生成输入的输出）取得完整 generic `Video` 的
访问宏。当前 CMake 以固定 NT4 profile 编译得到的原始 `ev_glue.c.obj` 保留 `_Video`
未解析符号；archive 检查同时显示：

```text
vglfunc.c.obj:  D _C_Video
ev_glue.c.obj:  U _Video
```

`llvm-objdump -dr` 进一步显示：

- `_setReadPointers` 从 `_Video` 的完整表槽位读取 provider；
- `_setWritePointers` 从 `_Video` 的完整表槽位读取 provider；
- `_setMarkPointers` 在四种 markset 分支中经 `_Video` 的三个连续槽位调用
  `setVideomark_byte`、`setVideomark_word`、`setVideomark_string`。

这与源代码 `ev_glue.c:672-706` 的行为一致：其首先选择
`simple_mark_evid`、`cga_mark_evid`、`unchained_mark_evid` 或 `chain4_mark_evid`，再把
三个函数指针交给 generated generic-vector provider。它不是仅靠 `C_Video` 指针即可完成的
direct call。

### storage 候选不是 initializer 证据

`ccpu386/ntstubs.c` 是固定 `ccpu386/sources` 列出的对象，定义五槽短
`struct VideoVector Video`。`ccpu386/localfm.c` 虽也定义同名短 storage，却未列入三个
可见版本的 `ccpu386/sources`。二者均不能充当 C-VIDC 81 槽 generic `Video` storage。

`vglob.c` 只提供 `GLOBAL_VGAGlobals` 的按名称访问器，并明确将 latch accessor 留给
`EvPtrs.edl`；它没有 generic vector storage 或 initialization。

## 结论

1. `C_Video` 与 generic `Video` 的关系是缺失 historical composition 的问题，不能由
   C-VID 宏转发解释为“它们必然相同”。
2. generic `Video` 是 C-VIDC glue 的真实运行时输入；它不仅是无害的静态链接残留。
3. 但本报告无法给出其 storage owner、81 个槽的 initializer 或生命周期时机，故不能将
   `_Video` 以 alias、copy、短表复用、零表、stub 或 `/FORCE:UNRESOLVED` 解决。
4. 允许继续采集的输入仅限同版本 `ccpu486.lib` member/listing、link map，或
   `host/genPg`/EDL generator 的原始输入和生成规则。任何 reconstruction 必须另行满足
   `R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md` 的 profile、single-definition、
   initialization、ABI 与正反 fixture 五项准入条件。

## Tail 候选的实际链接边界

另外的 default-disabled tail object 只拿取 `setWritePointers`、`setReadPointers` 和
`setMarkPointers` 的地址，不定义 `Video` 或任何 video handler。但这三个函数共处于
原始 `ev_glue.c.obj`，该 object 本身依赖 `EGA_CPU`、`gvi_pc_low_regen`、CGA 的
`bios_ch2_*_wrt_fn` 和整组 generated `*_evid` provider。

因而，“只测尾部三槽”的最终 EXE 仍需要这些原始 owner 的 clean link；本项目不允许用
stub 或 fake storage 跳过它们。这个事实把 generic `Video` 候选的下一项工作明确
为“原始 video provider 闭包采证”，而非“为了 table test 加 shim”。
