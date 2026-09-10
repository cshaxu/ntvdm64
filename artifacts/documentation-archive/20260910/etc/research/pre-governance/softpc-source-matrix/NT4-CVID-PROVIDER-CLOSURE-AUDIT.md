# NT4 C-VID Provider Closure Audit

状态：采证完成，2026-08-09。本报告记录固定 NT4 OpenNT 输入在 i386
`CPU_30_STYLE + C_VID` profile 下的 source/object 闭包；不准入 runtime、host
shim、BIOS table 缩减或任何 video 行为替代。

## 目的

验证 R1 generic `Video` tail 候选所需的原始 video owner 是否存在于可编译的
OpenNT 源输入中，并把“已有 source owner”与“已恢复历史 `ccpu486.lib`
composition”严格分开。

## 已验证的原始 object 输入

显式构建 default-disabled target
`ntdos64-opennt-video-provider-objects` 成功。它包含 19 个原始
`base/mvdm/softpc.new/base/video` 翻译单元，包括：

- `ega_prts.c`：`EGA_CPU`；
- `gvi.c`：`gvi_pc_low_regen`；
- `cga.c`：`bios_ch2_byte_wrt_fn`、`bios_ch2_word_wrt_fn`；
- EGA/CGA/VGA read/write 及 generated evidence provider 的原始 owner。

对该 target 的 COFF symbol 集计算结果为 19 objects、342 exported symbols、
290 imports。把它们放在同一 object closure 中后，`ev_glue.c.obj` 没有遗留的
provider import。这证明该原始 video source 集包含 `ev_glue` 所需的真实
EGA/CGA/provider owner；不表示它们已经初始化，也不表示启动路径已到达 video。

## 原始 vglob 的受限作用

单独显式构建原始
`base/mvdm/softpc.new/base/ccpu386/vglob.c` 成功。将它与：

1. 原始 video-provider objects；
2. 原始 C-VIDC objects；
3. 原始 `vglfunc.c` 的 `C_Video -> Video` object；
4. 本地 object-only tail initializer；

一起做 symbol-set closure 后，剩余未解析数从 125 降为 83。所有
`getVideo*`/`setVideo*` named accessor imports 均已消失；仅留下
`mouse_video_mode_changed`、`timer_video_enabled`、`working_video_funcs`
等外部 video/session 边界。

这是对 `vglob.c` 的正面、但受限的结论：它是该 source-level 闭包中 named
VGA-global accessor 的真实实现。它仍不定义 81 槽 `Video` storage，不绑定
`Cpu.Video`，也没有出现在可见的 `ccpu386/sources`。因此不得把这次单独编译
视为找回历史 `ccpu486.lib` 的 product composition。

## 加入 CCPU archive 后的边界

加入 default-disabled 原始 CCPU software-FPU archive 后，组合为 190 objects、
3,400 exported symbols、1,618 imports，剩余 84 个 imports。它们分为：

| 类别 | 代表符号 | 处置 |
| --- | --- | --- |
| CCPU 缺损语义 | `c_effective_addr`、`c_sas_touch`、`c_VirtualiseInstruction` | 仅按 caller 证据恢复；后两项当前仍 fail-closed。 |
| 原始 BIOS/ROM | `BIOS`、`copyROM`、`rom_init` | 属于完整 BIOS owner closure，不得以 reduced table 替代。 |
| 原始 SAS/I/O/ICA | `c_sas_*`、`io_*`、`ica_*`、`inb/outb` | 单独按可达性分类，不得为 video fixture 造假实现。 |
| 原始 host/event/display | `host_*`、`HostIdleNoActivity`、`paint_screen`、`VirtualX/Y` | 先确认动态 caller；只有失效 host dependency 才能形成窄 modern seam。 |
| CRT/数学库 | `malloc/free`、`memcpy`、`sin/cos` 等 | 工具链/历史 CRT 闭包，不属于 DOS 或 video 语义恢复。 |

这个 84-import 结果是静态 object-set 分析，未生成 EXE，也未调用 video、
BIOS、BOP、DEM 或 DOS。它的价值是把 R1 的下一阶段明确为“原始 owner 闭包和
动态 reachability 采证”，而不是以 stub、fake storage 或 linker force 追求一个
表面可运行的 fixture。

## 动态可达性裁定

这 84 项不能按静态 link presence 升格为启动必需项。现有受控的真实路径证据是：

```text
NTIO 50h/11h -> original demLoadDos -> NTDOS DosInit
  -> C4 C4 50 0F -> original demGetDrives boundary
```

`S5-NTDOS-DOSINIT-PREFIX-EXECUTION.md` 证明 software monitor 可执行到最后一个
`C4 C4 50 0F`，但在原始服务执行前 typed-refusal；`S5-NTDOS-FIRST-POSTLOAD-SVC.md`
证明该 service 的原始 owner 是 `demGetDrives`。两份记录均没有 video selector、
`init_vga_globals`、`set*Pointers` 或 `ev_glue` 的动态到达。

对 CCPU 缺损 import 的裁定同样不同：

- `c_effective_addr` 有实际 CCPU/BIOS caller，已是独立、受 fixture 约束的
  overlay/reconstruction 项；本次 84 项计数未将它的 object 纳入，不能误读为该
  恢复项被撤回或为新 host seam。
- `c_sas_touch`、`c_VirtualiseInstruction` 目前只见于生成的 `SasVector` slot，
  没有找到 CCPU instruction、BIOS、BOP、DEM 或 NTDOS 的直接 caller；它们继续
  fail-closed，不能为 clean link 实现推测语义。
- BIOS/ROM、SAS/I/O、display/event、timer 和 CRT 项在本闭包中均尚无新的动态
  admission。完整 `BIOS[]` 对这些 owner 的静态取址不能替代 selector trace。

所以 R1 的下一项不是把 video provider 接入 helper，而是继续寻找历史
`ccpu486` composition/生成输入，或在未来由两个独立、真实 caller 确定 storage
生命周期后再提出新的 reconstruction admission。S5 则独立沿 `demGetDrives` 的
真实原始 owner 推进。两条线都不得以本报告的 object 编译成功为由预先加入视频、
鼠标、计时、PC/AT 或 host API 行为。

## 复现

在既有 x86 Visual Studio 环境和
`artifacts/build/current/opennt-r5-clang-x86` CMake island 中：

```text
cmake --build ... --target ntdos64-opennt-video-provider-objects
cmake --build ... --target ntdos64-opennt-ccpu-vglob-objects
cmake --build ... --target ntdos64-opennt-ccpu-archive
```

随后以 `llvm-nm --defined-only --extern-only` 与
`llvm-nm --undefined-only` 汇总上述 object directory，使用同一集合抵消内部
definition。该方法是静态 link-input 采证，不模拟 linker archive extraction，
因此不应被误读为最终 link success。

## 后续限制

1. 先为 84 imports 建立 original-owner/dynamic-reachability ledger。
2. 仅在一个真实调用路径证明需要时，恢复缺失 CCPU 语义或准入现代 host seam。
3. 未获得 storage/lifecycle 证据前，generic `Video` 仍只允许 object-only
   reconstruction，不得接入 CCPU、BIOS、runner 或未来 runtime。
