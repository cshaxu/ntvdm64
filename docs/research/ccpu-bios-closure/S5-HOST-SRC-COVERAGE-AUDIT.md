# S5 Historical Host `src` Coverage Audit

状态：2026-08-09。此审计以固定 NT4 OpenNT
`base/mvdm/softpc.new/host/src/sources` 为准，检查本项目 CMake 是否已至少保留每个历史
host source unit 的独立编译入口。它只纠正采证覆盖度，不改变 runtime 路线。

## 结果

历史 `sources` 声明 45 个 C 单元。当前 CMake 已显式列出其中 35 个；以下 10 个原始
单元尚未在任何 CMake object target 出现：

| source | 历史职责 | 当前结论 |
| --- | --- | --- |
| `cpucstbs.c` | `A3CPU` 下的 NPX interrupt、cache flush 辅助 | x86/CCPU path 在预处理后通常为空；保留为跨 profile 构建输入，不是 CCPU 缺失语义。 |
| `fprt.c` | debug pipe、`printf`/`fprintf` 等 CRT 包装 | 缺少与其匹配的历史 CRT header 输入：现代 UCRT 已声明同名函数。保持 source-only reference，不通过改名或 mock CRT 强行编译。 |
| `nt_emm.c` | `LIM` 下的原始 EMS host backend | EMS source owner；不能以 x86 EMM 或自写 memory stub 混同。 |
| `nt_input.c` | 键盘/鼠标 host input、`nt_keybd_funcs` | 其动态路径最终会进入 event/console；只允许在真实 input trace 首达时恢复。 |
| `nt_munge.c` | EGA/VGA 像素转换 | `Video` binding 未决前冻结，不能以编译覆盖为由接入。 |
| `nt_sas.c` | Sun/`HOST_SAS` SAS host 实现 | 对目标 x86 `SIM32` profile 非 owner；`sim32.c + nt_mem.c` 才是已验证的 SAS pair。 |
| `nt_sec.c` | fullscreen/video section 与 VDM/console 协调 | 依赖历史 console security/section 语义；属于私有宿主 seam 相关 source。 |
| `nt_thred.c` | application-mode thread 创建/终止协调 | 它保留原始 `host_CreateThread` 生命周期边界；不得作为通用 `CreateThread` 替身。 |
| `nt_yoda.c` | `YODA` debugger 扩展 | 默认 profile 下条件编译；仅作历史输入。 |
| `stubs.c` | `MONITOR` 条件下的 SoftPC compatibility stubs | 是历史 product 的条件单元，而非允许本项目另写 no-op 的授权；必须按实际 profile/调用者单独审计。 |

此外，CMake 中的 `nt_aorc.c` 和 `sim32.c` 不属于这份 x86 `host/src/sources` 的普通
`SOURCES` 列：前者属于 RISC 条件来源，后者通过其他 build composition 输入。这个差异是
历史 build 规则的组成部分，不能以“名单不一致”简单删除。

## 对 S5 的影响

1. 这十个单元不能被当成 no-SM0 date/loader closure 的即时依赖；当前动态证据仍只覆盖
   `50h/14h`、`50h/11h` 和其紧邻 idle 通知。
2. `fprt.c` 与 `stubs.c` 特别危险：它们的名字或条件代码看似方便解决链接问题，但将它们
   用作现代输出/设备/quick-event 替代会直接违反“不以 local replacement 推进 runtime”。
3. 在建立新的 default-disabled compile inventory target 前，必须按 target profile 写明每个
   文件的预处理开关、header order、可能的 CRT/console collision 和它在历史 `sources`
   中的位置；此任务是下一项 source-only 工作，不是 runtime 接入。
4. `nt_input.c`、`nt_munge.c`、`nt_sec.c`、`nt_thred.c` 的恢复条件分别仍由 input、Video、
   console/fullscreen、thread lifecycle 的真实 caller trace 决定。

## 后续准入

当前的两个**只编译、不链接** default-disabled inventory target 已覆盖其中九个可在当前
headers 下保持原语义的输入：portable 组的五个单元和 native 组的四个单元均已通过 i386
compile。`fprt.c` 保持独立 source-only reference，直至获得对应历史 CRT header/toolchain。
这不是运行时阻塞，也不是允许重写 CRT wrapper 的理由。inventory target 必须：

- 不依赖或导出任何 local compatibility 行为；
- 不出现在 `ntdos64-run`、默认 CTest 或研究 runtime；
- 对每个需要 compiler-only compatibility 的源文件做单独登记；
- 对任何实质运行时依赖失败停止，记录 owner/seam，而不是补写实现。

只有审计完成且真实动态路径抵达对应历史 caller 后，某个 source unit 才可进入
owner-first link closure。

## 验证记录

2026-08-09，在
`artifacts/build/current/opennt-r2-bridge-ninja-i686` 中以 clang-cl i386 object build
通过：

- `ntdos64-opennt-historical-host-portable-inventory-objects`：
  `s5-host-src-inventory-ntdos64-opennt-historical-host-portable-inventory-objects-v4-20260809.log`；
- `ntdos64-opennt-historical-host-native-inventory-objects`：
  `s5-host-src-inventory-ntdos64-opennt-historical-host-native-inventory-objects-v4-20260809.log`。

两项均为 object target，未产生 link 或执行步骤。早期 `fprt.c` 失败记录保留在同目录的
`...portable-inventory-objects-v3-20260809.log`；失败原因是现代 UCRT 的 `stdio.h` 已声明
`printf`/`fprintf`，不是可由 compiler-only 宏在不改变历史符号语义的条件下消除的问题。
