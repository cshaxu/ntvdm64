# C 层向外依赖跟踪

生产基线为 `81f4cef3d`，固定 OpenNT 对照为 `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`。范围是两个 C 组件对 D、E、F 的依赖。采用 P7 最终生产构建的证据。其采集元数据中的 `449c717e8` 是当时尚未提交 P7 的 HEAD。

[逐点清单](points.jsonl) 每行一个稳定编号。一个编号按证据类别、外层提供组件和目标路径区分，符号项再区分符号。两套镜像、多个调用点和产品架构可以共用编号，各自的证据全部保留。普通 x86 C 符号去掉调用约定装饰后与 x64 合并，原始符号拼写仍在证据中。证据数组每行依次为证据种类、引用方组件、引用文件、行号或 COFF 节号、产品、原始符号或证据说明。节号不能当源码行号使用。

| 外层提供方 | 链接符号 | 头文件 | 搜索目录 | 产品导入 |
|---|---:|---:|---:|---:|
| D common | 10 | 17 | 1 | 0 |
| E frontend-base | 0 | 1 | 0 | 0 |
| E worker-base | 1 | 2 | 0 | 0 |
| F nthook-dll | 3 | 0 | 0 | 0 |
| F ntsrv-exe | 7 | 3 | 1 | 0 |
| F ntvdm-exe | 149 | 35 | 8 | 1 |
| F vdmredir-dll | 未取全 | 2 | 1 | 0 |
| F wow32-dll | 278 | 72 | 6 | 0 |
| 已观察合计 | 448 | 132 | 17 | 1 |

5,576 条原始证据归并为 599 项。598 项开放，另有 C0001 包含两处 WOW32 诊断字符串，已核实并排除。132 个头文件中，51 个仅见于编译器传递包含闭包。应先找到上游直接包含点。产品导入 C0134 是 VCDEX.DLL 对 ntvdm.exe 的二进制契约，独立于源码迁移。

## 如何逐项决定归属

1. 查固定 OpenNT 的原始调用方、声明及提供方，再对照现有镜像的非排除 diff。记录产品中谁拥有状态、谁控制生命周期、哪些上下文实际使用它。相同原因的多个编号可共用一次处理。
2. 若原始镜像调用方已被加上产品策略，优先让真正拥有该策略的 D、E 或 F 接回它，恢复 C 的原始调用；保留原始执行时序和失败语义。若原版本来就要求宿主提供服务，优先复用已有宿主接口，将具体实现留在外层。
3. 若外层重复定义了原版归 C 所有的数据或算法，核对原路径是否已经接入、能恢复哪些现有差异。只有职责成立且量过镜像代价才选择移回 C。逐项记录相对固定原版的非排除差异块数和行数，净增须在实施前依项目规则报批。避免为几个符号引入整份未接入镜像。
4. 纯构建依赖先试去掉多余搜索目录或强制头；传递包含优先处理上游。没有产品行为变化的项无需为它新造 ABI。
5. 编号保留，新增项追加。处置时在对应记录补入原始位置、迁移方向或保留理由、关联编号、镜像 diff 前后值、验证结果和交付提交。只有最终生产构建的包含记录、链接结果或二进制证据证实消除后才计为关闭。单个根因消除的关联项分别复核；批准保留和已消除分开计数。

## 起步点和证据缺口

- C0162–C0164 对应三个 BASE 文件后缀。固定原版的 `base/win32/client/basedll.h` 声明后缀描述符，`baseinit.c` 初始化，`vdm.c` 使用。当前 F 的 hook_classification.c 提供定义。这两个原版所有者文件均未位于当前 C 的对应目录。优先核对已有 BASE 声明及各产品初始化，比较恢复原版所有者与保留宿主绑定的实际代价。
- C0321 是 nt_event.c 对 worker-base 的退出请求。E 拥有 worker 连接。对照原版 console 退出顺序、通知及返回码，评估将产品协调留在 E、恢复 C 入口的办法。
- C0042 汇合了 28 条强制包含记录。目标头只将 terminate 映射为 mvdm_softpc_terminate。应按一次编译命名冲突处理，并对照符号定义与所有调用，避免改动 28 个镜像文件。
- VDMREDIR 缺链接 map。x86 largeint、movemem、callpr32 缺有效包含记录，三个 WOW 生成对象尚未归属。补证后只追加新项或更新既有项的证据。当前计数是已确认覆盖范围。


## S12 直接依赖口径

基线 598 个开放编号中，51 个只记录传递闭包，改列旁证，保留其原始记录。余下 547 个编号包括 448 个链接符号、20 个显式源码包含、7 个强制包含目标、54 个须核实直接包含者的短名头、17 个构建搜索目录、1 个产品导入。搜索目录逐个检查是否可移除；目录存在不能证明代码使用。两个相邻层之间的直接引用记在发起引用的源文件，不沿调用链重复归责。

S12 处理原地可消除项。S13 处理经固定镜像对照证明属于 F 的产品职责。S14 按案例检查可能从 F 恢复到 C 的原版职责。每种迁移均需记录镜像非排除差异点位和行数的前后值。D/E 服务依其真实所有者单列，不为凑迁移方向改归 F。

## P2 更新

重新读取 Ninja 展开的实际命令，发现 C0056 原先仅记录为传递旁证，实际是 46 个 SoftPC 宿主编译单元的强制头。该头内只有三个 CRT 符号改名，现由对应宿主编译组选用相同命名。C0138、C0140、C0144 三个产品私有搜索目录已从 C 单元移除。C0137、C0141 在 WOW32 图中的搜索目录也已移除，它们仍在 worker 图中使用，编号继续开放。

用相同产品对象和链接参数另输出 VDMREDIR 链接 map，取得 49 个对象归属、43 个有存活符号的对象、277 个匹配的公开符号。补入 13 条原缺失的引用证据，其中已有的线程服务复用旧号，七个新符号追加为 C0600–C0606。编号增加来自补证。当前 606 个编号，548 开放、50 仅传递旁证、7 已消除、1 已排除诊断。P1、P2 对镜像均为 0 块、0 行变化。


## P3 入口归属及逐项包含证据

NTVDM 进程入口、异常跳回及 worker 清理由 F 层持有。C 层保留原版 SoftPC 入口。一个所有权调整消除了 C0046 和 C0251–C0254，同时消除了五个传递包含目标。镜像非排除差异从 4 块、13 行降至 2 块、3 行。[入口核验记录](s12-p3-entry.json)

原列为传递的 51 项中，C0056 已按强制包含处理，C0068 经产品预处理证实由 wkfileio.c 直接包含。剩余 49 项中 5 项随入口迁移消失，44 项在 points.jsonl 保留实际包含路径及其首个直接边编号。539 个编译上下文涵盖本次 x86/x64 产品图内的 C 层 C/C++ 单元。不同包含路径可抵达同一目标；给出的见证路径用于定位直接调用方。

当前 606 个编号中，544 个直接或构建项开放，44 个传递证据仍存在，12 个直接或构建项已消除，5 个传递项已消除，1 个为诊断字符串。


## P4 x64 搜索目录

C0137、C0141、C0142、C0145、C0146 在全部 11 条 x64 镜像编译命令中消失，五项在 x86 仍开放。C0136 的 x64 命令从 11 条降到 6 条，分别属于 launcher 和 service。既核对全局选项，也核对从参考图逐对象复制的选项。按路径和编译上下文计算共消除 60 次目录传递。镜像变化为 0 块、0 行，稳定编号的开放数量不变。[命令计数与定向验证](s12-p4-native-roots.json)


## P5 x86 worker 搜索目录

C0137、C0145、C0146 在 worker 图原来各有 391 条镜像编译命令带入。清理后 C0137 仅 demgset.c，C0145 仅 vrnetapi.c，C0146 为零。WOW32 自身的 136 条 C0146 记录仍开放。共减少 1171 次目录传递，三个编号继续保留，镜像 0 块、0 行变化。[P5 构建及夹具记录](s12-p5-worker-roots.json)

## P6 链接器共同存储

对 11 个产品链接图的 `<common>` 存储补查了镜像调用者的 COFF 重定位、实际存活的链接节、候选对象的暂定定义及源文件。507 个「镜像产品、符号」组合中，486 个只查到内层存储候选，11 个可归属到唯一外层候选，10 个有内外层共同声明。后者逐项列在报告内，不能仅用共同声明判定外层拥有状态。[P6 存储定位及覆盖边界](s12-p6-common-storage.json)

新增 C0607–C0617。C0162–C0164 的两种 NTHOOK 绑定与 Run16 的 C0609–C0611 分别保留。现有 617 个编号，555 开放、44 条传递证据仍存在、12 个直接或构建项及 5 个传递项已消除、1 个诊断项。C0152–C0164 已记入原地处理判断，后续仍需按实际所有者决定迁移。P6 仅更新审计记录，镜像变化 0 块、0 行。


## P7 共同声明的状态归属

十组已逐项回查。七个命名管道槽位由 C 的 LoadVdmRedir 赋值，实际导出由 C 的 vrnmpipe.obj 提供；跨 DLL 的装载契约单独追踪。滚轮余量由原版 USER 控件代码写入。其余两项在 C 的 basesrv.h 定义，固定原版 srvinit.c 初始化服务堆及标签。当前服务镜像中，对这两个地址的直接 RIP 相对引用分别为 7、17 处读取，未发现直接写入。现有服务生命周期夹具自行设置 BaseSrvHeap，不能证明产品启动完成了初始化。S14 单列复核生产启动及实际分配路径。十组均未因共同声明增加外层存储编号。[逐项依据](s12-p6-common-storage.json)


## P8 固定原版调用方对照

462 个开放链接符号覆盖 170 个调用文件。929 个「调用文件、提供者符号」组合中，110 个在两版调用文件均出现同名标识符，149 个仅在当前调用文件出现，670 个两版均未直接写出该名字。清单按编号保存两版物理行号；这些是定位证据，宏展开、声明和实际执行语义仍须分别确认。[原版 blob 与核查口径](s12-p8-caller-anchors.json)

C0165–C0171 已登记针对服务端绑定的处理判断。C0456 的 38 个调用文件需要区分 WOW 宏、DLL 的 stdcall 实现与宿主原版 cdecl 导出。P5 两个二进制的返回指令及视频区行为不同，S14 须核对取指针和刷新配对。未改变任何绑定。镜像变化 0 块、0 行。

## P9 产品直接依赖文件清单

[逐文件明细](s12-p9-file-dependencies.json)按实际包含者或 COFF 调用者归责，每个文件列出直接包含、强制包含和链接符号编号。539 个编译上下文对应 519 个不同源文件，其中 251 个读到了外层头。110 个文件直接包含外层头（含 6 个头文件），9 个源文件被强制包含，3 个文件同时出现，去重共 116 个文件、158 个文件到目标头组合。链接绑定来自 170 个文件，编译与链接重叠 96 个，共涉及 190 个文件。仅搜索目录、间接包含和产品导入单独跟踪。

[强制头对照](s12-p9-forced-headers.json)中 11 个对象分别在相同输出路径编译两次，只剔除 COFF 时间戳。仅 srvvdm.obj 等价。移除该对象多余的强制头后，basesrv.h 仍直接包含它，C0022 保持开放。两个 x64 服务夹具通过，11 个发布映像与 P5 的区别限于包 GUID 和 PE 时间戳。镜像差异变化 0 块、0 行。

## P12 registry ownership

[Evidence](s12-p12-registry-candidate.json): C0017, C0036 and C0205–C0211
now resolve to B compat. WOW's product export bridge remains separately open.
Native full-value enumeration uses original class 1 and Name offset 20;
the rejected local class-2 candidate failed the new test and was never published.
The tracker has 617 IDs: 546 open, 44 supporting, 21 eliminated,
five eliminated transitive and one diagnostic. Mirror delta is zero points/lines.
File 002 retains C0024 for S13; it is not fully cleared.

## P13 directory callers

[Evidence](s12-p13-directory-callers.json): cmdkeyb.c and dem.c now directly
include B system_root.h. C0039 remains open for other consumers. Five files
reviewed, three cleared, two lifecycle moves for S13, 111 unreviewed.
Both cached architectures and normalized eleven-image identity passed; both
release destinations are hash-verified. No repeated runtime tests.

## P14 native I/O ownership

[Evidence](s12-p14-native-io.json) completes file 008. C0027 and C0307-C0316
now bind to B. 617 stable IDs remain: 535 open, 32 eliminated, 44 supporting,
five eliminated transitive, one diagnostic. Sequential review: 8/116, four
cleared direct header dependencies, four lifecycle moves reserved for S13.
Both cached architectures, native ABI tests and original DEM 115 assertions
passed. Eleven images are published and hash-verified. Mirror delta is zero.

## Files 009-011 ownership

[Ownership evidence](s12-file009-011-ownership.json) separates the two WOW
page-domain lifecycle callers from the native failure helper and VM bridge.
Files 009/010 are classified for S13; 011 is a local candidate pending delivery.

## P15 native failure ownership

[Delivery evidence](s12-p15-native-failure.json): C0241 binds to B; C0043
remains open for sim32.c. Eleven files reviewed, four cleared, seven lifecycle
files reserved for S13. 534 of 617 stable IDs remain open. Mirror delta zero.

## P16 CCPU wake carrier

[Delivery evidence](s12-p16-ccpu-wait.json) records the unchanged Windows event
carrier moved to B. C0037, C0212 and C0213 are eliminated; 531/617 remain open.
Sequential review is 12/116, five cleared and seven lifecycle files for S13.

## P17 mapping and generated ABI classification

[Source evidence](s12-p17-mapping-generated-ownership.json) classifies files
013-016 without changing product code. Review is 16/116: five cleared, eight
lifecycle files for S13 and three source/ABI files for S14; 100 unreviewed.
531 of 617 stable IDs remain open. The P16 release remains current.

## P18 keyboard metadata

[Delivery evidence](s12-p18-keyboard-history.json) records the B migration and
three targeted suites. Seven stable IDs are eliminated; 524/617 remain open.
[Files 017-018](s12-file017-018-ownership.json) remain S14 composition/ABI work.
Review is 19/116: six direct-header clears, eight lifecycle files, five source/ABI
files, 97 unreviewed. C0204 remains a direct link dependency.

## P19 mouse, BOP and EMS ownership

[Source evidence](s12-p19-mouse-bop-ems-ownership.json) classifies files020-023.
Config registry was already cleared by P12. Three lifecycle callers remain open.
Review is 23/116: seven direct-header clears, eleven lifecycle files and five
source/ABI/composition files; 93 unreviewed, 524/617 open IDs. P18 release unchanged.

P20: [event-thread ownership and native join delivery](s12-p20-event-thread.json).

P21: [display, mapping and callback header ownership](s12-p21-display-mapping-ownership.json).

P22: [NTIO directory and host exit ownership](s12-p22-system-loader.json).

P23: [redirector memory and WOW registry token ownership](s12-p23-redir-wow-binding.json).

P24: [WOW precompiled-header ownership and native ordinal](s12-p24-wow-precomp.json).

P25: [GDI alias compatibility ownership and publication](s12-p25-gdi-alias.json).

P26: [WOW callback/message unwind and native-dialog ownership](s12-p26-wow-unwind-ownership.json).

P27: [WOW registry callers and VDD declaration boundary](s12-p27-wow-vdd-abi.json).

P28: [WOW execution ownership and native process simplification](s12-p28-native-process.json).

P29: [Message progress constants and lifecycle ownership](s12-p29-message-progress.json).

P30: [Dispatch, fax registry, hooks and guest registry ownership](s12-p30-wow-registry-dispatch.json).

P31: [Clipboard identity, message cleanup and guest page publication](s12-p31-wow-clipboard-page-domain.json).

P32: [XMS/CSR ownership and original probe environment](s12-p32-user-probe.json).

P33: [BaseClient dependencies and native console handle compatibility](s12-p33-console-handle.json).

P34: [BaseSrv/OLE ownership and Browser compatibility](s12-p34-remote-browser.json).

P35: [Button ownership and native bitmap/icon leaves](s12-p35-bitmap-native.json).

P36: [Menu/default-window/resource ownership and actual compile contexts](s12-p36-menu-resource-ownership.json).

P37: [Combo entry, directory validation and child lifecycle ownership](s12-p37-combo-ownership.json).

[P38 dialog native leaves](s12-p38-dialog-native-leaves.json): file074, four SDK-only symbols moved to B, no mirror changes; remaining lifecycle/ABI ownership stays open.

[P39 dialog end and manager](s12-p39-dialog-end-manager.json): files075-076, original end/default/modal policy stays C; explicit task/borrow/message boundaries remain open.

[P40 dialog navigation](s12-p40-dialog-navigation.json): files077-079; keyboard/mnemonic lifecycle and original tree-layout composition distinguished.

[P41 drawing ownership](s12-p41-drawing-ownership.json): file080 classified; file081 native export/context investigation remains open. Review80/116,510/617 open.

[P42 native controls](s12-p42-native-controls.json): file081, unified native export carrier in B; five direct symbols cleared. Review81/116,505/617 open.

[P43 EDIT ownership](s12-p43-edit-ownership.json): files082-083, current-task/callback and native-return boundaries distinguished from original algorithms and storage. Review83/116,505/617 open.

[P44 control brushes](s12-p44-control-brushes.json): files084-085; unchanged native resource carrier moves B, two storage dependencies cleared. Review85/116,503/617 open.

[P45 EDIT creation](s12-p45-edit-create-ownership.json): file086; original construction stays C, extracted state function and mixed header remain open. Review86/116,503/617 open; no production changes.

[P46 rare EDIT operations](s12-p46-edit-rare-ownership.json): files087-088; callback/native-return and mixed-header dependencies distinguished. Review88/116,503/617 open; production unchanged.

[P47 DDE declarations](s12-p47-dde-header.json): file089 cleared by unchanged header move to B; both builds, dual publication and DDE fixture passed. Review89/116,502/617 open.

[P48 ListBox entry](s12-p48-list-entry-ownership.json): file090; original dispatch/storage/encoding remain C, shared lifecycle and ABI dependencies stay open. Review90/116,502/617 open.

[P49 ListBox items](s12-p49-list-items-ownership.json): file091; original algorithms and native memory/encoding remain their current inner owners; five lifecycle dependencies stay open. Review91/116,502/617 open.

[P50 ListBox interaction](s12-p50-list-interaction-ownership.json): file092; original interaction remains C, timer/drawing/native-return obligations and reset leaf separated. Review92/116,502/617 open.

[P51 ListBox directories](s12-p51-list-directory-ownership.json): files093-094; native filesystem/original geometry retained; five shared invocation symbols and mixed header remain. Review94/116,502/617 open.

[P52 ListBox lifecycle and heights](s12-p52-list-lifecycle-ownership.json): files095-096; original algorithms/storage retained, shared invocation and mixed header stay open. Review96/116,502/617 open.

[P53 DrawState contexts](s12-p53-drawing-ownership.json): file097; both production compositions verified, five F symbols remain; native leaves already B. Review97/116,502/617 open.

[P54 Client stubs](s12-p54-client-stubs-ownership.json): file098; five contexts, seven F symbols and three mixed headers remain. Worker export NtCurrentPeb verified B source ownership. Review98/116,502/617 open.

[P55 Capture/lookaside](s12-p55-capture-lookaside-ownership.json): file099; no F business calls, mixed original declaration header remains. Review99/116,502/617 open.

[P56 Class lookup ABI](s12-p56-class-lookup-abi.json): files100-101; shared SDK-only header moves F to B, original algorithms unchanged. Review101/116,501/617 open.

[P57 Cleanup/event ownership](s12-p57-cleanup-event.json): files102-103; native event reference moved to B, lifecycle responsibilities remain explicit. Review103/116,500/617 open.

[P58 Private-word setter](s12-p58-private-word-ownership.json): file104; original storage rules retained, three F edges classified. Review104/116,500/617 open; production unchanged.

[P59 Native profile boundary](s12-p59-profile-native.json): file105; native read-only opener moves to B, original query policy unchanged. Review105/116,499/617 open.

[P60 Task initialization](s12-p60-task-init-ownership.json): file106; original C ordering retained, F lifecycle callbacks classified, missing direct TLS link C0618 added. Review106/116;500/618 open. No product change.

[P61 Message-table ownership](s12-p61-message-table-ownership.json): file107; C algorithms, F storage/publication and original global owner traced. Review107/116;500/618 open. No product change.

[P62 Scheduler ownership](s12-p62-scheduler-ownership.json): file108; original scheduler policy and host wait/context boundary traced. Review108/116;500/618 open. No product changes.

[P63 RTL drawing](s12-p63-rtl-drawing-ownership.json): file109; native providers already B, captured color-depth context remains F. Review109/116;500/618 open. No product changes.

[P64 RTL text](s12-p64-rtl-text-ownership.json): file110; compiled context dependency distinguished from live prefix/underline/font helpers. Review110/116;500/618 open. No product changes.

[P65 RTL private reader](s12-p65-rtl-private-reader.json): file111; original index policy retained, B slot provider distinguished from F allocation and extent dependencies. Review111/116;500/618 open. No product changes.

[P66 Help inheritance](s12-p66-help-ownership.json): file112; retained-generation scalar distinguished from fresh native query. Review112/116;500/618 open. No product changes.

[P67 Text measurement boundary](s12-p67-text-measure.json): file113 cleared; SDK-only declarations shared in B, mirror unchanged. Cached builds, focused tests, eight A/B graphs and dual publication passed. Review113/116;500/618 open.

[P68 Window geometry](s12-p68-window-geometry-ownership.json): file114; both compile contexts traced, original algorithms retained. Review114/116;500/618 open. No product changes.

[P69 Final sequential files](s12-p69-final-file-ownership.json): files115-116; original declaration composition and service admission scope classified. Review116/116;500/618 open. Reconciliation remains before S12 closure; no product changes.

[P70 Inventory reconciliation](s12-p70-inventory-reconciliation.json): all618 stable IDs cross-indexed with the original190-file inventory;116 direct compile and74 link-only files. No status promotion. The latter136 symbol IDs include87 exclusive IDs (82 open,5 eliminated). Source-only;22 release hashes verified.

[P71 Console title and native I/O](s12-p71-console-title-native-io.json): link-only files1-2/74. C0282 requires session Console ownership treatment in S13; demerror four native I/O edges already eliminated in P14. Both actual product maps checked; no new elimination or mirror delta.

[P72 DEM buffer ownership](s12-p72-dem-buffer-ownership.json): link-only file3/74. Separates proven page-aware-copy requirement from unproven necessity of the whole lease design. C0194/C0196/C0197 remain open for S13. Both product COFF/maps and22 publication hashes checked; no production change.

[P73 BOP FE binding](s12-p73-bop-exit-binding.json): link-only file4/74. BIOS directly binds original CCPU exit; F wrapper removed; C0218 eliminated without mirror changes. Cached complete build, dual publication, CPU/BIOS and real DOS return checks passed. Current package26a87d88-8d03-4947-9982-69574aafd87e;499/618 IDs open.

[P74 BIOS address ownership](s12-p74-bios-address-ownership.json): link-only files5-8/74, all C0215. Original consumers stay C; CPU30 body is not a direct CPU40 replacement. S14 evaluates common address owner. Eight current objects and22 published hashes checked; no production changes.

[P75 Disk and descriptor ownership](s12-p75-disk-descriptor-ownership.json): link-only files9-11/74. Original INT15/disk/DMA consumers stay C; C0214 cannot alias CPU_DESCR reader because output ABI differs. C0214/C0215 need S14 CPU owner treatment. Six actual objects and22 publication hashes checked; no production change.

[P76 Initialization and termination ownership](s12-p76-init-termination-ownership.json): link-only files12-15/74. C0215 CPU address and C0608 original storage require S14; C0318 product admission precedes original destruction and belongs to S13 entry treatment. Current objects/common definitions and22 published hashes verified; no production change.

P77: [System address ownership](s12-p77-system-address-ownership.json); link-only files16-17 stay C; shared provider goes to S14 review. No production change.

P78: [Video address ownership](s12-p78-video-address-ownership.json); files18-20 remain C; C0216 joins C0215 CPU provider review in S14. No product change.

P79: [Presentation/thread/start ownership](s12-p79-presentation-thread-start-ownership.json); files21-23 assigned to grouped S13 responsibility review, no product change.

P80: [Console registration ownership](s12-p80-console-registration-ownership.json); files24-25 classified; C0189 remains the next in-place candidate. No product change.

P81: [Activity compatibility delivery](s12-p81-activity-owner.json); C0189 eliminated F-to-B, unchanged callback; final cached builds, normalized identity and eight A/B graphs passed.

P82: [Error/disk bindings](s12-p82-error-disk-bindings.json); files27-28 reviewed; error thread close is native-only, disk bindings already B since P14.

P83: [Console transition ownership](s12-p83-console-transition-ownership.json); file29 ten bindings traced through NTCON; original VGA/cursor calculations stay C.

P84: [Host/printer/serial ownership](s12-p84-host-printer-serial-ownership.json); files30-32 classified, including explicit unsupported printer-control boundary.

P85: [PIF directory reconciliation](s12-p85-pif-directory-reconciliation.json); two stale P11 statuses corrected; file33 remains in progress for C0221. No product change.

P86: [PIF path ownership delivery](s12-p86-pif-path-owner.json); C0221 moved unchanged to B, file33 complete; cached builds, dual publication, path/DOS tests and A/B gates passed.

P87: [ROM resource ownership delivery](s12-p87-rom-resource-owner.json); C0222 moved unchanged to B, file34 complete; six published ROM resources match immutable bytes, builds and focused gates passed.

P88: [Thread callback ABI](s12-p88-thread-callback-abi.json); files35-37 reviewed; corrected nt_thred actual callback selection without mirror edits; lifecycle ownership remains S13.

P89: [Timer/idle/VGA ownership](s12-p89-timer-idle-vga-ownership.json); files38-40 reviewed; timer native handle binding joins S13 cleanup, idle callback already B, painter/presentation ownership separated.

P90: [RAP provider ownership](s12-p90-rap-owner.json); files41-43 reviewed; three SDK-only provider symbols moved unchanged F to B, all cached builds, focused redirector tests and dual publication passed. DLC lifetime remains S13.

P91: [WOW alias and flush ownership](s12-p91-wow-alias-flush-ownership.json); files44-47 reviewed using current COFF/maps; C0456 original-provider/ABI pairing reserved for S14, product alias/callback scopes for S13. No code migration or new elimination.

P92: [Font provider ownership delivery](s12-p92-font-owner.json); files48-53 reviewed; six public-GDI/font-resource symbols moved F to B with SDK metrics layout, actual-object/native/negative checks and dual publication.

P93: [GDI cache and thunk ownership](s12-p93-gdi-cache-ownership.json); files54-60 reviewed; C0435 moved unchanged to B with identical 8-byte function, cached builds and dual publication. Native-call/flush contracts remain grouped for S13/S14.

P94: [Public style binding](s12-p94-wstruc-public-style.json); files61-65 reviewed, one wstruc caller edge removed without mirror changes; actual product symbol comparison, focused lifecycle checks and dual publication.

P95: [Native hook ownership](s12-p95-hook-native-ownership.json); files66-69 reviewed, C0402 SDK-only hook binding moved F to B unchanged;24-case boundary test, cached builds and dual publication.

P96: [Menu and timer ownership](s12-p96-menu-timer-ownership.json); files70-72 reviewed against actual product objects/maps; parser rewire separated from lifecycle cohort; no production edits.

P97: [Final file ownership review](s12-p97-final-file-ownership.json); all190 files reviewed, not all dependencies removed. C0169/C0170 remain S12 native-resource migration candidates before closure.

P98: [Resource binding ownership](s12-p98-resource-binding-ownership.json); C0169/C0170 moved to SDK-only compat, exact bodies/state retained; cached builds, actual x86/x64 object tests and dual release.

P99: [Ledger correction](s12-p99-ledger-correction.json); C0430 was eliminated in P28, confirmed against current product COFF/map and both release hashes. No production change.

P100: [Eliminated-edge reconciliation](s12-p100-eliminated-recheck.json); all88 eliminated IDs checked against current production objects/compiler inputs and specific retired-edge evidence; seven stale follow-up instructions corrected.

P101: [Open-ID evidence index](s12-p101-open-evidence-index.json); navigation for480 remaining IDs and explicit S13 cohort disposition for81 WOW native-call/lifecycle symbols. No dependency elimination claimed.

P102: [Classifier path ownership delivery](s12-p102-classifier-path-ownership.json); C0152 moves D to B with unchanged function body and original output ABI. Cached builds,8 A/B graphs,actual x86/x64 object tests and22 publication hashes passed.

P103: [Immutable config ownership](s12-p103-config-ownership.json); C0154/C0155 move D to B unchanged, actual x86/x64 object tests and dual release pass. Five session-thread edges explicitly remain S13.

P104: [Start-glass ownership](s12-p104-start-glass-ownership.json); C0320/C0440 use one B source with original per-product ABI, identical function bytes, cached builds and complete dual publication. Pure-relocation runtime waiver applies.

P105: [Console close ownership](s12-p105-console-close-ownership.json); five callers classified, isolated macro alias rejected for C4273. Graphics lifetime remains an open S13 cohort; no product change or new runtime claim.

P106: [First nine open header dispositions](s12-p106-header-dispositions.json); S13 lifecycle and S14 original declaration ownership separated, P102/P103 delivered providers excluded from remaining work. No dependency counted eliminated.

P107: [CCPU fallback ownership delivery](s12-p107-vector-defaults.json); three unchanged breakpoint providers move F to B, C0190-C0192 eliminated. Cached builds, eight boundary graphs and dual publication pass; all eleven normalized images equal P104.

P108: [S12 closure and complete successor handoff](s12-p108-closure-handoff.json); 96 eliminated,472 still open. S13 receives428 lifecycle edges and21 mixed groups;23 original-owner edges reserved for S14. Allocation is not implementation approval or dependency elimination.

S13 P2: [COMMAND and shared thread boundary design](s13-p2-command-thread-boundary.json); first remaining file reviewed as a coupled snapshot/thread/launch group. Per-image VDMREDIR initialization must be verified; no product migration or dependency elimination yet.

S13 P3: [Shared thread boundary delivery](s13-p3-thread-boundary.json); five direct C-to-D edges now resolve to B, with explicit per-image outer lifecycle binding. Cached builds, dual11 publication, ABI/module/async fixtures and real guest redirector read/write/NetAPI pass; no mirror change. COMMAND snapshot/native launch remains open.

S13 P4: [COMMAND boundary delivery](s13-p4-command-boundary.json); twelve direct outward edges now use an explicitly installed B service ABI. Original C algorithm and F mutable ownership remain unchanged. Cached builds, dual eleven-image publication, capture/launch fixtures and actual native EDIT/DIR pass. Mirror delta0 points/0 lines;113 eliminated,455 open.

S13 P5: [Console keyboard query boundary](s13-p5-console-keyboard-boundary.json); cmdkeyb retains its original algorithm and both query symbols now resolve to B, with explicit F routing operations. No mirror change; cached builds, dual publication, three Console transport cases and actual native EDIT return pass.115 eliminated,453 open.

S13 P6: [cmdmisc ownership review](s13-p6-cmdmisc-ownership-review.json); bounded environment, guest output and Console title groups mapped before implementation. No product change or dependency elimination; P5 remains the released baseline.

S13 P7: [cmdmisc boundary delivery](s13-p7-cmdmisc-boundary.json); pure environment and numeric location move to B, session copy and Console title bodies remain F behind installed B contracts. Six IDs eliminated;121 eliminated,447 open. Cached x86/x64/reference,8 boundary graphs,dual11 publication and targeted COMMAND/Console/WOW/location checks pass. Mirror remains9 points/52 lines;delta0/0.

S13 P8: [cmdpif compilation boundary](s13-p8-cmdpif-search-boundary.json); exact original source retained, three unused D/F search roots removed in product/reference graphs. Cached builds,8 boundary graphs and dual11 publication pass. All11 normalized images equal P7; repeated runtime tests waived. Shared IDs remain open:121 eliminated,447 open. Mirror0 points/0 lines;delta0/0.

S13 P9: [DEM guest-lease boundary](s13-p9-dem-lease-boundary.json); opaque B contract replaces outward memory declarations and static calls, while F mapping/commit bodies and C algorithms remain unchanged. Three IDs eliminated:124 eliminated,444 open. Cached builds, dual11 publication and targeted COMMAND/Console/DEM/redirector checks pass. Mirror15 points/49 lines;delta0/0.

S13 P10: [demgset directory registration](../t483-c-layer-dependencies/s13-p10-demgset-directory-boundary.json); existing B COMMAND table now carries the two registration operations. F state source and C algorithms unchanged; three IDs eliminated:127 eliminated,441 open. Cached builds, dual11 publication and targeted COMMAND/directory/Console/guest checks pass. Mirror3 points/7 lines;delta0/0.

S13 P11: [demsrch unavailable VDM boundary](s13-p11-demsrch-vdm-boundary.json); unused F callback state removed, B retains original cdecl unavailable result, C search algorithms byte-unchanged. C0188 eliminated:128 eliminated,440 open. Cached builds, dual11 publication, original ABI,115 directory assertions and real DOS DIR pass. Mirror5 points/25 lines;delta0/0.
