# T478 S1 — 69 条既有问题分类清单

后续处置：第 33 条已由 [T478 S2](m0-t478-s2-generic-thunk.md) 完成接线、EBX 修正、验证和工作树内发布；下列分类保留 S1 核对时的基线，不再将第 33 条视为当前未解决项。

第 04 条后续由 [S6](m0-t478-s6-sas-vector.md) 核销为“未证实有当前产品需要修复的缺陷”：保留布局和未支持槽位，不新增能力；仅在出现具体适用调用者或失败证据时重开。不是宣称所有间接路径绝对不可达。

第 10 条由 [S7](m0-t478-s7-xms-boundaries.md) 完成最小原版宿主修正：零大小与同大小返回值初始化，139 项宿主断言、真实 DOS 用例及 16 项 Full 门禁通过；不再作为未解决项。下表保留原审计基线。

第 11 条经 [S8](m0-t478-s8-suballoc-disposition.md) 确认当前选中代码与两版原版一致，正常生命周期通过；失败回滚问题作为原版已知限制保留，未声称修复。按用户原版语义上限，不扩展事务回滚；限制已登记 TODO。

第 13 条由 [S9](m0-t478-s9-fdc-session.md) 修复项目引入的线程局部登记和缺失会话绑定：同会话跨线程访问、代次与短复制退出测试通过，16 项 Full 门禁及本工作树发布完成。未进行物理软盘介质验收；原版设备/取消限制不扩展。

第 15 条经 [S10](m0-t478-s10-keyboard-layout.md) 确认正常两字符键盘代码宿主接线已通；过长配置的缓存缺陷与两版原版一致，实际对象可复现。按不新增容错的标准保留并登记限制，不宣称异常配置安全。

第 22 条经 [S11](m0-t478-s11-xms-dispatch.md) 确认公开 XMS 函数检查已存在，宿主 12 项内部 BOP 服务接线与原版一致；不把调试版检查提升为发布版新增容错，保留内部非法服务号限制。

第 24 条由 [S12](m0-t478-s12-redirector-loader.md) 修复既有失败路径的模块回收与残留回调，并删除未被当前产品调用的自写加载适配器。77 项定向检查及 16 项 Full 门禁通过，工作树发布完成；不是新增重试或网络能力。

第 28 条由 [S13](m0-t478-s13-keycode-bounds.md) 修复两个扫描码表比较符，表内容和有效映射不变；实际对象的两处严格边界及 268,322 项检查通过，16 项 Full 门禁与工作树发布完成。未声称物理键盘复现该边界。

## 范围与证据

用户明确：审计目标是 mvdm-host 和必要宿主接线；guest 镜像、guest 文件、guest 载荷和配置仅可作为定位或验证 mvdm-host 能力及缺失接线的证据，不单独审计、实施或列作 guest 包装工作。独立工具不构成宿主运行链的缺失。

针对 [69 行逐项核对表](m0-t478-s1-prior-findings.md) 作静态归因；原始分母、逐项来源、当前源码、实际 guest/受控测试区别均以该表为准。[源码锚点对照](m0-t478-s1-source-anchors.md)记录当前工作树基线与原版 200 个文件的身份。分类是发现和风险的来源，不是对真实用户影响、当前运行或未来修复可行性的统一结论；一条可能含多个子项。T478 S1 的两份完整审计清单与 T473 其他交接尚未全部核销，不能把本 69 行清单冒充整个 T478 最终收口。

原版路径的失败条件可在源码确认，但未有真实 guest 触发的只标风险。自主实现区分已被历史真实观察的代码页差异和只经受控/静态分析的候选。被排除的旧分支要核查原始条件与替代 owner，不允许一律恢复。

## 原版代码问题或条件风险：20 行

| 编号 | 既有问题与证据 | 现状及证据边界 |
| --- | --- | --- |
| 10 | [XMS block request](../../../artifacts/mvdm-audit/shard-0/xms-block-zero-boundary.md) | 原版代码风险：当前 XMS 零大小分配/相等大小重分配仍存在未初始化局部量路径；正常 guest 触发尚未确定 |
| 11 | [Suballocation failure](../../../artifacts/mvdm-audit/shard-2/suballoc-xms-callback-failure-review.md) | 原版代码问题候选：原 suballocator 在回调失败时的位图/回滚问题有受控复现；真实 guest 触发未证实 |
| 15 | [COMMAND keyboard layout](../../../artifacts/mvdm-audit/shard-3/cmd-keyboard-layout-role-review.md) | 原版代码风险：原版及当前 COMMAND 使用 28 字节输入拷入 5 字节缓存；现有输入校验能否阻止长值尚未证明 |
| 22 | [XMS dispatch and service inputs](../../../artifacts/mvdm-audit/shard-3/xms-service-role-review.md) | 原版代码风险：当前和 OpenNT 原版 XMSDispatch 仅 DBG 分支检查索引，release 直接下标调用；正常 guest 发出非法服务号尚未证明 |
| 24 | [Redirector load and task lifecycle](../../../artifacts/mvdm-audit/shard-3/redir-service-role-review.md) | 原版代码风险：OpenNT 原版 SafeLoadLibrary 也调用 LoadLibrary，而加载失败清理仍 CloseHandle(HMODULE)；真实失败路径尚未实测 |
| 28 | [Original scan-table exact bound](../../../artifacts/mvdm-audit/input-native-role-review.md#proven-one-past-key-table-read) | 原版代码问题：当前两个表边界仍使用 >；实际对象越界受控复现，真实输入触发未证实 |
| 29 | [ROM and CPU register owners](../../../artifacts/mvdm-audit/shard-1/rom-residency-xt-audit.md); [BIOS resource family](../../../artifacts/mvdm-audit/shard-2/bios-family-resource-review.md); [non-ROM read failure](../../../artifacts/mvdm-audit/rom-read-failure-review.md) | 原版代码问题：非 ROM 资源过大时当前源码返回前未关闭描述符；受控复现，真实加载路径未确定 |
| 34 | [WOW file cache failure](../../../artifacts/mvdm-audit/shard-1/wow-file-cache-image-and-test-design.md); [flush tail](../../../artifacts/mvdm-audit/shard-1/wow-file-cache-flush-review.md) | 原版代码问题：当前映射缓存尾节点仍被跳过；句柄复用和资源后果已有受控复现，真实 guest 未证实 |
| 42 | [WOW virtual-memory owner](../../../artifacts/mvdm-audit/shard-1/wow-virtual-memory-review.md) | 原版代码问题候选：原始 VDM 分配器标志判断/失败回滚需限定调用者，真实产品故障未证实 |
| 43 | [WOW parameter-map lifecycle](../../../artifacts/mvdm-audit/shard-1/wow-param-map-review.md) | 原版代码风险候选：原版与当前均按 guest 指针共享节点、仅首任务拥有并按任务清理；跨任务同时借用是否合法/可达尚未证实，DIV-347 的已修回滚不重复列出 |
| 47 | [EDIT tab-stop return](../../../artifacts/mvdm-audit/shard-2/wow-edit-tabstop-unthunk-review.md); [bounded repair design](../../../artifacts/mvdm-audit/host-ch-scope/wow-edit-tabstop-repair-design.md) | 原版代码问题：当前返回处理仍发生 fallthrough；此前对象与 Win16 观察需重新绑定当前产物 |
| 49 | [WOW DIB provider lifecycle](../../../artifacts/mvdm-audit/shard-3/wow-dib-provider-lifecycle-review.md) | 原版代码风险：当前/原版在 DIB selector 设置成功后若登记失败走清理，未见 selector 恢复且用 CloseHandle 处理位图；真实失败与泄漏未证明 |
| 50 | [DIB selected mapping correction](../../../artifacts/mvdm-audit/shard-3/wow-dib-selected-mapping-provider-review.md); [descriptor retirement](../../../artifacts/mvdm-audit/shard-3/wow-dib-descriptor-retirement-review.md) | 原版代码风险：DIB 映射的原版 provider 已接入；当前与原版销毁路径忽略 selector 回调结果后继续卸载映射，失败/重入触发未证明 |
| 51 | [WOW font query output boundaries](../../../artifacts/mvdm-audit/shard-3/wow-gdi31-font-output-review.md) | 原版代码风险：font/glyph 失败或短返回仍按请求量复制；kerning 一元素映射写多元素；实际 host/guest 输出影响未证明 |
| 55 | [WOW palette conversion](../../../artifacts/mvdm-audit/shard-0/wow-palette-image-review.md) | 原版代码问题：当前按请求数而非实际返回数复制调色板；短返回宿主及 thunk 受控证据，guest 暴露未证实 |
| 56 | [Embedding profile and classes mapping](../../../artifacts/mvdm-audit/shard-1/wow-embedding-review.md) | 原版代码风险与自主 provider 交界：原版枚举仅以 NO_MORE_ITEMS 退出，当前项目 provider 可返回 INVALID_HANDLE；持续失败时源码会循环，真实触发未证实；guest buffer 子项未定 |
| 57 | [WOW fax client](../../../artifacts/mvdm-audit/shard-1/wow-fax-owner-review.md) | 原版代码风险：当前传真分配后先清零再检查 NULL 等路径仍存在；fax 提供者与实际失败未确认 |
| 58 | [WOW printer setup and spool failure paths](../../../artifacts/mvdm-audit/shard-3/wow-spool-loader-callers-review.md) | [S26](m0-t478-s26-spool-failure.md) 已核对 ClosePrinter 失败也消费句柄，FreePrn 无需改动；修复原版启动失败、载体创建失败及设置入口加载失败的资源遗漏。实际 raw provider 限制仍保留 |
| 59 | [DLC completion event](../../../artifacts/mvdm-audit/shard-1/vdmredir-dlc-completion-event-audit.md) | 原版代码问题：当前第二事件失败检查仍指向第一个事件；DLC 提供者与故障触发未证实 |
| 62 | [Redirector asynchronous completion](../../../artifacts/mvdm-audit/shard-3/redir-async-role-review.md) | [S28](m0-t478-s28-async-review.md) 已隔离复现分配失败后的空队列异常；用户明确接受原版内存耗尽限制，正常路径不变。未声明通用 drain 或修复 |

## 自主实现问题或条件风险：4 行

| 编号 | 既有问题与证据 | 现状及证据边界 |
| --- | --- | --- |
| 04 | [SAS vector](../../../artifacts/mvdm-audit/shard-3/sas4gen-vector-audit.md) | 自主实现候选：SAS 两个备用实现为 debugbreak、另一槽为 NULL；实际调用可达性待核实 |
| 13 | [Disk close and FDC thread](../../../artifacts/mvdm-audit/shard-3/floppy-lease-failure-role-review.md); [single-owner assessment](../../../artifacts/mvdm-audit/shard-3/fdc-session-single-owner-design.md) | 自主实现问题候选：COMMAND 状态是线程局部，FDC 跨线程不能直接看到；真实设备触发和安全修复待验证 |
| 30 | [Codepage transition](../../../artifacts/mvdm-audit/shard-1/standalone-tool-chain-audit.md) | 自主实现 bug：独立 NTVWM/NTCON Console 代码页未同步；历史真实 DOS 输出复现，当前相关源码仍无传播路径 |
| 65 | [WOW Winsock asynchronous database requests](../../../artifacts/mvdm-audit/shard-1/wow-winsock-database-review.md) | [S29](m0-t478-s29-winsock-rundown.md) 已修复任务退出忽略取消结果而提前释放原生缓冲区的自主实现问题；323 项受控检查、三项真实 Win16 场景及 16 项 Full 通过。取消失败且永无完成通知时保留原生所有权至进程退出，原版句柄回绕不扩展 |

## 缺失宿主接线候选：4 行

| 编号 | 既有问题与证据 | 现状及证据边界 |
| --- | --- | --- |
| 09 | [Kernel directory reset](../../../artifacts/mvdm-audit/external-kernel-vdm-boundary.md) | 缺失原版接线：NT 内核目录索引重置服务没有生产绑定，原始慢速 fallback 已由 T430 实际 guest 测试；缺失主要为索引快速路径，删除锚点边界仍有风险 |
| 20 | [ForceDOS](../../../artifacts/mvdm-audit/shard-3/forcedos-original-owner-audit.md); [PIF](../../../artifacts/mvdm-audit/shard-3/pif-config-lifecycle-audit.md) | 缺失原版接线候选：原版 COMMAND 消费 CREATE_FORCEDOS，当前生产者未找到该标志；PIF 子问题仍待验证 |
| 26 | [Remote RAP provider](../../../artifacts/mvdm-audit/shard-3/remote-rap-role-review.md) | 缺失原版接线：RAP 原调用接项目未实现提供者；依赖私有传输，真实 guest 路径待验证 |
| 33 | [Generic thunk](../../../artifacts/mvdm-audit/shard-0/wow-generic-thunk.md); [published-code binding](../../../artifacts/mvdm-audit/shard-3/wow-generic-thunk-published-binding.md) | 缺失原版接线：当前 i386 分支为 #error，WOW32 构建生成器仅从 sources 收集 C 文件；原汇编 helper 未选入，历史测试有调用约定不匹配失败 |

09 是 NT 内核索引快速路径，原版慢速 fallback 已验证；20 仅追踪宿主 COMMAND 对 CREATE_FORCEDOS 的接收/传播，独立工具和 guest 包装不属于范围；26 的 RAP 私有传输边界不等于整个网络不可用；33 的原版 `i386` 分支是当前 `#error`，已有汇编 helper 和历史调用约定对照。每项是否应接入仍须依据宿主适用性、实际调用与依赖裁决。

## 已有部分交付、其余仅是尚未证明的覆盖边界：16 行

| 编号 | 既有问题与证据 | 现状及证据边界 |
| --- | --- | --- |
| 01 | [CPU far-transfer diagnostic](../../../artifacts/mvdm-audit/shard-3/cpu-far-diagnostic-audit.md); [BOP/interrupt inventory](../../../artifacts/mvdm-audit/shard-1/diagnostic-guest-access-inventory.md); [illegal-op diagnostic](../../../artifacts/mvdm-audit/shard-0/illegal-op-diagnostic-audit.md); [diagnostic recovery](../../../artifacts/mvdm-audit/s11-foundation-evidence/diagnostic-stack-candidate/review.md); [trace recovery](../../../artifacts/mvdm-audit/s11-foundation-evidence/trace-scalar-candidate/review.md) | [S30](m0-t478-s30-diagnostic-disposition.md) 已核对六处调用者及当前产品对象，既有修复有效；源码对照、实际 CCPU 异常顺序/CALLF 和诊断检查通过。任意嵌套异常不作完整覆盖声明，不因此新增能力 |
| 07 | [DLL-entry BOOLEAN return](../../../artifacts/mvdm-audit/dll-entry-boolean-return.md); [real attach failure](../../../artifacts/mvdm-audit/s11-foundation-evidence/wow-host-attach-failure/review.md); [current loader](../../../artifacts/mvdm-audit/s11-foundation-evidence/current-wow32-real-loader/review.md) | [S31](m0-t478-s31-dll-entry-disposition.md) 已确认修复有效；当前 BOOLEAN/OS-loader 测试及真实 WOW32 attach 失败、缺失导出的早期显式卸载均通过。任意活跃模块重载及 guest DLL 生命周期不属于本宿主问题 |
| 08 | [Original export-source/profile recovery](../../../artifacts/mvdm-audit/export-source-recovery-review.md); [published live export witness](../../../artifacts/mvdm-audit/s11-foundation-evidence/published-export-live/review.md) | [S32](m0-t478-s32-export-disposition.md) 已核对 140 条原版配置声明、274 项现行导出与旧序号连续性，并验证实际 VDD/MEM 创建及退出调用；模板恢复原版字节，Full 16 项及工作树发布完成。不宣称任意第三方消费者全覆盖 |
| 12 | [DPMI buffers and saved state](m0-t478-s33-dpmi-buffer-state.md) | S33 已核清：原版接线存在；133 项受控检查、45 项租约边界、session 测试、4 个真实 INT21 用例与 11 个映像函数绑定通过；保留原版固定池耗尽及非法输入前提，不新增容错 |
| 21 | [BIOS integrations](m0-t478-s34-bios-integration.md) | S34 修复原版 D11 从 PIC EOI 端口错误：74 项实际 PIC 检查、原 guest 向量、BIOS/EMS 及 IRQ 回归通过；Full 16/16、工作树发布完成。RTC 专项仍属 S35，不宣称全文件行为覆盖 |
| 27 | [BIOS memory and RTC](m0-t478-s35-rtc-investigation.md) | S35：实测可变 INT12、原版 ROM 异步/同步等待、闹钟 INT4A 与 IRQ/EOI 及状态恢复通过；取消后 busy 残留登记为原版 guest 限制。备用 host BOP70 计数缺陷保留为非正常路径限制，不宣称已修复；产品和发布包不变 |
| 32 | [IOS and CCPU port owners](../../../artifacts/mvdm-audit/ios-owner-review.md) | 部分已验证：原版 IOS port 拆分/整端口解绑是原始契约，70 项受控检查通过；未发现因此缺失原版接线 |
| 35 | [WOW shell, hooks and subclassing](m0-t478-s36-shell-hook-investigation.md) | S36：真实 Shell 子进程/返回、CBT 嵌套/否决/显式与遗留 hook 退出通过；原版清理真实 USER hook、槽隔离/复用及 subclass 生命周期通过，84 个当前函数绑定；无产品缺口，未增加容错 |
| 38 | [WINOLDAP guest module](../../../artifacts/mvdm-audit/shard-1/guest-winoldap-owner-review.md) | 部分已验证：O:/w31/SYSTEM 有另一版本 WINOLDAP.MOD；T445 在测试包用原版控制文件验证了 Win16/native/Win16 返回，实际加载身份仍未确定 |
| 40 | [DDEML](../../../artifacts/mvdm-audit/shard-2/ddeml-owner-boundary.md); [WINMM callback](../../../artifacts/mvdm-audit/external-winmm-callback-boundary.md) | 部分已完成：T465 已实测经典 WM_DDE_* 正常会话和任务退出，T466 已证明同步 WINMM 桥；DDEML guest 实际 owner、异步 WINMM 设备未证实 |
| 44 | [WRITE image identity](../../../artifacts/mvdm-audit/shard-2/win16-write-guest-review.md); [OLE server image](../../../artifacts/mvdm-audit/shard-0/ole-server-image-review.md) | 部分已完成：T475 S5 已在真实 Win16 WRITE 连续两次编辑、保存和退出并验证输出文件；OLE server 实际身份/路径及其他功能仍未核对 |
| 46 | [Raw spool and shared handle carrier](../../../artifacts/mvdm-audit/shard-1/wow-spool-carrier-review.md) | 部分已完成/原版限制：T468 已验证正常 HDC 打印及对话框；原版 raw profile/provider 不可用和突发任务退出限制由用户接受，异常所有权见 58 |
| 48 | [Clipboard formats](../../../artifacts/mvdm-audit/shard-2/wow-clipboard-format-failure-review.md); caret finding in [cross-shard findings](../../../artifacts/mvdm-audit/findings.md) | 部分已解决：T460 已验证文本/DIB 的受限场景；其他 clipboard 格式及 caret 仍缺实际证据 |
| 52 | [WOW keyboard thunks](../../../artifacts/mvdm-audit/shard-3/wow-keyboard-thunks-lifecycle-review.md); [driver build inputs](../../../artifacts/mvdm-audit/shard-3/wow-keyboard-driver-build-role-review.md) | 部分已接入：当前 wkbman.c 与历史审计所记哈希一致；真实 guest 输入、DBCS、失败和退出待验证 |
| 54 | [WOWFAX](../../../artifacts/mvdm-audit/shard-1/wowfax-trace.md); [multimedia alias selection](../../../artifacts/mvdm-audit/shard-3/wow-multimedia-alias-consumer-followup.md); [bridge image binding](../../../artifacts/mvdm-audit/shard-0/wow-media-resource-image-review.md) | 部分已完成：T466 已验证原版 timeGetTime 桥及 11 个入口绑定，并明确不恢复原本 #if 0 转换代码；真实 wave/MIDI/MCI/fax 设备及任务退出未验证 |
| 61 | [MSCDEX guest/VDD boundary](../../../artifacts/mvdm-audit/shard-0/mscdex-package-review.md); [build input roles](../../../artifacts/mvdm-audit/shard-3/mscdex-guest-vdd-input-role-review.md) | 只保留 host 侧问题：原版 RegisterVDD/宿主 loader 和光驱调用链是否可用待证；VCDEX guest 环境位置及安装不是 S1 独立任务 |

本节的已验证子项从待修 bug 中排除；未验证不等于已解决，也不自动构成缺陷或原版接线缺失。61 只保留宿主 VDD loader 和可定位宿主能力，guest 文件路径仅作证据。

## 已核销误判、已接受原版限制或非功能事项：17 行

| 编号 | 既有问题与证据 | 现状及证据边界 |
| --- | --- | --- |
| 14 | [COMMAND exit](../../../artifacts/mvdm-audit/shard-1/command-exit-owner-audit.md) | 已核销已验证场景：T471 S3 实测 DOS→WOW worker 异常退出→DOS 恢复；T477 P3 实测嵌套 CMD/DOS 父返回和退出 23 |
| 16 | [DOS command inputs](../../../artifacts/mvdm-audit/dos-command-input-review.md) | 已核销历史文件误判：85 个 DOS 命令源码/构建输入已有原始 role 对照，obsolete DIR 有原版替换；实际可选 guest 工具分 17/19 等单列 |
| 18 | [WIN87EM identity and loader](../../../artifacts/mvdm-audit/shard-3/win87em-build-input-role-review.md) | 已排除原版文件缺失误判：当前 ZIP 和 O:/w31/SYSTEM 均含清单匹配的 WIN87EM.DLL；仅用于特定 Win2 依赖，实际 guest loader 行为未证明 |
| 23 | [XMS A20/UMB owners](../../../artifacts/mvdm-audit/shard-0/xms-runtime-owner-review.md) | 已核销整体未接入假设：当前原版 XMS sources 与分发表包含 A20、UMB 初始化/申请/释放，历史对象已绑定；guest 指针和失败覆盖另保留 |
| 25 | [DLC guest-window lifetime](../../../artifacts/mvdm-audit/shard-3/dlc-window-role-review.md) | 已核销 host 分派未接入假设：当前 0x28 保留 VrVdmWindowInit；已查 guest 初始化调用被注释，其他 caller 和异步指针生命周期未证明 |
| 31 | [Original NT quick events](../../../artifacts/mvdm-audit/quick-event-owner-review.md) | 已排除错误候选：原版 NT recipe 选择 qevnt.c 而非 generic quick_ev.c；37 项受控检查通过，真实设备时序另属覆盖边界 |
| 36 | [WOW cross-language constants](../../../artifacts/mvdm-audit/shard-1/wow-cross-language-constants-review.md) | 已核销常量不匹配假设：当前 C 头 52 个 WOWCF 与 455 个 FUN 定义值均与原版相同，历史审计也核对原版 ASM；guest ABI 其他部分未由此证明 |
| 37 | [WOW current directory](../../../artifacts/mvdm-audit/shard-2/wow-directory-sync-review.md) | 已排除整条目录接线缺失：当前 sources 保留原版 wdos.c 和任务目录同步；原版失败次序与真实 Win16 运行覆盖仍需区分 |
| 39 | [SYSTEM.INI profile](../../../artifacts/mvdm-audit/shard-2/wow-kernel-profile-module-review.md); [language thunks](../../../artifacts/mvdm-audit/shard-1/wow-language-thunk-owner-audit.md) | 已排除整条语言接线缺失：当前 sources 选中原版 wulang.c；history 记录入口绑定；profile shadow 错误风险单列 56，DBCS/失败未证明 bug |
| 41 | [Original global/local heap declarations](../../../artifacts/mvdm-audit/host-ch-scope/wow-four-excluded-owner-recheck.md); [GLOBALFREEALL guest owner](../../../artifacts/mvdm-audit/host-ch-scope/globalfreeall-guest-owner-review.md) | 已核销误判：原版 guest kernel 自有 GlobalFreeAll，历史 host GLOBALFREEALL 原就 UNIMPLEMENTEDAPI；不能因 host slot 未实现断言缺失 guest cleanup |
| 53 | [Selected metafile versus dual-purpose reference source](../../../artifacts/mvdm-audit/wow16-extension-source-role-review.md) | 已核销误判：当前 sources 选择原版 wgmeta.c，GET/SETMETAFILEBITS 仍为 guest LOCALAPI；T456 验证有限 host 路径，guest callback 边界另列待验证 |
| 60 | [DLC event/post owner](../../../artifacts/mvdm-audit/shard-0/redir-dlc-post-image-review.md) | 已核销整体未接入假设：原版 DLC 事件与 post 对象有历史发布绑定，当前 dispatch 保留 DLC 槽；真实 provider 和异步退出仍未验证 |
| 63 | [Redirector dispatch](../../../artifacts/mvdm-audit/shard-0/redir-dispatch-release-review.md) | 已核销整体未接入假设：原版 50 槽有历史发布绑定，当前表保留服务；远程 RAP 与 DLC 等具体限制分别见 26/59/60 |
| 64 | [VDMREDIR image identity](../../../artifacts/mvdm-audit/shard-0/vdmredir-package-identity-review.md) | 已核销产品不一致：主工作树 T477 P4 提交 6db66ab22 发布后与安装目录哈希一致；本审计树保留原基线，旧快照差异不是代码 bug |
| 67 | Registry identifiers and selected-versus-linked reconciliation in [cross-shard findings](../../../artifacts/mvdm-audit/findings.md) | 非运行时归因问题：历史 registry 标识和链接选择文档有冲突；当前记录一致性仍需核对 |
| 68 | [Win16 TOOLHELP class thunks](../../../artifacts/mvdm-audit/shard-2/win16-toolhelp-guest-review.md) | 已核销为原版限制：T470 实际 guest 测试确认 ClassFirst 为原始 UNIMPLEMENTEDAPI 结果，用户已接受；非本项目缺失原版接线 |
| 69 | [Unused C-VID test-provider scaffold](../../../artifacts/mvdm-audit/cvidc-inert-scaffold-review.md) | 非功能 bug：仍有未选入生产的测试桩生成规则；历史图无消费者，当前构建图仍需复核 |

## 原有用户排除或宿主审计范围之外：8 行

| 编号 | 既有问题与证据 | 现状及证据边界 |
| --- | --- | --- |
| 02 | [Signed memory bit indexes](../../../artifacts/mvdm-audit/ccpu-header-macro-review.md#demonstrated-signed-memory-bit-index-difference); [shared decoder path](../../../artifacts/mvdm-audit/host-ch-scope/ccpu-bit-index-shared-path-review.md) | 按既有用户边界暂不处理；真实触发未证实 |
| 03 | [x87 operand spans](../../../artifacts/mvdm-audit/ccpu-header-macro-review.md#x87-source-inconsistency-runtime-result-inconclusive) | 按既有用户边界暂不处理；真实触发未证实 |
| 05 | [B067/B068 BIOS fallback](../../../artifacts/mvdm-audit/s11-foundation-evidence/b068-selected-boundary/review.md) | 原版代码问题（当前范围外）：B067/B068 fallback 的宽度被截成 IU8，原版同样如此；实际 fallback/外部 provider 尚未证实 |
| 06 | [486 profile gates](../../../artifacts/mvdm-audit/shard-3/cpu486-profile-boundary.md) | 用户排除 486 专项 |
| 17 | [ANSI.SYS package identity](../../../artifacts/mvdm-audit/shard-2/ansi-package-identity-review.md) | 范围外：ANSI.SYS 为可选 guest 载荷，缺包/guest 配置不单独构成 mvdm-host 接线问题；仅在证明 host 缺口时引用 |
| 19 | [APPEND and NLSFUNC package identities](../../../artifacts/mvdm-audit/shard-2/dos-append-nls-package-review.md); [locale source chain](../../../artifacts/mvdm-audit/shard-2/dos-locale-source-chain-review.md); [COUNTRY data versions](../../../artifacts/mvdm-audit/shard-0/country-bin86-identity-review.md) | 范围外：APPEND/NLSFUNC 为可选 guest 载荷，guest 包含与配置不作为 S1 实施目标；只有证实 host NLS/文件服务缺口才纳入 |
| 45 | [PIFEDIT and FORCEDOS](../../../artifacts/mvdm-audit/shard-1/pifedit-forcedos-family-review.md) | 范围外/重复：PIFEDIT 和 FORCEDOS 是独立工具/载荷，不作为 S1 审计对象；若其调用暴露 COMMAND host 标志缺口，见 20 |
| 66 | [BDE extension](../../../artifacts/mvdm-audit/shard-0/bde-debugger-extension-audit.md) | 范围外：BDE 为独立 NTSD 调试扩展，不属于 mvdm-host 的 DOS/WOW 运行链；除非证明其为宿主所需依赖，不作为 S1 接入目标 |

## 当前限制和交付状态

69 行均有一次分类与证据链接；分类不代表每条运行路径在此基线均经重新构建或实时测试。历史对象和 guest 实验有各自包哈希、控制条件与失败边界；当前源码存在或原版比较一致也不证明当前产物行为。原版 CPU30/硬件 V86 或独立 486 修复不进入任务。

本轮仅审计和文档，无产品源码、guest 或 release 更改；尚未提交。S1 仍为活动状态。

## S3 后续处置

第 20 条的宿主 ForceDOS 标志生产与 Run16 分派缺口已由
[S3](m0-t478-s3-forcedos.md)修复并完成产品验证。原版 COMMAND 和 broker
继续作为实现所有者；独立工具、guest 和未证明的 PIF 子问题不纳入本次修复。
以上保留原始审计分类，不以修复后的状态改写当时的证据或分母。

## S4 后续处置

第 26 条由 [S4](m0-t478-s4-remote-rap.md)分界核销：原版服务器枚举的
空 transport 路径调用公开 NetServerEnum，已恢复原版函数及转换并删除整体失败桩。
真正的私有 RAP、密码/会话密钥及私有 Browser 入口保留明确不可用契约。
实际 DOS 枚举返回与本机原生 API 一致的 6118；成功转换由原版处理器控测覆盖。
这不宣称私有 RAP 传输已实现，也不改写上表历史审计分类和分母。

## S5 后续处置

第 09 条在 [S5](m0-t478-s5-directory-restart.md)核销：现有普通恢复和已批准的
有限后继提示通过当前测试，无需新增生产实现。原版内核索引快路径保留为用户明确
接受的已登记缺口；提示缺失或不匹配时明确失败，不扩展目录变更容错能力。
