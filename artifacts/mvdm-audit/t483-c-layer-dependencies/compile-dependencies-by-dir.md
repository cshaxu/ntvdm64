# C 层直接向外编译依赖逐文件清单

按目录、文件名升序，大小写不敏感。范围仅 C 对 D/E/F 的源码直接包含和编译强制包含。A/B 依赖不计入。链接符号和仅传递包含另行跟踪。

共 116 个文件，其中 110 个实现文件、6 个头文件。mvdm-host 59 个，non-mvdm 57 个。依据 P9 当前产品编译记录。清单次序用于后续逐文件审查，列入清单不代表审查完成。

## 001 — src/mvdm-host/dos/command/cmdconf.c

- 原直接外层依赖 · C0039 · F ntvdm-exe · mvdm_softpc_firmware.h
- 当前直接依赖 · B compat · compat/include/system_root.h
- 处理：原版配置解析留在 C，公共 EXE 包目录能力由 D common 下沉到 B，四个 NtvdmGet 目录兼容查询由 F 下沉到 B。统一按 GetModuleFileNameW(NULL) 的 EXE 路径定位根目录，不再通过 session 取这四个查询的根目录。
- 当前文件直接外层头依赖 1→0；C0039 其他调用者仍开放。镜像非排除点位和行数均净变 0，原 #line 保留。本 S 累计镜像变化仍为 -2 点、-10 行。
- 两架构实际产品构建、包根目录专项、真实原版 PIF 专项、ROM/媒体/WOW 初始环境专项和八产品图 A/B 边界检查通过。11 映像双端发布哈希通过。无全量运行回归。
- 本次处理完成 1/116 个清单文件；当前直接向外编译依赖文件 115 个。原始清单保持编号顺序，其余条目按实际处理再更新。
- 原线程目录绑定候选已放弃，产品使用统一的进程 EXE 目录来源。

## 002 — src/mvdm-host/dos/command/cmdexec.c

- 直接包含 · C0024 · ntvdm-exe · src/ntvdm-exe/command/include/mvdm_command_native_child.h
- 直接包含 · C0036 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_shadow_registry.h

002 调用链核查：
- C0024：C mvdm-host → F ntvdm-exe，mvdm_command_native_child。实际管理 guest 命令/环境/标准句柄快照、命令服务类型和 session epoch/teardown。F 提供者依赖 guest_location 的租约和 session 生命周期。不能仅搬声明到 B。配置解析和原版执行逻辑仍留 C，需审查如何从 F 交付有明确寿命的输入与完成通知，或将产品专有接线迁至 F 的原版入口适配。接口设计未完成，保持开放。
- C0036：C mvdm-host → F ntvdm-exe，mvdm_shadow_registry。cmdexec.c 实际仅调用 RegOpenKeyEx、RegQueryValueEx、RegCloseKey，头文件通过 common/wow/registry.h 将其映射到 NTVDM.REG 兼容查询。原版 IsWowAppRunnable 具有相同三个 API 调用。提供者源码无 session/guest/mirror 依赖，仅系统 API、C 运行库和已下沉的 compat 包目录函数。建议将注册表兼容实现和共用声明整体归 B，F 保持初始化/关闭调用，C 保留原版查询流程。该迁移须覆盖其他原生及 WOW 查询调用方，不能恢复查询宿主注册表而丢失产品注册表语义。
- P12：注册表实现及共享声明下沉 B，C0036 消除；本文件直接外层头依赖 2→1。非排除镜像差异净变 0 点、0 行，#error/#line 不变。C0024 仍开放，归 S13 生命周期设计。
- 已顺序审查 2/116，全部解除外层编译依赖的文件仍为 1；后续 114 个文件尚待完整审查。其余七个调用文件同步注册表 include，不计为逐文件审查完成。

## 003 — src/mvdm-host/dos/command/cmdkeyb.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

003 结论：固件头仅用于 NtvdmGetSystemDirectoryA。改为直接包含 B system_root.h；注册表已由 P12 迁入 B。直接外层头依赖 1→0；原版键盘流程及 #line/#error 不变，镜像差异净变 0 点、0 行。P13 两架构 cached 构建、11组件归一化身份核对及双端发布哈希通过。

## 004 — src/mvdm-host/dos/command/cmdmisc.c

- 直接包含 · C0034 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_guest_location.h
- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

004 结论：C0034 的 cmdGetCurrentDir 保持数值地址并在结果确定后同步取得 guest 写入租约，F 提供者依赖 session 映射。C0039 的初始 WOW 环境投影发生于 cmdCreateVDMEnvironment 后、guest 复制前，内部调用 D common_guest_environment_root，并将进程堆结果转换为 CRT 所有权；不能只搬声明或让 B 反向依赖 D。两项归 S13 按输入准备、调用时序与释放契约处理，本文件未改，两个直接外层依赖仍开放；镜像变化 0 点、0 行。

## 005 — src/mvdm-host/dos/dem/dem.c

- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

005 结论：固件头仅用于 NtvdmGetSystemDirectoryA。直接包含 B system_root.h 即可；原版 DOS 初始化保留。外层头依赖 1→0；#line/#error 不变，镜像差异净变 0 点、0 行。P13 两架构 cached 构建、11组件归一化身份核对及双端发布哈希通过。

## 006 — src/mvdm-host/dos/dem/dem.h

- 直接包含 · C0034 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_guest_location.h

006 核查：dem.h 本身不使用 guest_location；demhndl.c 的 DOS 读写通过它使用 acquire/release 和数值地址。提供者调用 session_guest_memory_acquire/release，具有 F 生命周期依赖。不能单删头，也不能把 include 移到 demhndl.c 就宣称依赖消除。与 004 归入 S13 同一租约接线组；源码未改，镜像变化 0 点、0 行。

## 007 — src/mvdm-host/dos/dem/demgset.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0023 · ntvdm-exe · src/ntvdm-exe/command/include/mvdm_command_guest_directory.h

007 核查：C0017 注册表头已下沉 B。C0023 在 demSetDTALocation 登记 DOSWOWDATA 数值地址；F command_registration 保存 session/memory epoch，teardown 释放。消费者 mvdm_vm_notification operation 6 返回 CDS 数值地址与大小给驱动桥。归 S13 产品注册/销毁接线，当前一项直接外层依赖仍开放；源码未改，镜像变化 0 点、0 行。

## 008 — src/mvdm-host/dos/dem/demsrch.c

- 直接包含 · C0027 · ntvdm-exe · src/ntvdm-exe/session/dem_restart_hint.h

008 P14 结论：目录恢复 helper 与 NT I/O 包装整体下沉 B。helper 为独立 B 编译单元，仅头文件暴露原有状态/入口；避免重复原版 C 的 NTDLL 导入属性。两架构独立 B 接口测试及原版 DEM 115 断言通过。生产两架构 cached 构建、八产品图依赖检查通过；本文件直接外层头依赖 1→0。仅改 include 路径，镜像净变 0 点、0 行，#error/#line 不变。累计审查 8/116，直接外层编译依赖已清理 4，四个生命周期文件留待 S13，尚未审查 108。

## 009 — src/mvdm-host/dpmi32/i386/dpmi386.c

- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h

009 核查：C0045 的 WOW 页域由 F 创建页表、GDT 和客户端视图，并注册 session teardown。退出保护模式前调用 leave，恢复寄存器后调用 reenter；不能只换头或整体下沉 B。归 S13 与 010 成组处理模式转换接线。源码未改，镜像净变 0 点、0 行。

## 010 — src/mvdm-host/dpmi32/modesw.c

- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h

010 核查：与 009 共享 F 页域。leave 在清 PE 前，reenter 在保护模式段寄存器恢复后；时序属于迁移契约。归 S13 同组，未消除 C0045。源码未改，镜像净变 0 点、0 行。累计完成分类 10/116，四文件解除直接头依赖，六文件保留 S13 生命周期接线。

## 011 — src/mvdm-host/softpc.new/base/bios/virtual.c

- 直接包含 · C0043 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_termination.h
- 直接包含 · C0047 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_vm_bridge.h

011 审查中：C0047 VM bridge 涉及 IRQ、任务及输入生命周期，保留开放，归 S13。C0043 在本文件仅用于 invalid_virtual_instance，函数无 session 依赖；本地候选将原函数体迁 B，镜像只换 include。尚未完成构建、发布与验证，不能计入已消除。

011 P15：native failure 已迁 B，函数体与签名保持不变，产品链接验证 B 对象；两架构 cached 构建和 NIDDB 十组回滚专项通过，11组件双端发布哈希通过。直接外层头 2→1；C0241 消除，C0043 仍覆盖 sim32.c，C0047 保持开放。镜像净变 0 点、0 行；累计缩减 2 点、10 行。累计分类 11/116，四文件已解除直接外层头，七文件保留生命周期接线，105 未审查。

## 012 — src/mvdm-host/softpc.new/base/ccpu386/c_main.c

- 直接包含 · C0037 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_ccpu_wait.h

012 P16：等待实现及声明整体迁 B；仅 Windows 自动复位事件，无 session/guest/外层依赖。F 保持创建与关闭所有权，CCPU 事件位图及检查、等待、通知顺序不变。C0037、C0212、C0213 消除，直接外层头 1→0。两架构 cached 构建、八产品图 A/B 边界及真实 CCPU HLT/RESET 专项通过，11组件双端发布哈希通过。镜像只换 include，#error/#line 不变，净变 0 点、0 行；累计缩减 2 点、10 行。累计分类 12/116，五文件解除直接外层头，七文件保留生命周期接线，104 未审查；531/617 依赖开放。

## 013 — src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c

- 直接包含 · C0041 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h

013：ccpusas4.c 的 translate/resolve 读取 F 当前 session 的物理页与 EMS 别名记录；initialize 和 VdmMapDosMemory 注册 teardown，release_owner 清理所属记录。不能单搬头或让 B 依赖 session。归 S13 映射生命周期接线，C0041/C0238/C0240 保持开放。源码未改，镜像净变 0 点、0 行。

## 014 — src/mvdm-host/softpc.new/base/ccpu386/cpu4gen.h

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h

014：cpu4gen.h 使用 F 下的 x86 生成 gdpvar.h，内容是 CCPU/设备类型和 GDP 固定偏移，非产品业务。ADAPTER-SOFTPC-051 与当前来源政策将它限定为 NTVDMx64 外部生成 ABI 载体，禁止直接导入 C 镜像。简单搬 B 也不能消除其对 C 类型/布局的语义耦合。归 S14 来源和 ABI 归属审核，C0031 保持开放；源码未改，镜像净变 0 点、0 行。

## 015 — src/mvdm-host/softpc.new/base/ccpu386/sascdef.c

- 直接包含 · C0030 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/PigReg_c.h

015：sascdef.c 包含的 PigReg_c.h 提供 CpuRegsREC/NpxRegsREC/CpuStateREC，并内嵌 C 的 FPSTACKENTRY；同属获准的外部生成载体。保留原版 include，归 S14 与 014 同组审核，不以测试或名称 PIG 推断该声明可删除。C0030 保持开放；源码未改，镜像净变 0 点、0 行。

## 016 — src/mvdm-host/softpc.new/base/cpu/src/evid/vglob.c

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h

016：vglob.c 实际通过 GLOBAL_VGAGlobals 读写 VGA 状态，使用同一 x86 GDP 载体。当前载体 GDP_SIZE=43517，C-VID 镜像载体=43513，存在已记录的 TraceVector 位移差异，不能无验证换用。归 S14 同组核对来源、分配尺寸与所有访问布局，C0031 保持开放。源码未改，镜像净变 0 点、0 行。累计分类 16/116：五文件已解除、八生命周期项、三来源/ABI项，100 未审查；531/617 依赖开放。

## 017 — src/mvdm-host/softpc.new/base/cvidc/ev_glue.c

- 直接包含 · C0033 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_cvidc_vector_binding.h

017：ev_glue.c::setup_vga_globals 调用 F 的 bind_vectors，实际装配 C 的 Cpu/Sas/Video 表及 c_* 提供者，无通用 Windows 适配职责。不能下沉 B；归 S14 对照原版生成绑定及现有 C 载体，先核对初始化次序与镜像增量。C0033 保持开放；源码未改，镜像净变 0 点、0 行。

## 018 — src/mvdm-host/softpc.new/base/inc/sas.h

- 直接包含 · C0032 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h

018：sas.h 通过 sas4gen.h 取得 C 的 SAS_MEM_TYPE、SasVector 及调用签名。x86 外部生成载体较原版另含 sas_touch/VirtualiseInstruction/IsPageInstanceData 接口，受 NTVDMx64 来源例外约束。与 014-016 归 S14 来源/ABI 审核，不能把同名头直接互换；C0032 保持开放。源码未改，镜像净变 0 点、0 行。

## 019 — src/mvdm-host/softpc.new/base/keymouse/keyba.c

- 直接包含 · C0035 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_keyboard_history.h

019 P18：历史元数据实现/声明及独立单测迁 B，保持函数体和布局；C nt_event.c 继续拥有实例，锁与事件所有权不变。keyba.c 和 nt_event.c 只改 include，#error/#line 不变，镜像净变 0 点、0 行。C0035、C0198-C0203 消除，keyba.c 的 C0204 IRQ 接线仍开放。两架构 cached 构建、八产品图边界、历史/实际设备函数/输入归还三组专项通过；11组件双端发布哈希通过。累计分类 19/116：六文件解除直接外层头、八生命周期项、五来源/ABI/组合项、97 未审查；524/617 依赖开放，累计镜像缩减 2 点、10 行。第024文件仅同步共享头，不提前计作完成。

## 020 — src/mvdm-host/softpc.new/base/keymouse/mouse_io.c

- 直接包含 · C0049 · ntvdm-exe · src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h

020：mouse_io.c 三处调用仅查询 route_active；提供者在 ICA 锁内通过 console_client.c 的 current 查询当前 session->console_client->mouse。桥头同时带 D Console 鼠标协议，不能整体下沉 B。归 S13 显示所有者/输入路由接线；C0049 保持开放。源码未改，镜像净变 0 点、0 行。

## 021 — src/mvdm-host/softpc.new/host/src/config.c

- 直接包含 · C0017 · common · src/common/wow/registry.h

021：config.c 的原外层注册表头已在 P12 改为 B wow_registry.h，当前仅使用 RegOpenKeyEx/RegQueryValueEx/RegCloseKey。确认没有旧头引用，直接外层头已解除；不提前推断所有链接绑定均已解决。本轮无修改/构建/发布，镜像净变 0 点、0 行。

## 022 — src/mvdm-host/softpc.new/host/src/nt_bop.c

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h
- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h

022：nt_bop.c 在 WOW 初始化前进入 F 页域、初始化成功后激活 WOW 上下文，与009/010同属 session 页表/GDT生命周期。还依赖 C0031 GDP生成载体；MS_bop_4 内联 extern 调用 mvdm_vm_command_enter 是另一实际 F 链接，不能漏掉。归 S13 页域和产品入口接线，关联 S14生成ABI组；C0045/C0031保持开放。源码未改，镜像净变0点、0行。

## 023 — src/mvdm-host/softpc.new/host/src/nt_emm.c

- 直接包含 · C0040 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h
- 直接包含 · C0041 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h

023：nt_emm.c 先查询 session EMS别名，非别名保持原版 EM_*；别名路径调用 F guest-memory copy/move，内部逐段取得/释放 session租约并处理重叠。C0040/C0041不能仅迁声明，归S13与013/004/006成组审计映射、租约与销毁责任。源码未改，镜像净变0点、0行。累计分类23/116，七文件直接头依赖已解除、十一生命周期项、五来源/ABI/组合项、93未审查；524/617依赖开放，累计镜像缩减2点、10行。

## 024 — src/mvdm-host/softpc.new/host/src/nt_event.c

- 直接包含 · C0035 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_keyboard_history.h
- 直接包含 · C0038 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_event_thread.h
- 直接包含 · C0044 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_text_video.h
- 直接包含 · C0050 · ntvdm-exe · src/ntvdm-exe/softpc/mvdm_softpc_mouse_guest.h
- 直接包含 · C0052 · ntvdm-exe · src/ntvdm-exe/win32/console_client.h
- 直接包含 · C0054 · ntvdm-exe · src/ntvdm-exe/win32/console_input.h
- 直接包含 · C0055 · ntvdm-exe · src/ntvdm-exe/win32/console_text.h
- 直接包含 · C0057 · ntvdm-exe · src/ntvdm-exe/win32/worker_lifecycle.h
- 直接包含 · C0060 · worker-base · src/worker-base/connection.h

024 P20：全部九项原始头依赖已核查。键盘历史已由P18迁B；native alert/join现迁B，保持函数体、签名和原生NtAlertThread导入，C0038/C0217消除。剩余七项涉及session输入、鼠标队列、显示发布、通道激活及服务退出授权，归S13，不以迁声明掩盖。直接外层头8→7，镜像净变0点、0行，#error/#line不变；累计缩减2点、10行。24/116分类：7直接头清理、12生命周期、5来源ABI、92未审查。522/617开放。产品双架构已构建及双端发布，专项验证状态见P20证据。

## 025 — src/mvdm-host/softpc.new/host/src/nt_graph.c

- 直接包含 · C0044 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_text_video.h
- 直接包含 · C0053 · ntvdm-exe · src/ntvdm-exe/win32/console_graphics.h
- 直接包含 · C0055 · ntvdm-exe · src/ntvdm-exe/win32/console_text.h

025 P21：C0044/C0053/C0055：文本刷新/配置和路由查询使用当前session发布者；图形begin/end维护覆盖层、update_depth和dirty通知。不是平台独立能力，归S13原版绘图与产品发布接线。 源码未改，镜像净变0点、0行。

## 026 — src/mvdm-host/softpc.new/host/src/nt_inthk.c

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h

026 P21：C0031：固定原版第34行已有gdpvar.h include；当前保留的回调实现没有直接GDP访问。不能把无使用误记成需要GDP；删除原版include会新增镜像差异，本批不实施。归S14生成载体统一归属及必要性审查。 源码未改，镜像净变0点、0行。

## 027 — src/mvdm-host/softpc.new/host/src/nt_mem.c

- 直接包含 · C0041 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h

027 P21：C0041：InitIntelMemory成功发布前初始化F映射记录并注册session teardown；FreeIntelMemory先释放记录再释放Intel内存。归S13映射生命周期，与013/023成组，不迁声明掩盖调用。 源码未改，镜像净变0点、0行。

## 028 — src/mvdm-host/softpc.new/host/src/nt_mouse.c

- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h
- 直接包含 · C0050 · ntvdm-exe · src/ntvdm-exe/softpc/mvdm_softpc_mouse_guest.h

028 P21：C0045/C0050：route_active和mouse_apply使用当前session鼠标队列；publish_cursor校验domain.active、owner、PG、CR3后写入既有WOW视图。归S13输入路由与页域生命周期，保留原版ICA锁和调用顺序。 源码未改，镜像净变0点、0行。

## 029 — src/mvdm-host/softpc.new/host/src/nt_msscs.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

029 P22：Only NtvdmGetSystemDirectoryA is used from the F firmware header, to locate ntio.sys. Include the existing B system_root directly. Registry queries already use B; no loading/reset order changes. Mirror delta 0 points/0 lines; verification in s12-p22-system-loader.json.

## 030 — src/mvdm-host/softpc.new/host/src/nt_reset.c

- 直接包含 · C0057 · ntvdm-exe · src/ntvdm-exe/win32/worker_lifecycle.h

030 P22：host_applClose calls F lifecycle authorization before teardown. It requests NTSRV exit, waits for service/worker/console close and quiesces VM frontends. S13; not a generic B exit operation. Mirror delta 0 points/0 lines; verification in s12-p22-system-loader.json.

## 031 — src/mvdm-host/softpc.new/host/src/sim32.c

- 直接包含 · C0043 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_termination.h

031 P22：Failed InitIntelMemory calls F terminate_current_session, which calls original host_applClose then session_terminate_current. S13 failure/escape lifecycle; cannot replace with native ExitProcess without changing execution boundary. Mirror delta 0 points/0 lines; verification in s12-p22-system-loader.json.

## 032 — src/mvdm-host/softpc.new/obj.vdm/ntvdm.c

- 直接包含 · C0036 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_shadow_registry.h

032 P22：P12 already moved native registry projection to B. CpuEnvInit uses B NtOpenKey/NtEnumerateValueKey/NtClose macros; original CPU environment remains C. Direct outer header cleared, no new change. Mirror delta 0 points/0 lines; verification in s12-p22-system-loader.json.

## 033 — src/mvdm-host/vdmredir/vrnetapi.c

- 直接包含 · C0058 · vdmredir-dll · src/vdmredir-dll/include/mvdm_redirector_guest_copy.h

033 P23：C0058：F guest-copy performs segmented-address conversion and bounded worker lease acquire/release via D guest-memory imports. ANSI/OEM conversion is coupled to copy failure/commit; not a standalone B platform service. S13 redirector/worker memory boundary. No source change; mirror delta 0 points/0 lines.

## 034 — src/mvdm-host/vdmredir/vrnmpipe.c

- 直接包含 · C0012 · common · src/common/redirector/async.h

034 P23：C0012：D async header imports F worker prepare/complete/release. F owns session/epoch, guest addresses and staging; completion validates owner/epoch then commits payload before error/count, release clears PrivateAsyncState. S13; declaration relocation alone cannot remove this dependency. No source change; mirror delta 0 points/0 lines.

## 035 — src/mvdm-host/wow32/mapembed.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

035 P23：C0114：forced F Registry aliases provide DLL-owned DWORD HKEY tokens over parent EXE exported registry providers. Calls pin parent keys under a token lock. Generated DLL entry calls shutdown on explicit unload. S13 DLL binding/token lifetime; not interchangeable with B raw registry API. No source change; mirror delta 0 points/0 lines.

## 036 — src/mvdm-host/wow32/precomp.h

- 直接包含 · C0016 · common · src/common/wow/paths.h
- 直接包含 · C0069 · wow32-dll · src/wow32-dll/include/ntpsapi_wow32.h
- 直接包含 · C0082 · wow32-dll · src/wow32-dll/include/wow_clipboard_epoch.h
- 直接包含 · C0108 · wow32-dll · src/wow32-dll/include/wow_menu_parser_bindings.h
- 直接包含 · C0125 · wow32-dll · src/wow32-dll/include/wow_user_private_access.h

036 P24：C0069 native thread ordinal unchanged to B; mirror include spelling unchanged. Other four headers retain session shadow, clipboard cleanup, task menu parser and USER alias/callback ownership. S13; see s12-p24-wow-precomp.json. Direct outer headers5→4, mirror0点/0行，#error/#line不变。

## 037 — src/mvdm-host/wow32/walias.h

- 直接包含 · C0014 · common · src/common/wow/gdi_alias.h

037 P25：walias.h 留在 C，仅替换已有 include。独立 GDI alias 表实现由 F 迁 B，声明由 D 迁 B，函数体、声明字节、三个导出及 DLL 导入不变。实际编译只依赖 SDK/B，NTVDM 链接一份表。C0014 消除，直接外层头 1→0；镜像差异净变 0 点、0 行，#error/#line 不变。两架构 cached 全量构建、两个 GDI 专项、八产品图 A/B 检查和151个 WOW 对象边界通过；11组件双端发布。与P24只差包GUID和PE时间戳，按授权跳过重复guest运行。累计37/116：10个直接头解除、21个生命周期、6个来源ABI，79未审查；520/617开放，累计镜像缩减2点10行。

## 038 — src/mvdm-host/wow32/wcall16.c

- 直接包含 · C0075 · wow32-dll · src/wow32-dll/include/wow_callback_frame_lease.h

038 P26：wcall16 实际仅用 control capture/restore，并未用该头的内存租约。CCPU/TD恢复与F worker存活、callback scope的退栈有时序约束；S13以现有F CallBack16包装为承接候选，恢复必须先于callback_leave。 本文件未修改，镜像净变0点、0行，依赖保持开放；详见s12-p26-wow-unwind-ownership.json。仅源码审查，不宣称运行通过。

## 039 — src/mvdm-host/wow32/wcntl32.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

039 P26：wcntl32原版分配/转换仍在C；F helper记录dwTmp[1]并供正常和异常清理共同消费，且调用原版free16。S13成组处理捕获、正常释放、异常释放，不整体搬B，也不只移除正常路径记录。 本文件未修改，镜像净变0点、0行，依赖保持开放；详见s12-p26-wow-unwind-ownership.json。仅源码审查，不宣称运行通过。

## 040 — src/mvdm-host/wow32/wcommdlg.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h
- 强制包含 · C0083 · wow32-dll · src/wow32-dll/include/wow_common_dialog_calls.h

040 P26：wcommdlg同时有消息/回调栈清理和七个原生对话框调用绑定。后者实际交还/重获WOW执行权并恢复返回状态，不是纯SDK转发。S13拆分产品接线，原版转换仍留C。 本文件未修改，镜像净变0点、0行，依赖保持开放；详见s12-p26-wow-unwind-ownership.json。仅源码审查，不宣称运行通过。

## 041 — src/mvdm-host/wow32/wdde.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

041 P27：wdde的DDEIsTargetMSDraw确实调用RegOpenKeyEx/RegCloseKey；局部token不进入guest，但仍需使用父NTVDM注册表视图。C0114不是无用强制头，归S13同一token/父提供者组。源码未改，镜像净变0点0行。 详见s12-p27-wow-vdd-abi.json。

## 042 — src/mvdm-host/wow32/wkernel.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

042 P27：wkernel的原版注册表thunk将HKEY存入guest DWORD并在后续调用传回，不能替换成父端raw key。C0114归S13；profile shadow属于precomp已登记的共享头边。源码未改，镜像净变0点0行。 详见s12-p27-wow-vdd-abi.json。

## 043 — src/mvdm-host/wow32/wkfileio.c

- 直接包含 · C0068 · wow32-dll · src/wow32-dll/include/nt_vdd.h

043 P27：wkfileio与原版nt_vdd.h均未改动。F宏包装删除，三个保持原符号/参数/返回类型的cdecl声明改由B专用头提供，仅本编译入口选择该目录。原DEM提供者、导出/导入不变，C0068消除。两架构cached构建、双端11组件发布、8产品图/151WOW对象检查通过；11映像仅包GUID和PE时间戳不同，依授权跳过重复guest运行。旧VDD源码检查对已退役SFT适配的断言已修正为原版初始化/消费链并通过。本文件镜像净变0点0行，累计缩减2点10行。累计43/116：11头依赖解除、26生命周期、6来源ABI、73未审查；519/617开放。 详见s12-p27-wow-vdd-abi.json。

## 044 — src/mvdm-host/wow32/wkgthunk.c

- 强制包含 · C0123 · wow32-dll · src/wow32-dll/include/wow_user_native_extension.h

044 P28: Existing F native-call and callback scopes own execution handoff. Original conversion stays C; classify S13. C0123 remains open. Mirror delta 0 points/0 lines.

## 045 — src/mvdm-host/wow32/wkman.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0105 · wow32-dll · src/wow32-dll/include/wow_idle_hook_binding.h
- 直接包含 · C0119 · wow32-dll · src/wow32-dll/include/wow_task_termination.h
- 强制包含 · C0112 · wow32-dll · src/wow32-dll/include/wow_native_process.h

045 P28: remove redundant CreateProcess wrapper; sole caller passes two local structure addresses. Idle, termination and MsgWait remain S13 lifecycle bindings. Mirror delta 0/0, cumulative reduction 2 points/10 lines. Classified45/116, unreviewed71, open519/617. Both cached builds, dual eleven-image publication, eight A/B graphs, 150 WOW objects / nine negative controls and controlled Winmine/Sol/Write frontend checks passed. Recorded in P28 delivery. See s12-p28-native-process.json.

## 046 — src/mvdm-host/wow32/wmdisp32.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

046 P29: lifecycle/rollback group S13; shared normal and exception release require coordinated ownership. Mirror unchanged.

## 047 — src/mvdm-host/wow32/wmsg16.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

047 P29: six progress constants move to B; C converter retains all behavior. Only include target changed, error/line untouched.

## 048 — src/mvdm-host/wow32/wmsgcb.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

048 P29: two text progress constants now selected from B; no F lifecycle use. Mirror delta0/0, cumulative reduction2 points/10 lines.48/116 classified,13 header clears verified by build,29 lifecycle,6 source/ABI,68 unreviewed;519/617 open. Both cached builds, dual publication and three focused suites passed; all11 images normalized-identical to P28. See s12-p29-message-progress.json.

## 049 — src/mvdm-host/wow32/wow32.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0119 · wow32-dll · src/wow32-dll/include/wow_task_termination.h
- 直接包含 · C0128 · wow32-dll · src/wow32-dll/include/wow_user_thunk_scope.h

049 P30: W32Dispatch/W32TryCall invoke F borrow scope around original thunk; dispatch filter temporarily selects entry frame for original W32Exception and restores it before unwind. WOWSysErrorBox has native-call handoff; WOWStartupFailed calls service-authorized exit. S13: preserve original exception policy in C; assess entry/exception/exit binding together at existing F dispatch boundary. Do not move C TD/layout-dependent adapter into B. Registry C0017 already cleared. Mirror delta0/0; see s12-p30-wow-registry-dispatch.json.

## 050 — src/mvdm-host/wow32/wow32fax.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

050 P30: Read supported fax driver list; create/set shadow fax configuration. HKEY values stay in native locals, unlike WS32 guest registry thunks. Existing B wow_registry.h provides read-only projection but not complete create/set contract. S13: split local raw-key access from guest-token conversion through the same parent registry authority. Guest token wrapping is not proven necessary for this file; direct modern Registry substitution would select a different store. No new adapter or mirror change implemented. Mirror delta0/0; see s12-p30-wow-registry-dispatch.json.

## 051 — src/mvdm-host/wow32/wowhooks.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

051 P30: Hook callbacks capture stack before conversion; finalizers restore recursion/message/Parm16 state, release acquired16/32 keys, restore stack and fThunkDDEmsg. Journal and mouse callbacks also use stack scope. S13: group with wmdisp32/wcntl32 callback allocation/unwind; keep original hook conversion and normal copyback. Do not remove normal release while leaving outer unwind able to double-release, or move C-layout-aware release wholesale to B. Mirror delta0/0; see s12-p30-wow-registry-dispatch.json.

## 052 — src/mvdm-host/wow32/wshell.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

052 P30: WS32RegOpenKey/CreateKey store returned HKEY into guest DWORD; close/query/delete retrieve guest-supplied value. WOWRegDeleteKey recursively enumerates/deletes using same handle domain. F provider translates tokens to parent raw keys under lock; DLL explicit-unload invokes shutdown. S13: retain C conversion/recursive-delete policy; place token representation and lifecycle at explicit product integration boundary without parallel registry store. Token mapping and parent storage must not be conflated; no permanent exception approved. Mirror delta0/0; see s12-p30-wow-registry-dispatch.json.

## 053 — src/mvdm-host/wow32/wuclip.c

- 直接包含 · C0124 · wow32-dll · src/wow32-dll/include/wow_user_object_bindings.h

053 P31: Clipboard native HWND conversion uses current runtime object table and already published shared-window view; reverse conversion refreshes typed native result and verifies exact generation. S13: place native clipboard boundary and publication with existing F owner; original guest conversions remain C. Do not substitute LOWORD/truncation or move runtime-dependent object table to B. Mirror delta0/0; see s12-p31-wow-clipboard-page-domain.json.

## 054 — src/mvdm-host/wow32/wudlg.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

054 P31: W32DialogFunc captures stack before conversion; successful unthunk sets completed; finally always finishes message/stack. Other dialog message entry unwinds16. S13: same allocation/normal completion/exception unwind group as files039/046/051; no independent cleanup state. Mirror delta0/0; see s12-p31-wow-clipboard-page-domain.json.

## 055 — src/mvdm-host/wow32/wuman.c

- 直接包含 · C0015 · common · src/common/wow/page_domain.h

055 P31: USER16 initialization writes guest_shared_info and guest_csr_flag addresses. F page_domain_create registers session teardown; getters return zero when inactive. S13: initialization publication must remain tied to domain lifetime and guest address ownership. D declaration is not an independent mechanism suitable for B; assess existing F registration/init boundary. Mirror delta0/0; see s12-p31-wow-clipboard-page-domain.json.

## 056 — src/mvdm-host/wow32/wumsg.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

056 P31: Message entry finally paths call wow_message_unwind16; original message conversion and DDE cleanup precede those finalizers. Provider only replays designated input-only cleanup, not arbitrary output conversion. S13: audit whole message entry/unwind boundary with existing conversion scopes; preserve ordinary copyback and DDE ordering. Mirror delta0/0; see s12-p31-wow-clipboard-page-domain.json.

## 057 — src/mvdm-host/wow32/wuser.c

- 直接包含 · C0015 · common · src/common/wow/page_domain.h

057 P31: SetSysColors success path publishes each accepted element to guest server-info. F provider checks active domain, current owner, index, PG and CR3 before c_sas_storedw. S13: keep native success/failure authority and original conversion; move product publication with existing F integration boundary only after checking ordering and all publishers. No generic B pointer writer. Mirror delta0/0; see s12-p31-wow-clipboard-page-domain.json.

## 058 — src/mvdm-host/wow32/wuser31.c

- 直接包含 · C0124 · wow32-dll · src/wow32-dll/include/wow_user_object_bindings.h

058 P31: GetOpenClipboardWindow result passes through wow_user_window_public_guest_handle before GETHWND16. Same current-worker exact-generation conversion as wuclip.c. S13: group with file053; native cross-process window observation does not confer guest ownership. Do not invent separate alias mapping. Mirror delta0/0; see s12-p31-wow-clipboard-page-domain.json.

## 059 — src/mvdm-host/xms.486/xmsblock.c

- 直接包含 · C0040 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h
- 直接包含 · C0048 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_xms_memory.h

059 P32: S13: bounded guest-copy lease and cancellation owned by current session; copy_from reads move descriptor, copy_forward preserves forward overlapping-copy contract. Cancellation requests SESSION_CANCELLATION_REQUESTED; no new CPU486 change. Mirror delta0/0; both builds, targeted probe/boundary checks and dual publication passed. C0126 eliminated;61/116 classified,14 clears,40 lifecycle,7 source/ABI,55 unreviewed,518/617 open. See s12-p32-user-probe.json.

## 060 — src/non-mvdm/base/ntdll/csrutil.c

- 直接包含 · C0002 · common · src/common/base_srv/base_capture.h

060 P32: S14 original CSR declaration/header ownership, coordinated with S13 worker heap lifetime. D header includes C nt/ntlpcapi/ntcsrmsg and exposes original capture declarations plus CsrPortHeap/tag macros. F standalone worker creates/destroys heap. Moving this mixed header wholesale to B would recreate B-to-C dependency. Mirror delta0/0; both builds, targeted probe/boundary checks and dual publication passed. C0126 eliminated;61/116 classified,14 clears,40 lifecycle,7 source/ABI,55 unreviewed,518/617 open. See s12-p32-user-probe.json.

## 061 — src/non-mvdm/base/ntos/ex/probe.c

- 直接包含 · C0126 · wow32-dll · src/wow32-dll/include/wow_user_probe_bindings.h

061 P32: Move unchanged SDK-only declaration/environment shim header F to B; original range/alignment ProbeForRead remains C. Change its include and existing F/fixture consumer includes only. Header uses GetSystemInfo/RtlRaiseStatus, no runtime/product state. Mirror delta0/0; both builds, targeted probe/boundary checks and dual publication passed. C0126 eliminated;61/116 classified,14 clears,40 lifecycle,7 source/ABI,55 unreviewed,518/617 open. See s12-p32-user-probe.json.

## 062 — src/non-mvdm/base/win32/client/vdm.c

- 直接包含 · C0003 · common · src/common/base_srv/base_classifier.h
- 直接包含 · C0004 · common · src/common/base_srv/base_client.h
- 直接包含 · C0005 · common · src/common/base_srv/base_config.h
- 强制包含 · C0006 · common · src/common/base_srv/legacy_classifier_arch.h

062 P33: independent console handle predicate moved unchanged D to B; original vdm.c untouched. Mixed classifier/client/config/architecture headers require S14 source ABI and S13 transport/template ownership review; not wholesale B moves.62/116 classified,54 unreviewed;517/617 IDs open; C0153 eliminated after both builds, same link provider object and boundary verification.11 images published at both destinations and normalized-identical to P32. Mirror delta0/0, cumulative reduction2 points/10 lines. See s12-p33-console-handle.json.

## 063 — src/non-mvdm/base/win32/server/basesrv.h

- 直接包含 · C0022 · ntsrv-exe · src/ntsrv-exe/opennt/include/base_server.h

063 P34: S14: original CSR declarations are mixed with S13 authenticated request/resource binding. Dispatch binds borrowed CSR_THREAD and resource callbacks and restores both on exit; whole header cannot move to B. Original basesrv.h unchanged. Mirror delta0 points/0 lines; #error/#line unchanged. See s12-p34-remote-browser.json.

## 064 — src/non-mvdm/com/ole32/olethunk/olethk32/thoputil.cxx

- 直接包含 · C0113 · wow32-dll · src/wow32-dll/include/wow_ole_descriptor_private.h

064 P34: S14: original OLE private types are mixed with S13 guest pointer/ACP initialization binding. Keep original ConvertObjDescriptor in C; do not move the mixed precomp-dependent header to B. Original thoputil.cxx unchanged. Mirror delta0 points/0 lines; #error/#line unchanged. See s12-p34-remote-browser.json.

## 065 — src/non-mvdm/ds/netapi/xactsrv/apisrv.c

- 直接包含 · C0059 · vdmredir-dll · src/vdmredir-dll/include/remote_browser.h

065 P34: S12: SDK-only unavailable Browser provider/declaration moved F to B; original public enumeration stays C. Function body unchanged, WORD/cdecl status120 and untouched output semantics retained. Actual compiler closure uses B header; original handler fixture links B remote_browser.obj and passes37 checks. C0059/C0606 eliminated. Mirror delta0 points/0 lines; #error/#line unchanged. See s12-p34-remote-browser.json.

## 066 — src/non-mvdm/windows/core/ntuser/client/btnctl.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

066 P35: S13 task/borrow/button-state and drawing-resource lifecycle; coordinate S14 for extracted original SetWindowState/ClearWindowState and FNID. Original btnctl.c stays unchanged. Pure SDK icon-size body moves F to B unchanged; actual WOW32 map selects B wow_icon_size.obj, C0349 eliminated. Other dependencies remain open. Mirror delta0 points/0 lines; #error/#line unchanged. See s12-p35-bitmap-native.json.

## 067 — src/non-mvdm/windows/core/ntuser/client/cldib.c

- 直接包含 · C0072 · wow32-dll · src/wow32-dll/include/wow_bitmap_bindings.h

067 P35: Only five native macros plus SDK are used. Definitions moved to shared B header, reused by F bitmap header; original algorithms/globals unchanged. Actual cldib compiler dependencies no longer include F bitmap header. C0072 remains open for clres.c. Mirror delta0 points/0 lines; #error/#line unchanged. See s12-p35-bitmap-native.json.

## 068 — src/non-mvdm/windows/core/ntuser/client/clmenu.c

- 直接包含 · C0109 · wow32-dll · src/wow32-dll/include/wow_menu_resource_bindings.h

068 P36: S13 menu task identity/publication and invocation lifetime; coordinate S14 original resource declaration ownership. W32FindResource/W32LoadResource/W32LockResource/W32UnlockResource are C wres32.c providers, not F implementations. CommonLoadMenu retains original lock/parse/unlock ordering and its resource-retention behavior. Source unchanged; mirror delta0 points/0 lines. See s12-p36-menu-resource-ownership.json.

## 069 — src/non-mvdm/windows/core/ntuser/client/clmsg.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

069 P36: S13 default entry/borrow/name lifetime, with S14 selected original declaration cohort. Production default-client.obj selects WOW_ORIGINAL_DEFAULT_WINDOW and control-client/default-precomp.h, not the ordinary edit context branch. Source unchanged; mirror delta0 points/0 lines. See s12-p36-menu-resource-ownership.json.

## 070 — src/non-mvdm/windows/core/ntuser/client/clres.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h
- 直接包含 · C0072 · wow32-dll · src/wow32-dll/include/wow_bitmap_bindings.h
- 直接包含 · C0109 · wow32-dll · src/wow32-dll/include/wow_menu_resource_bindings.h

070 P36: S14 multi-context source/ABI composition, coordinated with S13 resource and drawing lifetime. Actual products compile this file as clres.obj and dialog-template.obj; latter selects WOW_ORIGINAL_DIALOG_TEMPLATE with control-client precomp, former bitmap/resource headers. Source unchanged; mirror delta0 points/0 lines. See s12-p36-menu-resource-ownership.json.

## 071 — src/non-mvdm/windows/core/ntuser/client/combo.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

071 P37: S13 original control entry, task/borrow lifetime and native call restoration; coordinate S14 state/FNID provenance. F edit_entry validates current WOW task and registered class atoms, retains existing window binding, enters task/borrow scopes, captures display geometry, calls original ComboBoxWndProc, then restores TLS and releases references/scopes in finally. Source unchanged; mirror delta0 points/0 lines. See s12-p37-combo-ownership.json.

## 072 — src/non-mvdm/windows/core/ntuser/client/combodir.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

072 P37: S13 current-runtime HWND-to-retained-state validation; original directory/list algorithms stay C. Only observed direct external symbol is wow_window_client_validate_window. It obtains current runtime, enters it if necessary, borrows exact retained window state, then restores runtime and last error. This is not interchangeable with public IsWindow. Source unchanged; mirror delta0 points/0 lines. See s12-p37-combo-ownership.json.

## 073 — src/non-mvdm/windows/core/ntuser/client/comboini.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

073 P37: S13 native creation/task/class binding; S14 original extracted state/FNID review. C chooses list/edit child classes from atomSysClass, geometry/styles, ANSI creator flag and ordered cleanup, including list destruction before parent becomes zombie. Preserve that creation/destruction policy and original state allocation/free. Source unchanged; mirror delta0 points/0 lines. See s12-p37-combo-ownership.json.

## 074 — src/non-mvdm/windows/core/ntuser/client/dlgbegin.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

074 P38: unchanged native HWND slot get/set and GUI/system query leaves moved F to B; C0379/C0382/C0398/C0399 eliminated by actual link map. Original template parsing, child construction and rollback remain C. Task/borrow/TLS/class enrollment remains S13; extracted original callbacks and private native ABI cohort remain S14. Mirror delta0 points/0 lines. See s12-p38-dialog-native-leaves.json.

## 075 — src/non-mvdm/windows/core/ntuser/client/dlgend.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

075 P39: C retains EndDialog state/result, owner enablement, focus/hide/activation and WM_NULL modal-loop wake. All eight remaining direct external symbols involve identity discovery, retained-state validation or native-call/task restoration; S13 boundary group. Do not replace no-notification zap with public SetActiveWindow/SetFocus pair. Source unchanged; mirror delta0 points/0 lines. See s12-p39-dialog-end-manager.json.

## 076 — src/non-mvdm/windows/core/ntuser/client/dlgmgr.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

076 P39: C retains promotion, default message policy, keyboard/modal loop and resource cleanup; F supplies registered callbacks, native procedure dispatch, context and scheduler/publication ownership. Already-cleared C0379 is supplied by B. Remaining source extraction provenance for FNID/SetWindowState/ClearWindowState/ReleaseEditDS belongs to S14; lifecycle composition to S13. Source unchanged; mirror delta0 points/0 lines. See s12-p39-dialog-end-manager.json.

## 077 — src/non-mvdm/windows/core/ntuser/client/dlgmgr2.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

077 P40: Original ANSI/Unicode keyboard handling, default button, tab/group/Enter/Escape and recursive item lookup remain C. F task/borrow scopes surround original keyboard entry; button scope delays BUTN reuse across focus callbacks and restores BST_DONTCLICK before releasing storage. S13 shared entry/lifetime boundary; no permanent exception. Mirror delta0 points/0 lines; see s12-p40-dialog-navigation.json.

## 078 — src/non-mvdm/windows/core/ntuser/client/dlgmgrc.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

078 P40: Original tree/group/tab navigation remains C. Actual COFF undefined callable is _IsDescendant, resolved to C control-view-winmgr.obj; gcWheelDelta is a common tentative declaration already assigned C ownership. No F linked function observed. C0064 nevertheless imports F-selected original WND/CLS/layout/macros and declarations. S14 original USER header/layout composition, not wholesale source migration or duplicated B layouts. Mirror delta0 points/0 lines; see s12-p40-dialog-navigation.json.

## 079 — src/non-mvdm/windows/core/ntuser/client/dmmnem.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

079 P40: Original mnemonic text matching, traversal, button flash and command behavior remain C. Four outer symbols are shared button enter/leave and task-aware GetWindowTextW/SendMessageW. Audit jointly with dlgmgr2 and button entry/cleanup in S13; do not split into parallel lifecycle paths. Mirror delta0 points/0 lines; see s12-p40-dialog-navigation.json.

## 080 — src/non-mvdm/windows/core/ntuser/client/draw.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

080 P41: Original PaintRect remains C. Shared retained-window validation is S13; no source changes or dependency elimination. File081 investigation remains active; see s12-p41-drawing-ownership.json.

## 081 — src/non-mvdm/windows/core/ntuser/client/drawtext.c

- 直接包含 · C0066 · wow32-dll · src/wow32-dll/include/control-drawing/precomp.h

081 P42: Original text body stays C. Five native linked providers moved B as one resolver group; TLS context and private declaration composition remain S13/S14. Mirror delta0 points/0 lines; see s12-p42-native-controls.json.

## 082 — src/non-mvdm/windows/core/ntuser/client/edecrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

082 P43: Keep original creation/destruction, font metrics, password, numeric classification and layout algorithms in C. Font/width native providers are already B under P42. GetDC/ReleaseDC/heap/GDI are native imports; FreeLookasideEntry resolves C rtlinit.obj and EditLookaside is defined in C editec.c. The BUTTON-only free macro is not selected for this object. Twelve residual outer symbols are task/client identity, registered callbacks, retained window state and caret/native-call lifecycle: S13 boundary plus S14 original state/FNID extraction cohort. Mirror delta0 points/0 lines; see s12-p43-edit-ownership.json.

## 083 — src/non-mvdm/windows/core/ntuser/client/editec.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

083 P43: Keep original text/undo/selection, EDIT dispatch and clipboard policy in C. Twenty-four actual outer bindings include task-aware menu/dispatch/borrow and A/W conversion, not SDK-only wrappers. Shared S13 entry/native-return ownership; S14 original extent/message-table/state composition. No whole-file migration or parallel EDIT implementation. Mirror delta0 points/0 lines; see s12-p43-edit-ownership.json.

## 084 — src/non-mvdm/windows/core/ntuser/client/editml.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

084 P44: Keep original multiline selection, line building, paste, scroll/timer scheduling and paint order C. Native system-timer leaves already B under P42. Remaining task context, callbacks and mapped native-return/publication are S13. FillWindow native API is B, while the F wrapper around it owns current native-call scope and CTLCOLOR return handling. Mirror delta0 points/0 lines; see s12-p44-control-brushes.json.

## 085 — src/non-mvdm/windows/core/ntuser/client/editsl.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

085 P44: Keep original single-line edit and hide/BeginPaint/EndPaint/show ordering C. Borrowed system-brush storage/resource provider moved B unchanged. Other mapped window/caret/native returns belong to shared S13 boundary, not an entire-file move. Mirror delta0 points/0 lines; see s12-p44-control-brushes.json.

## 086 — src/non-mvdm/windows/core/ntuser/client/edmlonce.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

086 P45: MLCreate remains C. One remaining outer function, extracted original ClearWindowState, is S14 source ownership coupled to S13 native-state publication; C0064 mixed header remains open. Native address query already B. Mirror delta0/0; see s12-p45-edit-create-ownership.json.

## 087 — src/non-mvdm/windows/core/ntuser/client/edmlrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

087 P46: Original multiline rare algorithms stay C; C0504/C0546/C0557 require shared callback/native-return ownership review. No new in-place leaf; mirror delta0/0. See s12-p46-edit-rare-ownership.json.

## 088 — src/non-mvdm/windows/core/ntuser/client/edslrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

088 P46: Original single-line creation/undo stays C. No remaining direct F function; C0064 mixed precomp remains an actual outward compile dependency, S14 composition. Native query already B. Mirror delta0/0. See s12-p46-edit-rare-ownership.json.

## 089 — src/non-mvdm/windows/core/ntuser/client/hdata.c

- 直接包含 · C0086 · wow32-dll · src/wow32-dll/include/wow_dde_data_bindings.h

089 P47: C0086 eliminated by byte-identical native declaration/layout header move F to B. Original hdata.c and its include spelling unchanged; actual product deps contain no D/E/F header. Mirror delta0/0. Both builds, dual publication and DDE fixture pass; all11 normalized images equal P44. See s12-p47-dde-header.json.

## 090 — src/non-mvdm/windows/core/ntuser/client/lb1.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

090 P48: Original ListBox algorithms stay C;15 actual outer symbols classified as shared entry/borrow/native return (S13), with original FNID/extent/message-table cohort (S14). No pure native provider remains to move in place. Mirror delta0/0; see s12-p48-list-entry-ownership.json.

## 091 — src/non-mvdm/windows/core/ntuser/client/lboxctl1.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

091 P49: Original item/scroll/sort/encoding policy remains C. Five actual outer symbols are shared metrics/message/native-return boundaries for S13; C0064 composition remains open. Mirror delta0/0; see s12-p49-list-items-ownership.json.

## 092 — src/non-mvdm/windows/core/ntuser/client/lboxctl2.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

092 P50: Keep original interaction in C; seventeen outer symbols stay open. Timer callback requires shared entry; reset native leaf is B-eligible but its VOID wrapper failure policy remains outer. Mirror delta0/0; see s12-p50-list-interaction-ownership.json.

## 093 — src/non-mvdm/windows/core/ntuser/client/lboxctl3.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

093 P51: Native filesystem, C encoding and original directory algorithms retained. Five F task/window/message symbols and C0064 remain open. Mirror0/0; see s12-p51-list-directory-ownership.json.

## 094 — src/non-mvdm/windows/core/ntuser/client/lboxmult.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

094 P51: All business calls resolve C; F precomp compile dependency remains for shared ABI cleanup. Keep original geometry. Mirror0/0; see s12-p51-list-directory-ownership.json.

## 095 — src/non-mvdm/windows/core/ntuser/client/lboxrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

095 P52: Original creation/destruction/resize remain C, font measurement B; seven F symbols stay open. Mirror0/0; see s12-p52-list-lifecycle-ownership.json.

## 096 — src/non-mvdm/windows/core/ntuser/client/lboxvar.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

096 P52: Original variable-height algorithms stay C. Only direct outer business symbol is current invocation metric snapshot; mixed header also open. Mirror0/0; see s12-p52-list-lifecycle-ownership.json.

## 097 — src/non-mvdm/windows/core/ntuser/client/mngrayc.c

- 直接包含 · C0062 · wow32-dll · src/wow32-dll/include/button-drawing/precomp.h
- 直接包含 · C0065 · wow32-dll · src/wow32-dll/include/control-draw-state/precomp.h

## 098 — src/non-mvdm/windows/core/ntuser/client/ntstubs.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h
- 直接包含 · C0077 · wow32-dll · src/wow32-dll/include/wow_class_query_bindings.h
- 直接包含 · C0095 · wow32-dll · src/wow32-dll/include/wow_edit_call_bindings.h

## 099 — src/non-mvdm/windows/core/ntuser/client/rtlinit.c

- 直接包含 · C0121 · wow32-dll · src/wow32-dll/include/wow_user_capture_bindings.h

## 100 — src/non-mvdm/windows/core/ntuser/client/winmgrc.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 101 — src/non-mvdm/windows/core/ntuser/kernel/class.c

- 直接包含 · C0076 · wow32-dll · src/wow32-dll/include/wow_class_lookup_bindings.h
- 直接包含 · C0078 · wow32-dll · src/wow32-dll/include/wow_class_register_bindings.h
- 直接包含 · C0079 · wow32-dll · src/wow32-dll/include/wow_class_remove_bindings.h

## 102 — src/non-mvdm/windows/core/ntuser/kernel/cleanup.c

- 直接包含 · C0081 · wow32-dll · src/wow32-dll/include/wow_cleanup_bindings.h

## 103 — src/non-mvdm/windows/core/ntuser/kernel/exitwin.c

- 直接包含 · C0117 · wow32-dll · src/wow32-dll/include/wow_task_order_bindings.h

## 104 — src/non-mvdm/windows/core/ntuser/kernel/getset.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 105 — src/non-mvdm/windows/core/ntuser/kernel/profile.c

- 直接包含 · C0118 · wow32-dll · src/wow32-dll/include/wow_task_profile_bindings.h

## 106 — src/non-mvdm/windows/core/ntuser/kernel/queue.c

- 直接包含 · C0116 · wow32-dll · src/wow32-dll/include/wow_task_init_bindings.h

## 107 — src/non-mvdm/windows/core/ntuser/kernel/server.c

- 直接包含 · C0087 · wow32-dll · src/wow32-dll/include/wow_default_message_bindings.h

## 108 — src/non-mvdm/windows/core/ntuser/kernel/taskman.c

- 直接包含 · C0117 · wow32-dll · src/wow32-dll/include/wow_task_order_bindings.h

## 109 — src/non-mvdm/windows/core/ntuser/rtl/draw.c

- 直接包含 · C0066 · wow32-dll · src/wow32-dll/include/control-drawing/precomp.h

## 110 — src/non-mvdm/windows/core/ntuser/rtl/drawtext.c

- 直接包含 · C0066 · wow32-dll · src/wow32-dll/include/control-drawing/precomp.h

## 111 — src/non-mvdm/windows/core/ntuser/rtl/getset.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 112 — src/non-mvdm/windows/core/ntuser/rtl/help.c

- 直接包含 · C0122 · wow32-dll · src/wow32-dll/include/wow_user_client_view_layout.h
- 直接包含 · C0129 · wow32-dll · src/wow32-dll/include/wow_window_client_state_bindings.h

## 113 — src/non-mvdm/windows/core/ntuser/rtl/text.c

- 直接包含 · C0066 · wow32-dll · src/wow32-dll/include/control-drawing/precomp.h

## 114 — src/non-mvdm/windows/core/ntuser/rtl/winmgr.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h
- 直接包含 · C0067 · wow32-dll · src/wow32-dll/include/control-geometry/precomp.h

## 115 — src/non-mvdm/windows/core/ntuser/rtl/wow.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 116 — src/non-mvdm/windows/core/ntuser/server/exports.c

- 直接包含 · C0020 · ntsrv-exe · src/ntsrv-exe/opennt/include/base_interactive.h
