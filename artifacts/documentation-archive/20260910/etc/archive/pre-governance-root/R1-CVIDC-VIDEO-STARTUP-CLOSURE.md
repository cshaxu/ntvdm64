# R1 C-VIDC Video Startup Closure

状态：采证中，2026-08-09。本文记录固定 NT4 输入中完整 C-VIDC `Video` 表与原始
视频初始化的真实关系。它不授权 video stub、PC/AT 设备替代、自动 lifecycle hook 或
runtime 准入。

## 已证实的启动顺序

历史 `base/support/main.c` 先调用 `cpu_init()`，随后按下列顺序完成 C-VIDC 的进程级
准备：

```text
main
  -> setup_global_data_ptr()       // ev_glue.c：分配 64 KiB Gdp
  -> setup_vga_globals()           // ev_glue.c：EGA_CPU.globals = &GLOBAL_VGAGlobals
  -> host_init_screen()
  -> InitialiseDosEmulation(...)
       -> reset()
            -> gvi_init(gfxAdapt)
                 -> ega_init()    // EGA profile
                      -> init_vga_globals()
                           -> SetMarkPointers(0)
```

最后一步必须按产品 profile 区分。通用 C-VIDC `evidgen.h` 会把
`SetMarkPointers` 定义为完整 `Video.setMarkPointers` 的间接调用；但固定产品的
`CPU_40_STYLE + C_VID` profile 中，`base/inc/egacpu.h:319-330` 随后将该宏改写为命名的
`setMarkPointers`。因此 `ega_prts.c` 的产品侧调用直接落到
`ev_glue.c::setMarkPointers(0)`，不读取完整表的 ordinal 80。

这不消除 `ev_glue.c` 内部的完整表需求：该函数通过 `Video` 的 ordinal 58--60 调用
`SetVideomark_byte`、`SetVideomark_word` 与 `SetVideomark_string`，更新原始 simple-mark
provider。完整 81-slot `Video` 的 storage 与历史 initializer owner 仍未找到；但“产品侧
必须用本地代码填充 78--80”这一推论已撤回。

## 原始 owner，不是缺失设备

| 符号 | 原始 owner | 在初始化中的作用 |
| --- | --- | --- |
| `gvi_pc_low_regen` | `base/video/gvi.c` | `gvi_init()` 所选适配器的 PC video-memory base；EGA 初始为 `CGA_REGEN_START` |
| `bios_ch2_byte_wrt_fn` / `bios_ch2_word_wrt_fn` | `base/video/cga.c` | BIOS character-write indirection，后续由 EGA write mode 或 `SetBiosWrites` 改写 |
| `EGA_CPU` | `base/video/ega_prts.c` | EGA CPU global state；`setup_vga_globals()` 设置其 `globals` 指针 |
| `Gdp` | `cvidc/ev_glue.c` | generated C-VIDC code 的 64 KiB global-data base，由 `setup_global_data_ptr()` 分配 |

因此此前把 `C_Video` 链接入最小 fixture 时出现这些 unresolved symbol，说明的是
`ev_glue.c` 整个 translation unit 被 archive extraction 拉入；它**不**证明
`setMarkPointers(0)` 本身已执行或上述全局都应由新代码重建。

## 已撤回的候选

此前两个本地候选将 `C_Video[0..77]` 映射到新的完整 `Video`，并把 78--80 绑定到命名
glue functions。profile 复核证明表尾不是产品侧调用路径，且候选没有历史 initializer 或
lifecycle owner 证据。两份 overlay、CMake target 与 manifest 条目均已移除；不保留为
reconstruction 或 runtime 输入。

## 下一项受限验证

可接受的下一 probe 必须同时满足：

- 原始 `ev_glue.c` 以 function-level section 编译，且 linker 仅保留
  `setMarkPointers(0)` 的可达闭包；
- 只观察 `Video` 的 58--60 slot 是否经原始 `setMarkPointers(0)` 改写；
- 不调用 `gvi_init()`、`ega_init()`、端口注册、BIOS、BOP、DEM、DOS 或 host display；
- 若 generated C-VIDC provider 仍要求更大的原始 closure，则记录该 closure并停止，
  不以 stub、空 handler 或 `/FORCE` 使其通过。

通过该 probe 也只证明一个 original function boundary，不能证明完整 reset 或 NTIO 已可执行。

## 函数级 section 诊断

2026-08-09，以固定 NT4 输入和同一 clang-cl historical island 给原始
`ev_glue.c` 额外施加 `/Gy`；因 COFF 直接 object link 仍要求整个 translation
unit 的 undefined 外部符号，fixture 还以 LLVM LTO 进行**链接诊断**，使 linker
按实际可达函数计算闭包。这不改变原始源码或准入任何 runtime object。其
`markset == 0` 的直接重定位闭包仅为完整 `Video` storage、`cevid_verbose`、
`printf` 和四个原始 mark-provider 数据组：`simple_mark_evid`、`cga_mark_evid`、
`unchained_mark_evid`、`chain4_mark_evid`。它不引用 `Gdp`、`EGA_CPU`、
`gvi_pc_low_regen`、BIOS character-write indirection 或 display host API。

直接把该 original object 交给当前 LLD 时，即便 `/Gy` 和 LTO 同时启用，链接器
仍会在 reachability 裁剪前要求整个 translation unit 的外部符号闭合；这会引入
`gvi_pc_low_regen`、`bios_ch2_*_wrt_fn`、`EGA_CPU` 等无关的 video/BIOS owner。
当前安装的 LLVM 也没有 `llvm-extract`，不能把单函数抽取做成可重复的本地命令。
因此未保留任何不可链接的 fixture 或 CMake target。下一步只能是取得可审计的
historical link/member input，或安装含 `llvm-extract` 的 LLVM 工具集后，将抽取步骤
作为严格 default-disabled 的 artifact-only diagnostic；在此之前，完整 `Video`
storage/initializer 仍为 unresolved，不能进入 reset 或 runtime。
