# NT4 CCPU C-VIDC Profile ABI Cut

状态：原始源码采证，2026-08-09。本报告记录固定 NT4 source snapshot 内可见的
构建/头文件边界；它不授权 overlay、重写 C-VIDC、替换设备语义或把候选对象放进 runtime。

## 问题

RISC `obj.vdm` 产品配置在 `CCPU` 非空时同时要求 `CCPU + C_VID`，并消费
`ccpu486.lib`。公开快照另保留了 CCPU 与 C-VIDC component source；它们各自编译时看到的
`struct VideoVector Video` 布局并不相同。关键限制是：这两个 component directory 不在
公开的 normal `dirs` traversal 中，现存输入不能证明它们如何生成或组成 `ccpu486.lib`。
因此该 ABI 差异是恢复组合的证据，而不是当前产品 archive member 的直接断言。

## 可复查的原始输入

| 输入 | 观察结果 |
| --- | --- |
| `obj.vdm/cdefine.inc:44-53` | 非 x86、`CCPU` 非空时定义 `CPU_40_STYLE + CCPU + C_VID`；同时把 `host/inc/<arch>/prod` 加入产品 include path。 |
| `obj.vdm/sources:98-135` | 非 x86 `LINKLIBS` 在其他 SoftPC component library 后放入 `$(CPULIBS)`；`CCPU` 非空时 `CPULIBS` 是 `obj.vdm/obj/*/ccpu486.lib`。 |
| `softpc.new/dirs`、`softpc.new/base/dirs` | NT4.5 与 XP 均遍历 `base`、`host`、`obj.vdm`，而 `base/dirs` 不列 `ccpu386` 或 `cvidc`。NT4.5/XP 的 `base/dirs` SHA-256 都是 `4324BD4E10B617FAF08B6F9DA81F754DAF16C3875DB6F2C45246C66AC841D1D1`。 |
| `base/ccpu386/sources:26-45,181` | `MAJORCOMP=spclib`、`MINORCOMP=ccpu`、`TARGETNAME=ccpu386`、`TARGETTYPE=LIBRARY`，并在末尾包含其本地 `ccpudefs.inc`。 |
| `base/ccpu386/ccpudefs.inc:1` | `C_DEFINES` 含 `CPU_30_STYLE`、`CPU_40_STYLE`、`NEW_CPU`、`CCPU`、`SPC386`、`ANSI`、`NTVDM`，不含 `C_VID`。 |
| `base/ccpu386/evidgen.h:8-23` | `VideoVector` 只有 5 槽：latches getter/setter 与三个 pointer-selection 函数。`ntstubs.c:7-14` 用这个 local header 定义同名 global `Video`。 |
| `host/inc/{mips,ppc}/{prod,pig}/evidgen.h` | 现存 host generated headers 同样是 5 槽，未补出完整 table。 |
| `base/cvidc/sources:26-91`、`base/cvidc/ccpudefs.inc:1` | 与 CCPU 相同地标记为 `MAJORCOMP=spclib`、`MINORCOMP=ccpu`，但 `TARGETNAME=Cvidc`；它使用自己的 local C-VIDC closure，且仅其本地定义额外含 `C_VID`。 |
| `base/cvidc/evidgen.h:6-175` | `VideoVector` 为 81 槽；它声明同名 global `Video`，且 `ev_glue.c` 对其使用 provider slot 58--60。 |

若将这些遗留 component source 单独编译，`sources` 的局部文件与 source-dir include
优先级意味着 `ccpu386/ntstubs.c` 可见的是 5 槽定义，C-VIDC translation units 则可见
81 槽声明。这不是单一公共 header 的普通 typedef 演进；但公开 build traversal 没有说明
它们是否、何时或以何种替代 header/object 进入最终 `ccpu486.lib`。

共同的 `spclib/ccpu` profile 标签与 C-VIDC 独有的 `C_VID` 定义构成一条有限的正向证据：
两个目录不是偶然同名的独立实验，而是历史构建系统归类为同一 CCPU profile 的不同输入。
但 `NTPROFILEINPUT=yes` 只是输入元数据，未给出 archive 组装顺序、对象筛选、header
覆盖、storage owner 或最终产物名称；它不能替代缺失的 `ccpu486.lib` 生成规则。

## 已能排除的解释

1. **“可见 `ccpu386/sources` 就是 ccpu486 的产品 recipe。”** `ccpu386` 与 `cvidc`
   未被 `base/dirs` 遍历，且其 `TARGETNAME` 分别是 `ccpu386`/`Cvidc`，没有公开规则把它们
   输出为 `obj.vdm/obj/*/ccpu486.lib`。
2. **“C_VID 宏自动把 standalone CCPU 编成 81 槽。”** 当前 `ccpu386/ccpudefs.inc` 没有该宏，且其
   local `evidgen.h` 本身仍是 5 槽；现存 descriptor 没有给出替换此 header 的规则。
3. **“host `prod` generator 输出了完整表。”** 当前 MIPS/PPC `prod` 和 `pig` copy 都是
   同一 5 槽 ABI。
4. **“C_Video 可直接充当 generic Video。”** C-VIDC `C_Video` 的 0--77 provider slots
   有 initializer，而 78--80 为零；`ev_glue.c` 需要的 generic `Video` 又是另一对象名。
5. **“链接器可安全选择其中一个同名对象。”** 5 槽与 81 槽布局的同名 external object
   没有类型安全或 COFF 层面的隔离；link order、weak symbol、alias 或 `/FORCE` 都不能
   证明调用方 offset 与实际 storage 一致。

## 仍缺少的历史输入

公开 `dirs` traversal 本身已证明存在未保留的 composite/prebuilt 阶段；仅凭 component
snapshot 不能确认最终 archive 的精确 object/member 选择，也不能排除当时 build 环境以
generated include、source override 或 profile-specific object 替换了 `ntstubs.c`。要消除
该不确定性，至少需要其一：

- 同 profile 的 `ccpu486.lib` 及 member listing；
- 链接 map/listing 或 build log；
- `host/genPg`/EDL 的生成输入与产物清单；
- 可复查的产品 build drop，能显示最终 include order 与 compile command。

## 恢复约束

在以上任一输入到位前，R1 不得把“缺少 full `Video` initializer”单独视作可实现任务。
它与 CCPU short-owner 的 profile selection 是一个原子问题：必须同时证明 storage owner、
81 个槽的 typed initializer、`C_Video` 与 generic `Video` 的地址关系和初始化时刻。

允许继续做的只有只读对象/descriptor 采证和 default-disabled fixture；fixture 必须 fail
closed，且不得改变 BIOS、BOP、DEM、DOS SVC、视频或 host-device 正常路径。

当前已获准的最小采证是 `ntdos64-opennt-cvidc-generic-video-vector-objects`：它只从原
`vglfunc.c` 重放完整 generic `Video` 的 0--77 typed initializer。该 object 的成功不解决
CCPU 的 5 槽 call-site ABI，不能进入或替代 `ccpu486` product composition。
