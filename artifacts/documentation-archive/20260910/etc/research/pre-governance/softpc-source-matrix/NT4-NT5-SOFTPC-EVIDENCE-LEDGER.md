# NT4-NT5 SoftPC Evidence Ledger

状态：进行中。每条记录仅陈述可复查事实；结论不足时使用 `unresolved`。

## E-BUILD-001: NT4 CPU backend library selection

- 状态：`confirmed`
- 问题：产品层如何在 V86/a4 与 CCPU software backend 之间选择？
- NT4 证据：`src/opennt/base/mvdm/softpc.new/obj.vdm/sources:132-136`。
  `CCPU` 为空时为 `a4$(PD).lib`，非空时为 `ccpu486.lib`。
- SHA-256：`F80C0D8513EA68500C792674D05DEE7F42F1F76AED98F4F5CA6BC409FFD61F3E`。
- 结论：两者是构建选择，不是可随意混合的对象来源。
- 禁止动作：把 a4/V86 对象作为 CCPU 缺符号的便利填充。

## E-BUILD-002: NT4 CCPU profile boundary

- 状态：`confirmed`
- 问题：NT4 的 x86 monitor 与 CCPU 是否是同一宏 profile？
- NT4 证据：`src/opennt/base/mvdm/softpc.new/obj.vdm/cdefine.inc:35-49`。
  x86 为 `MONITOR + C_VID + X86GFX`；非 x86 CCPU 为
  `CPU_40_STYLE + CCPU + C_VID`。
- SHA-256：`A5CC355BFAB633968217C85A4BB210F684039E08E7784578800C672F65772F14`。
- 结论：不是同一 profile。以 CCPU 研究 x64 软件执行时，必须单独说明其与 NT4
  x86 V86 产品路径不同。
- 禁止动作：把 `MONITOR` 或 `X86GFX` 无证加入 CCPU/C-VID 组合。

## E-BUILD-003: CCPU 与 C-VID 的内部定义

- 状态：`confirmed`
- 问题：CCPU 与 C-VID 的 source-level 定义是否完全相同？
- NT4 证据：
  - `base/ccpu386/ccpudefs.inc:1`：CPU_30_STYLE、CPU_40_STYLE、NEW_CPU、CCPU、SPC386、ANSI、NTVDM。
  - `base/cvidc/ccpudefs.inc:1`：以上定义外加 `C_VID`。
- SHA-256：
  - CCPU：`5AEAAA72F958EF891628C267EE4849A0203C25AF0FBE67B3FDC890B75DFB3FD8`
  - C-VID：`DE75D67C7168F04AE82715B4DD56663093589D3FDD36BD828C6267BD6D041A6D`
- build-profile 补充（2026-08-09）：两个 `sources` 都声明
  `MAJORCOMP=spclib`、`MINORCOMP=ccpu` 和 `NTPROFILEINPUT=yes`；仅 C-VIDC 的本地
  `ccpudefs.inc` 额外定义 `C_VID`。这确认两者被历史构建元数据归入同一 CCPU profile，
  但没有给出 archive member 选择、对象合并次序、include 覆盖或 storage owner。
- 结论：C-VID 是独立 source closure，不能被假定为 CCPU archive 的普通子集。

## E-BUILD-004: CCPU generated/library input absence across source snapshots

- 状态：`confirmed`
- 问题：后代源码快照是否补出了 `ccpu486.lib` 的成员、`host/genPg` 输入，或
  可替代的 SoftPC link map？
- NT4、NT4.5、XP 共同证据：三份 `base/ccpu386/sources:44` 都把
  `$(SOFTPC_TREE)\host\genPg\inc` 放入 include path；三份 product link
  descriptor 都在 CCPU 分支选择 `ccpu486.lib`。三份相应 `softpc.new` tree 内
  均没有 `host/genPg` 目录、`ccpu486.lib`、`ccpu486.map`、`cvidc.lib` 或
  `cvidc.map`。NT4.5/XP 的 `ccpu386/sources` SHA-256 都为
  `6DC49164A86EFB90776924F463FC0E1EC064436E21339B92E709449A73779431`。
- 范围限制：这是各自已定位 `softpc.new` tree 的 not-found，不是对整台机器、
  发行介质或未采集 build drop 的存在性断言。
- 重复快照校验：另一份本地 XP 根
  `D:\home\repos.hobby\winxpsrc\Source\XPSP1\NT\base\mvdm\softpc.new`
  的 `ev_glue.c`、`vglfunc.c`、两个 vector-storage source 与 component
  `sources` 文件均和已登记 XP root 逐字节一致，且同样没有上述输入；额外
  `OpenNT` / `OpenNT-4.5` 镜像中的同一组文件也与前述 NT4/NT4.5 hash 一致。
  它们是重复采证，不是新的实现来源。
- 结论：NT4.5、XP 没有提供缺失 CCPU/C-VIDC composition 的恢复输入；当前候选
  reconstruction 仍不能以“后代 tree 也这样引用”为正确性依据。
- build traversal 补充（2026-08-09）：NT4.5 与 XP 的 `softpc.new/base/dirs` 都不列
  `ccpu386` 或 `cvidc`，但产品 `obj.vdm/sources` 仍消费 `ccpu486.lib`。因此公开 source
  snapshot 保留的是独立 component descriptors 与最终消费者，缺少将它们组成该 composite
  library 的历史 step；不得把 `ccpu386/sources` 的 member list 当作 `ccpu486.lib` 清单。
- 下一动作：仅寻找版本匹配的 build drop、archive member、link map 或 generator；
  不把 V86/a4、Bochs、PCjs 或本地新 video/device 代码当成该输入的替代物。

## E-SAS-001: CCPU SAS vector 与入口输入跨版本保持不变

- 状态：`external confirmed`
- 固定比较文件为 `base/ccpu386/sascdef.c`、`ccpusas4.c`、`cpu4gen.h`、`sources` 和
  `c_main.c`。NT4.5 与 XP 的前四项和 NT4 分别逐字节一致；`c_main.c` 也均为
  `F6CD45A30B6575D38EF95083601594F1403A34EBEF1278DBB8362C19F4A14700`。
- SAS 核心输入的 SHA-256 为：
  - `sascdef.c`：`B8B7832D02BD8F4161E1F71DF2968051609DA065F4DA8287A65E6B14B38A749C`；
  - `ccpusas4.c`：`2802840EF84106AE0F99C6F17AF370DA66C9381A1355570F5998039FBB33534E`；
  - `cpu4gen.h`：`7C03D8F75644E6C2D0B4FB1A1E66AB93C55225DF61E8329D5FFFDF1D08A5FCCB`。
- 三份 `sascdef.c` 均在同一位置声明 `c_sas_touch`、`c_VirtualiseInstruction`，并把它们
  置于 `cSasPtrs` 的同一尾部 slot；三份 `ccpusas4.c` 均以 `SasSetPointers(&cSasPtrs)`
  安装该 vector。没有任一后代源码补出 C 定义、selector owner 或直接 C caller。
- 结论：后代重复输入强化了“这是缺失 generated/vector completion”的判断，但不提供其
  语义。两个 slot 继续 fail-closed；不得因跨版本一致而实现猜测行为。

## E-ENTRY-001: NT4/NT4.5 startup 相同，XP reset/video 已演化

- 状态：`external confirmed`
- NT4 与 NT4.5 的 `base/support/main.c`、`base/bios/reset.c`、
  `host/src/nt_msscs.c`、`base/video/gvi.c`、`base/video/ega_prts.c` 均逐字节一致。
  例如 `main.c` 的 SHA-256 为
  `5DFDCA835F1CA6FDDF07826F2B4AA9E25F4C8BD0CEEF21B2DCE9884DE721376C`，`reset.c` 为
  `46E2507AD47182125ED59206780763C5DADF6EA752A43D4D946451DE1024D90B`。
- XP 的同名文件均不相同：相对于 NT4，`main.c` 为 23 additions / 7 deletions，
  `reset.c` 为 1,330 / 688，`nt_msscs.c` 为 469 / 383，`gvi.c` 为 16 / 0。XP 还在
  `gvi.c` 中增加无参数 `gvi_init()`，并在 `reset.c` 增加对应调用分支。
- 结论：NT4.5 能作为 NT4 startup/reset 的同源旁证；XP 仅证明该区域后续发生产品演化，
  不可将其新增 reset/video 行为复制或反推到 NT4 SoftPC reconstruction。

## E-VEC-001: C-VID and generic video vectors

- 状态：`unresolved`
- 问题：非 PIG 的 CCPU + C-VID 组合中，`Video` 如何获得与 `C_Video` 等价的可调用向量？
- NT4 证据：
  - `base/cvidc/vglfunc.c:86` 定义 81-slot typed `C_Video`，但只显式初始化
    0--77；78--80 保持 aggregate 零值。生成的 `sinit013.c` 虽有
    `S_0441..0443` wrapper，却未被该 initializer 引用。
  - `base/ccpu386/ntstubs.c:7-14` 在非 PIG 分支声明 `C_Video`，但定义的是零初始化 `struct VideoVector Video`。
  - `base/cvidc/ev_glue.c:672` 的 `setMarkPointers` 在函数内部依赖通用 `Video`。
  - `base/inc/egacpu.h:319-330` 在 `CPU_40_STYLE + C_VID` 下将
    `SetWritePointers`、`SetReadPointers`、`SetMarkPointers` 改定向为命名的
    C-VIDC glue functions；产品侧调用不经过完整 `Video` 的 78--80 表尾。
- SHA-256：
  - `ntstubs.c`：`E25542825EB5EFB323DEAE41E8D8368E75E5DCC2ACABAD02A974F602E67641E8`
  - `vglfunc.c`：`22385F8A0F196C79B2F27B89EE33F7AE9D955304385D226522E2C06FA8726805`
  - `ev_glue.c`：`A12DDFD55DBD3B4426FF20075C46A665C6A30C6A955F24F6AB4872FB4895E935`
- 当前结论：源码已证明两个 vector storage 的存在和间接调用；尚未证明历史绑定者、
  其初始化时机、或 `ntstubs.c` 是否属于完整运行时链接组合。
- 跨版本采证（2026-08-09）：
  - NT4.5 与 XP 的 `ev_glue.c`、`vglfunc.c`、`cvidc/evidgen.h`、`ntstubs.c`、
    `localfm.c`、`ccpu386/evidgen.h`、两个 component `sources` 均与 NT4
    SHA-256 完全一致。
  - NT4.5 的 `obj.vdm/cdefine.inc` 与 NT4 完全一致；XP 的该文件有
    `FREEBUILD`/路径/`ARCX86` 产品差异，但 `CCPU` 非空的非 x86 分支仍为
    `CPU_40_STYLE + CCPU + C_VID`，并仍选择 `ccpu486.lib`。
  - 已检索的 Win2000 `private/mvdm` 快照仅有 `inc`、`wow16`、`wow32`，不存在
    `softpc.new`、`C_Video`、`setMarkPointers` 或 `ccpu486` source/build input；
    这仅是该声明快照范围内的 not-found，不能推断 Win2000 产品没有 SoftPC。
- 当前结论：三份可用 SoftPC 源码没有补出 `Video` 的历史 binding/initializer；
  缺口稳定存在，不能用“后代也相同”反推本地 overlay 的正确性。
- 允许的下一动作：寻找同版本的 `ccpu486.lib` member、link map 或 host generator
  输入；不将后代源码复制进 `src/opennt/`。
- 禁止动作：以 `Video = C_Video`、fake `setMarkPointers` 或任何 host shim 推进 runtime。

完整的两跳 dispatch、对象 relocation 与 storage-candidate 审计见
`NT4-CVID-DISPATCH-AND-STORAGE-AUDIT.md`。该报告确认 generic `Video` 是 C-VIDC
glue 的真实 provider 输入，同时确认当前输入没有其历史 owner 或 initializer；它不改变
本条的 `unresolved` 状态。

关联审计：`NT4-CVID-COMPOSITION-ENTRY-AUDIT.md` 已进一步确认原始
`main.c` 的 `setup_global_data_ptr()` / `setup_vga_globals()` 调用顺序，
并证明其 C-VID 原始实现不读写 `Video` 或 `C_Video`。因此这两个启动函数
不能被误作未找到 vector binding 的替代 initializer。该审计还确认，在
`CPU_40_STYLE + CCPU + C_VID` 且 EGA/VGA 配置下，
`InitialiseDosEmulation -> reset -> gvi_init -> init_vga_globals ->
SetMarkPointers(0)` 会在 NTIO 前动态抵达这三个通用 `Video` mark slot。
这使其成为 CCPU software-monitor reset 的 blocker，但不改变 NT4 x86
`MONITOR + X86GFX` 路径与 CCPU 路径互斥的结论。

### E-VEC-001 补充：产品侧表尾推论撤回

- 状态：`resolved-negative`
- 2026-08-09 复核：`video/ega_prts.c:1882` 写作 `SetMarkPointers(0)`；在产品使用的
  `CPU_40_STYLE + C_VID` profile，它经 `base/inc/egacpu.h:326-330` 展开为直接的
  `setMarkPointers(0)`。因此不能把该 caller 当作完整 `Video.setMarkPointers` ordinal 80 的
  调用证据。
- 仍未解决：被调用的 `ev_glue.c::setMarkPointers` 本身通过完整 `Video` 的 ordinal 58--60
  调用 mark providers，故完整表 storage/initializer owner 仍是 R1 缺口。
- 处置：此前本地 0--77 provider / 78--80 tail reconstruction 候选已从源码、CMake 和
  runtime 路径撤回。后续仅准入了更窄的 default-disabled object-only 字段采证：
  原始 `vglfunc.c` 可在 `C_Video -> Video` 的预处理 rename 下提供 typed 0--77
  initializer object，本地代码只以原始签名尝试写入 78--80 named glue 地址。该对象
  不定义 storage、不链接 CCPU、BIOS、fixture、runner 或 runtime，且不构成历史
  `ccpu486.lib` composition、storage lifecycle 或 `Cpu.Video` binding 的证据。约束见
  `R1-GENERIC-VIDEO-RECONSTRUCTION-ADMISSION.md`。

### E-VEC-001 补充：单函数闭包的工具限制

- 状态：`unresolved`
- 2026-08-09 证据：固定 NT4 `ev_glue.c` 以 `/Gy` 编译时，
  `setMarkPointers(0)` 的 COFF relocation 不触及 `Gdp`、`EGA_CPU`、
  `gvi_pc_low_regen` 或 BIOS write indirection；完整 object 的直接 LLD link
  仍要求这些不相关的 translation-unit symbols。启用 LTO 未改变该限制。
- 结论：这只排除了“整个 object 的 unresolved 列表等于 setMarkPointers 所需
  runtime 依赖”的错误推理；它没有给出 `Video` 的历史 binding/initializer。
- 后续：优先采集跨版本 library member、map 和 build-generator 输入；若安装具备
  `llvm-extract` 的工具集，抽取过程只能作为 artifact-only、default-disabled
  diagnostic，并且不能替代跨版本 source evidence。

### E-VEC-001 补充：后代 patch stack 的限定

- 状态：`external-reference-only`
- 2026-08-09 审计 `refs/ntvdmx64/ntvdmpatch/patch.cmd`、
  `patches/common/cvidc.patch`、`patches/common/ccpu.patch`、
  `patches/minnt/makefiles.patch` 与 `doc/cvidc.txt`：common patch 将声明写为
  `struct VideoVector C_Video, Video = { ... };`，其 initializer 只属于 `Video`，
  不初始化 `C_Video`；同时没有修改 NT4 `egacpu.h` 的 `C_VID` ordinary-latch 宏。
- 同一 patch stack 的最终 CCPU build rule 链接 `cvidcnew`，而非原 C-VIDC；文档把
  `cvidc-new` 描述为手工 rewrite，并承认原 generated C-VIDC 的多项已知缺陷。
- 结论：该外部路线只能作为“后代兼容尝试”的旁证，不能证明 NT4 的 `Video` storage
  owner、initializer 或启动时刻；不得据此增加 overlay、alias、copy 或 runtime binding。

关联的原始 source-profile ABI 审计见 `NT4-CCPU-CVIDC-PROFILE-ABI-CUT.md`：它确认
当前 CCPU component 的 local/host `evidgen.h` 是 5 槽，而 C-VIDC component 是 81 槽。
故 `Video` 问题同时包含 storage owner 与 translation-unit ABI/profile selection，不能只靠
补一个 initializer 解决。

### E-VEC-002：同树 EVID provider 语义，不构成 composition 输入

- 状态：`source evidence only`
- 固定 NT4 源树及其本地 OpenNT 镜像均有
  `base/mvdm/softpc.new/base/cpu/src/evid/vglob.c`。该原始文件定义
  `setVideomark_byte`、`setVideomark_word`、`setVideomark_string` 等命名函数，直接写入
  `GLOBAL_VGAGlobals`；它说明 `ev_glue.c::setMarkPointers(0)` 的 ordinal 58--60 provider
  的历史语义。
- 同目录含 `ev_glue.c` 与 `ev_stubs.c`。其中 `ev_stubs.c` 明确是 a4+EVID / CCPU+CEVID
  接口 stub，不能代替当前 C-VIDC product composition。
- 对固定 `softpc.new` 树的 `sources`、make/nmake、include、cmd/bat build descriptors 搜索
  未找到 `cpu/src/evid/vglob.c` 或该目录的构建引用。因此本项不能证明完整 `Video` 的
  storage/initializer owner，也不授权把该目录加入 CMake 或 runtime。
- 2026-08-09 复核补充：NT4、NT4.5 与 XP 的该目录均含同一组
  `ev_glue.c`、`ev_stubs.c`、`vglob.c`。其中 `vglob.c` 的 SHA-256 与
  `ccpu386/vglob.c` 相同（`E695A980...94395D8`）；`ev_stubs.c` 明确以空
  `setup_vga_globals` / `setup_global_data_ptr` 提供接口占位。前者不增加 vector
  owner，后者属于禁止进入 runtime 的历史 stub；该目录的较早 `ev_glue.c` 版本也不定义
  完整 `VideoVector` storage 或 initializer。
- 结论：它是恢复完整表字段级语义时可引用的原始同树证据；在找到历史组合规则前，仍保持
  source-only。

### E-VEC-003：跨版本 `vglob.c` 不包含短 Video ABI 或 binding

- 状态：`external confirmed`
- 问题：NT4.5 或 XP 是否保留了 NT4 快照未收集的短 `Video` initializer、latch provider，
  或 `Cpu.Video` binding？
- 固定文件复核（2026-08-09）：下列三个 `ccpu386/vglob.c` 的 SHA-256 都是
  `E695A9807285703119CFFB0C3BC8A3AA6433EDE43EFA499F21A6E736F94395D8`：
  - NT4：`src/opennt/base/mvdm/softpc.new/base/ccpu386/vglob.c`；
  - NT4.5：`D:\home\repos.hobby\opennt-4.5\nt\private\mvdm\softpc.new\base\ccpu386\vglob.c`；
  - XP：`D:\home\repos.hobby\winxpscodes\Source\XPSP1\NT\base\mvdm\softpc.new\base\ccpu386\vglob.c`。
- 文件事实：三个版本同样标为由 `Vglob.edl` 派生；原始注释明确说明
  `getVideolatches`/`setVideolatches` 仍留在 `EvPtrs.edl` 以供 pigging 使用。该文件只
  定义 `GLOBAL_VGAGlobals` 的命名 getter/setter；未定义短 ABI 的 latch 或
  `setWritePointers`/`setReadPointers`/`setMarkPointers`，也不写入 `Cpu.Video`。
- 相关固定输入同样一致：三版本的 `ccpu386/ntstubs.c`、`ccpu386/c_getset.c`、
  `ccpu386/sources`、`cvidc/ev_glue.c`、`cvidc/vglfunc.c` 与 `cvidc/sources` 的
  SHA-256 均分别相同；它们没有补出 `ccpu486.lib` producer 或 complete `Video`
  lifecycle owner。
- Win2000 限定结果：`D:\home\repos.hobby\win2000src\private` 当前快照未找到对应
  `softpc.new`、`vglob.c`、`ev_glue.c`、`c_getset.c` 或 `evidgen.h`；这只是所声明
  快照内的 `not found`，不推断 Windows 2000 产品不存在这些内容。
- 结论：后代同源 C 文件不能补写或授权重建短表。唯一仍合格的恢复输入类别是同版本
  `ccpu486.lib`/成员清单、link map/listing，或 `host/genPg`/EDL 生成输入。

### E-VEC-004：`localfm.c` 是未选入历史 product sources 的 storage 候选

- 状态：`confirmed negative`
- 固定 NT4 输入 `base/ccpu386/localfm.c:1-9` 定义 `IHP Gdp`、`CpuVector Cpu`、
  `SasVector Sas` 和短 `VideoVector Video`。NT4.5 与 XP 的同路径文件一致。
- 但三个版本的 `base/ccpu386/sources` 都不在 `SOURCES=` 中列出 `localfm.c`；它们列出
  `ntstubs.c`，后者也定义短 `Video`。因此 `localfm.c` 不能被当作漏编译的正式 owner，
  也不能用于消除当前组合诊断中的 duplicate/missing storage。
- 结论：该文件只证明 SoftPC 历史上存在过另一组 storage arrangement；没有对应 product
  link map、archive member 或 build rule 时，将它重新纳入构建属于新的 local composition，
  不可准入。

### E-BUILD-005：本地与公开索引检索的 composition 输入结果

- 状态：`not found in searched scope`
- 2026-08-09 已逐文件检索 `ntdos64`、本地 OpenNT、OpenNT-4.5、Win2000SRC private、两个
  XP source tree、MS-DOS source tree，以及 `repos.hobby` 下可见的 archive/image 文件；未发现
  `ccpu486.lib`、同名 archive、link map、listing、`host/genPg` 目录或未解包的相关 build drop。
- 对 `base/cpu/src/evid` 的二次审计确认：它包含 `vglob.c`、`ev_glue.c`、`ev_stubs.c`，但没有
  完整 `VideoVector` storage、initializer 或 sources/make descriptor。
- 公开全文检索 `ccpu486.lib`、`ccpu486 OpenNT`、`ccpu486 SoftPC`、`host/genPg SoftPC` 未得到
  可核验的相关命中；结果均为无关索引噪声，不列为来源。
- 结论：当前已知可访问输入不能恢复 historical composition。下一轮只接受具备可追溯出处、版本
  关联和成员/符号可审计性的 build drop、library、map、listing 或 generator 输入；不以本记录
  授权新的 local composition overlay。

### E-BUILD-006：EDL 与 generator 输入在已采集 SoftPC 树中缺失

- 状态：`not found in declared source scope`
- 2026-08-09 对 NT4、只读 OpenNT 镜像、NT4.5 与 XP 的四个已定位 `softpc.new` 根目录作了
  文件名限定检索：未找到 `Evid.edl`、`EvPtrs.edl`、`Vglob.edl`、`MkCpuInt` 或
  `host/genPg` 目录；构建描述中唯一的相关命中仍是每份 `ccpu386/sources:44` 对
  `$(SOFTPC_TREE)\host\genPg\inc` 的 include 引用。
- 本机目录清点亦未发现可采集的 MinNT 源树。本条不对未挂载介质、其他机器或未公开 build
  drop 作不存在性断言。
- 结论：现有快照保存了生成后的部分 C/H 输出，却没有能够重放 vector/generator lifecycle 的
  原始输入或规则。任何将来取得的 EDL/generator 候选必须先按版本、输入 hash、产物成员和
  product profile 做单独采证，不能直接接入 NT4 runtime。

### E-BUILD-007：本地产物级搜索未补回 CCPU/C-VIDC composition

- 状态：`not found in searched local artifact scope`
- 2026-08-09 对本地 OpenNT、OpenNT-4.5、Win2000 `private` 与 XP source trees 做
  文件名级产物搜索：未找到 `ccpu486.lib`、`ccpu386.lib`、`cvidc.lib`、`Vglob.edl`、
  `EvPtrs.edl`、`Evid.edl`、相应 map 或 listing。
- 发现的唯一完整历史产品映像为
  `D:\\home\\repos.hobby\\opennt-4.5\\nt\\public\\sdk\\lib\\i386\\ntvdm.exe`
  （2,971,188 bytes）。COFF header 为 `IMAGE_FILE_MACHINE_I386`；其符号表包含
  `MS_bop_0` 与 31 个 `sas_*` 符号，也包含 video 代码，但没有
  `c_cpu_*`、`c_effective_addr`、`c_sas_touch`、`c_VirtualiseInstruction`、
  `C_Video` 或 generic `Video` CCPU/C-VIDC symbol。
- 结论：该 i386 V86 product 可继续作为 historical host/BOP/DEM oracle，不能补作
  CCPU software-profile archive、其 member list，或 C-VIDC vector composition 的
  来源。该 not-found 仅覆盖已检索的本地树和已列出的本地产物，不对未采集介质作存在性
  断言。

### E-BUILD-008：MVDM/SoftPC 邻近静态库未含缺失的软件 monitor 组合

- 状态：`not found in searched local library scope`
- 2026-08-09 对四个本地历史根目录递归枚举 4,074 个 `.lib`，再对路径或文件名含
  `mvdm`、`softpc`、`ccpu`、`cvid`、`ntvdm` 的候选作 COFF 精确符号审计。OpenNT、
  OpenNT 4.5、Win2000 private、XP 分别得到 33、33、0、129 个候选；可由 LLVM
  解析的候选中，`c_cpu_init`、`c_effective_addr`、`c_sas_touch`、
  `c_VirtualiseInstruction`、`C_Video`、generic `Video`、`MS_bop_0`、`init_vga` 和
  `ev_glue` 均无定义命中。
- 不能解析的 OpenNT/4.5 候选全部处于 DOS V86、WOW16 或旧工具链库目录；XP 的同类
  候选也处于 DOS/WOW16/工具链 CRT 目录。它们不位于 `softpc.new` 的 CCPU/C-VIDC
  product 输入边界，故不作为待恢复的 32 位 monitor archive。
- 完整命令范围、计数、路径归属和不推导项见
  `NT4-SOFTPC-LIBRARY-ARTIFACT-AUDIT.md`。
- 结论：当前本地静态库不补回 historical composition；可接受的新证据类别不变，仍为
  同版本 `ccpu486.lib`、member list/map/listing 或原始 EDL/generator 输入。

### E-BUILD-009：XP 产品链接描述证实组合边界，但没有留下组合产物

- 状态：`external confirmed; negative artifact result`
- 2026-08-09 对只读 XP 树
  `D:\\home\\repos.hobby\\winxpscodes\\Source\\XPSP1\\NT\\base\\mvdm\\softpc.new\\obj.vdm`
  作限域清点。目录只含 `sources.inc`、`ntvdm.prf`、`ntvdm.c`、resource、区域构建描述与
  其他 product metadata；没有 `ccpu486.lib`、`.obj`、link map、listing、EDL 或
  `host/genPg` 输入。
- `obj.vdm/sources.inc:65-112` 是直接的历史产品链接图：`$(386)` 为真时链接
  `v86\\monitor\\...\\monitor.lib`；否则，只有在 `CCPU` 非空时才把
  `$(SOFTPC_TREE)\\obj.vdm\\$(O)\\ccpu486.lib` 放入 `CPULIBS`。这再次证明
  `ccpu486.lib` 是独立的、非 x86 V86 branch 的 composite input，不能由现有
  `ccpu386` 源文件或 V86 `ntvdm.exe` 代替。
- 同一 XP `ccpu386/sources` 确实列出 `ntstubs.c`；其非 PIG branch 定义零初始化
  generic `struct VideoVector Video`。这只确认 storage 的一个历史 source owner，
  不能证明 81 槽 provider binding、初始化时刻或完整 composite 的生成规则。
- `ntvdm.prf` 是文本 export profile：包含例如 `MS_bop_3@0`，但没有
  `c_cpu_*`、`c_effective_addr`、`c_sas_touch`、`c_VirtualiseInstruction`、`C_Video`
  或 generic `Video` 的可用 archive member 定义。因此它可佐证产品输出边界，不能作为
  缺失 CCPU/C-VIDC 输入或符号闭包来源。
- 处置：不从该 XP profile 引入对象、代码、overlay 或 shim。R1 继续只接受可审计的
  同版本 archive/member list、map/listing 或 EDL/generator 输入；S5 继续沿已到达的
  original `DosInit` caller 采证。

### E-BUILD-010：遗留 `base/cpu/src/evid` 不是缺失 CCPU/C-VIDC 组合输入

- 状态：`confirmed; negative candidate result`
- 2026-08-09 对只读 NT4 OpenNT 树
  `D:\home\repos.hobby\opennt\base\mvdm\softpc.new\base\cpu\src\evid`
  做限域清点。该目录只有 `ev_glue.c`、`ev_stubs.c`、`vglob.c` 三个 C 文件；不存在
  `sources`、makefile、EDL、generator 输入、archive、map、listing 或任何将其接入
  `obj.vdm` 的产品链接描述。
- `cpu/src/evid/vglob.c` 与 NT4 `ccpu386/vglob.c` 的 SHA-256 同为
  `E695A9807285703119CFFB0C3BC8A3AA6433EDE43EFA499F21A6E736F94395D8`。它只是同一
  named VGA-global accessor 输出的重复副本；文件注释仍明确说明
  `get/setVideolatches` 留在 `EvPtrs.edl`，故没有补出短 `Video` ABI 或 `Cpu.Video`
  binding。
- `cpu/src/evid/ev_stubs.c` 与 C-VIDC `ev_stubs.c` 的 SHA-256 同为
  `B289023DDDB237935D44129C5968E2B225A86E95F0CA84274E132FD9B41735BD`；
  `cpu/src/evid/ev_glue.c` 则与 C-VIDC `ev_glue.c` 不同，两个 SHA-256 分别为
  `6BD49962EA383CC53B11DB3091C3C97874ADF9EA0C3A76C0D8165EF6ECE21032` 和
  `A12DDFD55DBD3B4426FF20075C46A665C6A30C6A955F24F6AB4872FB4895E935`。
  在该无构建描述目录内也未发现 `Video`/`C_Video` storage、`Cpu.Video` 赋值或
  `SetMarkPointers` macro consumer。
- 结论：它提供的是较早/并列 EVID 源码痕迹，而不是 `ccpu486.lib` 的 member list、
  profile 或初始化顺序。不得把该目录迁入 `src/opennt`、加入 CMake、作为 overlay，或
  用它替代缺失的 CCPU/C-VIDC composition。可接受的新证据类型仍限于版本匹配的 archive、
  map/listing 或 EDL/generator 输入。

### E-BUILD-011：本机扩大范围与公开精确检索均未发现缺失生成输入

- 状态：`not found in searched local and public-index scope`
- 2026-08-09 对 `D:\home` 进行了按精确文件名的只读递归检索。已完成的范围内没有
  `ccpu486.lib`、`ccpu486.map`、`ccpu486.lst`、`ccpu486.lis`、`EvPtrs.edl` 或
  `Vglob.edl` 命中；此前已采集的 OpenNT、OpenNT-4.5、Win2000、XP 与本项目 artifacts
  也仍无这些输入。
- 对公开索引使用精确查询 `"ccpu486.lib"`、`"EvPtrs.edl" SoftPC`、
  `"Vglob.edl" SoftPC`、`"c_VirtualiseInstruction"`，当前检索服务无结果。该结果
  只说明公开可检索索引没有命中，不是对私有 build drop、发行介质、未索引档案或其他机器
  的不存在性断言。
- 结论：截至本次范围，缺失的是可审计的历史组合输入，而不是尚未编译的同名 C 文件。
  后续只可通过取得版本匹配的 archive/member list、link map/listing 或 EDL/generator
  输入改变此结论；不得把搜索空结果转化为本地 guessed implementation 的授权。

### E-BUILD-012：NTVDMx64 的 CCPU/C-VIDC 补丁是后代重组，不是 NT4 composition 输入

- 状态：`reference-only; negative historical-composition result`
- 2026-08-09 审计项目内 reference-only 材料
  `refs/ntvdmx64/ntvdmpatch/patches/old-src/makefiles.patch`
  （SHA-256 `DE527382321964E19D6A6BF5960CB5EE3235DCD361EA6B181C04E5F86ED78CD3`）和
  `patches/minnt/makefiles.patch`
  （SHA-256 `FBC3247B948D8991F0D537D2327A4F84C6D5AC1855B85C2540DDE92D9682E7DA`）。两者
  都把其修改后的 `obj.vdm` CCPU branch 明确改为单独链接
  `base/ccpu386/.../ccpu386.lib` 与 `base/cvidc/.../cvidc.lib`，并把 CCPU build flags
  改为带 `PROD`/`SPC486` 的后代 profile。
- 与之对照，固定 NT4 `src/opennt/base/mvdm/softpc.new/obj.vdm/sources`
  （SHA-256 `F80C0D8513EA68500C792674D05DEE7F42F1F76AED98F4F5CA6BC409FFD61F3E`）在
  `CCPU != ""` 时只选择单一 `obj.vdm/obj/*/ccpu486.lib`。这保留了“缺失的是更高层
  composite input”的结论，而不是把两个可见 library 的独立构建误写成原始 NT4 组合规则。
- 同一 reference tree 的实验性
  `experimental/cvidc-new/cvidc-minnt.patch`
  （SHA-256 `11699D6F1D5D4773531150896F14E757BC8B951774690F850B9BF2A5DC9A28CE`）只调整
  C-VIDC test target 的路径、SDK library 与调用约定；它没有提供 `ccpu486.lib` member list、
  generic `Video` 的 81-slot storage/initializer、`Cpu.Video` binding 或 EDL/generator input。
- 同一实验目录的 `test/libtest/main.c` 是 standalone DLL test harness：它显式构造
  `CVID_IF`、在 `DLL_PROCESS_ATTACH` 中调用测试侧 `setup_global_data_ptr` / `setup_vga_globals`，
  并以 `malloc` 自行配置 video-copy/scratch memory。它虽然显示 C-VIDC API 可在测试环境调用，
  却没有展示 NT4 `obj.vdm` 的 storage owner、link composition 或初始化时序；不得把该测试
  harness 当作 `BIOS[]`/CCPU runtime 的 provider。
- 结论：这些材料可作为“后代工程为 CCPU/HAXM 选择了不同 link composition”的比较证据，不能
  迁入 `src/opennt`、不能作为 NT4 build input，也不能授权 `Video = C_Video`、短表 adapter、
  复制 initializer 或任意 local composition overlay。

候选输入的接收、来源隔离、hash/member 审计和准入前限制见
`docs/R1-CCPU-CVIDC-INPUT-ACQUISITION-CONTRACT.md`。

### E-BOP-001：NT4 与 XP 的 service-byte IP 消费顺序不同

- 状态：`external confirmed`
- 固定 NT4 输入 `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c` 的 SHA-256 是
  `378A34807645277090BF4B38556EA210502F79A898855B649B39D6C4CCC47F66`；NT4.5 同路径文件逐字节相同。
  `MS_bop_0:149-154`、`MS_bop_2:282-288`、`MS_bop_4:315-321` 都先调用 dispatcher，随后才
  `setIP(getIP()+1)`。
- 只读 XP 输入 `D:\home\repos.hobby\winxpscodes\Source\XPSP1\NT\base\mvdm\softpc.new\host\src\nt_bop.c`
  的 SHA-256 是 `FAD3681177F9236B0CBB0C3CD291005DF0659AFEED3B44066FB834C16B65FA37`；对应
  `MS_bop_0:156-159`、`MS_bop_2:300-303`、`MS_bop_4:335-337` 都使用 `VdmMapFlat` 取 byte，
  先 `setIP` 再 dispatcher。
- 结论：这个差异改变 dispatcher 可观察到的 IP。NT4 的 fixed-profile trace、ABI facade 与未来 runtime
  均须保留“dispatch 后消费 service byte”。XP 仅作为差异证据，不是 NT4 source/build input。
- 详细逐项对照见 `NT4-NT5-BOP-ORDERING-DIFF.md`。

### E-INIT-001：NT4 DOS 初始化在 CONFIG projection 前需要 BIOS 内存服务

- 状态：`confirmed`
- 固定 NT4 `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:825` 发出 `BOP 12h`，取得常规内存；
  `:1012-1017` 设置 `AH=88h` 后发出 `BOP 15h`，取得扩展内存；`:1162-1163` 才设置 `DS:DX=config`
  并执行 `CMDSVC SVC_GETCONFIGSYS`。
- NT4.5 `sysinit1.asm` 与 NT4 字节相同，SHA-256
  `0A0126881DFB6A487D8702BC90C248B57A4079AC1C13452861C5B4F4D5E4B37A`；XP 文件 SHA-256
  `E704B99EA8B9BA79AC876F54E85FD081BB277BFE02AAE8A80AB0EBD4CCE0925B`，该区域仅有注释差异。
- 结论：CONFIG 是第一个已定位的 ambient filesystem/profile seam，不是初始化的第一个 host/machine
  契约。后续工作必须分别审计 `12h/15h` 所需的历史机器输入与 CONFIG projection；不得以预写寄存器、
  空 CONFIG 或自写 BIOS handler 伪造任一方向的成功。

  对两个 BOP 的原始 BIOS owner、BDA/CMOS 输入及其 core/mantle 含义，见
  `docs/research/ccpu-bios-closure/S5-SYSINIT-MEMORY-BOP-BOUNDARY.md`。

### E-HOST-001：NT4 与 NT4.5 的已到达 host seam 文件一致，XP 仅作差异证据

- 状态：`external confirmed`
- `demgset.c`：NT4
  `D:\home\repos.hobby\opennt\base\mvdm\dos\dem\demgset.c` 与 NT4.5
  `D:\home\repos.hobby\opennt-4.5\nt\private\mvdm\dos\dem\demgset.c` 的 SHA-256
  均为 `19C5999308A479CC9C0FCB30D23E8301D9A835408417AA965B85523DFB8A1653`。
  XP 文件 SHA-256 为 `A5B71091A6BC1865FA265E963BA767B00A1714E92788BE1B9957A9DEF125BAE3`。
  NT4 的 `demGetDrives` 从 `GetDriveTypeOem` 与 `GetPhysicalDriveType` 建立
  `PhysicalDriveTypes`，因此 A:/B:、USB/虚拟软驱或 image-backed policy 的语义属于已到达的
  DEM host-media seam，而不是 BIOS 或 CPU 默认值。
- `cmdconf.c`：NT4 与 NT4.5 SHA-256 均为
  `32892C8C489BED9E4BE8FAED12E1BAFBC6C580BA81A509425E8493DE1E6FFC0A`；XP 为
  `B3C93312CF1C632C87301753C1F0E33374F74F2D2B430C732930CBF8D3121A42`。
  NT4 `cmdGetConfigSys` 先调 `ExpandConfigFiles(TRUE)`，再把临时路径转换并写入 guest
  `DS:DX`；注释明确其失败将终止 VDM。它是 COMMAND 的 ambient config/temp-file owner，
  不是能够由空文件、宿主 `%SystemRoot%` 或 CPU shim 代替的服务。
- 结论：NT4.5 可作为 NT4 的逐字节同源旁证；XP 只能说明后来实现曾演化，不能授权引入其
  长路径、区域或 host policy 行为。详细准入边界见
  `docs/research/ccpu-bios-closure/S5-HOST-SEAM-ADMISSION-SNAPSHOT.md`。

### E-ENTRY-002：CCPU 的历史初始化顺序已知，但不属于 NT4 x86 V86 product

- 状态：`confirmed; not admitted as x86 runtime owner`
- NT4 `base/support/main.c:201-246` 先调用 `init_virtual_drivers()`、`config()`、
  `cmos_pickup()`，之后才调用 `cpu_init()`；`host/src/config.c:531` 在 configuration
  决策后调用 `sas_init(vdmMemorySize*ONEKB)`，并只在 `CPU_40_STYLE` 下调用
  `InitNtCpuInfo()`。CCPU profile 的 `cpu4gen.h` 再把 `cpu_init` 映射到 `c_cpu_init`。
- 但 NT4 `obj.vdm/sources` 明确把 i386 `MONITOR` 分支与非 x86
  `CPU_40_STYLE + CCPU` 分支分开。这一顺序只能证明 software CCPU profile 的 owner，不能
  作为 NT4 x86 V86 owner 或 x64 S4 lifecycle 的构建输入。
- 结论：不得以该顺序新建 `config -> sas_init -> c_cpu_init` glue。完整论证见
  `docs/research/ccpu-bios-closure/S5-CCPU-SESSION-OWNER-RECOVERY.md`。

### E-BUILD-013：本机可行动范围内未发现可接收的 CCPU composition 输入

- 状态：`not found in searched local artifact scope`
- 2026-08-09 对以下本机根目录作只读、精确文件名递归搜索：`D:\home\repos`、
  `D:\home\repos.hobby`、`D:\home\archive`、`D:\home\downloads`、
  `C:\Users\neko\Downloads`。检索名为 `ccpu486.lib`、`ccpu386.lib`、`cvidc.lib`、
  `EvPtrs.edl`、`Vglob.edl`、`Evid.edl`、`ccpu486.map`、`ccpu486.lst`、`ccpu486.lis`；无命中。
- 该结果补充 E-BUILD-011 的已检索范围，不对未挂载介质、私有 build drop、发行介质或未索引
  档案作不存在性断言。
- 结论：当前不能合法恢复 CCPU composite/session owner；不得以这次空结果授权 local glue、
  guessed generator、`Video = C_Video` 或任何替代执行后端。接收条件仍由
  `docs/R1-CCPU-CVIDC-INPUT-ACQUISITION-CONTRACT.md` 约束。

## 版本待采集清单

NT4.5 和 XP 已完成仓库外只读固定文件采证，但仍未迁入本工作区，也不是 build input。
Win2000 的声明快照仅完成 `private/mvdm` 范围检索。任何后续版本若需进入工作区，仍
必须先新增来源元数据和同一文件的 SHA-256，不得直接复制后代实现进 `src/opennt/`。
