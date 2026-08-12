# R1 C-VIDC Reference Comparison

状态：对照采证，2026-08-09。本文内容为中文；文件名遵循仓库英文命名规则。

## 目的与边界

本文记录固定 NT4 OpenNT 输入与本地 NTVDMx64 对照输入在 C-VIDC `Video` vector
断裂上的关系。它不授权把后者的代码、patch 或产品策略并入 OpenNT runtime，也不改变
`R1-CCPU-CVIDC-COMPOSITION-RECOVERY-SPEC.md` 的恢复前置条件。

NTVDMx64 在本项目中始终是外部参考：它可提示曾经出现过的兼容问题和候选语义，却不能
证明 NT4 的历史 storage owner、初始化时刻或构建 profile。

## NT4 原始事实

固定输入中存在三个独立对象：

1. `base/cvidc/vglfunc.c:86` 定义并初始化完整 81-slot `C_Video`；0--77 为 generated
   provider，78--80 没有 initializer。
2. `base/cvidc/ev_glue.c:672` 的 `setMarkPointers()` 依次经过完整 `Video` 的
   `SetVideomark_byte`、`SetVideomark_word`、`SetVideomark_string` slots（58--60）。
3. `base/ccpu386/ntstubs.c:14` 定义的是另一种 5-slot ABI 的 `Video` storage；其大小
   不能承载 C-VIDC 的 81-slot ABI。

产品侧并没有消除该问题：`base/inc/egacpu.h:318-331` 在 `C_VID` 下令部分 latch access
直接走 `C_Video`，并把 `SetWritePointers`、`SetReadPointers`、`SetMarkPointers` 改写为
命名 `ev_glue.c` 函数。该命名入口内部仍需要完整 `Video`。

因此，`C_Video` 与完整 `Video` 不能因名称相近被认为同一 storage；短 `Video` 也不能
承担完整表。

另一个约束来自产品 build graph：`obj.vdm/sources` 的 CCPU 分支只链接缺失的
`ccpu486.lib`，而不单独列出可见 `ccpu386` 或 `Cvidc` library；`vglob.c` 又不在
`ccpu386/sources` 中。因而本项目分开编译两个 source archive 的行为仅可用于诊断，不能
被视为原始 CCPU product composition。

## 外部对照事实

本地对照 patch
`upstream/ntvdmx64/ntvdmpatch/patches/common/cvidc.patch` 对
`base/cvidc/vglfunc.c` 作出如下变更：

```diff
-struct VideoVector C_Video =
+struct VideoVector C_Video, Video =
```

同一参考树的 `patches/common/ccpu.patch` 则注释掉了
`ccpu386/ntstubs.c` 内的短 `struct VideoVector Video;`。但上述 C 声明的 initializer
只附着在第二个 declarator `Video`；`C_Video` 会保持零初始化。该补丁并未改写
`base/inc/egacpu.h` 的 `C_VID` 宏，后者仍让普通 latch access 使用 `C_Video`。
因此它不是“同时初始化两个完整 table”的证据，更不能证明 NT4 的历史 composition。

补丁驱动脚本还表明，最终的 CCPU 路径将原 C-VIDC 替换为
`experimental/cvidc-new`，并在 `obj.vdm/sources.inc` 中链接
`cvidcnew\\...\\cvidc.lib`。参考树自己的 `doc/cvidc.txt` 把该组件描述为手工
rewrite，明确列出原 generated C-VIDC 的已知问题。故 `common/cvidc.patch` 只能记录为
一个非对称、未验证的中间兼容改动；它既不是原 C-VIDC 的可执行 oracle，也不是本项目
恢复 generic `Video` 的候选实现。

参考项目自己的 `doc/cvidc.txt` 还明确说明其 C-VIDC 路线包含生成代码的缺失、运行时
bug 与手工 rewrite，且其 `cvidc-new` 是独立实现。故该项目不能被当成 NT4 原构建的
二进制或 source-level authority。

## 本地官方二进制的适用范围

`D:\home\repos.hobby\OpenNT-4.5\nt\public\sdk\lib\i386\ntvdm.exe` 是可读的
官方 i386 COFF executable（SHA-256：
`06618D9788036AC59EF6C37FF4000DF9EF39C58521C595DB875A7E2ABD84C231`）。它保留 COFF
symbol/line-number table，且可观察到 `_MS_bop_0@0`、`_DemDispatch@4`、
`_setup_vga_globals@0` 等 x86 host/dispatcher 符号。

但它**不是** CCPU composite oracle：它不含 `c_cpu_*`、`C_Video`、完整 `Video`、
`setMarkPointers` 或 generated C-VIDC provider symbols。这符合同代
`obj.vdm/sources` 的产品条件：i386 `$(386)` 路径链接 V86 `monitor.lib`，CCPU library
仅在非-386 路径选择。故该二进制可用于未来 NT4.x host/BOP/DEM 行为的只读比对，不能用来
推断 `ccpu486.lib` 的成员、`Video` storage 或 C-VIDC 初始化。

同一 SDK 的 `alpha`、`mips`、`ppc` library directories 未发现 `ntvdm.exe`、`ccpu486.lib`
或 `cvidc.lib`；因此当前本地官方二进制输入没有保留 CCPU product oracle。

## 可得与不可得的结论

| 命题 | 结论 | 依据 |
| --- | --- | --- |
| 当前 NT4 snapshot 缺少完整 `Video` 的可审计 binding/initializer owner | 已证实 | 原始 `vglfunc.c`、`ev_glue.c`、`ntstubs.c` 及 R1 COFF 审计 |
| 后代项目曾尝试把 generated initializer 放到完整 `Video`，同时将短 owner 注释掉 | 已证实为参考项目的一段补丁行为；最终路线另换 `cvidcnew` | NTVDMx64 `cvidc.patch`、`ccpu.patch`、`minnt/makefiles.patch` |
| 该尝试同时正确初始化 `C_Video` 与 generic `Video` | 被 C declarator 初始化规则及未改写的 `egacpu.h` 否定 | `cvidc.patch` 与 NT4 `egacpu.h` |
| 该策略就是 NT4 历史构建方法 | 未证实 | 缺失 `ccpu486.lib`、map、生成输入和历史 build log |
| `C_Video`、完整 `Video`、短 `Video` 可做 alias 或 memcpy | 被 ABI 事实否定 | 81-slot 与 5-slot ABI 分离 |
| 可将 NTVDMx64 patch 直接接入本项目 runtime | 禁止 | 本项目 SoftPC-first 治理和本报告边界 |

## 后续动作

1. 继续寻找 NT4 同 profile 的 `ccpu486.lib` member、link map、build log 或生成输入；
   这是唯一能把候选策略升级为历史实现证据的路径。
2. 若找不到，则以两个原始 caller 的只读 trace 分别定义完整 `Video` 的必需 slot 和
   初始化时刻；这仍不能跳过 ABI/single-definition proof。
3. 在满足恢复规格前，不产生 `Video = C_Video`、table copy、linker alias 或任何
   runtime overlay。
