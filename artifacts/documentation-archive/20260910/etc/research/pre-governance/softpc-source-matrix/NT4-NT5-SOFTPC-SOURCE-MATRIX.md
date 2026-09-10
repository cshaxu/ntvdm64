# NT4-NT5 SoftPC Source Matrix

状态：进行中。本文是独立、只增的研究输入；不修改 R1、reset trace、路线图、CMake 或任何历史源码。

## 目的

以 Windows NT 4.0 的 SoftPC/NTVDM 为行为目标，比较 NT4、NT4.5、Windows 2000
及 Windows XP 的同源代码和构建描述。后代源码只作为证据来源，不自动成为 NT4 的
实现输入，也不改变 NT4 的语义目标。

本文先建立可复查的 NT4 基线。当前工作区只正式包含 `src/opennt/`；NT4.5、
Win2000 与 XP 均只可作为仓库外、只读的采证输入，不能自动成为 NT4 的构建输入。

## 结论规则

每一项仅可处于下列状态之一：

- `confirmed`：有当前版本的文件、行号和 SHA-256 证据。
- `different`：至少两个已采集版本存在可定位的差异。
- `not found`：在已声明且已完成检索的范围内没有找到证据。
- `not collected`：版本树未进入本研究工作区，或尚未完成该项检索。
- `external confirmed`：仓库外只读快照已定位、已对固定文件作 hash/行号采证；
  它不是本工作区的 source/build input。
- `unresolved`：已有相互不足或冲突的证据；禁止据此改 runtime。

## 版本可用性

| 版本 | 研究角色 | 当前状态 | 可用根目录 |
|---|---|---|---|
| NT4 OpenNT | 目标语义与首要证据 | confirmed | `src/opennt/` |
| NT4.5 | 构建/生成输入旁证 | external confirmed | `D:\home\repos.hobby\opennt-4.5\nt\private\mvdm\softpc.new` |
| Windows 2000 | host 与构建演变旁证 | not found in declared snapshot | `D:\home\repos.hobby\win2000src\private\mvdm` 仅含 inc/WOW 层 |
| Windows XP | 最晚的同源比较样本 | external confirmed | `D:\home\repos.hobby\winxpscodes\Source\XPSP1\NT\base\mvdm\softpc.new` |

## 合同矩阵

| ID | 语义或构建合同 | NT4 基线 | NT4.5 | Win2000 | XP | 当前状态 | NT4 可采取动作 |
|---|---|---|---|---|---|---|---|
| BUILD-001 | CPU backend 选择 | `obj.vdm/sources` 在 `CCPU` 非空时选择 `ccpu486.lib`，否则选 `a4$(PD).lib` | 同一文件/规则 | 无 SoftPC build tree | XP `sources.inc` 同一规则 | confirmed (NT4/4.5/XP) | 保持两路径互斥；不得把 V86/a4 对象混入 CCPU trace |
| BUILD-002 | 产品层 CCPU profile | 非 x86 分支为 `CPU_40_STYLE + CCPU + C_VID`；x86 分支为 `MONITOR + C_VID + X86GFX` | 同一文件 | 无 SoftPC build tree | 同一非 x86 分支；x86 另加 `ARCX86` 产品差异 | confirmed (NT4/4.5/XP) | CCPU 研究目标须单独标示为软件解释 profile，不能伪称 NT4 x86 V86 profile |
| BUILD-003 | CCPU/C-VID 内部宏 | `ccpu386/ccpudefs.inc` 含 CPU_30/40、CCPU、SPC386；`cvidc/ccpudefs.inc` 另含 C_VID | 同一文件 | 无 SoftPC build tree | 同一文件 | confirmed (NT4/4.5/XP) | 用原始 source-level profile 作编译实验基线 |
| BUILD-005 | CCPU 生成/产物输入 | `ccpu386/sources` 引 `host/genPg/inc`；`obj.vdm/sources` 引 `ccpu486.lib`，但两者不在 tree | 同一缺失 | 无 SoftPC build tree | 同一缺失 | confirmed missing input (NT4/4.5/XP) | 不得把缺失产物等同为任一现存 C source 组；优先寻找 archive/member 或原始 generator |
| BUILD-006 | 遗留 `base/cpu/src/evid` 目录 | 仅有 `ev_glue.c`、`ev_stubs.c`、`vglob.c`，无 `sources`、makefile、EDL、archive 或 link metadata | not collected | not collected | not collected | confirmed, not an admitted product input | `vglob.c` 与 `ccpu386/vglob.c` 字节相同；其余两文件属于无构建归属的遗留 EVID 变体，不得迁入 NT4 CCPU/C-VIDC 组合。 |
| BUILD-004 | PIG 变体 | `obj.vdm/cdefine.inc` 仅当构建变量 `PIG` 非空时传递 `-DPIG` | not collected | not collected | not collected | confirmed (NT4 only) | 不得根据名称猜测 PIG；需以完整 sources/link 图确认 |
| VEC-001 | C-VID 0--77 provider vector | 生成的 `cvidc/vglfunc.c` 定义 `C_Video`，但只显式初始化 0--77；81-slot 结构的 78--80 保持零值 | 同一文件 | 无 SoftPC source | 同一文件 | confirmed (NT4/4.5/XP) | 仅作为部分 generated provider 证据；不得当作完整 `Video` initializer |
| VEC-002 | 通用 `Video` vector owner | `ccpu386/ntstubs.c` 非 PIG 分支定义零初始化 `Video`；`localfm.c` 也定义同名 storage | 同一文件 | 无 SoftPC source | 同一文件 | confirmed (NT4/4.5/XP) | 禁止本地 `Video = C_Video` 替代；先恢复历史链接/初始化合同 |
| VEC-003 | `setMarkPointers` 调用合同 | 产品 `CPU_40_STYLE + C_VID` 下，`egacpu.h` 将 `SetMarkPointers` 改写为直接的 `ev_glue.c::setMarkPointers`；该函数内部读取完整 `Video[58..60]` | 同一文件 | 无 SoftPC source | 同一文件 | product edge confirmed; internal table owner unresolved | 后代源码未给出完整表 binding/initializer；需 library member、map 或 generator 输入 |
| SAS-001 | CCPU SAS vector | `sascdef.c`、`ccpusas4.c`、`cpu4gen.h` 与 `c_main.c` 固定基线 | 四文件逐字节相同 | 无 SoftPC build tree | 四文件逐字节相同 | confirmed (NT4/4.5/XP) | 后代没有补出 opaque slot 语义；`c_sas_touch`/`c_VirtualiseInstruction` 继续 fail-closed |
| ENTRY-001 | startup/reset 顺序 | `support/main.c`、`bios/reset.c`、`host/src/nt_msscs.c`、video init 为 NT4 入口基线 | 四个 owner 文件逐字节相同 | 无 SoftPC build tree | 四文件均不同；出现 XP 专有 video/reset 分支 | different (NT4=4.5; XP differs) | NT4/4.5 可互证；XP 只能作为差异证据，不能回灌 NT4 reset 语义 |
| HOST-001 | host dependency 面 | timer、console、disk、process 等待由实际调用抵达后登记 | not collected | not collected | not collected | not collected | 不提前创建现代 shim |
| HOST-002 | `demGetDrives` 的 host-drive 分类 | `demgset.c` 经 `GetDriveTypeOem` 和 `GetPhysicalDriveType` 建立 `PhysicalDriveTypes`，再由 DEM 使用 | 文件逐字节相同 | 无 SoftPC build tree | 实现有差异，但仍保留该 DEM/host-drive ownership | different (NT4=NT4.5; XP differs) | NT4 的 A:/B: 与 fixed/removable 分类仍是原 DEM 的 host-media policy；不得把 XP 代码或固定 inactive 结果倒灌为 NT4 语义。 |
| HOST-003 | `cmdGetConfigSys` 的临时 CONFIG projection | `cmdconf.c` 调 `ExpandConfigFiles(TRUE)`，将输出路径写回 guest `DS:DX`，失败终止 VDM | 文件逐字节相同 | 无 SoftPC build tree | 实现有差异，接口与临时文件职责仍在 COMMAND owner | different (NT4=NT4.5; XP differs) | 这是 ambient product integration，须先定义 session-local projection；不得把历史 `%SystemRoot%`、用户 locale 或临时路径直接带入 NT4 fixed profile。 |
| BOP-001 | 带 service byte 的 IP 消费次序 | `MS_bop_0/2/4` 都是 dispatcher 返回后才 `setIP(+1)` | 同 NT4，文件逐字节相同 | 无 SoftPC build tree | 三者均改为 `setIP(+1)` 后才 dispatcher | different (NT4=NT4.5; XP differs) | NT4 固定 profile 必须保留 post-dispatch consumption；不得倒灌 XP 次序或 `VdmMapFlat`。见 `NT4-NT5-BOP-ORDERING-DIFF.md`。 |
| INIT-001 | `sysinit1` 首批 host/BIOS 请求 | `BOP 12h`、`BOP 15h/AH=88h` 在 `CMDSVC SVC_GETCONFIGSYS` 之前 | 与 NT4 相同 | not collected | `sysinit1` 有仅注释差异，BOP site 保持 | confirmed (NT4/4.5/XP) | CONFIG projection 不是首个外部契约；先记录机器内存服务，不在 fixture 中合成返回。见 `NT4-NT5-BOP-ORDERING-DIFF.md`。 |
| PRODUCT-001 | NT4 V86 product owner 旁证 | 固定 i386 image 同时含 `MS_bop_0`、`DemDispatch`、`memory_size`、`cassette_io` | not collected | not collected | not collected | confirmed (NT4.5 SDK sample) | 仅作 V86 host/BIOS/DEM owner oracle；不能作为 CCPU source/composition 或 x64 backend。见 `NT4-V86-PRODUCT-ORACLE-AUDIT.md`。 |

## 采证顺序

1. 对每个新增版本先记录树根、revision/来源、文件清单和 hash。
2. 比较 `sources`、`sources.inc`、`cdefine.inc`、各 `ccpudefs.inc`，重建构建图。
3. 比较 CCPU、C-VID、SAS 的 exported symbols、结构定义和初始化者。
4. 比较入口/reset 顺序，最后才比较 host 层。
5. 每项发现写入证据台账；未进入台账的推测不得影响 overlay、shim 或 runtime。

## 主线程交接规则

本目录完成一个可复核结论后，只向主线程提供本文和台账的链接及 ID。主线程决定是否
引用到 R1、路线图或治理台账；本矩阵作者不同时修改那些活跃文件。
