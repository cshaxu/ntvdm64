# T478 S1 — 既有问题核对表（进行中）

分母是 [T473 既有恢复工作清单](../../../artifacts/mvdm-audit/recovery-worklist.md) 的 69 条具名行。历史阶段号仅作追溯；同一行可能包含多个问题。当前状态为已核对的有限证据，证据不足的条目明确保留原因，不能推断为缺陷或已修复；历史绑定不自动等于当前发布镜像或实际运行。其他审计总账与工作清单正文的补充问题尚需交叉核对。

针对这 69 行报告中可映射的最多五个源码锚点，已将当前文件与原版的字节和换行归一结果记录在[源码锚点对照](m0-t478-s1-source-anchors.md)：60 行有锚点，共 200 条；9 行没有可映射锚点。它只确认文件身份/差异，不能独立证明实际选中或运行。原版比较树的只读 HEAD 已核验为 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c，且已跟踪文件无未提交修改。

## 逐行初筛摘要

| 初筛状态 | 行数 | 解读 |
| --- | ---: | --- |
| 原版代码问题或风险 | 21 | 有原版来源或受控证据，部分真实触发尚未证明。 |
| 自主实现问题或风险候选 | 4 | 项目代码承担边界；可达性或真实故障仍须核对。 |
| 缺失原版宿主接线候选 | 4 | 包括原版 helper、RAP、ForceDOS 宿主链和目录索引快速路径；适用性和依赖仍须逐项确认。 |
| 已有部分交付或既有通过证据 | 16 | 仅解决或验证部分子项，余下不能整行排除。 |
| 用户排除或宿主审计范围外 | 7 | CPU 内部两项、486 专项及 guest/独立工具范围外条目；guest 仅作宿主证据。 |
| 已排除误判或非功能问题 | 17 | 测试脚手架或记录一致性。 |
| 证据不足或跨边界混合 | 0 | 包括原版限制、部分已接入但未验证、包身份和调用未明；不猜测归责。 |

这些是按每行首要初筛标签统计，不是 69 个独立缺陷；一行可能含多个功能、多个来源或已解决与未解决的子项。已交付部分只在其有证据的范围内排除，未知项不作为已确认缺失。分类尚需对照最新构建和后续交付，不是 S1 最终审计结论。

| 编号 | 原阶段 | 既有问题及证据 | 当前核对状态 |
| --- | --- | --- | --- |
| 01 | S11 | [CPU far-transfer diagnostic](../../../artifacts/mvdm-audit/shard-3/cpu-far-diagnostic-audit.md); [BOP/interrupt inventory](../../../artifacts/mvdm-audit/shard-1/diagnostic-guest-access-inventory.md); [illegal-op diagnostic](../../../artifacts/mvdm-audit/shard-0/illegal-op-diagnostic-audit.md); [diagnostic recovery](../../../artifacts/mvdm-audit/s11-foundation-evidence/diagnostic-stack-candidate/review.md); [trace recovery](../../../artifacts/mvdm-audit/s11-foundation-evidence/trace-scalar-candidate/review.md) | 已有交付证据；更广嵌套/异常未全覆盖 |
| 02 | Held outside T | [Signed memory bit indexes](../../../artifacts/mvdm-audit/ccpu-header-macro-review.md#demonstrated-signed-memory-bit-index-difference); [shared decoder path](../../../artifacts/mvdm-audit/host-ch-scope/ccpu-bit-index-shared-path-review.md) | 按既有用户边界暂不处理；真实触发未证实 |
| 03 | Held outside T | [x87 operand spans](../../../artifacts/mvdm-audit/ccpu-header-macro-review.md#x87-source-inconsistency-runtime-result-inconclusive) | 按既有用户边界暂不处理；真实触发未证实 |
| 04 | S11 | [SAS vector](../../../artifacts/mvdm-audit/shard-3/sas4gen-vector-audit.md) | 自主实现候选：SAS 两个备用实现为 debugbreak、另一槽为 NULL；实际调用可达性待核实 |
| 05 | S11/S13 | [B067/B068 BIOS fallback](../../../artifacts/mvdm-audit/s11-foundation-evidence/b068-selected-boundary/review.md) | 原版代码问题（当前范围外）：B067/B068 fallback 的宽度被截成 IU8，原版同样如此；实际 fallback/外部 provider 尚未证实 |
| 06 | User boundary | [486 profile gates](../../../artifacts/mvdm-audit/shard-3/cpu486-profile-boundary.md) | 用户排除 486 专项 |
| 07 | S11 | [DLL-entry BOOLEAN return](../../../artifacts/mvdm-audit/dll-entry-boolean-return.md); [real attach failure](../../../artifacts/mvdm-audit/s11-foundation-evidence/wow-host-attach-failure/review.md); [current loader](../../../artifacts/mvdm-audit/s11-foundation-evidence/current-wow32-real-loader/review.md) | 已有交付证据；显式卸载与 guest 生命周期未全覆盖 |
| 08 | S11 | [Original export-source/profile recovery](../../../artifacts/mvdm-audit/export-source-recovery-review.md); [published live export witness](../../../artifacts/mvdm-audit/s11-foundation-evidence/published-export-live/review.md) | 已有交付证据；其他消费者与任务生命周期未全覆盖 |
| 09 | S12 | [Kernel directory reset](../../../artifacts/mvdm-audit/external-kernel-vdm-boundary.md) | 缺失原版接线：NT 内核目录索引重置服务没有生产绑定，原始慢速 fallback 已由 T430 实际 guest 测试；缺失主要为索引快速路径，删除锚点边界仍有风险 |
| 10 | S12 | [XMS block request](../../../artifacts/mvdm-audit/shard-0/xms-block-zero-boundary.md) | 原版代码风险：当前 XMS 零大小分配/相等大小重分配仍存在未初始化局部量路径；正常 guest 触发尚未确定 |
| 11 | S12 | [Suballocation failure](../../../artifacts/mvdm-audit/shard-2/suballoc-xms-callback-failure-review.md) | 原版代码问题候选：原 suballocator 在回调失败时的位图/回滚问题有受控复现；真实 guest 触发未证实 |
| 12 | S12 | [DPMI buffers and saved state](../../../artifacts/mvdm-audit/shard-0/dpmi-buffer-state-review.md) | 部分已完成：T430 S6 已修复项目 guest-memory lease 所有者校验并验证 DPMI/XMS/EMS 代表路径；原版 buffer/save 源级候选未证明真实 bug |
| 13 | S12 | [Disk close and FDC thread](../../../artifacts/mvdm-audit/shard-3/floppy-lease-failure-role-review.md); [single-owner assessment](../../../artifacts/mvdm-audit/shard-3/fdc-session-single-owner-design.md) | 自主实现问题候选：COMMAND 状态是线程局部，FDC 跨线程不能直接看到；真实设备触发和安全修复待验证 |
| 14 | S12 | [COMMAND exit](../../../artifacts/mvdm-audit/shard-1/command-exit-owner-audit.md) | 已核销已验证场景：T471 S3 实测 DOS→WOW worker 异常退出→DOS 恢复；T477 P3 实测嵌套 CMD/DOS 父返回和退出 23 |
| 15 | S12 | [COMMAND keyboard layout](../../../artifacts/mvdm-audit/shard-3/cmd-keyboard-layout-role-review.md) | 原版代码风险：原版及当前 COMMAND 使用 28 字节输入拷入 5 字节缓存；现有输入校验能否阻止长值尚未证明 |
| 16 | S12/S13/S17 | [DOS command inputs](../../../artifacts/mvdm-audit/dos-command-input-review.md) | 已核销历史文件误判：85 个 DOS 命令源码/构建输入已有原始 role 对照，obsolete DIR 有原版替换；实际可选 guest 工具分 17/19 等单列 |
| 17 | S12 | [ANSI.SYS package identity](../../../artifacts/mvdm-audit/shard-2/ansi-package-identity-review.md) | 范围外：ANSI.SYS 为可选 guest 载荷，缺包/guest 配置不单独构成 mvdm-host 接线问题；仅在证明 host 缺口时引用 |
| 18 | S11/S14 | [WIN87EM identity and loader](../../../artifacts/mvdm-audit/shard-3/win87em-build-input-role-review.md) | 已排除原版文件缺失误判：当前 ZIP 和 O:/w31/SYSTEM 均含清单匹配的 WIN87EM.DLL；仅用于特定 Win2 依赖，实际 guest loader 行为未证明 |
| 19 | S12 | [APPEND and NLSFUNC package identities](../../../artifacts/mvdm-audit/shard-2/dos-append-nls-package-review.md); [locale source chain](../../../artifacts/mvdm-audit/shard-2/dos-locale-source-chain-review.md); [COUNTRY data versions](../../../artifacts/mvdm-audit/shard-0/country-bin86-identity-review.md) | 范围外：APPEND/NLSFUNC 为可选 guest 载荷，guest 包含与配置不作为 S1 实施目标；只有证实 host NLS/文件服务缺口才纳入 |
| 20 | S12 | [ForceDOS](../../../artifacts/mvdm-audit/shard-3/forcedos-original-owner-audit.md); [PIF](../../../artifacts/mvdm-audit/shard-3/pif-config-lifecycle-audit.md) | 缺失原版接线候选：原版 COMMAND 消费 CREATE_FORCEDOS，当前生产者未找到该标志；PIF 子问题仍待验证 |
| 21 | S11/S12/S13/S17 | [BIOS integrations](../../../artifacts/mvdm-audit/bios-native-role-review.md) | 部分已验证：BIOS 分派和部分 guest 错误路径有证据；EMS 有效路径、设备 IRQ 生命周期未覆盖 |
| 22 | S12 | [XMS dispatch and service inputs](../../../artifacts/mvdm-audit/shard-3/xms-service-role-review.md) | 原版代码风险：当前和 OpenNT 原版 XMSDispatch 仅 DBG 分支检查索引，release 直接下标调用；正常 guest 发出非法服务号尚未证明 |
| 23 | S12 | [XMS A20/UMB owners](../../../artifacts/mvdm-audit/shard-0/xms-runtime-owner-review.md) | 已核销整体未接入假设：当前原版 XMS sources 与分发表包含 A20、UMB 初始化/申请/释放，历史对象已绑定；guest 指针和失败覆盖另保留 |
| 24 | S12 | [Redirector load and task lifecycle](../../../artifacts/mvdm-audit/shard-3/redir-service-role-review.md) | 原版代码风险：OpenNT 原版 SafeLoadLibrary 也调用 LoadLibrary，而加载失败清理仍 CloseHandle(HMODULE)；真实失败路径尚未实测 |
| 25 | S12 | [DLC guest-window lifetime](../../../artifacts/mvdm-audit/shard-3/dlc-window-role-review.md) | 已核销 host 分派未接入假设：当前 0x28 保留 VrVdmWindowInit；已查 guest 初始化调用被注释，其他 caller 和异步指针生命周期未证明 |
| 26 | S12 | [Remote RAP provider](../../../artifacts/mvdm-audit/shard-3/remote-rap-role-review.md) | 缺失原版接线：RAP 原调用接项目未实现提供者；依赖私有传输，真实 guest 路径待验证 |
| 27 | S13 | [BIOS memory and RTC](../../../artifacts/mvdm-audit/shard-3/bios-memory-rtc-audit.md) | 部分已验证：原版内存和 RTC 入口有历史链接，INT 12 有旧 guest 测试；RTC BOP 70、IRQ/EOI 未覆盖 |
| 28 | S13 | [Original scan-table exact bound](../../../artifacts/mvdm-audit/input-native-role-review.md#proven-one-past-key-table-read) | 原版代码问题：当前两个表边界仍使用 >；实际对象越界受控复现，真实输入触发未证实 |
| 29 | S13 | [ROM and CPU register owners](../../../artifacts/mvdm-audit/shard-1/rom-residency-xt-audit.md); [BIOS resource family](../../../artifacts/mvdm-audit/shard-2/bios-family-resource-review.md); [non-ROM read failure](../../../artifacts/mvdm-audit/rom-read-failure-review.md) | 原版代码问题：非 ROM 资源过大时当前源码返回前未关闭描述符；受控复现，真实加载路径未确定 |
| 30 | S13 | [Codepage transition](../../../artifacts/mvdm-audit/shard-1/standalone-tool-chain-audit.md) | 自主实现 bug：独立 NTVWM/NTCON Console 代码页未同步；历史真实 DOS 输出复现，当前相关源码仍无传播路径 |
| 31 | S13 | [Original NT quick events](../../../artifacts/mvdm-audit/quick-event-owner-review.md) | 已排除错误候选：原版 NT recipe 选择 qevnt.c 而非 generic quick_ev.c；37 项受控检查通过，真实设备时序另属覆盖边界 |
| 32 | S13 | [IOS and CCPU port owners](../../../artifacts/mvdm-audit/ios-owner-review.md) | 部分已验证：原版 IOS port 拆分/整端口解绑是原始契约，70 项受控检查通过；未发现因此缺失原版接线 |
| 33 | S14 | [Generic thunk](../../../artifacts/mvdm-audit/shard-0/wow-generic-thunk.md); [published-code binding](../../../artifacts/mvdm-audit/shard-3/wow-generic-thunk-published-binding.md) | 缺失原版接线：当前 i386 分支为 #error，WOW32 构建生成器仅从 sources 收集 C 文件；原汇编 helper 未选入，历史测试有调用约定不匹配失败 |
| 34 | S14 | [WOW file cache failure](../../../artifacts/mvdm-audit/shard-1/wow-file-cache-image-and-test-design.md); [flush tail](../../../artifacts/mvdm-audit/shard-1/wow-file-cache-flush-review.md) | 原版代码问题：当前映射缓存尾节点仍被跳过；句柄复用和资源后果已有受控复现，真实 guest 未证实 |
| 35 | S14 | [WOW shell, hooks and subclassing](../../../artifacts/mvdm-audit/shard-0/wow-shell-hook-image-review.md) | 部分已接入：原版 shell/hook/subclass 函数有历史镜像绑定；真实 callback 嵌套与退出未证明 |
| 36 | S14 | [WOW cross-language constants](../../../artifacts/mvdm-audit/shard-1/wow-cross-language-constants-review.md) | 已核销常量不匹配假设：当前 C 头 52 个 WOWCF 与 455 个 FUN 定义值均与原版相同，历史审计也核对原版 ASM；guest ABI 其他部分未由此证明 |
| 37 | S14 | [WOW current directory](../../../artifacts/mvdm-audit/shard-2/wow-directory-sync-review.md) | 已排除整条目录接线缺失：当前 sources 保留原版 wdos.c 和任务目录同步；原版失败次序与真实 Win16 运行覆盖仍需区分 |
| 38 | S14 | [WINOLDAP guest module](../../../artifacts/mvdm-audit/shard-1/guest-winoldap-owner-review.md) | 部分已验证：O:/w31/SYSTEM 有另一版本 WINOLDAP.MOD；T445 在测试包用原版控制文件验证了 Win16/native/Win16 返回，实际加载身份仍未确定 |
| 39 | S14 | [SYSTEM.INI profile](../../../artifacts/mvdm-audit/shard-2/wow-kernel-profile-module-review.md); [language thunks](../../../artifacts/mvdm-audit/shard-1/wow-language-thunk-owner-audit.md) | 已排除整条语言接线缺失：当前 sources 选中原版 wulang.c；history 记录入口绑定；profile shadow 错误风险单列 56，DBCS/失败未证明 bug |
| 40 | S14 | [DDEML](../../../artifacts/mvdm-audit/shard-2/ddeml-owner-boundary.md); [WINMM callback](../../../artifacts/mvdm-audit/external-winmm-callback-boundary.md) | 部分已完成：T465 已实测经典 WM_DDE_* 正常会话和任务退出，T466 已证明同步 WINMM 桥；DDEML guest 实际 owner、异步 WINMM 设备未证实 |
| 41 | S14 | [Original global/local heap declarations](../../../artifacts/mvdm-audit/host-ch-scope/wow-four-excluded-owner-recheck.md); [GLOBALFREEALL guest owner](../../../artifacts/mvdm-audit/host-ch-scope/globalfreeall-guest-owner-review.md) | 已核销误判：原版 guest kernel 自有 GlobalFreeAll，历史 host GLOBALFREEALL 原就 UNIMPLEMENTEDAPI；不能因 host slot 未实现断言缺失 guest cleanup |
| 42 | S14 | [WOW virtual-memory owner](../../../artifacts/mvdm-audit/shard-1/wow-virtual-memory-review.md) | 原版代码问题候选：原始 VDM 分配器标志判断/失败回滚需限定调用者，真实产品故障未证实 |
| 43 | S14 | [WOW parameter-map lifecycle](../../../artifacts/mvdm-audit/shard-1/wow-param-map-review.md) | 原版代码风险候选：原版与当前均按 guest 指针共享节点、仅首任务拥有并按任务清理；跨任务同时借用是否合法/可达尚未证实，DIV-347 的已修回滚不重复列出 |
| 44 | S14/S15 | [WRITE image identity](../../../artifacts/mvdm-audit/shard-2/win16-write-guest-review.md); [OLE server image](../../../artifacts/mvdm-audit/shard-0/ole-server-image-review.md) | 部分已完成：T475 S5 已在真实 Win16 WRITE 连续两次编辑、保存和退出并验证输出文件；OLE server 实际身份/路径及其他功能仍未核对 |
| 45 | S17 | [PIFEDIT and FORCEDOS](../../../artifacts/mvdm-audit/shard-1/pifedit-forcedos-family-review.md) | 范围外/重复：PIFEDIT 和 FORCEDOS 是独立工具/载荷，不作为 S1 审计对象；若其调用暴露 COMMAND host 标志缺口，见 20 |
| 46 | S15/S16 | [Raw spool and shared handle carrier](../../../artifacts/mvdm-audit/shard-1/wow-spool-carrier-review.md) | 部分已完成/原版限制：T468 已验证正常 HDC 打印及对话框；原版 raw profile/provider 不可用和突发任务退出限制由用户接受，异常所有权见 58 |
| 47 | S15 | [EDIT tab-stop return](../../../artifacts/mvdm-audit/shard-2/wow-edit-tabstop-unthunk-review.md); [bounded repair design](../../../artifacts/mvdm-audit/host-ch-scope/wow-edit-tabstop-repair-design.md) | 原版代码问题：当前返回处理仍发生 fallthrough；此前对象与 Win16 观察需重新绑定当前产物 |
| 48 | S15 | [Clipboard formats](../../../artifacts/mvdm-audit/shard-2/wow-clipboard-format-failure-review.md); caret finding in [cross-shard findings](../../../artifacts/mvdm-audit/findings.md) | 部分已解决：T460 已验证文本/DIB 的受限场景；其他 clipboard 格式及 caret 仍缺实际证据 |
| 49 | S15 | [WOW DIB provider lifecycle](../../../artifacts/mvdm-audit/shard-3/wow-dib-provider-lifecycle-review.md) | 原版代码风险：当前/原版在 DIB selector 设置成功后若登记失败走清理，未见 selector 恢复且用 CloseHandle 处理位图；真实失败与泄漏未证明 |
| 50 | S15 | [DIB selected mapping correction](../../../artifacts/mvdm-audit/shard-3/wow-dib-selected-mapping-provider-review.md); [descriptor retirement](../../../artifacts/mvdm-audit/shard-3/wow-dib-descriptor-retirement-review.md) | 原版代码风险：DIB 映射的原版 provider 已接入；当前与原版销毁路径忽略 selector 回调结果后继续卸载映射，失败/重入触发未证明 |
| 51 | S15 | [WOW font query output boundaries](../../../artifacts/mvdm-audit/shard-3/wow-gdi31-font-output-review.md) | 原版代码风险：font/glyph 失败或短返回仍按请求量复制；kerning 一元素映射写多元素；实际 host/guest 输出影响未证明 |
| 52 | S15 | [WOW keyboard thunks](../../../artifacts/mvdm-audit/shard-3/wow-keyboard-thunks-lifecycle-review.md); [driver build inputs](../../../artifacts/mvdm-audit/shard-3/wow-keyboard-driver-build-role-review.md) | 部分已接入：当前 wkbman.c 与历史审计所记哈希一致；真实 guest 输入、DBCS、失败和退出待验证 |
| 53 | S15 | [Selected metafile versus dual-purpose reference source](../../../artifacts/mvdm-audit/wow16-extension-source-role-review.md) | 已核销误判：当前 sources 选择原版 wgmeta.c，GET/SETMETAFILEBITS 仍为 guest LOCALAPI；T456 验证有限 host 路径，guest callback 边界另列待验证 |
| 54 | S16 | [WOWFAX](../../../artifacts/mvdm-audit/shard-1/wowfax-trace.md); [multimedia alias selection](../../../artifacts/mvdm-audit/shard-3/wow-multimedia-alias-consumer-followup.md); [bridge image binding](../../../artifacts/mvdm-audit/shard-0/wow-media-resource-image-review.md) | 部分已完成：T466 已验证原版 timeGetTime 桥及 11 个入口绑定，并明确不恢复原本 #if 0 转换代码；真实 wave/MIDI/MCI/fax 设备及任务退出未验证 |
| 55 | S16 | [WOW palette conversion](../../../artifacts/mvdm-audit/shard-0/wow-palette-image-review.md) | 原版代码问题：当前按请求数而非实际返回数复制调色板；短返回宿主及 thunk 受控证据，guest 暴露未证实 |
| 56 | S16 | [Embedding profile and classes mapping](../../../artifacts/mvdm-audit/shard-1/wow-embedding-review.md) | 原版代码风险与自主 provider 交界：原版枚举仅以 NO_MORE_ITEMS 退出，当前项目 provider 可返回 INVALID_HANDLE；持续失败时源码会循环，真实触发未证实；guest buffer 子项未定 |
| 57 | S16 | [WOW fax client](../../../artifacts/mvdm-audit/shard-1/wow-fax-owner-review.md) | 原版代码风险：当前传真分配后先清零再检查 NULL 等路径仍存在；fax 提供者与实际失败未确认 |
| 58 | S16 | [WOW printer setup and spool failure paths](../../../artifacts/mvdm-audit/shard-3/wow-spool-loader-callers-review.md) | 原版代码风险：ClosePrinter 失败返回非零错误时仍可执行 FreePrn；T468 已验证正常 HDC/对话框但明确未验证 raw provider 失败 |
| 59 | S17 | [DLC completion event](../../../artifacts/mvdm-audit/shard-1/vdmredir-dlc-completion-event-audit.md) | 原版代码问题：当前第二事件失败检查仍指向第一个事件；DLC 提供者与故障触发未证实 |
| 60 | S17 | [DLC event/post owner](../../../artifacts/mvdm-audit/shard-0/redir-dlc-post-image-review.md) | 已核销整体未接入假设：原版 DLC 事件与 post 对象有历史发布绑定，当前 dispatch 保留 DLC 槽；真实 provider 和异步退出仍未验证 |
| 61 | S17 | [MSCDEX guest/VDD boundary](../../../artifacts/mvdm-audit/shard-0/mscdex-package-review.md); [build input roles](../../../artifacts/mvdm-audit/shard-3/mscdex-guest-vdd-input-role-review.md) | 只保留 host 侧问题：原版 RegisterVDD/宿主 loader 和光驱调用链是否可用待证；VCDEX guest 环境位置及安装不是 S1 独立任务 |
| 62 | S17 | [Redirector asynchronous completion](../../../artifacts/mvdm-audit/shard-3/redir-async-role-review.md) | 原版代码风险：当前异步队列分配失败后仍可能发送中断、退出缺少明显本地 drain；尚无真实触发证明 |
| 63 | S17 | [Redirector dispatch](../../../artifacts/mvdm-audit/shard-0/redir-dispatch-release-review.md) | 已核销整体未接入假设：原版 50 槽有历史发布绑定，当前表保留服务；远程 RAP 与 DLC 等具体限制分别见 26/59/60 |
| 64 | S17 | [VDMREDIR image identity](../../../artifacts/mvdm-audit/shard-0/vdmredir-package-identity-review.md) | 已核销产品不一致：主工作树 T477 P4 提交 6db66ab22 发布后与安装目录哈希一致；本审计树保留原基线，旧快照差异不是代码 bug |
| 65 | S17 | [WOW Winsock asynchronous database requests](../../../artifacts/mvdm-audit/shard-1/wow-winsock-database-review.md) | 自主实现风险：T467 已修复分配检查、显式取消及任务关联并通过 guest 测试；当前任务退出仍忽略取消结果再释放 buffer，失败竞态未证实 |
| 66 | S17 | [BDE extension](../../../artifacts/mvdm-audit/shard-0/bde-debugger-extension-audit.md) | 范围外：BDE 为独立 NTSD 调试扩展，不属于 mvdm-host 的 DOS/WOW 运行链；除非证明其为宿主所需依赖，不作为 S1 接入目标 |
| 67 | S17 | Registry identifiers and selected-versus-linked reconciliation in [cross-shard findings](../../../artifacts/mvdm-audit/findings.md) | 非运行时归因问题：历史 registry 标识和链接选择文档有冲突；当前记录一致性仍需核对 |
| 68 | S17 | [Win16 TOOLHELP class thunks](../../../artifacts/mvdm-audit/shard-2/win16-toolhelp-guest-review.md) | 已核销为原版限制：T470 实际 guest 测试确认 ClassFirst 为原始 UNIMPLEMENTEDAPI 结果，用户已接受；非本项目缺失原版接线 |
| 69 | S18 | [Unused C-VID test-provider scaffold](../../../artifacts/mvdm-audit/cvidc-inert-scaffold-review.md) | 非功能 bug：仍有未选入生产的测试桩生成规则；历史图无消费者，当前构建图仍需复核 |

## 用户范围澄清

S1 审计目标为 mvdm-host 及其必要的宿主接线。Guest 内容（包括镜像代码、文件身份、是否打包或默认配置）只可用于判定或验证一个可定位的 mvdm-host 能力或缺失接线；不再单独建立 guest 审计、guest 载荷、guest 配置或 guest 修复任务。17、19 从宿主接线候选中移出；45 的独立工具和 66 的独立 debugger 扩展不作为宿主运行链任务。61 只保留宿主 VDD 加载和可定位能力问题；20 只保留 host 侧标志/原版 COMMAND 消费链。历史包路径和哈希观察保留为证据，不是 guest 工作授权。

## 分类规则

“原版代码问题”指对应问题存在于原始实现，且本次检查的现行源码仍保留该问题；不等于真实 guest 已触发。“自主实现问题候选”指项目自有承接代码的局部缺陷或边界；“缺失原版接线候选”指既有原始路径或提供者未被当前链路承接。历史构建/发布映射与当前源码是不同证据，未绑定当前产物时明确保留限制。未证实的行为不得归责。

## 当前直接核对

- 66：固定 OpenNT `base/mvdm/bde/sources` 将 BDE 定义为 `ntsd/ntsdexts` 下的动态链接目标，选择 `bdestack.c` 和 `bdeutil.c`。当前 `src`、构建脚本和已查 ZIP/安装目录未见该扩展；这是**原版独立调试工具未纳入**，不是正常 DOS/WOW 主执行链的故障或既有 debugger 所有能力都缺失。是否要提供独立 NTSD 扩展、现代 debugger 依赖是否可用，仍需产品范围和可行性决定。


- 17、19：当前 ZIP 与 `O:/winnt/system32`、`O:/w31/SYSTEM` 均未找到 ANSI.SYS、APPEND.EXE、NLSFUNC.EXE，ZIP 默认 CONFIG.NT/AUTOEXEC.NT 无对应启用行。工作树保留只读 guest 输入：ANSI.SYS `c929c0893bba4c6454632d3408ee4f7661b51cf5c2ce20035dcd4283cd623c85`、APPEND `73d6928246f369aac5de742e149196bf4dc3b5801a6effeed6d014e52841d0ac`、NLSFUNC `a49e8c6c392f0fd323b52c1fa612420a9fc0258ba59413ccb3d2d19b1f229ee8`；后两项历史审计标为 OpenNT-4.5 来源，不能未经产品/来源准入替换或复制。三个是可选 guest 载荷范围候选，不是已证明的 host 源码接线 bug，也不能因默认不加载宣称工作中功能坏掉。原 guest 只读且不可重建/修改。
- 50：核销“整个原始 Vdm 映射未接”的误判后，仍有独立失败路径：当前与 OpenNT 原版 `wdib.c::W32CheckAndFreeDibSectionInfo` 都未检验 `CallBack16(RET_SETDIBSEL)` 的结果，就执行 `VdmRemoveVirtualMemory`、删除 bitmap 并释放记录。若回调失败/重入导致 selector 未退休，则可能留下失效引用；[selector 生命周期审计](../../../artifacts/mvdm-audit/shard-3/wow-dib-descriptor-retirement-review.md)没有证明该故障在真实 guest 发生，不能把原始失败路径误标已解决。


- 18、38：当前 `O:/w31/SYSTEM/WIN87EM.DLL` 的 SHA-256 为 `30d6b965d45eddf306268c9526b01494df4841b8b530bab2654c379e1cf49db2`，与 ZIP、原版清单一致；不能因 `O:/winnt/system32` 没有它就认定原版组件缺失。该 loader 分支只针对特定 Win2 版本/依赖，实际 guest load 尚未观察。`O:/w31/SYSTEM/WINOLDAP.MOD` 存在但 SHA-256 为 `80657da18efe1f24c82d8ee9e33f3f99e9b131e0cb0c324089d6ea6f3affbbcd`，不是历史 NT4 原版控制文件的 `cc9c8bc27e11843c6739d0798efa2ac41c4ef71a6a34b68f1fec4a8`；文件存在不能证明运行时选用了哪个版本。T445 S6 用原版控制文件验证六个 Win16/native/Win16 返回场景，结果见 [记录](m0-t445-s6-real-chains.md)。
- 20、45：当前 `assets/winnt.zip` 含 `WINNT/SYSTEM32/FORCEDOS.EXE`（50,176 字节，SHA-256 `5e93e8b89865d611391b25c6bf451cfa63f07d6643bb422da36542e2e554d320`）；旧报告对 ZIP 没有该名称的观察已过时。它不在所查 `O:/winnt/system32` 或 `O:/w31/SYSTEM`，也没有由此证明 CREATE_FORCEDOS 进入当前 Run16/NTSRV 链。当前 ZIP 和两处目录未发现 PIFEDIT.EXE；它是独立编辑器，不等于已有 PIF 数据格式支持，产品是否要求该 UI 仍未确认。
- 50：[DIB emulated-address 复核](../../../artifacts/mvdm-audit/shard-3/wow-dib-selected-mapping-provider-review.md)纠正 T455 早期的“x86 直接指针”描述：历史选中的 x86 WOW 构建实际没有定义 `i386`，使用并链接原版 VdmAddVirtualMemory/VdmAddDescriptorMapping/VdmRemoveVirtualMemory。当前 `wdib.c` 保留非 i386 分支，而真正 i386 分支已按条件排除；单凭 host x86 不能断言应重新启用 i386 分支。descriptor 更新/失败清理不由此自动证明完整，相关具体风险见 49。


- 16：[DOS 命令 85 个输入的既有角色审计](../../../artifacts/mvdm-audit/dos-command-input-review.md)已按原始 link 响应、汇编 include 和配方核销；旧 DIR 明确被原版 `dir.asm` 取代，旧 message bindings 已由原版消息服务或本地 ID 取代。不要把不编进宿主的 guest 构建文件当成缺失宿主接线；可选 guest 可执行文件另见 17、19。
- 30：当前 NTVWM `console_state.c` 读取本机 input/output code page，但 `src/ntvwm-exe` 未有 setter 或将它们送给 NTCON 的后续消费；NTCON `console_frontend.c` 返回自身 Console 的 code page。既有[隔离实际 COMMAND→native CUI→DOS guest 实验](../../../artifacts/mvdm-audit/shard-1/standalone-tool-chain-audit.md)将隐藏 Console 从 437 改为 850，前端仍为 437；后续 DOS `INT 21h` 输出字节 9B 呈现 U+00A2 而非 CP850 的 U+00F8。它证明该旧测试包的具体跨 carrier 差异，当前代码路径仍有同样未传播结构；未重新运行当前包，不能推广到所有输出方式或所有 Console 生命周期。
- 36：本次直接对照固定 OpenNT 与当前 `inc/wowcmpat.h`、`inc/wowusr.h` 中 `#define` 的首个值 token：52 个 WOWCF/WOWCFEX、455 个 FUN 名称均相同。既有 [ASM/C 原版常量审计](../../../artifacts/mvdm-audit/shard-1/wow-cross-language-constants-review.md)对应数值也匹配。此处只核销常量数值不一致假设，不声称 guest 结构打包、镜像或全部执行已验证。
- 37、39：当前 `wow32/sources` 含 `wdos.c`、`wulang.c`，其原版目录和 ANSI 字符转换入口在现存源码中，原先发现主要是测试缺口；不能将它们当成整条原版 thunk 未接入。`wdos.c` 失败时可能部分发布的顺序是历史原版契约，未证明 bug；profile/registry 的具体失败风险另见 56。
- 43：当前与固定 OpenNT 原版 `wow32/wparam.c` 均以 16 位 guest 指针查找既有 node 并递增引用，仅首次创建时登记 `htask16`；`FreeParamMap` 按首次任务清理。若同一指针允许跨任务活跃借用，则首任务退出可能影响另一任务；这种借用是否合法及实际可达仍未证明。项目 DIV-347 已修 NULL/刷新回滚，不能把保留的原始 owner 规则误判为 DIV-347 新引入。


- 12：T430 S6 的 [memory/transition 收口](m0-t430-s6-memory-transition-boundaries.md)已修复项目 lease 跨上下文错误释放，45 项断言通过，并保留 DPMI/XMS/EMS 代表证据。该已修项目 bug 从待办排除；原版 `buffer.c`/`savestat.c` 的其他故障条件没有因此自动证明有 bug。
- 22：当前 `xms.486/xmsdisp.c` 的 XMSDispatch 索引只在 `#if DBG` 中检查，当前 debug 分支被 `#error` 排除，release 直接调用 `apfnXMSSvc[iSvc]`；固定 OpenNT 原版的检查同样仅在 DBG。原版 dispatch 上游和正常 guest 是否能给出非法号尚未证明；不把移除 debug 分支误判成 release 新增缺陷。零长度分配另见 10。
- 23：当前 `xms.486/sources` 选择 `xmsa20.c`/`xmsumb.c`，`xmsdisp.c` 保留 A20、UMB 初始化/申请/释放的原版分发；[原始 owner 与历史绑定](../../../artifacts/mvdm-audit/shard-0/xms-runtime-owner-review.md)排除“整组没接入”的假设，保留未验证的 guest A20 状态指针和 teardown。
- 25：当前 `vdmredir/vrdisp.c` 的 0x28 是 `VrVdmWindowInit`；[guest 路径审计](../../../artifacts/mvdm-audit/shard-3/dlc-window-role-review.md)看到已审 guest 的初始化调用被注释。保留外部 TSR 和 pointer 生命周期不确定性，不把 host 分发缺失作为既定事实。


- 24：OpenNT 原版 `nt_bop.c::SafeLoadLibrary` 本身调用 `LoadLibrary` 并返回模块，且 VDMREDIR 失败清理同样使用 `CloseHandle`；当前项目 `mvdm_softpc_load_library` 也返回 `LoadLibraryA/W` 的模块。因此此处的关闭 API 不匹配是沿用的**原版代码风险**，而非仅因项目适配器才引入。缺失导出故障注入和真实资源释放仍未验证。
- 41：当前 `wktbl2.h` 将 GLOBALFREEALL 标为 UNIMPLEMENTEDAPI 与原版相同；[原版 guest owner 和 ZIP 中 guest 导出](../../../artifacts/mvdm-audit/host-ch-scope/globalfreeall-guest-owner-review.md)是另一条直接路径。因此“host slot 未实现等于整个 guest GlobalFreeAll 缺失”已排除；guest 所有可能退出条件未由此证明。
- 53：当前 `wow32/sources` 选择 `wgmeta.c`，`wgtbl2.h` 把 GET/SETMETAFILEBITS 标为 LOCALAPI；[T456 原 owner 审计](m0-t456-s3-metafile-owner-host-audit.md)已证明有限 public-host 路径。不要把历史双用途备选源的未选中视为需恢复的缺失接线；guest callback 可变/失败路径仍未验证。
- 60、63：当前 `vdmredir/vrdisp.c` 仍将 0x26/0x27/0x28 分派到原 async/DLC/window 所有者；[50 槽原版目标绑定](../../../artifacts/mvdm-audit/shard-0/redir-dispatch-release-review.md)和 [DLC 事件对象绑定](../../../artifacts/mvdm-audit/shard-0/redir-dlc-post-image-review.md)排除“整个 redirector/DLC 分发表未接入”的误判。未证实 DLC provider、网络语义或所有槽都可用，RAP 等单项限制另列。


- 14：T471 S3 的 [异常路径实际 guest 证据](m0-t471-s3-progress.md)已完成 DOS COMMAND 启动 WINMINE、准确终止 WOW worker、返回 DOS、执行 MEM/EXIT；T477 S2 P3 又验证真实嵌套 CMD/DOS 输入、父返回和退出 23。上述曾提出的具体场景已核销，不把它们反复作为待修复问题；这不是所有可能异常路径的证明。
- 40：T465 [经典 DDE 验收](m0-t465-s4-dde-acceptance.md)包括两个真实 Win16 任务与 native peer 的消息及任务退出；T466 已证明同步 timer 桥。原版 `wddeml32.c` 与 `wddetbl2.h` 不在当前镜像选中路径，但原版 guest 也有自己的 DDEML；必须先证明真实 guest 调用哪个 owner，不能从未选择 host thunk 推断缺失，也不能把 classic DDE 成功扩展为 DDEML/异步设备通过。


- 46、58：T468 [收口](../../history/m0-t468-closure.md)已完成实际 HDC 打印、对话框正常/取消路径；原始 raw profile/provider 不可用和 abrupt raw-job 限制明确保留，不应当成未实现替代品。当前与 OpenNT `wspool.c::WG32CloseJob` 相同：ClosePrinter 返回 FALSE 时把非零 GetLastError 存入 ul，随后 `if (ul) FreePrn`；[T468 修正说明](m0-t468-s3-printing-retirement.md)保留 provider 真正的错误/句柄所有权未复现。正常路径已核销，错误路径列为原版风险。
- 54：T466 [收口](../../history/m0-t466-closure.md)验证 original `timeGetTime` 五参数桥和 11 个入口，不应再要求恢复原本关闭的旧转换器；物理多媒体设备请求并未由这项交付证明。
- 65：T467 [收口](../../history/m0-t467-closure.md)完成已有源级三项修复以及 guest async completion/cancel/task-loss/async-select，正常已验证部分排除；本次留下独立的 provider 取消失败后 buffer 是否仍被写入的风险，尚无 race 复现。
- 68：T470 [guest 实测](m0-t470-s2-toolhelp-query-disposition.md)确认 ClassFirst 返回原始不可用结果，且 [T470 收口](../../history/m0-t470-closure.md)将其作为保留的原版 guest/WOW 限制；没有证据表明这里遗漏了一条本项目已有原版接线。


- 31：当前原版 `softpc.new/base/system/sources` 选择 `qevnt.c`，不选择 generic `quick_ev.c`；[NT quick-event 所有者与 37 项受控检查](../../../artifacts/mvdm-audit/quick-event-owner-review.md)排除了“应该恢复通用 quick-event 实现”的历史误判。真实设备时序覆盖不足不自动构成缺陷。
- 32：当前原版 `softpc.new/base/support/sources` 选择 `ios.c`；[IOS 对象证据](../../../artifacts/mvdm-audit/ios-owner-review.md)确认 word 拆分、整端口解绑及 callback 快照属于原 NT 契约，不能当成项目自主替代 bug。未测设备路径另行保留。
- 44：T475 S5 的 [WRITE 编辑保存证据](m0-t475-s5-process-hook-import.md#write-edit-and-save-comparison)在实际 Win16 guest 的两个会话中验证编辑、保存文件、关闭和重启；该有限功能已解决，不覆盖 WRITE 所有功能、OLE server 镜像或本任务全部包身份。
- 49：当前 `wow32/wdib.c::W32HandleDibDrv` 在 `RET_SETDIBSEL` 成功后再登记 DIB；若登记失败，finally 没有明显恢复 selector，并对 `hbm` 调用 `CloseHandle`。固定原版有同样语句。须验证实际 callback 和失败时资源，不宣称已经发生运行时泄漏。
- 51：当前与原版 `wgdi31.c` 的 `WG32GetKerningPairs` 都以请求数而非返回数调用 `putkerningpairs16`；当前与原版 `wstruc.c` 都仅按一个 `KERNINGPAIR16` 长度取得/flush 指针，却循环写 `c` 个元素。`GetFontData`/`GetGlyphOutline` 也按请求长度回写。映射实现及短返回失败是否导致 guest 影响尚待证据，不能仅据源码断言损坏。


- 05：历史 [B067/B068 实际选择与源码分析](../../../artifacts/mvdm-audit/s11-foundation-evidence/b068-selected-boundary/review.md)确认原版及选中路径中 word/dword fallback 返回窄化为 IU8；仅在特定 fallback 条件下成立，并按已有边界排除独立 CPU 内部修复。外部 guest provider 与实际可达性仍未知。
- 09：T430 S5 的[目录重置交付](m0-t430-s5-dem-directory-reset.md)已验证原版慢速 fallback 与实际 guest FCB 常规行为；无生产绑定的内核 VdmQueryDirectoryFile 索引快路径是性能/能力边界而非普通 FCB 缺失。原始删除记忆名的限制和其他失败路径不能当作已解决，且不能直接把 NT 内核 IRP 当作用户态函数接入。
- 56：原版和当前 `wow32/mapembed.c` 的 `UpdateEmbeddingAllKeys` 均仅在 `ERROR_NO_MORE_ITEMS` 时停止。项目 `wow_registry_enum_key` 在父 provider 不可用时返回 `ERROR_INVALID_HANDLE`；持续返回此错误时循环不能按该条件终止，是原版代码与当前承接相遇的源级风险，尚未证明生产条件会持续发生。其他 buffer 写入子项不能混同为同一 bug。


- 54：当前 `wow32/wmmstru2.c` 和 `wmmedia1.c` 的 `#error` 位于 `#if 0` 内；固定 OpenNT 原版对应大分支也位于 `#if 0`。不能仅因为内容被替换就恢复这些历史禁用的 provider。历史证据表明当前还有独立的 `WMM32CallProc32` 桥；MCI、wave、MIDI 与传真能力是否完整仍需分别验证。


- 14：T477 S2 P3 的 [Full 记录](m0-t477-s2-frontend-instances.md)报告真实 nested CMD/DOS input、parent return 和直接退出 23 通过，故这几个历史正向样例已有交付证据；这并不覆盖异常退出和 T476/T477 合并后新包。
- 24：当前 `nt_bop.c` 和 OpenNT 原版均有 `CloseHandle(hVdmRedir)`；当前 `mvdm_softpc_firmware.c` 的 `SafeLoadLibrary` 承接使用 `LoadLibraryW`。这是原版调用与当前适配器之间的资源语义风险，尚未实测缺失导出的实际模块引用和全部回调清理。
- 64：本次只读快照中本工作树 `assets/release/VDMREDIR.DLL` 为 `71340bdf011ba016feb70423ece4e64d2f6b8a25f3df0b4c3c748416e31741b4`，`O:/winnt/system32/VDMREDIR.DLL` 为 `a1aebaabbb4e777ff8d24f25854e5240d62db78b5fa0194253fab05f2870e0f0`；本工作树无对应 Ninja 缓存文件。该快照随后被主工作树 T477 P4 交付取代：只读复核主工作树提交 `6db66ab22` 后，其 `assets/release/VDMREDIR.DLL` 与安装目录均为 `a1aebaabbb4e777ff8d24f25854e5240d62db78b5fa0194253fab05f2870e0f0`，集成 P4 报告发布哈希一致。此审计树仍在原始基线 5ec7351c9，并非同一包；未写入、rebase 或干预主工作树。


- 17、18、19、38、61、68：只读检查当前 `assets/winnt.zip` 文件名和 `O:/winnt/system32` 对应路径。ZIP 不含 ANSI.SYS、APPEND.EXE、NLSFUNC.EXE 或 WINOLDAP.MOD；含 WIN87EM.DLL、MSCDEXNT.EXE、TOOLHELP.DLL 及 VCDEX.DLL。所查安装目录缺前述文件及 WIN87EM、WINOLDAP、VCDEX、TOOLHELP，含 MSCDEXNT。此为精确目录/归档观察，不能推断其他 guest 路径、实际加载或整个功能缺失。
- 61 历史报告称 ZIP 不含 VCDEX 的结论已被当前归档推翻。当前 ZIP VCDEX SHA-256 `3679db6245b7717d3e58c19b19097c8031d6c5a94df773b5c01e5ca7ed98ebde`，与 `src/mvdm-host/vdd/samples/mscdex/vdd/obj/i386/vcdex.dll` 一致；ZIP 与安装目录的 MSCDEXNT 同为 `b3151a2dfe4a67a6c2e6113071d1f1c713bb78a04adea986fe01540eb96762d0`。安装路径、搜索、ABI 与真实光驱请求仍未验证。
- 68：当前 `src/mvdm-host/wow32/wthtbl2.h` 仍将 CLASSFIRST 和 CLASSNEXT 指向 `UNIMPLEMENTEDAPI`；[历史审计](../../../artifacts/mvdm-audit/shard-2/win16-toolhelp-guest-review.md)显示原版相同，故不能直接判为移植丢失或自主实现 bug；其他 TOOLHELP 入口不可一概而论。


- 10：当前 `src/mvdm-host/xms.486/xmsblock.c` 在 size=0 时跳过 `SAAllocate` 后仍读取 `BaseAddress`；重分配还有类似条件。原版对应代码有相同形态。原始 HIMEM 对相同大小重分配有前置过滤，不可把源级风险说成已复现的 guest 缺陷。
- 15：当前和 OpenNT 原版 `dos/command/cmdkeyb.c` 都有 `szBuf[28]`、`szPrev[5]` 及 `strcpy`。当前来源还经过项目 registry；能否收到可触发的长值尚未证实，不能断言发生溢出。
- 20：当前 `src/mvdm-host/dos/command/cmdmisc.c` 仍消费 `CREATE_FORCEDOS`；对 `src/run16-exe`、`src/common` 和 `src/ntsrv-exe` 的当前源码检索未找到该标志的生产者。历史原版 utility 的完整调用、PIF 和用户入口仍需核实，不能把普通 DOS 启动失败作为结论。
- 29：`src/mvdm-host/softpc.new/host/src/nt_rez.c` 的打开文件过大路径在 `_close` 之前返回；[原始行为与提取函数受控测试](../../../artifacts/mvdm-audit/rom-read-failure-review.md)支持归为原版问题；当前运行时路径仍待验证。
- 57：`src/mvdm-host/wow32/wow32fax.c` 先对 `malloc_w` 返回值做 `RtlZeroMemory`，然后才检查 NULL。其他未初始化返回及资源所有权问题应分开核验；[原始代码和旧发布绑定](../../../artifacts/mvdm-audit/shard-1/wow-fax-owner-review.md)不证明真实 fax 路径可用。
- 62：`src/mvdm-host/vdmredir/vrinit.c` 的队列分配失败直接返回，相关原始中断及退出路径仍需结合 provider 时序确认；[历史审计](../../../artifacts/mvdm-audit/shard-3/redir-async-role-review.md)没有复现实时故障。
- 65：当前 `src/mvdm-host/wow32/wsdata.c` 的 `WWS32CancelAsyncRequestsForTask` 不使用取消调用的返回结果，随后释放 buffer 和 context。该任务退出逻辑属于注册的项目修改；[历史分析](../../../artifacts/mvdm-audit/shard-1/wow-winsock-database-review.md)尚未证明取消失败后仍会有写入或损坏。


- 04：`src/ntvdm-exe/softpc/mvdm_ccpu_vector_defaults.c` 的两个 SAS 备用提供者现在调用 `__debugbreak()`；生成头还声明 `IsPageInstanceData`。原审计看到发布到向量但调用/可达性未知；这是一项项目承接边界，不是证明当前用户发生崩溃。
- 13：`src/ntvdm-exe/command/source/mvdm_command_guest_state.c` 当前仍使用线程局部状态；[FDC 归属分析](../../../artifacts/mvdm-audit/shard-3/fdc-session-single-owner-design.md) 有受控的跨线程证据，实际设备触发和 guest/CPU 同步尚未证明。
- 33：当前 `src/mvdm-host/wow32/wkgthunk.c` 的 `#ifdef i386` 整个分支是 `#error`；固定原版在该分支调用 `WK32ICallProc32MakeCall`，原始 `wow32/i386/sources` 列出 `callpr32.asm`。`tools/build/New-T404S5Wow32ProviderNinja.ps1` 从 `wow32/sources` 收集 77 个 `.c` 文件，源码/构建脚本未检索到该 helper 的其他接入。原 `callpr32.asm` 仍在镜像，当前选择非 i386 的 C 路径。历史实际对象与 guest 对照详见[原始汇编接线分析](../../../artifacts/mvdm-audit/shard-0/wow-generic-thunk.md)和[发布绑定](../../../artifacts/mvdm-audit/shard-3/wow-generic-thunk-published-binding.md)；仍需重建并检查最新实际链接图、处理非易失寄存器 ABI 和覆盖调用约定，不能全局打开 i386 宏。
- 34：`src/mvdm-host/wow32/wkfileio.c` 的 FlushMapFileCaches 仍按 `hpfNext` 非零循环，跳过尾节点；[对象绑定与受控复现](../../../artifacts/mvdm-audit/shard-1/wow-file-cache-flush-review.md)不证明真实用户文件泄漏。
- 55：`src/mvdm-host/wow32/wgpal.c` 的 WG32GetPaletteEntries 仍按请求数量复制；[原版和短返回证据](../../../artifacts/mvdm-audit/shard-0/wow-palette-image-review.md)限定了真实 guest 未验证。
- 59：`src/mvdm-host/vdmredir/vrdlc5c.c` 中第二个 CreateEvent 后仍检查第一个句柄；[原版与历史选择证据](../../../artifacts/mvdm-audit/shard-1/vdmredir-dlc-completion-event-audit.md)没有证明当前 DLC 提供者可到达该路径。
- 26：`src/vdmredir-dll/source/mvdm_redirector_remote_unavailable.c` 的 RxpTransactSmb、RxNetUserPasswordSet 和 GetLanmanSessionKey 返回 ERROR_CALL_NOT_IMPLEMENTED；不代表本地网络和全部重定向不可用。当前源码事实与历史链接映射见[原审计](../../../artifacts/mvdm-audit/shard-3/remote-rap-role-review.md)，仍需绑定本次产物和实际调用。
- 28：`src/mvdm-host/softpc.new/host/src/nt_keycd.c` 的两个 KeyMsgToKeyCode 表边界仍使用 `>`；此前实际对象的哨兵测试证明恰好等于长度时可越界。当前源码仍如此；真实输入和 guest 影响未证明。
- 47：`src/mvdm-host/wow32/wmsgem.c` 的 **UnThunkEMMsg16** 中 EM_SETTABSTOPS 清理后仍落入 EM_GETWORDBREAKPROC；此前修复建议和受控/guest 观察见[设计](../../../artifacts/mvdm-audit/host-ch-scope/wow-edit-tabstop-repair-design.md)。入口 ThunkEMMsg16 的同名 case 自带 break，不能据此认定返回路径已修复。

01、07、08 的“已有交付证据”仅复述原清单中已发布的有限案例，尚未重新绑定本次基线；不能把相关项的所有子问题标为已解决。

## 后续

69 行均有初步分类，但多项仅复核既有记录和当前源码存在，尚未重新绑定完整编译图、最新产物或真实运行；同一行也可能混合已解决和未解决子项。需要逐项完成这些核实，再排除真正完成的部分并形成最终待处理名单。两份原始审计和清单正文另有表外发现，不纳入本次 69 行分母。禁止将初步分类描述为全部事实已验证或 S1 已收口。
