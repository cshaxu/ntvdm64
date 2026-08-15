# Local Code Governance Ledger

状态：M92 已完成，2026-08-08。本文是本仓库本地代码的唯一分类账本；
`src/opennt/SOURCE-MANIFEST.json` 是 OpenNT 岛内每个受管文件的来源、hash 和
字节数记录。

## 目的

OpenNT 代码恢复、现代宿主兼容、trace 观察和早期实验必须有不可混淆的边界。
本账本不授权任何本地代码进入研究型 `ntvdm.exe`；正式路径仍须遵守
`SOFTPC-RECONSTRUCTION-GOVERNANCE.md` 所规定的历史职责链。

## 规范分类

| 类别 | 允许内容 | 不允许内容 | 目标目录 |
| --- | --- | --- | --- |
| `overlay/reconstruction` | 已由 caller-first 证实的缺损 OpenNT 语义恢复 | 从比较工程复制的实现、DOS/BOP 重定义 | `overlay/<原始路径>` |
| `overlay/instrumentation` | 原始函数中的可关闭 checkpoint、只读断言 | 改变返回、设备响应或启动成功条件 | `overlay/<原始路径>` |
| `compat/compiler` | SDK/CRT/SEH/声明、宏和编译器兼容 | 运行时行为或 DOS 策略 | `compat/compiler` |
| `compat/host` | 已由活跃历史 caller 证明必要的现代宿主接缝 | BIOS、BOP、SVC、PC/AT 或 DOS 语义 | `compat/host` |
| `reconstruction/monitor` | 由已审计的历史 CPU caller/owner 逐条约束的独立 software-monitor mechanics | BIOS/BOP/DEM/DOS dispatcher、IVT/BDA、PC/AT 设备、host policy 或外部代码复制 | `refs/archive/reconstruction/monitor` |
| `trace/fixtures` | 停止、快照、断言和受控输入夹具 | 让 DOS 或 BOP/SVC 继续前进的替代实现 | `trace/fixtures` |
| `archived/legacy-adapters` | 早期自写 CCPU、BOP/DEM、CLI、介质或 monitor 实验 | 任何正式 runtime、默认 build 或验收路径 | `archived/legacy-adapters` |

物理分类已将自写 OpenNT 辅助代码迁入 `local/compat/compiler`、
`local/compat/host`、`local/trace/fixtures` 与
`local/archived/legacy-adapters`；OpenNT 原始源码与 `overlay/<原始路径>`
保持不动。后续迁移或删除必须先记录原 owner、调用方、manifest 与 CMake 影响；
不得为了目录美观破坏历史证据。

## 当前清点与处置

| 当前集合 | 当前事实 | 规范分类 / 处置 |
| --- | --- | --- |
| `local/trace/fixtures/historical_bios_owner_closure_bridge.c` | S5 v2 date trace 的 source-closure 对照；以原始 `Start_of_M_area` 取代 archived SM0 aperture | `trace/fixtures`；仅 default-disabled link diagnostic，不能运行或承担 BIOS/BOP/DOS/host policy。它用于证明 SM0 退出后的真实原始闭包。 |
| `local/trace/fixtures/historical_msbop_owner_closure_bridge.c` | S5 no-SM0 的原始 `MS_bop_0 -> DemDispatch(14h)` 对照；只为测量完整 `BIOS[]` 静态表的增量而直接调用 `MS_bop_0` | `trace/fixtures`；仅 default-disabled link diagnostic，不能运行，也绝不是正式 runtime dispatch 的替代。 |
| `CMakeLists.txt: ntdos64-opennt-ccpu-host-intel-memory-owner-objects` | 仅编译原始 `host/src/nt_mem.c`，供 no-SM0 owner-closure 在已有原始 `sim32.c` 之外恢复 `InitIntelMemory` / `FreeIntelMemory` 的真实 owner | `local-build-metadata`；default-disabled，不含本地实现，不进入 runner/CTest/runtime；`VdmSetPhysRecStructs` 仍是待调研的私有 VDM seam，禁止以 shim 或固定结果补齐。 |
| `overlay/.../ccpu386/c_effective_addr.c` | 唯一已明确标为缺损 SoftPC 语义恢复的 overlay | `overlay/reconstruction`，保留；继续 caller-first 验证 |
| `overlay/.../base/dos/emm_mngr.c` 的 `NTDOS64_CCPU_LIM_WRITEBACK_RECONSTRUCTION` 分支 | 仅在 `ntdos64-opennt-ccpu-lim-writeback-reconstruction-objects` 中显式暴露原始 `LIM_b_write`、`LIM_w_write`、`LIM_str_write` 代码块，解释 CCPU SAS 的 `LIM` writeback 引用 | `overlay/reconstruction`；只进入 default-disabled full-source link diagnostic，不运行、不可进入 BIOS/BOP/DEM/runner。该分支复用原始函数体，尚未有实际 EMS caller，不能视为 EMS runtime 或 host shim。 |
| `overlay/base/mvdm/inc/softpc.h` | S5 CCPU/SAS profile 对历史 x86 V86 `GetVDMAddr` 宏的受限地址路由覆盖 | `overlay/reconstruction`；只在 `NTDOS64_CCPU_SAS_GETVDMADDR` 下将 segment:offset 交给原 `c_GetLinAdd`，仅两个 default-disabled S5 fixture 可链接；不恢复 CPU、DOS、BOP 或设备语义 |
| `overlay/.../main.c`、`c_main.c`、`reset.c`、`nt_eoi.c`、`nt_hosts.c`、`nt_reset.c`、`nt_msscs.c`、`nt_event.c`、`mouse_io.c` | 启动链/host 观察插桩 | `overlay/instrumentation`；默认关闭，不可作为 runtime 逻辑 |
| `overlay/.../host/src/nt_cpu.c` | CCPU compile profile 中排除与 CCPU SAS owner 冲突、且该 wrapper branch 未使用的旧 `Start_of_M_area` / `Length_of_M_area` 声明 | `compat/compiler`；仅 default-disabled object/link-diagnostic closure，不改变或提供 lifecycle 行为 |
| `local/compat/compiler/opennt_ccpu_sim32_x86_abi_compat.h` | 仅声明原始 `ccpusas4.c:c_GetLinAdd`，并将 Sim32 的历史 V86 线性指针 import 名映射到该已有 CCPU owner | `compat/compiler`；只用于 default-disabled x86 Sim32 SAS object gate，不提供指针转换、内存、设备或 DOS 行为 |
| `local/compat/compiler/` | 编译器、SDK、CRT、SEH 或历史声明兼容 | `compat/compiler`；逐项标明是否含行为宏。S5 的 `opennt_dem_ntdecl_compat.h` 只补 `demgset.c` 缺失的 `RtlOemStringToUnicodeString` 与 `RtlDosPathNameToNtPathName_U` `NTAPI` 原型，使其匹配现代 x86 `ntdll` 的 `@12` / `@16` imports；不能解释为 host shim 或 drive policy。 |
| `local/compat/host/byob_ntio_path.c` | 现代 BYOB 路径接缝 | 候选 `compat/host`；必须补充真实历史 caller、输入/失败域和正常路径准入证据 |
| `local/compat/host/historical_bios_bridge_v1.[ch]` | S2 i386 历史侧固定 record 与 `BIOS[50h]` date trace 调用；自身调用 `sas_init/c_cpu_init`，并向其私有 SAS backing 写入 service byte | `trace/fixtures` 内的旧 bridge，默认关闭；只允许既有 `50h/14h` date trace。它不是 shared-aperture transport，不能扩展 selector/service，不能成为通用 BOP/DEM/DOS dispatcher、S4 bridge 或 runtime shim。详见 `S5-SHARED-APERTURE-TRANSPORT-ADMISSION.md`。 |
| `local/compat/host/historical_bios_bridge_v2.[ch]`、`local/trace/fixtures/historical_transport_v2_helper.c`、`historical_transport_v2_*` | CCPU/SM0 date trace 的旧 cross-bitness record 对照 | `trace/fixtures`，明确不可准入。helper 将 archived SM0 owner 与原始 host owner 混合，当前完整 source-closure 链接也因 `_host_error`、mouse、timer duplicate owner 失败；不得以 `/FORCE`、符号重命名、删 owner 或 no-op shim 修复。不得扩展 selector/service 或作为 S4/runtime bridge。详见 `docs/research/ccpu-bios-closure/S5-TRANSPORT-V2-BASELINE-REJECTION.md`。 |
| `local/compat/host/historical_dem_byob_profile_v1.[ch]` | 历史 `DemInit` 完成后的本地 BYOB DOS 目录替换 | `compat/host`；只能在已抵达的原始 `demLoadDos` caller 上使用；不构造文件名、不打开/读取文件、不实现 DOS/BOP/DEM 语义，默认 build 不链接 |
| `local/compat/host/historical_vdm_session_context_v1.c` | 原始 `scaffold/i386/softpc.c` 所有的 `VDMForWOW` host session state | `compat/host`；当前仅固定 non-WOW false 值，使 `DemInit` 可读取其原始上下文；不导入 scaffold entry、IVT/BDA、设备或 DOS 行为，默认 build 不链接 |
| `local/compat/host/historical_nt_process_heap_v1.c` | 原 `demgset.c::GetPhysicalDriveType` 经 `RtlDosPathNameToNtPathName_U` 分配路径后所需的历史 `RtlProcessHeap` entry | `compat/host`；只将已被 trace 触达且现代 x86 `ntdll` 不再导出的 process-heap entry 映射到 `GetProcessHeap()`，由原 `RtlFreeHeap` 继续释放；不参与 drive 分类、cache、DPB、BOP/SVC 或 runtime。 |
| `local/compat/host/oem_facade_v1/*` | 原 `oemuni` 的 modern host facade 重建 | `compat/host`；当前仅 F1 `CreateFileOem`，显式 resource root 内的 OEM 转换与 open；不得误称完整 facade，也不得导入 archived drive/root/environment 策略 |
| `local/trace/fixtures/host_console_last_event_shim.c` | host-main trace 的无副作用控制台事件占位 | `trace/fixtures`，不得升级为 console 实现 |
| `local/trace/fixtures/original_sas_lifecycle_trace_fixture.c` | 原始 `ccpusas4.c -> sim32.c + nt_mem.c` 的 SAS 生命周期采证 | `trace/fixtures`；默认关闭，只验证 allocation/RAM/teardown；不得执行 CPU、BIOS、BOP、DEM、设备或 DOS |
| `refs/archive/reconstruction/transport/shared_aperture_v1.h`、`tests/shared_aperture_v1_host_fixture.c`、`local/trace/fixtures/shared_aperture_v1_helper.c` | S5 x64/i386 同 backing raw-section 采证 | `reconstruction/transport` + `trace/fixtures`；默认关闭，只验证定宽 offset、section 映射与双向字节可见性；不得链接 SAS、CCPU、BIOS、BOP、DEM、DOS、设备或 runtime。 |
| `local/compat/host/shared_aperture_sas_host_v1.[ch]`、`local/trace/fixtures/shared_aperture_sas_v1_helper.c`、`tests/shared_aperture_s4_post_sas_fixture.c` | 原始 `host_sas_init/host_sas_term` 的 section-allocation seam 与 lifecycle 后 S4 ordinary-instruction 采证 | `compat/host` + `trace/fixtures`；默认关闭。adapter 仅把已验证的 named-section view 交给原 `sas_init`，并在原 `sas_term` 时保留 section 的释放所有权；fixture 只验证同一 backing、SAS teardown 与一条 S4 real-mode `NOP`。不得承载 CPU-state/BOP/BIOS/DEM/DOS dispatch、设备响应或 runtime 成功条件。 |
| `local/trace/fixtures/original_bios50_date_fixture.c`、`historical_bios_bridge_v1_fixture.c` | S1 direct historical BIOS[50h] closure 与 S2 record-ABI 采证 | `trace/fixtures`；默认关闭；前者不解码 guest 指令，后者只允许 date trace，均不得升级为 runtime |
| `local/trace/fixtures/original_demload_dos_byob_fixture.c` | 原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch -> demLoadDos` service `11h` 的 BYOB 加载闭包采证 | `trace/fixtures`；默认关闭，仅记录原始 NTDOS 加载路径和 IP 消耗；不得作为加载器、DOS runtime 或成功判定的替代 |
| `local/trace/fixtures/original_demload_dos_direct_fixture.c` | 原始 `demLoadDos` 的 direct diagnostic | `trace/fixtures`；默认关闭，仅用于将原始 loader 与 `MS_bop_0` 返回后的 idle 生命周期分离定位；不得作为 dispatcher、启动路径或 runtime 替代 |
| `local/trace/fixtures/original_demgetdrives_byob_fixture.c` | 原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch(0Fh) -> demGetDrives` 的 caller-first 采证 | `trace/fixtures`；默认关闭，保留原 `oemuni/file.c::GetDriveTypeOem` 与 `demgset.c` 的完整职责；只记录历史返回寄存器和 DEM cache，不提供或修改 drive policy、DPB、DOS/BOP/SVC 结果或 runtime 成功条件。 |
| `local/trace/fixtures/original_demgetdpblist_trace_fixture.c` | 同一 DEM session 内原始 `50h/0Fh -> demGetDrives` 后接 `50h/46h -> demGetDPBList` 的 caller-first guest-write 采证 | `trace/fixtures`；默认关闭。仅提供寄存器和 SAS destination aperture；由已登记的受限 `GetVDMAddr` reconstruction 路由原 DEM 写入。不得伪造 cache/DPB、调用 disk/media/drive policy，或替代 BOP/SVC/runtime 行为。 |
| `local/trace/fixtures/original_demsetdta_trace_fixture.c` | 原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch(1Bh) -> demSetDTALocation` 的 guest-address 采证 | `trace/fixtures`；默认关闭，只设置 `DOSWOWDATA::lpSftAddr` 所需的最小 SAS fixture byte，以观察原 owner 保存的指针；不构造 DOS data、DPB、BOP/SVC 结果或 host capability。 |
| `local/trace/fixtures/original_demsetharderror_trace_fixture.c` | 原始 `BIOS[50h] -> MS_bop_0 -> DemDispatch(32h) -> demSetHardErrorInfo` 的 guest-address 采证 | `trace/fixtures`；默认关闭，只观察原 owner 保存的 VHE/device-chain 指针；不触发 hard-error、INT 24、设备、BOP/SVC 结果或 host capability。 |
| `local/trace/fixtures/original_cmdsetinfo_trace_fixture.c` | 原始 `BIOS[54h] -> MS_bop_4 -> CmdDispatch(05h) -> cmdSetInfo` 的 closure 采证 | `trace/fixtures`；默认关闭，链接完整原始 COMMAND dispatch table；仅记录指针或异常 RVA，不缩表、不替代 COMMAND/BOP/DOS/host 行为。 |
| `ntdos64-opennt-historical-host-*-inventory-objects` | S5 对 NT4 `host/src/sources` 覆盖差异的 compile-only 清点；portable 与 native 两组共 9 个原始 source unit | 仅 historical-inputs、default-disabled object target；不链接 runner、fixture 或 runtime。`fprt.c` 因缺失匹配的历史 CRT header 保持 source-only reference，禁止通过改名、mock CRT 或输出 shim 使其进入当前工具链。详见 `docs/research/ccpu-bios-closure/S5-HOST-SRC-COVERAGE-AUDIT.md`。 |
| `refs/archive/reconstruction/monitor/*` | S4 新建 software-monitor CPU/memory mechanics | `reconstruction/monitor`；仅当逐条符合 S3 与 S4 准入记录时可存在，默认关闭，不能包含 BIOS/BOP/DEM/DOS 或 host policy |
| `refs/archive/reconstruction/video/reconstructed_cvidc_video_tail.[ch]`、`ntdos64-opennt-reconstructed-cvidc-video-tail-objects` | R1 的本地 C-VIDC `Video` 向量尾部恢复：仅将原始、具类型兼容性的 `setWritePointers`、`setReadPointers`、`setMarkPointers` 接到 ordinal 78--80 | `reconstruction/video`；default-disabled object-only。它不定义 `Video`/`C_Video` 存储、不复制或别名 0--77、不使用 CCPU 的短表，且不链接 fixture、runner 或 runtime。它只是独立重建候选，并不证明或声称恢复了丢失的历史 `ccpu486` 组合；准入和禁止项见 `docs/R1-GENERIC-VIDEO-RECONSTRUCTION-ADMISSION.md`。 |
| `local/trace/fixtures/*fixture*.c`、`config_prefix_*`、`gfi_*`、`original_*` | 有界 trace 和 fixture 支持 | `trace/fixtures`；必须默认关闭，不能修改 guest/BOP/SVC 结果 |
| `ccpu_host_*`、`ccpu_register_abi_bridge.c`、`ccpu_unresolved_vector_guard.c`、`ccpu_vidc_global_storage.c` | 早期自写 CCPU/host 闭合实验 | `archived/legacy-adapters`；不得链接到正式链 |
| `local/archived/legacy-adapters/ccpu_monitor_sm0_foundation.c` | SM0 bounded-trace foundation；其 `host_error`、mouse install 与 timer symbols 是早期 fail-closed 占位 | `archived/legacy-adapters`；可用于独立 trace，但不得与同名 OpenNT host owner (`nt_error.c`、`nt_mouse.c`、`nt_timer.c`) 同时进入 historical source-closure target。详见 `docs/research/ccpu-bios-closure/S5-ARCHIVED-ADAPTER-OWNER-COLLISION.md`。 |
| `direct_cli_*`、`contained_*`、根 `refs/archive/legacy-adapters/nvtdm.c`、`bop_rewrite.c`、`dos_*`、`owned_monitor_*`、`nt4_*`、`whpx_*` | 早期 CLI、DOS、介质、monitor 或外部执行实验 | `archived/legacy-adapters` 或独立 comparison；不得成为 SoftPC 修复替代物 |
| `archived/legacy-adapters/reconstructed_monitor_r2/*` | 早期自写 D6/C4 BOP 解码、合成 `BIOS[]` 调用和 NOP execution fixture | `archived/legacy-adapters`；只保留作 historical trace 对照，不得成为 monitor、BOP dispatcher 或 runtime 的一部分 |

`M92-OPENNT-OVERLAY-AUDIT.md` 已逐项记录初始 29 个 overlay 的 caller/owner、差异与
运行期风险：`c_effective_addr.c` 是唯一已确认的缺损 SoftPC 语义
`overlay/reconstruction`；S5 的 `base/mvdm/inc/softpc.h` 是另一个受限 profile 路由 overlay，
不应误称缺失 CPU 语义恢复；
`host/inc/x86/prod/sas4gen.h` 则是单独的、未准入的 `overlay/reconstruction candidate`，
不能被误列为 compiler compatibility。其余为 `overlay/instrumentation`、
`compat/compiler` 或未准入的 `compat/host candidate`。
`M92-OVERLAY-DELTA-MATRIX.md` 是对应的机械复核矩阵：逐项列出固定基线存在性、
delta 行数、manifest/hash 复核和准入类别，用于发现后续漂移。

`ntdos64-opennt-cvidc-generic-video-vector-objects` 是单独登记的
`trace/fixtures` build-only evidence target：它第二次编译原始 `base/cvidc/vglfunc.c`，
以 preprocessor symbol rename 产生完整 ABI 的 generic `Video` 0--77 typed initializer。
它不含本地 C source、不链接 CCPU 或任何运行链、也不进入 default build；不得被升级为
`Video` historical owner、C-VIDC runtime binding 或 SoftPC composition 的证据。
即使某个文件看似只含 compiler compatibility 或 checkpoint，也不得据此推断其可进入
runtime。

## 根 `src/` 清点与处置

根 `src/` 不再是“备用 runtime 实现”的位置。除经 S4 明确准入的
`refs/archive/reconstruction/monitor/` 外，根 CMake 只注册现代 CLI 的分类/BYOB 验证；下列集合
均已从 target 和 CTest 图移除：

| 文件集合 | 分类 | 处置 |
| --- | --- | --- |
| `ntdos64_run.c`、`byob_identity.*`、`byob_profile.*`、`byob_image.*` | `current/cli-admission` | 保留在默认图；只做 PE/DOS 分类、BYOB 身份核验和明确 engine handoff，不执行 guest。 |
| `runner_engine_probe.c`、`byob_*_test.c` | `current/cli-verification` | 保留为 CLI admission 的受控验证；不引入 SoftPC 或 DOS 语义。 |
| `reconstruction/monitor/*` | `reconstruction/monitor` | S4 的 default-disabled 机械执行 slice；仅能通过明示 bridge transport 抵达历史 host chain，不能进入 CLI 或默认 CTest。 |
| `redacted_trace.*`、`contained_dem_file_tokens.*`、`tools/historical/archived/nosrvbld_class_extract.c` | `trace-or-tool evidence` | 源码保留、无 root target；仅在专门治理批准后作为证据重新评估。 |
| `owned_monitor_*`、`nt4_oracle_date_adapter.*`、`nt4_ntio_bootstrap_plan.*`、`nt4_ntdos_load_plan.*`、`nt4_bootstrap_transaction.*`、`nt4_mantle_initial_state.*` | `archived/legacy-adapters` | 已从 root target/CTest 图移除；不得复活为 monitor、bootstrap、mantle 或 runtime。 |
| `reconstructed_monitor_r2/*` | `archived/legacy-adapters` | 已从 root target/CTest 图移除；仅保留既有受控 trace 断言，不得承担 BOP、BIOS 或执行后端职责。 |
| `nvtdm.c`、`bop_rewrite.*`、`dos_namespace.*`、`dos_file_service.*`、`transition_registry.*`、`bootstrap_profile.*` | `archived/legacy-adapters` | 已从 source-only custom target 移除；只能作为早期设计和 trace 对照。 |
| `whpx_*` | `archived/external-backend-probes` | 已从 root target/CTest 图移除；不是 SoftPC 运行路线。 |
| `ntvdm64_core_machine_probe.c` | `archived/external-comparison-probe` | 外部 source/include/archive integration 已从 root CMake 删除；文件不再可配置或链接。 |

`refs/ntvdmx64-derived/`、`refs/bochs/`、`src/pcjs/`（若存在）以及仓库外的 `ntvdm64`
均为 `reference-only`。它们不得被 root CMake 或 OpenNT 运行 target 消费；保留的
上游构建文件只是原始材料的一部分，不能被解释成当前项目授权的构建路线。

`refs/opennt-45/` 同样是比较证据而非 NT4 输入。M92 已移除 OpenNT CMake 中唯一的
跨树 `emm_mngr.c` recovery target；该中间 EMS 实现的缺损只能继续作为 NT4 caller
first 调查项，不能用后续版本源码充当构建期补件。

## M92 验收与队列

**M92-LOCAL-CODE-GOVERNANCE-RECONCILIATION** 已完成物理分类与 overlay 语义
审计；它不实现 DOS 运行时功能。`config.c` 与 `nt_msscs.c` 的候选 seam 后续必须
单独通过 caller-first runtime admission，不能借 M92 自动晋升。

1. [x] 校验物理文件、`SOURCE-MANIFEST.json` 和 CMake 输入三者一致；修复所有未登记
   overlay/shim、过期 hash 或错误来源类别。
2. [x] 为每个本地 `.c/.h` 记录：规范分类、原始 owner/调用方、行为边界、默认构建状态、
   测试目标和处置（`retained`、`trace-only`、`archived`、`remove-candidate`）。
3. 将文件迁至规范目录，或在不移动文件的过渡期以 CMake target 和此账本明确等价分类。
4. 证明正式 `ntdos64-run` 与未来研究型 `ntvdm.exe` 均不链接 `trace/fixtures` 或
   `archived/legacy-adapters`；`compat/host` 的每一项都必须有已抵达的历史 caller。
5. 已从 root `CMakeLists.txt` 删除 `NTDOS64_ENABLE_NTVDM64_CORE_EXPERIMENT`、
   外部 source/include/archive 路径及其 probe target；WHPX/mantle/早期实验 target
   与 CTest 也已移出当前配置图。还必须移除 OpenNT CMake 对 OpenNT-4.5、NTVDMx64
   或其他比较树的任何 source/include/link 路径。验证必须证明默认 configure、target
   列表和 CTest 均不再出现这些名称。

M92 完成前，禁止以“能编译”或“fixture 已到达”推断某 shim/overlay 可进入正式
SoftPC runtime。任何新增或改变的本地文件必须先通过本表的 caller-first 分类。
