# S5 CCPU Product Composition Inventory

状态：2026-08-09。本文区分“CCPU 普通源码是否缺失”和“历史 NTVDM 产品是否仍有
可复建的组合输入”。它不授权以 stub、短 `BIOS[]`、伪 Video，或 `/FORCE:UNRESOLVED`
产出 executable。

## 直接清点结果

`base/mvdm/softpc.new/base/ccpu386/sources` 声明 130 个 C source。按其 `SOURCES=`
清单逐项检查，本仓库中 **130/130 均存在**，合计 888,141 bytes。

因此，下列结论成立：

- `C4 C4` BOP decode 所在的 `c_main.c` 并未丢失；
- `ccpusas4.c`、get/set register、real/protected-mode helper 与普通 instruction
  unit 均在该 source-list 内；
- 当前不能把 CCPU 无法成为完整 product 归因为“少了一个普通 `.c` 文件”。

## 历史组合不连续处

原始 build metadata 仍暴露一个未闭合的 profile/product 连接：

| 输入 | 直接事实 | 未知/缺口 |
| --- | --- | --- |
| `ccpu386/sources` | `TARGETNAME=ccpu386`，并包含 `ccpudefs.inc`。 | 该 library 如何在最终 product 中变成/并入 `ccpu486.lib` 未在当前 source tree 的 module metadata 中解释。 |
| `ccpudefs.inc` | 同时定义 `CPU_30_STYLE`、`CPU_40_STYLE`、`NEW_CPU`、`CCPU`、`SPC386`、`NTVDM`。 | 仅定义编译 profile，不给出完整的 final archive/member order 或生成器产物。 |
| `obj.vdm/sources` | `CCPU` 非空时，`CPULIBS` 指向 `obj.vdm/obj/*/ccpu486.lib`。 | 固定快照没有对应的可追溯 archive、map 或生成规则。 |
| `cvidc/sources` | C-VIDC 是独立 archive，含 generated `SINIT*`/`SEVID*` 与 `ev_glue.c`。 | `ev_glue.c` 对 generic 81-slot `Video` storage/initializer 的组合仍未被该 source tree 完整说明。 |
| `ntvdm.c` link surface | 最终 product 同时拉入 host/src、system、bios、video、dos、comms、disks、keymouse 以及 DEM/DPMI/XMS/OEM libraries。 | 这是一个完整历史产品而非“CPU+BIOS”二元链接；各项 profile 条件和顺序必须被恢复。 |

这与 NT4 x86 的历史 V86 `MONITOR` 路径也不能混同。CCPU 配置是软件 CPU
profile 的材料和语义证据，不自动证明它是原 NT4 x86 产品实际使用的 execution
profile。

## 当前 clean-link 证据的分类

最近的 `direct-ccpu-linker-unresolved.txt` 显示的未解析符号可分为：

1. **CCPU/生成语义缺口**：`EDL_fast_bop`、`c_sas_touch`、
   `c_VirtualiseInstruction`。前两者没有在本地 NT4/NT4.5/NT5/XP 搜索范围内找到
   原始实现；其中后两项已有 fail-closed 治理约束，不能以 no-op 填补。
2. **C-VIDC product composition**：`setVideo*`/`getVideo*` 群及 Video storage
   初始化。这是完整 `BIOS[]` 静态引用带入的产品闭包，不是 NTIO 当前动态路径已要求
   的视频设备语义。
3. **历史 host/product APIs**：console、VDM control、NT native string/file API、
   timer/event、OEM/video/disk provider。它们应先按真实 caller 到达情况分类为
   original owner、允许的 modern seam，或未准入；不能为让链接成功而通配替换。

即使固定 `sysinit` 的 `BOP 12h` 仅动态需要 `BIOS[0x12] -> memory_size()`，
原始 `bios.c` 的完整 table 仍静态引用所有条目。将 table 缩为单个 entry 会制造一个
新的 runtime，而不是恢复原始 SoftPC。

## 对 R1 的实际含义

R1.1 不能拆成“给 CCPU 加一个 BOP decoder”或“单独链接 memory_size”。正确的
恢复单元为：

```text
historical CCPU profile initialization
  + historical SAS lifecycle
  + complete BIOS[] static composition
  + profile-matched C-VIDC workspace/vector inputs
  + only dynamically reached historical host owners
```

允许的下一步按 ROI 排序：

1. 在本地所有历史树及已有构建痕迹中继续寻找 `ccpu486.lib` 的 archive/map/member
   list、CCPU profile 的 final link response，或生成器输出清单；
2. 将上述链接输入与当前 130-file source list 作 member-by-member 对照，确认是命名/
   archive 规则丢失，还是确有生成 unit 缺失；
3. 仅在原始 caller 已动态到达时，恢复其 owner 或准入窄 modern host seam；
4. 对无原始实现的 CCPU slot 保持 fail-closed，并记录其实际 reachability。

明确不做：将 archived `EDL_fast_bop` trap、旧 BOP/DEM adapter 或 `Video=C_Video`
接回 runtime；它们最多是负例/trace 证据，不能充当历史 product composition。

## 本地跨版本 archive 采证结果

针对 archive/map/response/profile 文件名及 `ccpu486`、`ccpu386.lib`、`CPULIBS`
文本，对可用本地快照进行目标化扫描的结果如下：

| 树 | 结果 |
| --- | --- |
| 固定 NT4 输入与 `D:/home/repos.hobby/opennt` | 只有 `softpc.new/obj.vdm/sources` 与 `ntvdm.prf` 的 profile 引用；未发现 `ccpu486.lib`、map、response 或 member list。 |
| `D:/home/repos.hobby/opennt-4.5` | 同样只有 `nt/private/mvdm/softpc.new/obj.vdm/sources` 和 profile 引用；未发现产物或生成输入。 |
| `D:/home/repos.hobby/win2000src/private` | 未发现此 SoftPC/CCPU archive 或对应 build metadata。 |
| `D:/home/repos.hobby/winxpscodes` | 未发现 NTVDM SoftPC/CCPU archive；命中的 `wow64/mscpu` 是另一套 XP WOW64 CPU 组件，不能替代历史 CCPU。 |

这只是对当前本地来源的负证据，不排除外部历史 build drop 存在该输入；但它关闭了
“继续在已登记四份本地树中搜索 archive”这一项的 ROI。下一步若要推进 clean link，
应恢复/取得 profile 级组合证据，而不是重复 filename scan。

同一轮定向检查还确认，NT4、NT4.5 与本地 XP 路径中均不存在下列被 component
metadata 引用或与生成组合相关的目录：

```text
softpc.new/host/genPg
softpc.new/base/cpu/src/evid
softpc.new/base/jcode/hdigest/c
```

这排除了“生成源仍在树中、只需修正 include path”的解释。它们只能视为未随当前
source snapshot 保留的 generator/profile build input；不得以新建空目录、复制邻近
header 或以 local overlay 假装其历史输出。

## 证据来源

- `src/opennt/base/mvdm/softpc.new/base/ccpu386/sources`
- `src/opennt/base/mvdm/softpc.new/base/ccpu386/ccpudefs.inc`
- `src/opennt/base/mvdm/softpc.new/base/cvidc/sources`
- `src/opennt/base/mvdm/softpc.new/obj.vdm/sources:131-136`
- `artifacts/build/current/direct-ccpu-linker-unresolved.txt`
- `docs/research/ccpu-bios-closure/S5-CCPU-BOP12-DISPATCH-CONTRACT.md`
