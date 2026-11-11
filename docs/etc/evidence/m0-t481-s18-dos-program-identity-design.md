# T481 S18：NTVDM 程序身份与请求归属设计审查

状态：研究设计，等待用户审核。禁止据此直接实施产品修复。
基线：c223177be。用户进一步要求纯宿主，不依赖 INSIGNIA/NIDDB、
NTVDM64.386 或某一种 guest 操作系统。原版 guest 不修改、不重新编译。

## 证据接收与审计

接收 [侧会话记录](../../../test/observation/dos-identity-probe.md) 及其六个未提交
测试文件，原样保留。逐项核对 build/side-dos-identity/findings.json 中两个
case 的证据 SHA256，均匹配；阅读原始事件日志、A/B RUN/END 与 Run16 快照。
没有重复执行客机或修改产品/发布文件。

| DOSONLY | Worker | 相同 PSP | 相同 CR3 | A/B 物理 PSP | 错配 |
| --- | --- | --- | --- | --- | --- |
| 1 | 2336 | 10DF | 001DC000 | 00292DF0 / 00333DF0 | A=5 被 B=7 覆盖；A 退出选中 7，B 退出无槽 |
| 0 | 8208 | 10DB | 001DC000 | 00292DB0 / 00333DB0 | 相同；A 返回后 B 的 Run16 退出，A 仍等待 |

两个独立 DOS fixture 自报 VM 2/3。这个 VM ID 仅是证据，不能变成产品的
Win3.1 专用识别协议。角色名不参与 observer 匹配。END 在 INT21/4C 前写出，
所以 END 单独不是终止证明；本次另有 VDD terminate 事件。创建/终止和错误
等待者的证据成立，Windows 正常关闭未成立，最终由原 runner 超时清理。
探针 xtrn2phy 可能影响转换缓存，不能宣称完全无扰动。

探针审查限制：prepare 脚本依赖当前 Ninja 文本格式和历史候选目录；运行器
共享 events.log/package，不能并行运行；两秒等待是观察延迟而非退出证明；
分析器证明具体错配，不验证不同退出码；控制程序关闭失败仍需在未来正式
回归中修正。这些文件目前是已审阅的复现材料，不是通过的发布门禁。

## 原版与现实现的责任链

1. `mvdm-guest/dos/v86/doskrnl/dos/msproc.asm::Xfer_To_User` 及 HMA 的
   `inc/lmstub.asm::disa20_xfer`：原版 EXEC 在进入用户程序前发
   SVC_DEMENTRYDOSAPP；DX 是 PSP，DS:SI/AX:DI 是入口/初始栈。
2. `mvdm-host/dos/dem/demmisc.c::demEntryDosApp`：原版
   VDDCreateUserHook(PDB)。`demsrch.c::demTerminatePDB` 从 BX 取退出 PSP，
   调 VDDTerminateUserHook。普通退出在 `msctrlc.asm` 的 arena_free_process
   之前通知；Exit_keep_process 跳过该通知。
3. `ntvdm-exe/softpc/ntvdm_dos_observer.c`：自主 active_psp 仅匹配 PSP；
   created 覆盖同 PSP 活动槽；父关系同样只按 PSP；direct 取 worker 全局值。
4. `ntsrv-exe/opennt/source/task_observation.c`：按 occurrence 查找退出记录，
   不是栈 pop。但首个 ENTER 被折入尚无 occurrence 的 DIRECT root；这也是
   必须删除的隐式创建关联假设，不能只修 active_psp。
5. `mvdm-host/dos/command/cmdexec.c::cmdReturnExitCode` 与
   `cmdmisc.c::cmdGetNextCmd` 把 COMMAND 返回码交给 GetNextVDMCommand。
   `non-mvdm/base/win32/client/vdm.c` 普通 DOS Get 不设置 iTask（只在
   ASKING_FOR_PIF 使用），所以不能声称现有 iTask 已是 DOS 完成身份。
6. `non-mvdm/base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand` 完成时
   取最后 BUSY，写 ErrorCode 并唤醒父事件；交付时则有确切的 pDOSRecord。
   `management.c::service_dos_delivery_trace` 又扫描最后 BUSY 获取标签。
   `base_rpc_client_launch.c` 把标签存成单一 launch.observation_direct。

第 3 项是项目观察器错误；第 6 项原版单链关联假设不适合当前乱序重入。
保留原版队列/错误码/父事件机制，替换选择记录的依据，不重写完成引擎。

## 已有候选身份：能证明什么

| 候选 | 判定 |
| --- | --- |
| PSP；CR3+PSP | 实测不唯一，拒绝 |
| 名字、父 PSP、环境段、启动顺序 | 不是对象身份；两个 COMMAND 的父/环境也重复，拒绝 |
| 物理 PSP/页框/PTE 地址 | 本次可区分；不含跨重映射、换页、释放复用的生存期保证，不作永久键 |
| occurrence 序号 | 可以表示已经识别的一次执行，不能帮助退出端选择实例；保留现有序号，不另发一套 |
| NIDDB instance 数据对象 | 已有宿主分配/切换/释放，但依赖 guest 通知；按用户要求不作为方案前提 |
| 原版 RDRINFO 宿主对象 | 有既存往返通道，但可空且 guest 会检查它；仅作进一步证明的候选，不直接全面采用 |

目前审查的 DEM/VDD 通知、HostTerminatePDB、COMMAND 返回和通用内存 lease
中，未找到可直接使用的“DOS 分配对象代际 ID”。guest DOS arena 分配不是
宿主 malloc 对象，宿主 lease/内存 epoch 也不是每个程序的分配生存期。
这说明**现有这些接口的信息不足**，不是证明所有纯宿主方案都不可能。
不能因 NIDDB 不活动就认定只有一个上下文，更不能猜 guest OS 类型。

## 纯宿主候选中的关键新发现：RDRINFO

`cmdmisc.c` 将 cmdCheckStandardHandles 返回的对象值写入 CMDINFO.pRdrInfo。
原版 `tcode.asm` 保存 SCS_RDRINFO 到 RES_RDRINFO；下一次 CMDGETNEXTCMD
把它传回 CMDINFO，RETURNEXITCODE 则用 BX:CX 传回。两个完成入口都存在
原样往返能力，不需要改 guest 二进制。

但是 `cmdredir.c::cmdCheckStandardHandles` 在没有标准句柄时返回 NULL。
`tcode.asm::tsr_loop` 会检查 RES_RDRINFO 的两部分，决定是否进入原 TSR
提示符路径。**无条件创建非空完成对象会改变语义，不能作为极简捷径。**
现有对象还会被 cmdCheckCopyForRedirection 释放，所以身份须在原清理前
取出，完成请求与资源释放分别守住一次性规则。不能把 guest 提供的值
不经所属校验直接当宿主指针；不通过字段填充或特定地址取巧保留 TSR 分支。

可研究的最小用法：对确实具有原版对象的交付，在该对象的宿主附属记录中
绑定 NTSRV 已有请求键，通过原往返通道取回；不增 guest 字段、不改变 NULL、
标准句柄掩码和 TSR 判断。不具有该对象的路径仍缺完整解法，不能混入
last-BUSY 兜底。对象存在也尚不证明任意 OBSERVED 子程序可取得它。

## 推荐架构契约（可审核，尚非完整可实施修复）

只有一条程序事实流、一个请求完成权威：

- NTSRV 拥有现有 DIRECT 请求键、请求状态、退出码和 Run16 等待事件。
- NTVDM 拥有宿主内执行关联；原版 create/terminate 提供程序事实，原版
  COMMAND 返回提供请求结果。两者各司其职，不互相代替。
- 原 Get 选定待交付 pDOSRecord 时直接带出它已有的身份，经现有 RPC
  携带，不再事后扫描 BUSY。跨进程只传 NTSRV 标量键，不传对象指针。
- 返回必须携带同一次交付的键，服务端校验 worker generation、所属请求、
  已交付/未完成状态后，沿原版写码/事件路径完成该条。重复或未知返回不
  触及其他请求，NO_PARENT/环境/PIF 查询保持非完成操作。
- OBSERVED 不凭“第一个 ENTER”升级为 DIRECT 实体。只有确证绑定才能
  关联；内部 EXEC 子程序退出只结束其观察实例，不释放宿主请求。
- 现有 observer 槽位改为不会覆盖活动实例的事实存储；已有 occurrence
  继续用作传输身份。真正决定匹配的对象依据必须先证明，不用新编号替代。
- 父关系与 direct 归属使用相同的已证明依据；缺证据则未知，不能用
  ambient direct、最近请求或栈顶补全。异步 outbox 携带已捕获的身份。
- 观察缺口保持 GAP/UNCERTAIN，不误删另一实例。请求身份缺失是控制失败，
  走既有 NTSRV 失败/清理协议；不能伪装成功或无限等待错误匹配。

不新增 VM 管理、VM 枚举、轮询、guest 内存全扫描、第二套完成状态机。
普通 DOS、不同多任务系统走相同接口契约。是否能取得可靠身份按接口
证据判断，不按“里面是不是 Windows”判断。

## 必须保留的语义与尚缺的证明

| 情形 | 契约与缺口 |
| --- | --- |
| 正常/乱序退出 | 明确请求键完成对应等待者；观察实例必须可从退出通知反查 |
| 同名、同 PSP 的活动实例 | 名字/PSP 不能定身份；不覆盖，不选最后一项 |
| PSP 释放后复用 | 确证旧对象终止后才创建新 occurrence；漏退出不能冒充正常复用 |
| 包装程序与内部 EXEC | 子程序终止不是包装请求结束；父关联缺证据不猜 |
| TSR | COMMAND 返回与驻留观察分开；无普通 terminate 不能宣称驻留体消失 |
| worker 整体结束 | 按既有生命周期终止/失联原因处理，不合成每个程序的正常退出码 |
| 观察丢失 | 可以降低观察完整度，不能误完成 DIRECT；这不是自动获批的验收降级 |

纯宿主且不改 guest 的现有限制下，尚未证明一个适用于任意 DOS 多任务
系统的 OBSERVED 稳定键。即使修好 DIRECT token 往返，也不能声称完成了
全部程序身份追踪。需要准确关联与只报告未知之间的取舍必须由用户审核，
不能默默把“未知”写成完整修复。此轮不提出采用基础驱动的替代方案。

## 有限的后续证明清单

1. 仅做诊断：记录两条 COMMAND 完成入口的实际 RDRINFO 及对应交付请求，
   覆盖无句柄/文件/管道、失败 EXEC、批处理和 TSR。证明原对象的分配、保存、
   返回、释放，不改值，不加入产品补丁。确认所有必要路径是否有载体。
2. 沿原版 DOS EXEC/arena/返回对象核对是否存在通知两端均能取得的稳定
   对象契约；对同名同 PSP、页重映射与复用做反例验证。不能仅重复一次
   A/B 成功就把物理地址当生存期 ID；找不到即明确缺口并停止扩展调查。
3. 核对实际创建如何与交付建立关联，包括第二个 COMMAND、包装程序及
   内部 EXEC。不能预设父 PSP 或 worker 当前 direct 正确。

只有这三项闭合后，才能冻结完整实现方案。届时受影响链限定为 observer、
COMMAND 交付/返回适配、common 既有 RPC、BaseSrv 精确选择和观察绑定；
NTMON 仅消费更准确的既有记录。NIDDB/驱动/guest 不进入修改范围。

后续回归固定为 DOSONLY=0/1 × 正序/乱序退出；同名同时运行；同 PSP 不同
实例及释放复用；包装/内部 EXEC；A=41h、B=42h 分别返回正确等待者；TSR
返回/驻留边界；原单链嵌套、错误 EXEC、零句柄、文件和管道不退化。
必须观察相应 Run16 和请求状态，不能仅凭任务总数平衡或界面顺序通过。

## 继续核查：原版载体与分配边界

本轮只读追溯，无产品修改、构建或新运行探针。结论不能作为运行验收。

- `src/non-mvdm/base/win32/client/vdm.c::BaseCheckVDM` 在传递标准句柄时，
  明确把 CONSOLE_HANDLE 清为零；`cmdCheckStandardHandles` 只有至少一个
  非零句柄才分配 RDRINFO。因此空 RDRINFO 是原版正常 Console 路径允许的
  情形，不应当仅作为罕见错误路径处理。当前 common RPC 可能另行映射
  endpoint，仍需运行证据确认具体请求，但这不能建立普遍非空的 ABI 契约。
- `tcode.asm::tsr_loop` 读取 RES_RDRINFO 的两个字，决定是否进入
  tsr_nordr / tsr_ret。原代码并非简单的完整 DWORD 非零检查；不应利用
  低字为零等编码技巧绕过该分支。强制分配一个伪重定向对象不是透明适配。
- `src/mvdm-guest/dos/v86/doskrnl/dos/alloc.asm::$alloc` 直接写 arena_size、
  arena_owner；`arena_free_process` 按 owner PSP 遍历并清除 owner。
  这两个核查路径没有提供宿主分配对象或分配代次。MCB owner 仍然只是 PSP，
  读取 MCB 不能自动解决跨上下文相同 PSP，也不能证明重映射后的连续身份。
- `src/ntvdm-exe/session/guest_memory_lease.c::lease_begin` 才递增 epoch；
  此 epoch 保护 provider 生命周期。它不是每次 DOS EXEC/arena 分配递增，
  不能与 PSP 拼接后冒充程序生命周期键。
- `mvdm_guest_location` 明确只保存 segment/offset，非 opaque mapping ID。
  `pdb.inc` 也未定义给宿主 observer 使用的实例 cookie。文件中的历史
  “Reserved areas are probably safe” 注释不是任意 DOS 软件都不会使用或
  复制这些字节的保证；不能据此擅自写 PSP 保留区。程序复制 PSP 时还需要
  区分新实例与继承旧 cookie，本轮没有授权或证明这种 guest 数据协议。
- observer 的 callback_owner 校验执行线程与 session，保护的是 worker
  归属；多个 guest 程序本来就在同一模拟执行线程中轮流运行，不能把该
  线程 ID 当作额外的程序区分维度。

由此排除的是“已有现成通用对象键”的上述候选，不是宣称纯宿主在理论上
不可能实现。模拟器能看到内存与指令，不等于现有回调已经包含分配、迁移、
复制和销毁的对象语义。若要从任意内存复制推导身份传播，会引入另一套
跟踪机制，超出本任务极简、复用现有机制的目标，当前不推荐。

当前方案不能冻结为“PSP 换成另一个简单 key 即可”。控制侧应采用明确的
交付请求键；但纯宿主如何把这个键无损带过 COMMAND 的所有返回路径，
以及 OBSERVED 如何在地址重映射后反查，仍须分别有证据。二者未闭合前，
不提交声称通用修复的产品补丁，也不静默改变为按程序名、地址或启动次序猜测。

## 宿主分配并随载荷保存 ID：专项调查

所有权上可行：NTSRV 继续签发 DIRECT 请求 ID；OBSERVED 可沿用 worker
现有 occurrence 与 worker generation 的组合。后者是观察实例身份，不另造
一套请求权威。关键不是发号，而是 guest 上下文恢复后原样取得同一个号。

### 已核实的候选落点

| 落点 | 调查结果 |
| --- | --- |
| 只存在宿主表 | 可以保存 ID，但退出仍只有 PSP，未解决选中哪一行 |
| PSP/MCB 物理地址的旁表 | 不修改 guest；只得到地址标签，迁移与内存复用的对象连续性仍未证明 |
| PSP 填充字段 | DOS PDB_PAD1 的 0x42–0x4F 与 Windows/WOW 的有效字段重合，不能通用占用 |
| COMMAND CMDINFO | `cmdsvc.h` 的原版结构没有独立的保留 token 字段；扩大结构不等于未改 guest 就会保存和回传 |
| RDRINFO | 有原版往返，但允许为空且影响 TSR，不是透明通用扩展槽 |
| 环境变量 | EXEC 可继承并复制，子程序可以替换环境；不构成不可混淆的执行实例标记，不改变环境来修此问题 |
| 宿主 shadow metadata 跟随 guest 内存 | 理论研究方向；须定义复制、迁移、清零、释放和复用的标签规则，现有 lease 不具备此语义，不属于已找到的极简复用方案 |

具体冲突证据：

- `src/mvdm-guest/dos/v86/inc/pdb.inc` 的 PDB_PAD1 长 0Eh，位于版本字后、
  PDB_Call_system 前，即 0x42–0x4F。
- `src/mvdm-guest/wow16/kernel31/pdb.inc` 的相同区间依次是 PDB_Chain、
  PDB_Partition、PDB_NextPDB、PDB_GlobalHeap、PDB_Entry_stack，总计 14 字节。
  这是“DOS 标作保留不等于其他 guest 不用”的具体反例；不是声称所有普通
  DOS PSP 都实际使用这些 Windows 字段。
- `doskrnl/dos/misc.asm::CreateCopy` 用 CX=80h、REP MOVSW 复制整个 256 字节
  PDB。新程序在可靠 create 通知中重打 ID 可以解决这条普通复制路径，
  但不能据此证明没有 create 通知的上下文克隆也会获得新 ID；反过来，
  换页复制必须保留 ID，不能把每一次字节复制都当作创建。
- `doskrnl/dos/msproc.asm` 从调用方或父 PSP 取得环境并复制；环境标签会
  继承，因此不能同时充当父请求标签与唯一的子执行实例标识。

若存在一个被明确保留的 per-instance cookie 字段，宿主创建时写入、结束
时经当前 guest 映射读取，确实能避免把物理地址当永久身份，且无需 guest
执行读取 cookie 的新指令。但是当前没有找到这样的原版字段契约。
未经批准往 PSP 保留区写号是新的 guest 数据协议，不属于仅改宿主旁表；
本轮只研究可行性，不实施这些写入。

结论：宿主签发 ID 是正确的所有权设计，但现有结构没有已证明可安全承载
该 ID 的统一位置。不能以“加 ID”宣称问题已解决；同样不能据此推导纯宿主
方案在理论上不可行。当前不推荐 PSP 填充/环境变量/伪重定向对象三个捷径。

## 模拟器映射对象身份核查：第二条路线的结论

范围：现有 CCPU TLB/地址转换、SAS 物理访问、物理绑定/EMS alias、原版
nt_mem 分配/释放与 guest memory lease。仅静态源码核查，没有运行测试，
没有写入 guest 或修改产品代码。

结论：这些现有层没有满足“跨重映射保留程序身份，并区分释放后复用”的
现成对象契约。因此不能将它们的指针、槽位或地址直接作为程序身份键。
这是对复用当前映射机制的否定结论，不是对所有纯宿主设计的不可能性证明。

| 层/实际源码 | 保存的是什么 | 为什么不是执行实例身份 |
| --- | --- | --- |
| `c_tlb.c::TLB_ENTRY`、`lin2phy`、`flush_tlb` | la/pa、有效位、dirty、访问权限 | 4 路 8 项缓存，next_set 循环复用，flush 清有效位；缓存失效不是程序结束 |
| `ccpusas4.c::c_GetPhyAdd` | 当前 guest physical 到当前 host backing 的换算 | 普通 RAM 是 Start_of_M_area + addr；不是带对象生命周期的 handle |
| `mvdm_softpc_physical_mapping.c::physical_mapping_record` | session、整机 normal_base、页数组 | 一个 session 的页绑定，不是一个 DOS 实例；页槽原地写入 backing 地址，无分配代次 |
| 同文件 `physical_alias_record` / `VdmMapDosMemory` | owner、destination、source、size | 同 destination/size 会直接替换 source；alias 对象稳定不代表内容或程序稳定 |
| `nt_mem.c::SECTION_HEADER` | 宿主 Intel 地址区间及 allocated/committed/remap flags | 初始 RAM 整块分配；guest 在其中分配 PSP 不创建对应 section；释放还能合并并删除 header |
| `guest_memory_lease.c` | 一次复制访问的 lease、provider epoch | 生命周期与 DOS 分配/结束不一致，不能用 epoch 修补粒度差异 |

实际换算链为当前 guest 线性地址经 CCPU TLB/页表取得物理地址，然后
SAS 经 alias/外部 backing 或普通 RAM 基址取得宿主地址。此链回答
“现在去哪里读这些字节”，没有回答“这些字节仍然属于哪个执行实例”。

两个源码级反例足以排除直接复用，不需要重新运行整套 Win3.1：

1. 同一 DOS arena 内，A 结束、内存释放、B 复用该 PSP 区间；原版 guest
   可以只更新 MCB/PSP 字节。宿主整机分配、页绑定、lease provider 均可
   保持不变。给这些宿主对象编一个号，不能区分 A 和 B。
2. guest 把实例数据复制/换入另一页，再调整映射；页表/TLB 能呈现新地址，
   但没有携带“这是原实例迁移，而不是新实例的复制”的语义。相同 alias
   记录甚至可原地指向不同 source。地址或映射记录 ID 都不能补出该关系。

第一项有原版 arena 分配/释放源码依据，第二项是接口所允许而没有身份
保证的反例，不冒称已在当前 Win3.1 探针中实际观测到迁移。已保存探针
只证明两次采样的物理 PSP 不同且各自创建/退出一致。

新增映射 generation 只能识别映射变化，不会自动识别程序连续性；保持
全部内存复制的身份传播则是新机制，不是现成映射对象的极简复用。本路线
不推荐进入产品实现，不能用一次同物理地址的成功测试放宽此前身份要求。

## 新实测：宿主等待与 guest 调度不是同一条栈

用户要求实际验证机制。本轮使用当前 S17 candidate-final 包与缓存库构建
独立诊断 NTVDM，只增加原 observer 的线程/寄存器/栈采样；不修改原版
程序或产品源码。复用私有桌面 runner、独立 Win16 控制程序与 A/B DOS
测试程序。原 O:/w31 与正式资产不变。两项 addon 都关闭，DOSONLY=0。

证据目录：`build/t481-s18-scheduling/dos0-fixed`；文件哈希和输入摘要见
其中 mechanism-findings.json。A/B 并非 EDIT/QBASIC 替换版本；本轮验证
两个 DOS VM 的执行机制，不冒称完成字面上的 EDIT/QBASIC 交互验收。

步骤：先 A.RUN，再 B.RUN；只有观察到 B.RUN 后，宿主才分别创建 A.PNG、
B.PNG。两个程序在等待退出信号的循环中，各自读到信号并写 A.LIV/B.LIV。
两份应答都到达后才释放 A.GO；观察 A.END 与 terminate，再释放 B.GO。
因此 LIVE 是“后启动的 B 已存在，较早启动的 A 又实际执行了代码”的见证，
不是仅凭两个进程记录推断并发。

- worker PID 37280；两程序创建/终止回调都在 TID 24460。
- A/B PSP 均为 0DCA，guest 自报 VM ID 为 2/3。
- A/B 创建及退出物理 PSP 分别为 0028FCA0 / 00331CA0；CR3 均为 001DC000。
- A.RUN、B.RUN、A.LIV、B.LIV、A.END、B.END 均取得；两条 terminate 顺序为 A/B。
- 两个内层 Run16 为 A=24164、B=34548。A 退出后 B 的 Run16 消失、A 仍等待，
  重现错误完成配对。END 写在 INT21/4C 之前，实际退出由 terminate 另行佐证。

源码闭环：`cmdExec32` 创建 cmdCreateProcess 辅助线程；该线程等待宿主
子进程（这里是 Run16）。模拟执行线程的 GetNextVDMCommand 收到重入
命令后置 Carry 并返回，**不是在 cmdExec32 的 C 栈帧里递归跑到 DOS 子程序
结束**。`tcode.asm::d16_retback` 根据 Carry 转入取命令/执行；结束后
`d16_return32` 才重新进入 SVC_RETURNEXITCODE。普通阻塞没有可交付重入
命令的情形不可与此混同。

因此有三类不同的状态：宿主辅助线程等待 Run16、Run16 等待 NTSRV 请求、
guest Windows 在同一模拟 CPU 上调度多个 DOS 执行现场。前两者等待不代表
整个模拟器始终卡在最内层程序。一个 COMMAND 内同步 EXEC 的父子关系，
也不能套成不同 guest VM 之间的全局后进先出关系。

限制：第一轮因测试信号使用四字符扩展名（PING/LIVE）超时，已记录在
 dos0-fresh/failure.txt，不算通过；修正为 PNG/LIV 后取得上述见证。
优化构建的 CaptureStackBackTrace 只取得一帧，不能当成完整宿主栈展开证据；
C 返回机制依据实际源码，线程与 A/B 活跃依据实测。测试未声称证明 guest
每个时间片的调度算法或所有 DOS 多任务系统的行为。Windows 整体正常退出
另行记录，不以程序调度见证冒充全量集成通过。

本轮清理结果：控制程序未使 Windows 正常退出，runner 达诊断期限后清理；
不能计作正常关机通过。已核实 O:/tmp/s180 与 runner 包副本 O:/tmp/n82e
均不存在，保留 build 下的诊断输入及证据。
