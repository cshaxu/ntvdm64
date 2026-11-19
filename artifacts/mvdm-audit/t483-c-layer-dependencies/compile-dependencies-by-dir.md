# C 层直接向外编译依赖逐文件清单

按目录、文件名升序，大小写不敏感。范围仅 C 对 D/E/F 的源码直接包含和编译强制包含。A/B 依赖不计入。链接符号和仅传递包含另行跟踪。

共 116 个文件，其中 110 个实现文件、6 个头文件。mvdm-host 59 个，non-mvdm 57 个。依据 P9 当前产品编译记录。清单次序用于后续逐文件审查，列入清单不代表审查完成。

## 001 — src/mvdm-host/dos/command/cmdconf.c

- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

001 核查：仅使用 NtvdmGetWindowsDirectoryA。原版 cmdconf.c:209 查询宿主 Windows 目录，当前函数读取 session_mvdm_system_root。结果用于 SHELL、COUNTRY 与 SystemRoot 展开。包目录绑定仍需解决，C0039 保持开放。未修改源码，镜像变化 0 点、0 行。

001 修正后的归属：目录兼容查询及长度、编码和系统目录拼接由 B compat 提供，C cmdconf.c 保留原版配置处理。F 选择目录并负责其生命周期。生产 app_package_layout_set_process_media_roots 经 common_system_root_a 从已加载 EXE 推导根目录；显式 setter 仍为已有接口和夹具所用，不应以 EXE 推导替换所有查询。

001 具体接线候选：B 持有当前线程的只读目录上下文，提供现有 NtvdmGetWindowsDirectoryA/W 与 NtvdmGetSystemDirectoryA/W 同形接口。F 通过现有 session_register_thread_hook 在 bind 时绑定目录、unbind 时解除，复用 session 的失败回滚和线程计数。B 不包含 session、common 或任何外层头，也不回调 F 取目录。上下文所指存储的寿命必须覆盖绑定，禁止额外维护第二份可变根目录。需验证无绑定失败、显式根、两个线程不同根、解绑后查询、短缓冲和编码转换。当前仅设计，未实施；镜像变化 0 点、0 行。

## 002 — src/mvdm-host/dos/command/cmdexec.c

- 直接包含 · C0024 · ntvdm-exe · src/ntvdm-exe/command/include/mvdm_command_native_child.h
- 直接包含 · C0036 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_shadow_registry.h

## 003 — src/mvdm-host/dos/command/cmdkeyb.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

## 004 — src/mvdm-host/dos/command/cmdmisc.c

- 直接包含 · C0034 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_guest_location.h
- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

## 005 — src/mvdm-host/dos/dem/dem.c

- 直接包含 · C0039 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_firmware.h

## 006 — src/mvdm-host/dos/dem/dem.h

- 直接包含 · C0034 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_guest_location.h

## 007 — src/mvdm-host/dos/dem/demgset.c

- 直接包含 · C0017 · common · src/common/wow/registry.h
- 直接包含 · C0023 · ntvdm-exe · src/ntvdm-exe/command/include/mvdm_command_guest_directory.h

## 008 — src/mvdm-host/dos/dem/demsrch.c

- 直接包含 · C0027 · ntvdm-exe · src/ntvdm-exe/session/dem_restart_hint.h

## 009 — src/mvdm-host/dpmi32/i386/dpmi386.c

- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h

## 010 — src/mvdm-host/dpmi32/modesw.c

- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h

## 011 — src/mvdm-host/softpc.new/base/bios/virtual.c

- 直接包含 · C0043 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_termination.h
- 直接包含 · C0047 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_vm_bridge.h

## 012 — src/mvdm-host/softpc.new/base/ccpu386/c_main.c

- 直接包含 · C0037 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_ccpu_wait.h

## 013 — src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c

- 直接包含 · C0041 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h

## 014 — src/mvdm-host/softpc.new/base/ccpu386/cpu4gen.h

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h

## 015 — src/mvdm-host/softpc.new/base/ccpu386/sascdef.c

- 直接包含 · C0030 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/PigReg_c.h

## 016 — src/mvdm-host/softpc.new/base/cpu/src/evid/vglob.c

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h

## 017 — src/mvdm-host/softpc.new/base/cvidc/ev_glue.c

- 直接包含 · C0033 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_cvidc_vector_binding.h

## 018 — src/mvdm-host/softpc.new/base/inc/sas.h

- 直接包含 · C0032 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h

## 019 — src/mvdm-host/softpc.new/base/keymouse/keyba.c

- 直接包含 · C0035 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_keyboard_history.h

## 020 — src/mvdm-host/softpc.new/base/keymouse/mouse_io.c

- 直接包含 · C0049 · ntvdm-exe · src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h

## 021 — src/mvdm-host/softpc.new/host/src/config.c

- 直接包含 · C0017 · common · src/common/wow/registry.h

## 022 — src/mvdm-host/softpc.new/host/src/nt_bop.c

- 直接包含 · C0031 · ntvdm-exe · src/ntvdm-exe/softpc/include/generated/x86/prod/gdpvar.h
- 直接包含 · C0045 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h

## 023 — src/mvdm-host/softpc.new/host/src/nt_emm.c

- 直接包含 · C0040 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h
- 直接包含 · C0041 · ntvdm-exe · src/ntvdm-exe/softpc/include/mvdm_softpc_physical_mapping.h

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
