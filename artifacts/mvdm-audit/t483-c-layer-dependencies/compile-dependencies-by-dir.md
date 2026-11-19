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

## 025 — src/mvdm-host/softpc.new/host/src/nt_graph.c

- 直接包含 · C0044 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_text_video.h
- 直接包含 · C0053 · ntvdm-exe · src/ntvdm-exe/win32/console_graphics.h
- 直接包含 · C0055 · ntvdm-exe · src/ntvdm-exe/win32/console_text.h

## 026 — src/mvdm-host/softpc.new/host/src/nt_inthk.c

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h

## 027 — src/mvdm-host/softpc.new/host/src/nt_mem.c

- 直接包含 · C0041 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h

## 028 — src/mvdm-host/softpc.new/host/src/nt_mouse.c

- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h
- 直接包含 · C0050 · ntvdm-exe · src/ntvdm-exe/softpc/mvdm_softpc_mouse_guest.h

## 029 — src/mvdm-host/softpc.new/host/src/nt_msscs.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

## 030 — src/mvdm-host/softpc.new/host/src/nt_reset.c

- 直接包含 · C0057 · ntvdm-exe · src/ntvdm-exe/win32/worker_lifecycle.h

## 031 — src/mvdm-host/softpc.new/host/src/sim32.c

- 直接包含 · C0043 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_termination.h

## 032 — src/mvdm-host/softpc.new/obj.vdm/ntvdm.c

- 直接包含 · C0036 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_shadow_registry.h

## 033 — src/mvdm-host/vdmredir/vrnetapi.c

- 直接包含 · C0058 · vdmredir-dll · src/vdmredir-dll/include/mvdm_redirector_guest_copy.h

## 034 — src/mvdm-host/vdmredir/vrnmpipe.c

- 直接包含 · C0012 · common · src/common/redirector/async.h

## 035 — src/mvdm-host/wow32/mapembed.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

## 036 — src/mvdm-host/wow32/precomp.h

- 直接包含 · C0016 · common · src/common/wow/paths.h
- 直接包含 · C0069 · wow32-dll · src/wow32-dll/include/ntpsapi_wow32.h
- 直接包含 · C0082 · wow32-dll · src/wow32-dll/include/wow_clipboard_epoch.h
- 直接包含 · C0108 · wow32-dll · src/wow32-dll/include/wow_menu_parser_bindings.h
- 直接包含 · C0125 · wow32-dll · src/wow32-dll/include/wow_user_private_access.h

## 037 — src/mvdm-host/wow32/walias.h

- 直接包含 · C0014 · common · src/common/wow/gdi_alias.h

## 038 — src/mvdm-host/wow32/wcall16.c

- 直接包含 · C0075 · wow32-dll · src/wow32-dll/include/wow_callback_frame_lease.h

## 039 — src/mvdm-host/wow32/wcntl32.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 040 — src/mvdm-host/wow32/wcommdlg.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h
- 强制包含 · C0083 · wow32-dll · src/wow32-dll/include/wow_common_dialog_calls.h

## 041 — src/mvdm-host/wow32/wdde.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

## 042 — src/mvdm-host/wow32/wkernel.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

## 043 — src/mvdm-host/wow32/wkfileio.c

- 直接包含 · C0068 · wow32-dll · src/wow32-dll/include/nt_vdd.h

## 044 — src/mvdm-host/wow32/wkgthunk.c

- 强制包含 · C0123 · wow32-dll · src/wow32-dll/include/wow_user_native_extension.h

## 045 — src/mvdm-host/wow32/wkman.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0105 · wow32-dll · src/wow32-dll/include/wow_idle_hook_binding.h
- 直接包含 · C0119 · wow32-dll · src/wow32-dll/include/wow_task_termination.h
- 强制包含 · C0112 · wow32-dll · src/wow32-dll/include/wow_native_process.h

## 046 — src/mvdm-host/wow32/wmdisp32.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 047 — src/mvdm-host/wow32/wmsg16.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 048 — src/mvdm-host/wow32/wmsgcb.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 049 — src/mvdm-host/wow32/wow32.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0119 · wow32-dll · src/wow32-dll/include/wow_task_termination.h
- 直接包含 · C0128 · wow32-dll · src/wow32-dll/include/wow_user_thunk_scope.h

## 050 — src/mvdm-host/wow32/wow32fax.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

## 051 — src/mvdm-host/wow32/wowhooks.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 052 — src/mvdm-host/wow32/wshell.c

- 强制包含 · C0114 · wow32-dll · src/wow32-dll/include/wow_registry_binding.h

## 053 — src/mvdm-host/wow32/wuclip.c

- 直接包含 · C0124 · wow32-dll · src/wow32-dll/include/wow_user_object_bindings.h

## 054 — src/mvdm-host/wow32/wudlg.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 055 — src/mvdm-host/wow32/wuman.c

- 直接包含 · C0015 · common · src/common/wow/page_domain.h

## 056 — src/mvdm-host/wow32/wumsg.c

- 直接包含 · C0110 · wow32-dll · src/wow32-dll/include/wow_message_lifetime.h

## 057 — src/mvdm-host/wow32/wuser.c

- 直接包含 · C0015 · common · src/common/wow/page_domain.h

## 058 — src/mvdm-host/wow32/wuser31.c

- 直接包含 · C0124 · wow32-dll · src/wow32-dll/include/wow_user_object_bindings.h

## 059 — src/mvdm-host/xms.486/xmsblock.c

- 直接包含 · C0040 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h
- 直接包含 · C0048 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_xms_memory.h

## 060 — src/non-mvdm/base/ntdll/csrutil.c

- 直接包含 · C0002 · common · src/common/base_srv/base_capture.h

## 061 — src/non-mvdm/base/ntos/ex/probe.c

- 直接包含 · C0126 · wow32-dll · src/wow32-dll/include/wow_user_probe_bindings.h

## 062 — src/non-mvdm/base/win32/client/vdm.c

- 直接包含 · C0003 · common · src/common/base_srv/base_classifier.h
- 直接包含 · C0004 · common · src/common/base_srv/base_client.h
- 直接包含 · C0005 · common · src/common/base_srv/base_config.h
- 强制包含 · C0006 · common · src/common/base_srv/legacy_classifier_arch.h

## 063 — src/non-mvdm/base/win32/server/basesrv.h

- 直接包含 · C0022 · ntsrv-exe · src/ntsrv-exe/opennt/include/base_server.h

## 064 — src/non-mvdm/com/ole32/olethunk/olethk32/thoputil.cxx

- 直接包含 · C0113 · wow32-dll · src/wow32-dll/include/wow_ole_descriptor_private.h

## 065 — src/non-mvdm/ds/netapi/xactsrv/apisrv.c

- 直接包含 · C0059 · vdmredir-dll · src/vdmredir-dll/include/remote_browser.h

## 066 — src/non-mvdm/windows/core/ntuser/client/btnctl.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 067 — src/non-mvdm/windows/core/ntuser/client/cldib.c

- 直接包含 · C0072 · wow32-dll · src/wow32-dll/include/wow_bitmap_bindings.h

## 068 — src/non-mvdm/windows/core/ntuser/client/clmenu.c

- 直接包含 · C0109 · wow32-dll · src/wow32-dll/include/wow_menu_resource_bindings.h

## 069 — src/non-mvdm/windows/core/ntuser/client/clmsg.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 070 — src/non-mvdm/windows/core/ntuser/client/clres.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h
- 直接包含 · C0072 · wow32-dll · src/wow32-dll/include/wow_bitmap_bindings.h
- 直接包含 · C0109 · wow32-dll · src/wow32-dll/include/wow_menu_resource_bindings.h

## 071 — src/non-mvdm/windows/core/ntuser/client/combo.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 072 — src/non-mvdm/windows/core/ntuser/client/combodir.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 073 — src/non-mvdm/windows/core/ntuser/client/comboini.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 074 — src/non-mvdm/windows/core/ntuser/client/dlgbegin.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 075 — src/non-mvdm/windows/core/ntuser/client/dlgend.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 076 — src/non-mvdm/windows/core/ntuser/client/dlgmgr.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 077 — src/non-mvdm/windows/core/ntuser/client/dlgmgr2.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 078 — src/non-mvdm/windows/core/ntuser/client/dlgmgrc.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 079 — src/non-mvdm/windows/core/ntuser/client/dmmnem.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 080 — src/non-mvdm/windows/core/ntuser/client/draw.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 081 — src/non-mvdm/windows/core/ntuser/client/drawtext.c

- 直接包含 · C0066 · wow32-dll · src/wow32-dll/include/control-drawing/precomp.h

## 082 — src/non-mvdm/windows/core/ntuser/client/edecrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 083 — src/non-mvdm/windows/core/ntuser/client/editec.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 084 — src/non-mvdm/windows/core/ntuser/client/editml.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 085 — src/non-mvdm/windows/core/ntuser/client/editsl.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 086 — src/non-mvdm/windows/core/ntuser/client/edmlonce.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 087 — src/non-mvdm/windows/core/ntuser/client/edmlrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 088 — src/non-mvdm/windows/core/ntuser/client/edslrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 089 — src/non-mvdm/windows/core/ntuser/client/hdata.c

- 直接包含 · C0086 · wow32-dll · src/wow32-dll/include/wow_dde_data_bindings.h

## 090 — src/non-mvdm/windows/core/ntuser/client/lb1.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 091 — src/non-mvdm/windows/core/ntuser/client/lboxctl1.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 092 — src/non-mvdm/windows/core/ntuser/client/lboxctl2.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 093 — src/non-mvdm/windows/core/ntuser/client/lboxctl3.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 094 — src/non-mvdm/windows/core/ntuser/client/lboxmult.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 095 — src/non-mvdm/windows/core/ntuser/client/lboxrare.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

## 096 — src/non-mvdm/windows/core/ntuser/client/lboxvar.c

- 直接包含 · C0064 · wow32-dll · src/wow32-dll/include/control-client/precomp.h

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
