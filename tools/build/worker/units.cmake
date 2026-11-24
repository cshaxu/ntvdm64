# Explicit admitted composition; no cached graph is read.
worker_unit("obj/vcdex/vcdex.obj" "${NTVDM_REPO}/src/mvdm-host/vdd/samples/mscdex/vdd/vcdex.c" "${profile_18}")
worker_resource("obj/vcdex/vcdex.res" "${NTVDM_REPO}/src/mvdm-host/vdd/samples/mscdex/vdd/vcdex.rc" "/nologo")
worker_link("VCDEX.DLL" "/nologo /dll /machine:x86 /entry:VDDInitialize@12 /base:0x2000000 /map:$out.map /def:${NTVDM_REPO}/src/mvdm-host/vdd/samples/mscdex/vdd/vcdex.def /implib:$out.lib /out:$out" "kernel32.lib user32.lib"
  INPUTS "obj/vcdex/vcdex.obj" "obj/vcdex/vcdex.res" "ntvdm.lib" "common-root.lib"
  DEPENDS "${NTVDM_REPO}/src/mvdm-host/vdd/samples/mscdex/vdd/vcdex.def")
worker_unit("obj/cvidc/vglob.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/cpu/src/evid/vglob.c" "${profile_5}")
worker_unit("obj/ntvdm/session/session.obj" "${NTVDM_REPO}/src/ntvdm-exe/session/session.c" "${profile_19}")
worker_unit("obj/tests/cpu40_descriptor_domain_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/dpmi/cpu40_descriptor_domain_fixture.c" "${profile_12}")
worker_unit("obj/tests/rtl_x86_fixture.obj" "${NTVDM_REPO}/test/opennt-host/rtl_x86_fixture.c" "${profile_1}")
worker_unit("obj/tests/environment_projection_fixture.obj" "${NTVDM_REPO}/test/opennt-host/environment_projection_fixture.c" "${profile_1}")
worker_unit("obj/tests/cvidc_vector_binding_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/cvidc_vector_binding_fixture.c" "${profile_7}")
worker_unit("obj/tests/cvidc_sas_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/cvidc_sas_fixture.c" "${profile_5}")
worker_unit("obj/tests/x87_layout_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/x87_layout_fixture.c" "${profile_1}")
worker_unit("obj/tests/cvidc_vector_provider_stubs.obj" "generated/cvidc_vector_provider_stubs.c" "${profile_1}")
worker_unit("obj/base-debug/debuggng.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/debuggng.c" "${profile_1}")
worker_unit("obj/base-debug/ega_dump.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/ega_dump.c" "${profile_1}")
worker_unit("obj/base-debug/btrace.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/btrace.c" "${profile_1}")
worker_unit("obj/base-debug/trace.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/trace.c" "${profile_1}")
worker_unit("obj/base-debug/profile.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/profile.c" "${profile_1}")
worker_unit("obj/base-debug/pigyoda.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/pigyoda.c" "${profile_1}")
worker_unit("obj/base-debug/yoda.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/yoda.c" "${profile_1}")
worker_unit("obj/base-debug/dasm.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/debug/dasm.c" "${profile_1}")
worker_unit("obj/host/ntvdm_entry.obj" "${NTVDM_REPO}/src/mvdm-host/softpc.new/obj.vdm/ntvdm.c" "${profile_13}")
worker_resource("obj/host/softpc-resource.res" "${CMAKE_BINARY_DIR}/generated/softpc-embedded-roms.rc" "/nologo /d PROD /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/ds/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/net/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/private/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/ds/netapi/rpcxlate\" /I \"${NTVDM_REPO}/src/non-mvdm/private/windows/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/ddk/inc\" /I \"${NTVDM_REPO}/src/mvdm-host/inc\" /I \"${NTVDM_REPO}/src/compat/include/generated/x86/prod\" /I \"${NTVDM_REPO}/src/mvdm-host/xms.486\" /I \"${NTVDM_REPO}/src/mvdm-host/dpmi32\" /I \"${NTVDM_REPO}/src/mvdm-host/vdmredir\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/ccpu386\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/host/inc\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/cvidc\" /I \"${NTVDM_REPO}/src/mvdm-host/dos/dem\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/inc\"")
worker_unit("obj/adapter-win32/console_client.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/console_client.c" "${profile_19}")
worker_unit("obj/adapter-win32/worker_lifecycle.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/worker_lifecycle.c" "${profile_19}")
worker_unit("obj/adapter-win32/command_process_compat.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/command_process_compat.c" "${profile_19}")
worker_unit("obj/adapter-win32/wow_private_unavailable.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/wow_private_unavailable.c" "${profile_19}")
worker_unit("obj/adapter-win32/wow_hard_error_dialog.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/wow_hard_error_dialog.c" "${profile_19}")
worker_unit("obj/adapter-win32/wow_hard_error_decoder.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/wow_hard_error_decoder.c" "${profile_19}")
worker_unit("obj/adapter-win32/mvdm_base_vdm_environment.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/mvdm_base_vdm_environment.c" "${profile_19}")
worker_unit("obj/opennt-abi-host-compat/opennt_support_rtl.obj" "${NTVDM_REPO}/src/compat/opennt_support_rtl.c" "${profile_20}")
worker_archive("common-opennt-compat.lib"
  "obj/opennt-abi-host-compat/opennt_support_rtl.obj")
worker_unit("obj/opennt-abi-host-compat/console_grid.obj" "${NTVDM_REPO}/src/compat/console_grid.c" "${profile_21}")
worker_unit("obj/adapter-wow-worker/wow_user_session_binding.obj" "${NTVDM_REPO}/src/compat/wow_user_session_binding.c" "${profile_22}")
worker_unit("obj/adapter-win32/console_compat.obj" "${NTVDM_REPO}/src/compat/console_compat.c" "${profile_22}")
worker_unit("obj/adapter-wow-worker/wow_gdi_alias.obj" "${NTVDM_REPO}/src/compat/wow_gdi_alias.c" "${profile_22}")
worker_unit("obj/adapter-win32/ntioapi_facade.obj" "${NTVDM_REPO}/src/compat/ntioapi_facade.c" "${profile_22}")
worker_unit("obj/adapter-win32/dem_restart_hint.obj" "${NTVDM_REPO}/src/compat/dem_restart_hint.c" "${profile_22}")
worker_unit("obj/adapter-win32/invalid_virtual_instance.obj" "${NTVDM_REPO}/src/compat/invalid_virtual_instance.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_softpc_ccpu_wait.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_ccpu_wait.c" "${profile_22}")
worker_unit("obj/lib-win32/crt-compat/crt_compat.obj" "${NTVDM_REPO}/src/lib-win32/crt-compat/crt_compat.c" "${profile_23}")
worker_archive("lib-crt-compat.lib"
  "obj/lib-win32/crt-compat/crt_compat.obj")
worker_unit("obj/lib-win32/thread/thread.obj" "${NTVDM_REPO}/src/lib-win32/thread/thread.c" "${profile_23}")
worker_archive("lib-thread.lib"
  "obj/lib-win32/thread/thread.obj")
worker_unit("obj/tests/lib_thread.obj" "${NTVDM_REPO}/test/lib-win32/thread/thread_test.c" "${profile_1}")
worker_link("lib-thread-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/lib_thread.obj" "lib-thread.lib")
worker_unit("obj/compat/thread_start.obj" "${NTVDM_REPO}/src/compat/thread_start.c" "${profile_24}")
worker_archive("compat-worker-thread.lib"
  "obj/compat/thread_start.obj")
worker_unit("obj/adapter-basesrv/mvdm_command_guest_state.obj" "${NTVDM_REPO}/src/compat/mvdm_command_guest_state.c" "${profile_22}")
worker_unit("obj/adapter-basesrv/mvdm_command_launch.obj" "${NTVDM_REPO}/src/compat/mvdm_command_launch.c" "${profile_22}")
worker_unit("obj/adapter-basesrv/mvdm_command_launch_request.obj" "${NTVDM_REPO}/src/compat/mvdm_command_launch_request.c" "${profile_22}")
worker_unit("obj/adapter-basesrv/mvdm_command_native_child.obj" "${NTVDM_REPO}/src/compat/mvdm_command_native_child.c" "${profile_22}")
worker_unit("obj/adapter-basesrv/mvdm_command_binding.obj" "${NTVDM_REPO}/src/ntvdm-exe/command/source/mvdm_command_binding.c" "${profile_25}")
worker_unit("obj/adapter-monitor/mvdm_vdm_tib.obj" "${NTVDM_REPO}/src/compat/mvdm_vdm_tib.c" "${profile_23}")
worker_unit("obj/compat/dbg_registers.obj" "${NTVDM_REPO}/src/compat/dbg_registers.c" "${profile_24}")
worker_unit("obj/mvdm-debugger/dbg.obj" "${NTVDM_REPO}/src/mvdm-host/dbg/dbg.c" "${profile_1}")
worker_unit("obj/adapter-softpc/mvdm_vm_binding.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_binding.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_dpmi_binding.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_dpmi_binding.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_softpc_firmware.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_firmware.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_a20.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_a20.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_softpc_guest_memory.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_softpc_execution.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_softpc_execution.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_vm_notification.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_notification.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_bridge.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_bridge.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_wake.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_wake.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_continuation.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_continuation.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_guest_io.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_guest_io.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_input.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_input.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_input_bop.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_input_bop.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_vm_execute.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_vm_execute.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_vm_io.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_vm_io.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_vm_executor.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_vm_executor.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_vm_ingress.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_ingress.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_standalone_worker.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_standalone_worker.c" "${profile_19}")
worker_unit("obj/adapter-softpc/ntvdm_dos_observer.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/ntvdm_dos_observer.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_softpc_presentation_font.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/mvdm_softpc_presentation_font.c" "${profile_19}")
worker_unit("obj/adapter-softpc/mvdm_softpc_text_video.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_text_video.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_keyboard_reset.obj" "${NTVDM_REPO}/src/compat/mvdm_keyboard_reset.c" "${profile_24}")
worker_unit("obj/adapter-softpc/mvdm_softpc_mouse_input.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_mouse_input.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_softpc_mouse_bridge.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_mouse_bridge.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_softpc_mouse_guest.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_mouse_guest.c" "${profile_22}")
worker_unit("obj/adapter-softpc/v7_pointer_binding.obj" "${NTVDM_REPO}/src/ntvdm-exe/softpc/v7_pointer_binding.c" "${profile_26}" "generated/v7_selected_plane.h")
worker_unit("obj/adapter-softpc/mvdm_softpc_descriptor_fields.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_descriptor_fields.c" "${profile_24}")
worker_unit("obj/compat/mvdm_softpc_mouse_context.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_mouse_context.c" "${profile_22}")
worker_unit("obj/compat/mvdm_softpc_wow_page_domain.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_wow_page_domain.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_shadow_registry.obj" "${NTVDM_REPO}/src/compat/mvdm_shadow_registry.c" "${profile_22}")
worker_unit("obj/compat/pif_path.obj" "${NTVDM_REPO}/src/compat/pif_path.c" "${profile_22}")
worker_unit("obj/compat/guest_location.obj" "${NTVDM_REPO}/src/compat/guest_location.c" "${profile_24}")
worker_unit("obj/compat/dpmi_services.obj" "${NTVDM_REPO}/src/compat/dpmi_services.c" "${profile_24}")
worker_unit("obj/compat/mvdm_cpu_access.obj" "${NTVDM_REPO}/src/compat/mvdm_cpu_access.c" "${profile_24}")
worker_unit("obj/compat/mvdm_vm_command_entry.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_command_entry.c" "${profile_24}")
worker_unit("obj/compat/mvdm_vm_application.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_application.c" "${profile_24}")
worker_unit("obj/compat/mvdm_vm_io.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_io.c" "${profile_24}")
worker_unit("obj/compat/mvdm_vm_capture.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_capture.c" "${profile_24}")
worker_unit("obj/compat/mvdm_softpc_termination.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_termination.c" "${profile_24}")
worker_unit("obj/compat/wow_hard_error.obj" "${NTVDM_REPO}/src/compat/wow_hard_error.c" "${profile_24}")
worker_unit("obj/compat/host_context.obj" "${NTVDM_REPO}/src/compat/host_context.c" "${profile_24}")
worker_unit("obj/compat/wow_worker_state.obj" "${NTVDM_REPO}/src/compat/wow_worker_state.c" "${profile_24}")
worker_unit("obj/compat/guest_memory_lease.obj" "${NTVDM_REPO}/src/compat/guest_memory_lease.c" "${profile_24}")
worker_unit("obj/compat/wow_user_runtime.obj" "${NTVDM_REPO}/src/compat/wow_user_runtime.c" "${profile_24}")
worker_unit("obj/compat/mvdm_softpc_observation.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_observation.c" "${profile_24}")
worker_unit("obj/compat/wow_shared_clock.obj" "${NTVDM_REPO}/src/compat/wow_shared_clock.c" "${profile_24}")
worker_unit("obj/compat/wow_window_graph.obj" "${NTVDM_REPO}/src/compat/wow_window_graph.c" "${profile_24}")
worker_unit("obj/compat/physical_mapping.obj" "${NTVDM_REPO}/src/compat/physical_mapping.c" "${profile_24}")
worker_unit("obj/compat/mvdm_firmware.obj" "${NTVDM_REPO}/src/compat/mvdm_firmware.c" "${profile_22}")
worker_unit("obj/compat/wow_start_glass.obj" "${NTVDM_REPO}/src/compat/wow_start_glass.c" "${profile_22}")
worker_unit("obj/compat/softpc_xt_storage.obj" "${NTVDM_REPO}/src/compat/softpc_xt_storage.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_keyboard_history.obj" "${NTVDM_REPO}/src/compat/mvdm_keyboard_history.c" "${profile_22}")
worker_unit("obj/adapter-softpc/mvdm_softpc_event_thread.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_event_thread.c" "${profile_22}")
worker_unit("obj/compat/mvdm_softpc_worker_exit.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_worker_exit.c" "${profile_22}")
worker_unit("obj/compat/mvdm_redirector_guest_copy.obj" "${NTVDM_REPO}/src/compat/mvdm_redirector_guest_copy.c" "${profile_27}")
worker_unit("obj/compat/remote_browser.obj" "${NTVDM_REPO}/src/compat/remote_browser.c" "${profile_22}")
worker_unit("obj/compat/remote_rap.obj" "${NTVDM_REPO}/src/compat/remote_rap.c" "${profile_22}")
worker_unit("obj/adapter-redir/mvdm_redirector_worker_copy.obj" "${NTVDM_REPO}/src/ntvdm-exe/redir/mvdm_redirector_worker_copy.c" "${profile_19}")
worker_unit("obj/compat/mvdm_redirector_async.obj" "${NTVDM_REPO}/src/compat/mvdm_redirector_async.c" "${profile_22}")
worker_unit("obj/opennt-rtl-x86/largeint.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/x86/largeint.asm" "/nologo /c /coff")
worker_unit("obj/opennt-rtl-x86/movemem.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/x86/movemem.asm" "/nologo /c /coff")
worker_unit("obj/opennt-rtl/environ.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/environ.c" "${profile_28}")
worker_unit("obj/opennt-rtl/error.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/error.c" "${profile_29}")
worker_unit("obj/kernel-vdm/mvdm_monitor_printer.obj" "${NTVDM_REPO}/src/compat/mvdm_monitor_printer.c" "${profile_23}")
worker_unit("obj/opennt-netlib/ntstatus.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/ntstatus.c" "${profile_30}")
worker_unit("obj/opennt-netlib/copystr.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/copystr.c" "${profile_30}")
worker_unit("obj/opennt-netlib/allocstr.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/allocstr.c" "${profile_31}")
worker_unit("obj/opennt-netlib/initoem.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/initoem.c" "${profile_30}")
worker_unit("obj/opennt-netlib/memalloc.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/memalloc.c" "${profile_30}")
worker_unit("obj/opennt-netlib/time.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/time.c" "${profile_30}")
worker_unit("obj/opennt-netlib/timezone.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/timezone.c" "${profile_30}")
worker_unit("obj/opennt-netlib/xlatesvc.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/xlatesvc.c" "${profile_30}")
worker_unit("obj/opennt-netlib/svcinfo.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/svcinfo.c" "${profile_30}")
worker_unit("obj/opennt-netlib/packstr.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/packstr.c" "${profile_30}")
worker_unit("obj/opennt-xactsrv/apiwksta.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/apiwksta.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/apiuse.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/apiuse.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/apimsg.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/apimsg.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/apisvc.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/apisvc.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/apisrv.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/apisrv.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/xssubs.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/xssubs.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/strucsiz.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/strucsiz.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/examine.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/examine.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/ascii.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/ascii.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/pointer.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/pointer.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/convert.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/convert.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/xsunicod.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv/xsunicod.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/auxdata.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/auxdata.c" "${profile_32}")
worker_unit("obj/opennt-xactsrv/parmnum.obj" "${NTVDM_REPO}/src/non-mvdm/ds/netapi/rap/parmnum.c" "${profile_32}")
worker_unit("obj/opennt-base-vdm/vdm.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c" "${profile_30}")
worker_unit("obj/opennt-base-client/client.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c" "${profile_33}")
worker_unit("obj/opennt-base-client/classifier.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c" "${profile_34}")
worker_unit("obj/opennt-base-client/capture.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntdll/csrutil.c" "${profile_35}")
worker_archive("opennt-base-client.lib"
  "obj/opennt-base-client/client.obj" "obj/opennt-base-client/classifier.obj" "obj/opennt-base-client/capture.obj")
worker_unit("obj/opennt-base-server/srvvdm.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/server/srvvdm.c" "${profile_36}")
worker_unit("obj/opennt-base-server/exports.obj" "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/server/exports.c" "${profile_37}")
worker_archive("opennt-base-server.lib"
  "obj/opennt-base-server/srvvdm.obj" "obj/opennt-base-server/exports.obj")
worker_unit("obj/opennt-base-bindings/service.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/base_service.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/service-core.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/service_core.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/worker-registry.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/worker_registry.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/frontend-registry.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/frontend_registry.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/native-commands.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/native_commands.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/service-lifecycle.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/lifecycle.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/service-management.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/management.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/task-observation.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/task_observation.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/worker-spawn.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/worker_spawn.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/command.obj" "${NTVDM_REPO}/src/common/base_srv/base_command.c" "${profile_39}")
worker_unit("obj/opennt-base-bindings/values.obj" "${NTVDM_REPO}/src/common/base_srv/base_values.c" "${profile_39}")
worker_unit("obj/opennt-base-bindings/payload.obj" "${NTVDM_REPO}/src/common/base_srv/base_payload.c" "${profile_39}")
worker_unit("obj/opennt-base-bindings/startup.obj" "${NTVDM_REPO}/src/common/base_srv/base_startup.c" "${profile_39}")
worker_unit("obj/opennt-base-bindings/dispatch.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/base_dispatch.c" "${profile_38}")
worker_unit("obj/opennt-base-bindings/resources.obj" "${NTVDM_REPO}/src/compat/base_resource.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/resource-scope.obj" "${NTVDM_REPO}/src/compat/base_resource_scope.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/streams.obj" "${NTVDM_REPO}/src/compat/base_stream.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/waits.obj" "${NTVDM_REPO}/src/compat/base_wait.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/registry.obj" "${NTVDM_REPO}/src/compat/base_process.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/reservation.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/base_reservation.c" "${profile_40}")
worker_unit("obj/opennt-base-bindings/request.obj" "${NTVDM_REPO}/src/compat/base_request.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/config.obj" "${NTVDM_REPO}/src/compat/base_vdm_config.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/capture-heap.obj" "${NTVDM_REPO}/src/compat/base_capture_heap.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/process.obj" "${NTVDM_REPO}/src/compat/base_console_handle.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/interactive.obj" "${NTVDM_REPO}/src/compat/base_interactive.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/classifier-path.obj" "${NTVDM_REPO}/src/compat/base_classifier_path.c" "${profile_22}")
worker_unit("obj/opennt-base-bindings/config-command.obj" "${CMAKE_BINARY_DIR}/generated/base_vdm_config.c" "${profile_37}" "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c")
worker_archive("common-base-srv-bindings.lib"
  "obj/opennt-base-bindings/command.obj" "obj/opennt-base-bindings/values.obj" "obj/opennt-base-bindings/payload.obj" "obj/opennt-base-bindings/startup.obj" "obj/opennt-base-bindings/config.obj" "obj/opennt-base-bindings/capture-heap.obj" "obj/opennt-base-bindings/process.obj" "obj/opennt-base-bindings/classifier-path.obj" "obj/opennt-base-bindings/config-command.obj" "obj/common/native_image.obj")
worker_archive("opennt-base-bindings.lib"
  "obj/opennt-base-bindings/service.obj" "obj/opennt-base-bindings/service-core.obj" "obj/opennt-base-bindings/worker-registry.obj" "obj/opennt-base-bindings/frontend-registry.obj" "obj/opennt-base-bindings/native-commands.obj" "obj/opennt-base-bindings/service-lifecycle.obj" "obj/opennt-base-bindings/service-management.obj" "obj/opennt-base-bindings/task-observation.obj" "obj/opennt-base-bindings/worker-spawn.obj" "obj/opennt-base-bindings/dispatch.obj" "obj/opennt-base-bindings/resources.obj" "obj/opennt-base-bindings/resource-scope.obj" "obj/opennt-base-bindings/streams.obj" "obj/opennt-base-bindings/waits.obj" "obj/opennt-base-bindings/registry.obj" "obj/opennt-base-bindings/reservation.obj" "obj/opennt-base-bindings/request.obj" "obj/opennt-base-bindings/interactive.obj")
worker_unit("obj/broker-transport/rpc_security.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/rpc_security.c" "${profile_41}")
worker_unit("obj/broker-transport/rpc_scope.obj" "${NTVDM_REPO}/src/common/base_srv/scope.c" "${profile_41}")
worker_unit("obj/broker-transport/vdm_receipt.obj" "${NTVDM_REPO}/src/compat/vdm_receipt.c" "${profile_41}")
worker_unit("obj/broker-transport/vdm_delivery.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/vdm_delivery.c" "${profile_41}")
worker_unit("obj/broker-transport/vdm_payload.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_payload.c" "${profile_41}")
worker_unit("obj/broker-transport/vdm_message.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_message.c" "${profile_41}")
worker_archive("common-base-srv-transport.lib"
  "obj/broker-transport/rpc_scope.obj" "obj/broker-transport/vdm_payload.obj" "obj/broker-transport/vdm_message.obj")
worker_archive("broker-transport.lib"
  "obj/broker-transport/rpc_security.obj" "obj/broker-transport/vdm_receipt.obj" "obj/broker-transport/vdm_delivery.obj")
worker_unit("obj/tests/vdm_protocol_test.obj" "${NTVDM_REPO}/test/common/base_srv/vdm_payload_test.c" "${profile_42}")
worker_link("vdm-protocol-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/vdm_protocol_test.obj" "common-base-srv-transport.lib")
worker_group("opennt-broker-owners" "opennt-base-client.lib" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib")
worker_unit("obj/tests/base_reservation_test.obj" "${NTVDM_REPO}/test/adapter-basesrv/base_reservation_test.c" "${profile_43}")
worker_link("basesrv-reservation-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/base_reservation_test.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/tests/base_service_reservation_test.obj" "${NTVDM_REPO}/test/adapter-basesrv/base_service_reservation_test.c" "${profile_44}")
worker_unit("obj/tests/original_classifier.obj" "${NTVDM_REPO}/test/broker/original_classifier.c" "${profile_44}")
worker_link("original-classifier-test.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/original_classifier.obj" "obj/run16/support.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/original_server_lifecycle.obj" "${NTVDM_REPO}/test/broker/original_server_lifecycle.c" "${profile_44}")
worker_link("original-server-lifecycle-test.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/original_server_lifecycle.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-client.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_group("broker-original-fixtures" "original-classifier-test.exe" "original-server-lifecycle-test.exe")
worker_unit("obj/tests/base_service_fixture.obj" "${NTVDM_REPO}/test/adapter-basesrv/base_service_fixture.c" "${profile_44}")
worker_link("basesrv-service-reservation-test.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/base_service_reservation_test.obj" "obj/tests/base_service_fixture.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/tests/native_observation_fixture.obj" "${NTVDM_REPO}/test/adapter-basesrv/native_observation_fixture.c" "${profile_44}")
worker_link("native-observation-fixture.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/native_observation_fixture.obj" "obj/tests/base_service_fixture.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/tests/dos_observation_fixture.obj" "${NTVDM_REPO}/test/adapter-basesrv/dos_observation_fixture.c" "${profile_44}")
worker_link("dos-observation-fixture.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/dos_observation_fixture.obj" "obj/tests/base_service_fixture.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/tests/basesrv_idle_policy.obj" "${NTVDM_REPO}/test/adapter-basesrv/idle_policy_test.c" "${profile_45}")
worker_link("basesrv-idle-policy-test.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/basesrv_idle_policy.obj")
worker_unit("obj/tests/native_service_layout.obj" "${NTVDM_REPO}/test/adapter-basesrv/native_service_layout_test.c" "${profile_44}")
worker_link("native-service-layout-test.exe" "/nologo /out:$out" "ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/native_service_layout.obj")
worker_unit("obj/tests/base_client_rpc_first.obj" "${NTVDM_REPO}/test/integration/base_client_rpc_first_test.c" "${profile_46}")
worker_link("base-client-rpc-first-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/base_client_rpc_first.obj" "obj/ntvwm/next_command.obj" "worker-base.lib" "frontend-client.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-handle.lib"
  DEPENDS "ntsrv.exe" "ntvwm.exe")
worker_unit("obj/frontend/window/window_frame.obj" "${NTVDM_REPO}/src/ntcon-exe/window_frame.c" "${profile_48}")
worker_unit("obj/frontend/window/window_controller.obj" "${NTVDM_REPO}/src/ntcon-exe/window_controller.c" "${profile_48}")
worker_unit("obj/frontend/window/window_input_queue.obj" "${NTVDM_REPO}/src/ntcon-exe/window_input_queue.c" "${profile_48}")
worker_unit("obj/frontend/window/window_keyboard.obj" "${NTVDM_REPO}/src/ntcon-exe/window_keyboard.c" "${profile_48}")
worker_unit("obj/frontend/window/window_mouse.obj" "${NTVDM_REPO}/src/ntcon-exe/window_mouse.c" "${profile_48}")
worker_archive("frontend-window.lib"
  "${nxvm_objects}" "obj/frontend/window/window_frame.obj" "obj/frontend/window/window_controller.obj" "obj/frontend/window/window_input_queue.obj" "obj/frontend/window/window_keyboard.obj" "obj/frontend/window/window_mouse.obj")
worker_unit("obj/tests/console_title_native.obj" "${NTVDM_REPO}/test/app/console_title_native_test.c" "${profile_49}")
worker_link("console-title-native-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/console_title_native.obj")
worker_unit("obj/tests/console_keyboard_layout_native.obj" "${NTVDM_REPO}/test/app/console_keyboard_layout_native_test.c" "${profile_49}")
worker_link("console-keyboard-layout-native-test.exe" "/nologo /out:$out" "kernel32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/console_keyboard_layout_native.obj")
worker_unit("obj/tests/console_graphics_native.obj" "${NTVDM_REPO}/test/app/console_graphics_native_test.c" "${profile_50}")
worker_link("console-graphics-native-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/console_graphics_native.obj")
worker_unit("obj/tests/console_bitmap.obj" "${NTVDM_REPO}/test/app/console_bitmap_test.c" "${profile_49}")
worker_unit("obj/tests/console_bitmap_provider.obj" "${NTVDM_REPO}/src/compat/console_bitmap.c" "${profile_49}")
worker_link("console-bitmap-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib gdi32.lib"
  INPUTS "obj/tests/console_bitmap.obj" "obj/tests/console_bitmap_provider.obj")
worker_unit("obj/tests/console_graphics_provider.obj" "${NTVDM_REPO}/src/compat/console_graphics.c" "${profile_50}")
worker_unit("obj/tests/v7_pointer.obj" "${NTVDM_REPO}/src/compat/v7_pointer.c" "${profile_49}")
worker_unit("obj/tests/v7_pointer_test.obj" "${NTVDM_REPO}/test/app/v7_pointer_test.c" "${profile_49}")
worker_link("v7-pointer-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib gdi32.lib"
  INPUTS "obj/tests/v7_pointer_test.obj" "obj/tests/v7_pointer.obj")
worker_unit("obj/tests/v7_pointer_binding_test.obj" "${NTVDM_REPO}/test/ntvdm-exe/softpc/v7_pointer_binding_test.c" "${profile_49}")
worker_link("v7-pointer-binding-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib gdi32.lib"
  INPUTS "obj/tests/v7_pointer_binding_test.obj" "obj/adapter-softpc/v7_pointer_binding.obj")
worker_unit("obj/tests/v7_graphics_test.obj" "${NTVDM_REPO}/test/app/v7_graphics_test.c" "${profile_50}")
worker_unit("obj/tests/frontend_scope_lifetime.obj" "${NTVDM_REPO}/test/app/frontend_scope_lifetime_test.c" "${profile_49}")
worker_link("frontend-scope-lifetime-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib"
  INPUTS "obj/tests/frontend_scope_lifetime.obj" "obj/run16/frontend_scope.obj" "obj/frontend/session_service.obj" "frontend-base.lib" "common-native-hook-context.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib")
worker_unit("obj/tests/native_lifetime_pair.obj" "${NTVDM_REPO}/test/app/native_lifetime_pair_test.c" "${profile_49}")
worker_link("native-lifetime-pair-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/native_lifetime_pair.obj")
worker_link("native-lifetime-pair-target.exe" "/nologo /subsystem:windows /entry:mainCRTStartup /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/native_lifetime_pair.obj")
worker_unit("obj/run16/entry.obj" "${NTVDM_REPO}/src/run16-exe/main.c" "${profile_39}")
worker_unit("obj/common/application_search.obj" "${NTVDM_REPO}/src/common/application_search.c" "${profile_49}")
worker_unit("obj/tests/application_search.obj" "${NTVDM_REPO}/test/common/application_search_test.c" "${profile_49}")
worker_link("application-search-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/application_search.obj" "obj/common/application_search.obj")
worker_unit("obj/common/guest_environment.obj" "${NTVDM_REPO}/src/compat/guest_environment.c" "${profile_22}")
worker_unit("obj/tests/guest_environment.obj" "${NTVDM_REPO}/test/compat/guest_environment_test.c" "${profile_49}")
worker_link("guest-environment-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/guest_environment.obj" "obj/common/guest_environment.obj" "obj/common/system_root.obj")
worker_unit("obj/worker-base/connection.obj" "${NTVDM_REPO}/src/worker-base/connection.c" "${profile_49}")
worker_unit("obj/worker-base/shutdown_close.obj" "${NTVDM_REPO}/src/worker-base/shutdown_close.c" "${profile_49}")
worker_unit("obj/ntvwm/next_command.obj" "${NTVDM_REPO}/src/ntvwm-exe/next_command.c" "${profile_49}")
worker_unit("obj/worker-base/publication.obj" "${NTVDM_REPO}/src/compat/publication.c" "${profile_22}")
worker_unit("obj/worker-base/input_watch.obj" "${NTVDM_REPO}/src/worker-base/input_watch.c" "${profile_49}")
worker_unit("obj/worker-base/input_readiness.obj" "${NTVDM_REPO}/src/compat/input_readiness.c" "${profile_22}")
worker_unit("obj/worker-base/task_observation.obj" "${NTVDM_REPO}/src/worker-base/task_observation.c" "${profile_49}")
worker_archive("worker-base.lib"
  "obj/worker-base/connection.obj" "obj/worker-base/shutdown_close.obj" "obj/worker-base/publication.obj" "obj/worker-base/input_watch.obj" "obj/worker-base/input_readiness.obj" "obj/worker-base/task_observation.obj")
worker_unit("obj/tests/worker_task_observation.obj" "${NTVDM_REPO}/test/app/worker_task_observation_test.c" "${profile_49}")
worker_link("worker-task-observation-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/worker_task_observation.obj" "worker-base.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/worker_input_watch.obj" "${NTVDM_REPO}/test/worker-base/worker_input_watch_test.c" "${profile_49}")
worker_link("worker-input-watch-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/worker_input_watch.obj" "worker-base.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/worker_shutdown.obj" "${NTVDM_REPO}/test/app/worker_shutdown_test.c" "${profile_49}")
worker_link("worker-shutdown-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/worker_shutdown.obj" "worker-base.lib" "frontend-client.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/lib-win32/pipe/pipe_transfer.obj" "${NTVDM_REPO}/src/lib-win32/pipe/pipe_transfer.c" "${profile_22}")
worker_unit("obj/lib-win32/pipe/pipe_pair.obj" "${NTVDM_REPO}/src/lib-win32/pipe/pipe_pair.c" "${profile_22}")
worker_archive("lib-pipe.lib"
  "obj/lib-win32/pipe/pipe_transfer.obj" "obj/lib-win32/pipe/pipe_pair.obj")
worker_unit("obj/tests/win32_pipe_pair.obj" "${NTVDM_REPO}/test/lib-win32/pipe/pipe_pair_test.c" "${profile_49}")
worker_link("win32-pipe-pair-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_pipe_pair.obj" "lib-pipe.lib")
worker_unit("obj/common/system_root.obj" "${NTVDM_REPO}/src/compat/system_root.c" "${profile_22}")
worker_archive("common-root.lib"
  "obj/common/system_root.obj" "obj/common/guest_environment.obj")
worker_unit("obj/tests/common_system_root.obj" "${NTVDM_REPO}/test/compat/system_root_test.c" "${profile_49}")
worker_link("common-system-root-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/common_system_root.obj" "common-root.lib")
worker_unit("obj/lib-win32/process-observation/console_members.obj" "${NTVDM_REPO}/src/lib-win32/process-observation/console_members.c" "${profile_22}")
worker_archive("common-console.lib"
  "obj/common/console_client.obj")
worker_unit("obj/lib-win32/rpc/local_binding.obj" "${NTVDM_REPO}/src/lib-win32/rpc/local_binding.c" "${profile_22}")
worker_unit("obj/lib-win32/rpc/peer.obj" "${NTVDM_REPO}/src/lib-win32/rpc/peer.c" "${profile_22}")
worker_archive("lib-rpc.lib"
  "obj/lib-win32/rpc/local_binding.obj" "obj/lib-win32/rpc/peer.obj")
worker_unit("obj/lib-win32/process-launch/attributes.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/attributes.c" "${profile_22}")
worker_unit("obj/lib-win32/process-launch/process.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/process.c" "${profile_22}")
worker_unit("obj/lib-win32/process-launch/command_line.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/command_line.c" "${profile_22}")
worker_archive("lib-process-launch.lib"
  "obj/lib-win32/process-launch/attributes.obj" "obj/lib-win32/process-launch/process.obj" "obj/lib-win32/process-launch/command_line.obj")
worker_unit("obj/tests/ntvdm_command_launch.obj" "${NTVDM_REPO}/test/ntvdm-exe/command/command_launch.c" "${profile_49}")
worker_link("ntvdm-command-launch-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/ntvdm_command_launch.obj" "obj/adapter-win32/command_process_compat.obj" "obj/compat/command_services.obj" "obj/adapter-basesrv/mvdm_command_launch_request.obj" "lib-process-launch.lib")
worker_unit("obj/tests/ntvdm_command_capture.obj" "${NTVDM_REPO}/test/ntvdm-exe/command/command_capture.c" "${profile_49}")
worker_link("ntvdm-command-capture-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib ntdll.lib"
  INPUTS "obj/compat/guest_memory_lease.obj" "obj/compat/host_context.obj" "obj/tests/ntvdm_command_capture.obj" "obj/compat/mvdm_vm_capture.obj" "obj/adapter-basesrv/mvdm_command_native_child.obj" "obj/adapter-basesrv/mvdm_command_binding.obj" "obj/compat/command_services.obj" "obj/compat/guest_location.obj" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "obj/guest-memory-coordination.obj")
worker_unit("obj/tests/ntvdm_command_directory.obj" "${NTVDM_REPO}/test/adapter-mvdm-host-out/basesrv/t394_s4_command_scalar_state_fixture.c" "${profile_51}" "${CMAKE_BINARY_DIR}/command-directory-owner.inc")
worker_link("ntvdm-command-directory-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/ntvdm_command_directory.obj" "obj/adapter-basesrv/mvdm_command_guest_state.obj" "obj/compat/guest_location.obj" "session.lib" "obj/guest-memory-coordination.obj")
worker_unit("obj/tests/vdm_control.obj" "${NTVDM_REPO}/test/mvdm-host/vdm_control_test.c" "${profile_1}")
worker_link("vdm-control-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/vdm_control.obj" "obj/compat/vdm_control.obj")
worker_unit("obj/tests/win32_process_launch.obj" "${NTVDM_REPO}/test/lib-win32/process-launch/process.c" "${profile_49}")
worker_link("win32-process-launch-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_process_launch.obj" "lib-process-launch.lib")
worker_unit("obj/lib-win32/process-image/image.obj" "${NTVDM_REPO}/src/lib-win32/process-image/image.c" "${profile_22}")
worker_archive("lib-process-image.lib"
  "obj/lib-win32/process-image/image.obj")
worker_unit("obj/tests/process_image_machine.obj" "${NTVDM_REPO}/test/lib-win32/process-image/image_machine_test.c" "${profile_49}")
worker_link("process-image-machine-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/process_image_machine.obj" "lib-process-image.lib")
worker_unit("obj/lib-win32/process-observation/observation.obj" "${NTVDM_REPO}/src/lib-win32/process-observation/observation.c" "${profile_22}")
worker_archive("lib-process-observation.lib"
  "obj/lib-win32/process-observation/observation.obj" "obj/lib-win32/process-observation/console_members.obj")
worker_unit("obj/tests/win32_process_observation.obj" "${NTVDM_REPO}/test/lib-win32/process-observation/observation_test.c" "${profile_49}")
worker_link("win32-process-observation-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_process_observation.obj" "lib-process-observation.lib")
worker_unit("obj/tests/win32_process_observation_failure.obj" "${NTVDM_REPO}/test/lib-win32/process-observation/observation_failure_test.c" "${profile_49}")
worker_link("win32-process-observation-failure-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_process_observation_failure.obj")
worker_unit("obj/lib-win32/handle/duplicate.obj" "${NTVDM_REPO}/src/lib-win32/handle/duplicate.c" "${profile_22}")
worker_archive("lib-handle.lib"
  "obj/lib-win32/handle/duplicate.obj")
worker_unit("obj/lib-win32/file/identity.obj" "${NTVDM_REPO}/src/lib-win32/file/identity.c" "${profile_22}")
worker_archive("lib-file.lib"
  "obj/lib-win32/file/identity.obj" "obj/lib-win32/file/native_path.obj")
worker_unit("obj/tests/win32_file_identity.obj" "${NTVDM_REPO}/test/lib-win32/file/identity_test.c" "${profile_49}")
worker_link("win32-file-identity-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_file_identity.obj" "lib-file.lib")
worker_unit("obj/tests/win32_handle.obj" "${NTVDM_REPO}/test/lib-win32/handle/handle.c" "${profile_49}")
worker_link("win32-handle-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_handle.obj" "lib-handle.lib")
worker_unit("obj/tests/win32_process_attributes.obj" "${NTVDM_REPO}/test/lib-win32/process-launch/attributes.c" "${profile_49}")
worker_link("win32-process-attributes-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_process_attributes.obj")
worker_unit("obj/tests/win32_rpc_peer.obj" "${NTVDM_REPO}/test/lib-win32/rpc/peer_test.c" "${profile_49}")
worker_link("win32-rpc-peer-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_rpc_peer.obj")
worker_unit("obj/common/native_command.obj" "${NTVDM_REPO}/src/common/rpc/native_command.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/common/frontend_control.obj" "${NTVDM_REPO}/src/common/rpc/frontend_control.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/common/worker_control.obj" "${NTVDM_REPO}/src/common/rpc/worker_control.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/common/management.obj" "${NTVDM_REPO}/src/common/rpc/management.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/common/dos_observation.obj" "${NTVDM_REPO}/src/common/rpc/dos_observation.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/common/interface_query.obj" "${NTVDM_REPO}/src/common/rpc/interface_query.c" "${profile_49}" "obj/basesrv/rpc_management.h")
worker_unit("obj/common/rpc_management_stub.obj" "obj/basesrv/rpc_management_c.c" "${profile_49}" "obj/basesrv/rpc_management.h")
worker_unit("obj/common/rpc_memory_crt.obj" "${NTVDM_REPO}/src/common/rpc/memory_crt.c" "${profile_49}")
worker_unit("obj/common/rpc_memory_heap.obj" "${NTVDM_REPO}/src/common/rpc/memory_heap.c" "${profile_49}")
worker_archive("common-rpc.lib"
  "obj/common/rpc_memory_crt.obj" "obj/common/native_command.obj" "obj/common/frontend_control.obj" "obj/common/worker_control.obj" "obj/common/management.obj" "obj/common/dos_observation.obj" "obj/common/interface_query.obj" "obj/common/rpc_management_stub.obj")
worker_unit("obj/tests/common_management.obj" "${NTVDM_REPO}/test/common/rpc/management_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("common-management-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/common_management.obj" "common-rpc.lib" "lib-rpc.lib")
worker_unit("obj/tests/rpc_interface_query.obj" "${NTVDM_REPO}/test/common/rpc/interface_query_test.c" "${profile_49}")
worker_link("rpc-interface-query-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/rpc_interface_query.obj" "common-rpc.lib" "lib-rpc.lib")
worker_unit("obj/tests/common_worker_control.obj" "${NTVDM_REPO}/test/common/rpc/worker_control_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("common-worker-control-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/common_worker_control.obj" "common-rpc.lib" "lib-rpc.lib")
worker_unit("obj/tests/common_frontend_control.obj" "${NTVDM_REPO}/test/common/rpc/frontend_control_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("common-frontend-control-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/common_frontend_control.obj" "common-rpc.lib" "lib-rpc.lib")
worker_unit("obj/tests/common_native_command.obj" "${NTVDM_REPO}/test/common/rpc/native_command_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("common-native-command-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/common_native_command.obj" "common-rpc.lib" "lib-rpc.lib")
worker_unit("obj/tests/common_console_members.obj" "${NTVDM_REPO}/test/lib-win32/process-observation/console_members_test.c" "${profile_49}")
worker_link("common-console-members-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/common_console_members.obj")
worker_unit("obj/tests/common_rpc_binding.obj" "${NTVDM_REPO}/test/lib-win32/rpc/local_binding_test.c" "${profile_49}")
worker_link("common-rpc-binding-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/common_rpc_binding.obj")
worker_unit("obj/run16/launch_options.obj" "${NTVDM_REPO}/src/run16-exe/launch_options.c" "${profile_49}")
worker_unit("obj/tests/run16_launch_options.obj" "${NTVDM_REPO}/test/app/run16_launch_options_test.c" "${profile_49}")
worker_link("run16-launch-options-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/run16_launch_options.obj" "obj/run16/launch_options.obj")
worker_unit("obj/tests/run16_gui_wait.obj" "${NTVDM_REPO}/test/app/run16_gui_wait_test.c" "${profile_49}")
worker_link("run16-gui-wait-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/run16_gui_wait.obj")
worker_link("run16-gui-wait-target.exe" "/nologo /subsystem:windows /entry:wmainCRTStartup /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/run16_gui_wait.obj")
worker_unit("obj/run16/support.obj" "${NTVDM_REPO}/src/compat/opennt_support_rtl.c" "${profile_52}")
worker_unit("obj/run16/rpc_client_core.obj" "${NTVDM_REPO}/src/common/base_srv/base_rpc_client_core.c" "${profile_53}" "obj/basesrv/service.h")
worker_unit("obj/run16/rpc_client_launch.obj" "${NTVDM_REPO}/src/common/base_srv/base_rpc_client_launch.c" "${profile_53}" "obj/basesrv/service.h")
worker_unit("obj/common/frontend_rpc_client.obj" "${NTVDM_REPO}/src/common/base_srv/frontend_client.c" "${profile_53}" "obj/basesrv/service.h")
worker_archive("base-rpc-client-core.lib"
  "obj/run16/rpc_client_core.obj" "obj/common/frontend_rpc_client.obj")
worker_unit("obj/compat/base_dispatch.obj" "${NTVDM_REPO}/src/compat/base_dispatch.c" "${profile_54}")
worker_unit("obj/tests/base_dispatch.obj" "${NTVDM_REPO}/test/compat/base-dispatch/base_dispatch_test.c" "${profile_55}")
worker_link("base-dispatch-test.exe" "/nologo /subsystem:console /manifest:embed /out:$out" "kernel32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/base_dispatch.obj" "obj/compat/base_dispatch.obj")
worker_archive("base-rpc-client-launch.lib"
  "obj/run16/rpc_client_launch.obj" "obj/compat/base_dispatch.obj")
worker_unit("obj/run16/stub.obj" "obj/basesrv/service_c.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/frontend/console_frontend.obj" "${NTVDM_REPO}/src/ntcon-exe/console_frontend.c" "${profile_49}")
worker_unit("obj/compat/console_video_buffer.obj" "${NTVDM_REPO}/src/compat/console_video_buffer.c" "${profile_22}")
worker_unit("obj/frontend-base/channel.obj" "${NTVDM_REPO}/src/frontend-base/channel.c" "${profile_49}")
worker_unit("obj/frontend-base/service.obj" "${NTVDM_REPO}/src/frontend-base/service.c" "${profile_49}")
worker_archive("frontend-base.lib"
  "obj/frontend-base/channel.obj" "obj/frontend-base/service.obj" "obj/compat/console_video_buffer.obj")
worker_unit("obj/tests/frontend_base_video.obj" "${NTVDM_REPO}/test/frontend-base/video_test.c" "${profile_49}")
worker_unit("obj/worker/vm_frontend.obj" "${NTVDM_REPO}/src/ntvdm-exe/frontend/vm_frontend.c" "${profile_49}")
worker_unit("obj/worker/vm_host.obj" "${NTVDM_REPO}/src/ntvdm-exe/frontend/vm_host.c" "${profile_49}")
worker_unit("obj/compat/mvdm_vm_text.obj" "${NTVDM_REPO}/src/compat/mvdm_vm_text.c" "${profile_22}")
worker_archive("ntvdm-vm-frontend.lib"
  "obj/worker/vm_frontend.obj" "obj/worker/vm_host.obj" "obj/compat/mvdm_vm_text.obj")
worker_unit("obj/tests/vm_frontend.obj" "${NTVDM_REPO}/test/ntvdm-exe/frontend/vm_frontend_test.c" "${profile_49}")
worker_unit("obj/tests/frontend_base_channel.obj" "${NTVDM_REPO}/test/frontend-base/channel_test.c" "${profile_49}")
worker_unit("obj/frontend/console_channel.obj" "${NTVDM_REPO}/src/ntcon-exe/console_channel.c" "${profile_49}")
worker_unit("obj/run16/frontend_scope.obj" "${NTVDM_REPO}/src/run16-exe/frontend_scope.c" "${profile_49}")
worker_unit("obj/frontend/session_service.obj" "${NTVDM_REPO}/src/ntcon-exe/session_service.c" "${profile_49}")
worker_unit("obj/tests/console_frontend_test.obj" "${NTVDM_REPO}/test/app/console_frontend_test.c" "${profile_49}")
worker_link("v7-graphics-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/v7_graphics_test.obj" "obj/tests/console_graphics_provider.obj" "obj/tests/console_bitmap_provider.obj" "obj/tests/v7_pointer.obj")
worker_unit("obj/tests/ntvwm_next_command.obj" "${NTVDM_REPO}/test/observation/ntvwm_next_command_test.c" "${profile_49}")
worker_link("ntvwm-next-command-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_next_command.obj" "obj/ntvwm/next_command.obj")
worker_unit("obj/tests/frontend_window_library.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_library_test.c" "${profile_56}")
worker_link("frontend-window-library-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_window_library.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj")
worker_unit("obj/tests/frontend_window_controller.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_controller_test.c" "${profile_56}")
worker_link("frontend-window-controller-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_window_controller.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj")
worker_unit("obj/tests/frontend_window_keyboard.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_keyboard_test.c" "${profile_56}")
worker_link("frontend-window-keyboard-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_window_keyboard.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj")
worker_unit("obj/tests/frontend_window_mouse.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_mouse_test.c" "${profile_56}")
worker_link("frontend-window-mouse-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_window_mouse.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj")
worker_unit("obj/tests/frontend_window_lifetime.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_lifetime_test.c" "${profile_56}")
worker_link("frontend-window-lifetime-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_window_lifetime.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj")
worker_unit("obj/tests/frontend_window_payload.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_payload_test.c" "${profile_56}")
worker_link("frontend-window-payload-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_window_payload.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj")
worker_link("console-frontend-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/console_frontend_test.obj" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj")
worker_unit("obj/tests/console_video.obj" "${NTVDM_REPO}/test/app/console_video_test.c" "${profile_49}")
worker_link("frontend-base-channel-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_base_channel.obj" "frontend-base.lib" "lib-pipe.lib")
worker_link("ntvdm-vm-frontend-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/vm_frontend.obj" "ntvdm-vm-frontend.lib" "frontend-base.lib" "lib-pipe.lib")
worker_unit("obj/tests/frontend_base_service.obj" "${NTVDM_REPO}/test/frontend-base/service_test.c" "${profile_49}")
worker_link("frontend-base-service-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_base_service.obj" "frontend-base.lib")
worker_link("frontend-base-video-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_base_video.obj" "frontend-base.lib")
worker_link("console-video-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/console_video.obj" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj")
worker_unit("obj/tests/keyboard_history_carrier.obj" "${NTVDM_REPO}/src/compat/mvdm_keyboard_history.c" "${profile_22}")
worker_unit("obj/tests/keyboard_origin.obj" "${NTVDM_REPO}/test/compat/keyboard_origin_test.c" "${profile_49}")
worker_link("keyboard-origin-test.exe" "/nologo /out:$out" "kernel32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/keyboard_origin.obj" "obj/tests/keyboard_history_carrier.obj")
worker_unit("obj/common/native_launch.obj" "${NTVDM_REPO}/src/common/codec/native_launch.c" "${profile_49}")
worker_archive("common-codec.lib"
  "obj/common/native_launch.obj")
worker_unit("obj/common/native_process.obj" "${NTVDM_REPO}/src/common/native_process.c" "${profile_49}")
worker_unit("obj/frontend/frontend_session.obj" "${NTVDM_REPO}/src/ntcon-exe/frontend_session.c" "${profile_57}")
worker_unit("obj/frontend/native_request_client.obj" "${NTVDM_REPO}/src/run16-exe/native_request_client.c" "${profile_49}")
worker_unit("obj/frontend/bootstrap_client.obj" "${NTVDM_REPO}/src/run16-exe/bootstrap_client.c" "${profile_49}")
worker_unit("obj/frontend/main.obj" "${NTVDM_REPO}/src/ntcon-exe/main.c" "${profile_49}")
worker_unit("obj/tests/control_transfer.obj" "${NTVDM_REPO}/test/lib-win32/pipe/pipe_transfer_test.c" "${profile_49}")
worker_link("control-transfer-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/control_transfer.obj")
worker_unit("obj/common/console_client.obj" "${NTVDM_REPO}/src/compat/console_transport.c" "${profile_22}")
worker_unit("obj/lib-win32/file/native_path.obj" "${NTVDM_REPO}/src/lib-win32/file/native_path.c" "${profile_22}")
worker_archive("frontend-client.lib"
  "obj/frontend/native_request_client.obj" "obj/frontend/bootstrap_client.obj")
worker_unit("obj/frontend/session_arguments.obj" "${NTVDM_REPO}/src/ntcon-exe/session_arguments.c" "${profile_49}")
worker_unit("obj/tests/frontend_session_arguments.obj" "${NTVDM_REPO}/test/app/frontend_session_arguments_test.c" "${profile_49}")
worker_link("frontend-session-arguments-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_session_arguments.obj" "obj/frontend/session_arguments.obj")
worker_unit("obj/ntvwm/main.obj" "${NTVDM_REPO}/src/ntvwm-exe/main.c" "${profile_58}")
worker_unit("obj/ntvwm/console_state.obj" "${NTVDM_REPO}/src/ntvwm-exe/console_state.c" "${profile_58}")
worker_unit("obj/ntvwm/execution.obj" "${NTVDM_REPO}/src/ntvwm-exe/execution.c" "${profile_58}")
worker_group("ntvwm-source-closure" "worker-base.lib" "obj/ntvwm/next_command.obj" "obj/ntvwm/main.obj" "obj/ntvwm/presentation.obj" "obj/compat/native_console_publication.obj" "obj/compat/native_console_text_frame.obj" "obj/ntvwm/console_state.obj" "obj/compat/native_console_diagnostics.obj" "obj/ntvwm/execution.obj" "obj/common/native_process.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "lib-process-launch.lib" "lib-process-image.lib" "lib-process-hook.lib" "lib-handle.lib" "lib-file.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/run16/image_classification.obj" "${NTVDM_REPO}/src/run16-exe/image_classification.c" "${profile_39}")
worker_unit("obj/common/native_image.obj" "${NTVDM_REPO}/src/common/native_image.c" "${profile_39}")
worker_unit("obj/lib-win32/process-hook/staging.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/staging.c" "${profile_59}")
worker_unit("obj/lib-win32/process-hook/helper.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/helper.c" "${profile_59}")
worker_unit("obj/lib-win32/process-hook/payload.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/payload.c" "${profile_59}")
worker_unit("obj/lib-win32/process-hook/startup_hooks.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/startup_hooks.c" "${profile_59}")
worker_unit("obj/lib-win32/process-hook/minhook/src/buffer.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/buffer.c" "${profile_59}")
worker_unit("obj/lib-win32/process-hook/minhook/src/trampoline.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/trampoline.c" "${profile_59}")
worker_unit("obj/lib-win32/process-hook/minhook/src/hde/hde32.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/hde/hde32.c" "${profile_59}")
worker_unit("obj/common/native-hook/packet.obj" "${NTVDM_REPO}/src/common/native-hook/packet.c" "${profile_49}")
worker_unit("obj/common/native-hook/context.obj" "${NTVDM_REPO}/src/common/native-hook/context.c" "${profile_60}" "obj/basesrv/service.h")
worker_unit("obj/common/native-hook/installer.obj" "${NTVDM_REPO}/src/common/native-hook/installer.c" "${profile_60}" "obj/basesrv/service.h")
worker_unit("obj/nthook32/entry.obj" "${NTVDM_REPO}/src/nthook-dll/entry.c" "${profile_60}" "obj/basesrv/service.h")
worker_unit("obj/nthook32/create_process.obj" "${NTVDM_REPO}/src/nthook-dll/create_process.c" "${profile_60}" "obj/basesrv/service.h")
worker_unit("obj/nthook32/classification.obj" "${NTVDM_REPO}/src/nthook-dll/hook_classification.c" "${profile_61}")
worker_archive("lib-process-hook.lib"
  "obj/lib-win32/process-hook/staging.obj" "obj/lib-win32/process-hook/helper.obj" "obj/lib-win32/process-hook/payload.obj" "obj/lib-win32/process-hook/startup_hooks.obj" "obj/lib-win32/process-hook/minhook/src/buffer.obj" "obj/lib-win32/process-hook/minhook/src/trampoline.obj" "obj/lib-win32/process-hook/minhook/src/hde/hde32.obj")
worker_unit("obj/tests/win32_process_hook.obj" "${NTVDM_REPO}/test/lib-win32/process-hook/payload_test.c" "${profile_60}")
worker_link("win32-process-hook-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/win32_process_hook.obj" "lib-process-hook.lib")
worker_archive("common-native-hook-context.lib"
  "obj/common/native-hook/context.obj" "obj/common/native-hook/packet.obj")
worker_archive("common-native-hook-installer.lib"
  "obj/common/native-hook/installer.obj" "obj/common/native_image.obj")
worker_link("nthook32.dll" "/nologo /dll /opt:ref /out:$out /map:$out.map /implib:nthook32-import.lib /def:\"${NTVDM_REPO}/src/nthook-dll/nthook32.def\"" "kernel32.lib user32.lib uxtheme.lib ntdll.lib advapi32.lib rpcrt4.lib legacy_stdio_definitions.lib"
  INPUTS "obj/nthook32/entry.obj" "obj/common/rpc_memory_heap.obj" "obj/nthook32/create_process.obj" "obj/nthook32/classification.obj" "obj/common/native_image.obj" "obj/run16/stub.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "obj/run16/support.obj" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib"
  DEPENDS "${NTVDM_REPO}/src/nthook-dll/nthook32.def")
worker_unit("obj/tests/nthook_install.obj" "${NTVDM_REPO}/test/integration/nthook_install_test.c" "${profile_60}")
worker_link("nthook-install-test.exe" "/nologo /manifest:embed /manifestuac:\"level='asInvoker' uiAccess='false'\" /subsystem:console /entry:wmainCRTStartup /out:$out" "kernel32.lib ntdll.lib"
  INPUTS "obj/tests/nthook_install.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "obj/run16/support.obj" "original-opennt-rtl-x86.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib"
  DEPENDS "nthook32.dll" "nthook-gui-test.exe" "run16.exe")
worker_link("nthook-gui-test.exe" "/nologo /manifest:embed /manifestuac:\"level='asInvoker' uiAccess='false'\" /subsystem:windows /entry:wmainCRTStartup /out:$out" "kernel32.lib ntdll.lib"
  INPUTS "obj/tests/nthook_install.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "obj/run16/support.obj" "original-opennt-rtl-x86.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/run16_image_classification.obj" "${NTVDM_REPO}/test/observation/run16_image_classification_test.c" "${profile_49}")
worker_link("run16-image-classification-test.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/run16_image_classification.obj" "obj/run16/image_classification.obj" "obj/common/native_image.obj" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "obj/run16/support.obj" "original-opennt-rtl-x86.lib")
worker_unit("obj/tests/native_gui_startup_probe.obj" "${NTVDM_REPO}/test/observation/native_gui_startup_probe.c" "${profile_49}")
worker_link("native-gui-startup-probe.exe" "/nologo /subsystem:windows /entry:wWinMainCRTStartup /opt:ref /out:$out" "kernel32.lib user32.lib"
  INPUTS "obj/tests/native_gui_startup_probe.obj")
worker_unit("obj/compat/native_console_text_frame.obj" "${NTVDM_REPO}/src/compat/native_console_text_frame.c" "${profile_22}")
worker_unit("obj/compat/native_console_publication.obj" "${NTVDM_REPO}/src/compat/native_console_publication.c" "${profile_22}")
worker_unit("obj/compat/native_console_diagnostics.obj" "${NTVDM_REPO}/src/compat/native_console_diagnostics.c" "${profile_22}")
worker_unit("obj/tests/ntvwm_text_frame.obj" "${NTVDM_REPO}/test/observation/ntvwm_text_frame_test.c" "${profile_62}")
worker_link("ntvwm-text-frame-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_text_frame.obj" "obj/compat/native_console_text_frame.obj" "frontend-base.lib")
worker_unit("obj/tests/ntvwm_native_colour_probe.obj" "${NTVDM_REPO}/test/observation/ntvwm_native_colour_probe.c" "${profile_1}")
worker_link("ntvwm-native-colour-probe.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_native_colour_probe.obj" "obj/compat/native_console_text_frame.obj")
worker_unit("obj/ntvwm/presentation.obj" "${NTVDM_REPO}/src/ntvwm-exe/presentation.c" "${profile_49}")
worker_unit("obj/tests/ntvwm_presentation.obj" "${NTVDM_REPO}/test/observation/ntvwm_presentation_test.c" "${profile_49}")
worker_unit("obj/tests/ntvwm_presentation_provider.obj" "${NTVDM_REPO}/src/ntvwm-exe/presentation.c" "${profile_63}")
worker_link("ntvwm-presentation-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_presentation.obj" "obj/tests/ntvwm_presentation_provider.obj" "obj/compat/native_console_publication.obj" "obj/ntvwm/console_state.obj" "obj/compat/native_console_diagnostics.obj" "obj/compat/native_console_text_frame.obj" "frontend-base.lib" "worker-base.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/frontend_text_handoff.obj" "${NTVDM_REPO}/test/app/frontend_text_handoff_test.c" "${profile_49}")
worker_link("frontend-text-handoff-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_text_handoff.obj" "obj/frontend/frontend_session.obj" "frontend-window.lib" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj")
worker_unit("obj/tests/ntvwm_execution_lifetime.obj" "${NTVDM_REPO}/test/observation/ntvwm_execution_lifetime_test.c" "${profile_49}")
worker_link("ntvwm-execution-lifetime-test.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/ntvwm_execution_lifetime.obj" "obj/ntvwm/execution.obj" "obj/common/native_process.obj" "common-codec.lib" "common-native-hook-installer.lib" "common-native-hook-context.lib" "obj/run16/support.obj" "original-opennt-rtl-x86.lib" "lib-process-launch.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib"
  DEPENDS "nthook32.dll" "run16.exe")
worker_unit("obj/tests/ntsrv_native_job_tracker.obj" "${NTVDM_REPO}/test/observation/ntsrv_native_job_tracker_test.c" "${profile_49}")
worker_unit("obj/tests/native_job_tracker_research.obj" "${NTVDM_REPO}/test/observation/native_job_tracker_research.c" "${profile_49}")
worker_link("ntsrv-native-job-tracker-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntsrv_native_job_tracker.obj" "obj/tests/native_job_tracker_research.obj")
worker_unit("obj/tests/ntvwm_surviving_client.obj" "${NTVDM_REPO}/test/observation/ntvwm_surviving_client_test.c" "${profile_49}")
worker_link("ntvwm-surviving-client-test.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/ntvwm_surviving_client.obj")
worker_unit("obj/tests/ntvwm_close.obj" "${NTVDM_REPO}/test/observation/ntvwm_close_test.c" "${profile_49}")
worker_link("ntvwm-close-test.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/ntvwm_close.obj" "obj/ntvwm/console_state.obj" "obj/compat/native_console_diagnostics.obj" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/ntvwm_close_events.obj" "${NTVDM_REPO}/test/app/ntvwm_close_events_test.c" "${profile_64}")
worker_link("ntvwm-close-events-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_close_events.obj" "obj/compat/native_console_diagnostics.obj" "common-console.lib")
worker_unit("obj/tests/relative_mouse_bridge.obj" "${NTVDM_REPO}/test/ntvdm-exe/softpc/softpc_relative_mouse_bridge_test.c" "${profile_49}")
worker_link("relative-mouse-bridge-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/relative_mouse_bridge.obj" "obj/compat/host_context.obj" "obj/compat/mvdm_softpc_mouse_context.obj" "obj/adapter-softpc/mvdm_softpc_mouse_bridge.obj" "obj/adapter-softpc/mvdm_softpc_mouse_input.obj")
worker_unit("obj/tests/frontend_request_client.obj" "${NTVDM_REPO}/test/app/frontend_request_client_test.c" "${profile_49}")
worker_link("frontend-request-client-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/frontend_request_client.obj" "frontend-client.lib" "obj/common/native_process.obj" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib")
worker_unit("obj/tests/frontend_gui_boundary.obj" "${NTVDM_REPO}/test/app/frontend_gui_boundary_test.c" "${profile_49}")
worker_link("frontend-gui-boundary-test.exe" "/nologo /subsystem:windows /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib"
  INPUTS "obj/tests/frontend_gui_boundary.obj")
worker_unit("obj/tests/frontend_chain_target.obj" "${NTVDM_REPO}/test/app/frontend_chain_target.c" "${profile_49}")
worker_link("frontend-chain-cui.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/frontend_chain_target.obj" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/frontend_chain_input.obj" "${NTVDM_REPO}/test/app/frontend_chain_input.c" "${profile_49}")
worker_link("frontend-chain-input.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/frontend_chain_input.obj")
worker_link("frontend-chain-gui.exe" "/nologo /subsystem:windows /entry:wmainCRTStartup /opt:ref /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/frontend_chain_target.obj" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/broker_frontend_bootstrap.obj" "${NTVDM_REPO}/test/integration/broker_frontend_bootstrap_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("broker-frontend-bootstrap-test.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/broker_frontend_bootstrap.obj" "ntvdm-vm-frontend.lib" "frontend-base.lib" "lib-pipe.lib" "frontend-client.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-handle.lib"
  DEPENDS "ntcon.exe" "ntsrv.exe")
worker_unit("obj/tests/native_console_capture.obj" "${NTVDM_REPO}/test/app/native_console_capture_test.c" "${profile_49}")
worker_link("native-console-capture-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/native_console_capture.obj" "obj/ntvwm/console_state.obj" "obj/compat/native_console_diagnostics.obj" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/ntvwm_capture_race.obj" "${NTVDM_REPO}/test/observation/ntvwm_capture_race_test.c" "${profile_49}")
worker_link("ntvwm-capture-race-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_capture_race.obj" "obj/compat/native_console_diagnostics.obj")
worker_unit("obj/tests/ntvwm_geometry_handoff.obj" "${NTVDM_REPO}/test/observation/ntvwm_geometry_handoff_probe.c" "${profile_49}")
worker_link("ntvwm-geometry-handoff-probe.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/ntvwm_geometry_handoff.obj")
worker_unit("obj/tests/console_channel_lifetime.obj" "${NTVDM_REPO}/test/app/console_channel_lifetime_test.c" "${profile_49}")
worker_link("console-channel-lifetime-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/console_channel_lifetime.obj" "frontend-base.lib" "obj/frontend/frontend_session.obj" "frontend-window.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "obj/opennt-abi-host-compat/console_grid.obj")
worker_unit("obj/tests/console_frame_failure.obj" "${NTVDM_REPO}/test/app/console_frame_failure_test.c" "${profile_57}")
worker_link("console-frame-failure-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/console_frame_failure.obj" "frontend-base.lib" "frontend-window.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "obj/opennt-abi-host-compat/console_grid.obj")
worker_unit("obj/tests/console_pointer_dispatch_test.obj" "${NTVDM_REPO}/test/app/console_pointer_dispatch_test.c" "${profile_49}")
worker_link("console-pointer-contract-test.exe" "/nologo /manifest:embed /manifestuac:\"level='asInvoker' uiAccess='false'\" /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib"
  INPUTS "obj/tests/console_pointer_dispatch_test.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj")
worker_unit("obj/tests/console_client_test.obj" "${NTVDM_REPO}/test/ntvdm-exe/win32/console_client_test.c" "${profile_49}")
worker_unit("obj/tests/console_client.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/console_client.c" "${profile_49}")
worker_unit("obj/tests/worker_lifecycle.obj" "${NTVDM_REPO}/src/ntvdm-exe/win32/worker_lifecycle.c" "${profile_49}")
worker_link("console-client-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib ntdll.lib"
  INPUTS "obj/tests/console_client_test.obj" "obj/compat/command_services.obj" "obj/compat/mvdm_softpc_mouse_context.obj" "obj/tests/console_client.obj" "obj/compat/console_services.obj" "obj/compat/console_activation.obj" "obj/tests/worker_lifecycle.obj" "obj/adapter-softpc/mvdm_softpc_event_thread.obj" "obj/compat/mvdm_softpc_worker_exit.obj" "obj/tests/console_graphics_provider.obj" "obj/tests/console_bitmap_provider.obj" "obj/tests/v7_pointer.obj" "obj/frontend/console_frontend.obj" "frontend-base.lib" "worker-base.lib" "lib-handle.lib" "obj/opennt-abi-host-compat/console_grid.obj" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/tests/softpc_text_video.obj" "${NTVDM_REPO}/test/ntvdm-exe/softpc/softpc_text_video_test.c" "${profile_65}" "${CMAKE_BINARY_DIR}/text-video-owner.inc")
worker_link("softpc-text-video-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/softpc_text_video.obj" "obj/adapter-softpc/mvdm_softpc_text_video.obj")
worker_unit("obj/tests/console_text_producer.obj" "${NTVDM_REPO}/test/ntvdm-exe/win32/console_text_producer_test.c" "${profile_49}")
worker_unit("obj/tests/console_text_provider.obj" "${NTVDM_REPO}/src/compat/console_text.c" "${profile_49}")
worker_link("console-text-producer-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/console_text_producer.obj" "obj/tests/console_text_provider.obj")
worker_unit("obj/tests/run16_broker_bootstrap.obj" "${NTVDM_REPO}/test/run16-exe/broker_bootstrap_test.c" "${profile_66}" "obj/basesrv/service.h")
worker_link("run16-broker-bootstrap-test.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/run16_broker_bootstrap.obj" "obj/common/application_search.obj" "obj/run16/image_classification.obj" "obj/common/native_image.obj" "obj/run16/launch_options.obj" "obj/run16/frontend_scope.obj" "frontend-client.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-native-hook-context.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib")
worker_unit("obj/tests/console_video_observed.obj" "${NTVDM_REPO}/test/app/console_video_observed.c" "${profile_67}")
worker_unit("obj/tests/run16_package_observed.obj" "${NTVDM_REPO}/test/app/run16_package_observed.c" "${profile_68}" "obj/basesrv/service.h")
worker_link("frontend-video-observer.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/frontend/main.obj" "obj/frontend/session_service.obj" "obj/frontend/frontend_session.obj" "frontend-window.lib" "obj/tests/console_video_observed.obj" "obj/frontend/console_channel.obj" "frontend-base.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "obj/opennt-abi-host-compat/console_grid.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/basesrv/entry.obj" "${NTVDM_REPO}/src/ntsrv-exe/main.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/basesrv/stub.obj" "obj/basesrv/service_s.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/worker/stub.obj" "obj/basesrv/service_c.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/monitor/main.obj" "${NTVDM_REPO}/src/ntmon-exe/main.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/monitor/stub.obj" "obj/basesrv/service_c.c" "${profile_49}" "obj/basesrv/service.h")
worker_unit("obj/monitor/trace_view.obj" "${NTVDM_REPO}/src/ntmon-exe/trace_view.c" "${profile_49}")
worker_unit("obj/tests/native_capture.obj" "${NTVDM_REPO}/test/adapter-basesrv/native_capture_test.c" "${profile_69}")
worker_link("native-capture-test.exe" "/nologo /subsystem:console /out:$out" "rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/native_capture.obj" "obj/opennt-base-client/capture.obj" "obj/opennt-base-bindings/capture-heap.obj" "obj/run16/support.obj")
worker_unit("obj/tests/monitor_session.obj" "${NTVDM_REPO}/test/observation/monitor_session_test.c" "${profile_49}")
worker_link("monitor-session-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/monitor_session.obj")
worker_unit("obj/tests/session_lifecycle.obj" "${NTVDM_REPO}/test/session/session_lifecycle_test.c" "${profile_49}")
worker_link("session-lifecycle-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/session_lifecycle.obj" "session.lib")
worker_unit("obj/tests/guest_location.obj" "${NTVDM_REPO}/test/adapter-mvdm-host-out/softpc/t280_s20_mvdm_guest_location_fixture.c" "${profile_19}")
worker_link("guest-location-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/guest_location.obj" "obj/compat/guest_location.obj" "session.lib" "obj/guest-memory-coordination.obj")
worker_unit("obj/tests/wow_shared_clock.obj" "${NTVDM_REPO}/test/compat/wow_shared_clock_test.c" "${profile_24}")
worker_link("wow-shared-clock-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/wow_shared_clock.obj" "obj/compat/wow_shared_clock.obj")
worker_unit("obj/tests/wow_window_graph.obj" "${NTVDM_REPO}/test/compat/wow_window_graph_test.c" "${profile_24}")
worker_link("wow-window-graph-test.exe" "/nologo /out:$out" "libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib"
  INPUTS "obj/tests/wow_window_graph.obj" "obj/compat/wow_window_graph.obj")
worker_unit("obj/tests/monitor_rpc_test.obj" "${NTVDM_REPO}/test/adapter-basesrv/monitor_rpc_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("monitor-rpc-test.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map" "rpcrt4.lib kernel32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/monitor_rpc_test.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/tests/worker_identity_version.obj" "${NTVDM_REPO}/test/integration/worker_identity_version_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("worker-identity-version-test.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map" "rpcrt4.lib kernel32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/worker_identity_version.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "lib-process-launch.lib" "lib-handle.lib"
  DEPENDS "ntsrv.exe")
worker_unit("obj/tests/monitor_layout.obj" "${NTVDM_REPO}/test/observation/monitor_layout_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("monitor-layout-test.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map" "rpcrt4.lib kernel32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/monitor_layout.obj" "obj/monitor/trace_view.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/tests/monitor_trace_view.obj" "${NTVDM_REPO}/test/observation/monitor_trace_view_test.c" "${profile_49}")
worker_link("monitor-trace-view-test.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map" "rpcrt4.lib kernel32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/monitor_trace_view.obj" "obj/monitor/trace_view.obj")
worker_unit("obj/tests/monitor_live_view.obj" "${NTVDM_REPO}/test/observation/monitor_live_view_test.c" "${profile_49}" "obj/basesrv/service.h")
worker_link("monitor-live-view-test.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map" "rpcrt4.lib kernel32.lib advapi32.lib legacy_stdio_definitions.lib gdi32.lib"
  INPUTS "obj/tests/monitor_live_view.obj" "obj/monitor/trace_view.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_link("ntvdm.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map /def:generated/ntvdm-wow32-provider.def" "rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "worker-base.lib" "ntvdm-vm-frontend.lib" "frontend-base.lib" "lib-pipe.lib" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "obj/adapter-redir/mvdm_redirector_worker_copy.obj" "obj/compat/mvdm_redirector_async.obj" "obj/adapter-wow-worker/wow_gdi_alias.obj" "obj/adapter-wow-worker/wow_user_session_binding.obj" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "opennt-base-client.lib" "common-base-srv-bindings.lib" "original-opennt-rtl-x86.lib" "softpc-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "common-base-srv-transport.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "original-softpc-host-roots.lib" "obj/host/softpc-resource.res" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib"
  DEPENDS "generated/ntvdm-wow32-provider.def"
  TIB_CHECK)
worker_unit("obj/tests/keyba_observed.obj" "${NTVDM_REPO}/test/observation/keyba_observed.c" "${profile_1}")
worker_unit("obj/tests/nt_event_observed.obj" "${NTVDM_REPO}/test/observation/nt_event_observed.c" "${profile_14}")
worker_link("ntvdm-keyboard-observer.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map /def:generated/ntvdm-wow32-provider.def" "rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/keyba_observed.obj" "obj/tests/nt_event_observed.obj" "worker-base.lib" "ntvdm-vm-frontend.lib" "frontend-base.lib" "lib-pipe.lib" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "obj/adapter-redir/mvdm_redirector_worker_copy.obj" "obj/compat/mvdm_redirector_async.obj" "obj/adapter-wow-worker/wow_gdi_alias.obj" "obj/adapter-wow-worker/wow_user_session_binding.obj" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "opennt-base-client.lib" "common-base-srv-bindings.lib" "original-opennt-rtl-x86.lib" "softpc-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "common-base-srv-transport.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "original-softpc-host-roots.lib" "obj/host/softpc-resource.res" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib"
  DEPENDS "generated/ntvdm-wow32-provider.def"
  TIB_CHECK)
worker_unit("obj/tests/console_client_observed.obj" "${NTVDM_REPO}/test/observation/console_client_observed.c" "${profile_49}")
worker_unit("obj/tests/console_channel_observed.obj" "${NTVDM_REPO}/test/observation/console_channel_observed.c" "${profile_49}")
worker_link("ntvdm-console-wire-observer.exe" "/nologo /subsystem:console /opt:ref /out:$out /map:$out.map /def:generated/ntvdm-wow32-provider.def" "rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/console_client_observed.obj" "worker-base.lib" "ntvdm-vm-frontend.lib" "frontend-base.lib" "lib-pipe.lib" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "obj/adapter-redir/mvdm_redirector_worker_copy.obj" "obj/compat/mvdm_redirector_async.obj" "obj/adapter-wow-worker/wow_gdi_alias.obj" "obj/adapter-wow-worker/wow_user_session_binding.obj" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "opennt-base-client.lib" "common-base-srv-bindings.lib" "original-opennt-rtl-x86.lib" "softpc-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "common-base-srv-transport.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "original-softpc-host-roots.lib" "obj/host/softpc-resource.res" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib"
  DEPENDS "generated/ntvdm-wow32-provider.def"
  TIB_CHECK)
worker_link("frontend-console-wire-observer.exe" "/nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map" "rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/frontend/main.obj" "obj/frontend/session_arguments.obj" "obj/frontend/session_service.obj" "obj/frontend/frontend_session.obj" "frontend-window.lib" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/tests/console_channel_observed.obj" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "obj/opennt-abi-host-compat/console_grid.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_unit("obj/worker/package_layout.obj" "${NTVDM_REPO}/src/ntvdm-exe/package_layout.c" "${profile_19}")
worker_unit("obj/compat/mvdm_softpc_effective_address.obj" "${NTVDM_REPO}/src/compat/mvdm_softpc_effective_address.c" "${profile_24}")
worker_unit("obj/ccpu/fmstubs_vector_defaults.obj" "${NTVDM_REPO}/src/compat/mvdm_ccpu_vector_defaults.c" "${profile_22}")
worker_unit("obj/ccpu/fmstubs_activity_check.obj" "${NTVDM_REPO}/src/compat/mvdm_ccpu_activity_check.c" "${profile_21}")
worker_resource("obj/redir/vdmredir.res" "${NTVDM_REPO}/src/mvdm-host/vdmredir/vdmredir.rc" "/nologo /d PROD /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/non-mvdm/ds/netapi/xactsrv\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/ds/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/net/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/private/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/ds/netapi/rpcxlate\" /I \"${NTVDM_REPO}/src/non-mvdm/private/windows/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/ddk/inc\" /I \"${NTVDM_REPO}/src/mvdm-host/inc\" /I \"${NTVDM_REPO}/src/compat/include/generated/x86/prod\" /I \"${NTVDM_REPO}/src/mvdm-host/xms.486\" /I \"${NTVDM_REPO}/src/mvdm-host/dpmi32\" /I \"${NTVDM_REPO}/src/mvdm-host/vdmredir\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/ccpu386\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/host/inc\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/cvidc\" /I \"${NTVDM_REPO}/src/mvdm-host/dos/dem\" /I \"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/inc\"")
worker_archive("original-ccpu386.lib"
  "${ccpu_objects}")
worker_archive("original-softpc-bios.lib"
  "${bios_objects}")
worker_archive("original-softpc-keymouse.lib"
  "${keymouse_objects}")
worker_archive("original-softpc-system.lib"
  "${system_objects}")
worker_archive("original-softpc-disks.lib"
  "${disks_objects}")
worker_archive("original-softpc-support.lib"
  "${support_objects}")
worker_archive("original-softpc-video.lib"
  "${video_objects}")
worker_archive("original-softpc-cvidc.lib"
  "${cvidc_objects}" "obj/cvidc/vglob.obj")
worker_archive("original-softpc-comms.lib"
  "${comms_objects}")
worker_archive("original-softpc-dos.lib"
  "${dos_objects}")
worker_archive("original-mvdm-dem.lib"
  "${dem_objects}")
worker_archive("original-mvdm-command.lib"
  "${command_objects}")
worker_archive("original-mvdm-redir.lib"
  "${redir_objects}")
worker_archive("original-mvdm-xms.lib"
  "${xms_objects}")
worker_archive("original-mvdm-dpmi32.lib"
  "${dpmi_objects}")
worker_archive("original-mvdm-host-suballoc.lib"
  "${suballoc_objects}")
worker_archive("original-mvdm-host-oemuni.lib"
  "${oemuni_objects}")
worker_archive("original-softpc-base-trace.lib"
  "obj/base-debug/debuggng.obj" "obj/base-debug/ega_dump.obj" "obj/base-debug/btrace.obj" "obj/base-debug/trace.obj" "obj/base-debug/profile.obj" "obj/base-debug/pigyoda.obj" "obj/base-debug/yoda.obj" "obj/base-debug/dasm.obj")
worker_archive("original-softpc-host-roots.lib"
  "${host_objects}" "obj/host/ntvdm_entry.obj")
worker_archive("original-softpc-host-fixture-roots.lib"
  "${host_objects}")
worker_archive("softpc-bindings.lib"
  "obj/adapter-softpc/mvdm_vm_binding.obj" "obj/adapter-softpc/mvdm_dpmi_binding.obj" "obj/adapter-softpc/mvdm_softpc_firmware.obj" "obj/adapter-softpc/mvdm_a20.obj" "obj/adapter-softpc/mvdm_softpc_guest_memory.obj" "obj/adapter-softpc/mvdm_softpc_execution.obj" "obj/adapter-softpc/mvdm_vm_notification.obj" "obj/adapter-softpc/mvdm_vm_bridge.obj" "obj/adapter-softpc/mvdm_vm_wake.obj" "obj/adapter-softpc/mvdm_vm_continuation.obj" "obj/adapter-softpc/mvdm_vm_guest_io.obj" "obj/adapter-softpc/mvdm_vm_input.obj" "obj/adapter-softpc/mvdm_vm_input_bop.obj" "obj/adapter-softpc/mvdm_vm_execute.obj" "obj/adapter-softpc/mvdm_vm_io.obj" "obj/adapter-softpc/mvdm_vm_executor.obj" "obj/adapter-softpc/mvdm_vm_ingress.obj" "obj/adapter-softpc/mvdm_standalone_worker.obj" "obj/adapter-softpc/ntvdm_dos_observer.obj" "obj/adapter-softpc/mvdm_softpc_presentation_font.obj" "obj/adapter-softpc/mvdm_softpc_text_video.obj" "obj/adapter-softpc/mvdm_keyboard_reset.obj" "obj/adapter-softpc/mvdm_softpc_mouse_input.obj" "obj/adapter-softpc/mvdm_softpc_mouse_bridge.obj" "obj/adapter-softpc/mvdm_softpc_mouse_guest.obj" "obj/adapter-softpc/v7_pointer_binding.obj" "obj/adapter-softpc/mvdm_softpc_descriptor_fields.obj" "obj/compat/mvdm_softpc_mouse_context.obj" "obj/compat/mvdm_softpc_wow_page_domain.obj" "obj/adapter-softpc/mvdm_shadow_registry.obj" "obj/compat/pif_path.obj" "obj/compat/guest_location.obj" "obj/compat/dpmi_services.obj" "obj/compat/mvdm_cpu_access.obj" "obj/compat/mvdm_vm_command_entry.obj" "obj/compat/mvdm_vm_application.obj" "obj/compat/mvdm_vm_io.obj" "obj/compat/mvdm_vm_capture.obj" "obj/compat/mvdm_softpc_termination.obj" "obj/compat/wow_hard_error.obj" "obj/compat/physical_mapping.obj" "obj/compat/mvdm_firmware.obj" "obj/compat/wow_start_glass.obj" "obj/adapter-softpc/mvdm_softpc_ccpu_wait.obj" "obj/compat/softpc_xt_storage.obj" "obj/adapter-softpc/mvdm_keyboard_history.obj" "obj/adapter-softpc/mvdm_softpc_event_thread.obj" "obj/compat/mvdm_softpc_worker_exit.obj")
worker_archive("softpc-fixture-bindings.lib"
  "obj/adapter-softpc/mvdm_vm_binding.obj" "obj/adapter-softpc/mvdm_dpmi_binding.obj" "obj/adapter-softpc/mvdm_softpc_firmware.obj" "obj/adapter-softpc/mvdm_a20.obj" "obj/adapter-softpc/mvdm_softpc_guest_memory.obj" "obj/adapter-softpc/mvdm_softpc_execution.obj" "obj/adapter-softpc/mvdm_vm_notification.obj" "obj/adapter-softpc/mvdm_vm_bridge.obj" "obj/adapter-softpc/mvdm_vm_wake.obj" "obj/adapter-softpc/mvdm_vm_continuation.obj" "obj/adapter-softpc/mvdm_vm_guest_io.obj" "obj/adapter-softpc/mvdm_vm_input.obj" "obj/adapter-softpc/mvdm_vm_input_bop.obj" "obj/adapter-softpc/mvdm_vm_execute.obj" "obj/adapter-softpc/mvdm_vm_io.obj" "obj/adapter-softpc/mvdm_vm_executor.obj" "obj/adapter-softpc/mvdm_vm_ingress.obj" "obj/adapter-softpc/ntvdm_dos_observer.obj" "obj/adapter-softpc/mvdm_softpc_presentation_font.obj" "obj/adapter-softpc/mvdm_softpc_text_video.obj" "obj/adapter-softpc/mvdm_keyboard_reset.obj" "obj/adapter-softpc/mvdm_softpc_mouse_input.obj" "obj/adapter-softpc/mvdm_softpc_mouse_bridge.obj" "obj/adapter-softpc/mvdm_softpc_mouse_guest.obj" "obj/adapter-softpc/v7_pointer_binding.obj" "obj/adapter-softpc/mvdm_softpc_descriptor_fields.obj" "obj/compat/mvdm_softpc_mouse_context.obj" "obj/compat/mvdm_softpc_wow_page_domain.obj" "obj/adapter-softpc/mvdm_shadow_registry.obj" "obj/compat/pif_path.obj" "obj/compat/guest_location.obj" "obj/compat/dpmi_services.obj" "obj/compat/mvdm_cpu_access.obj" "obj/compat/mvdm_vm_command_entry.obj" "obj/compat/mvdm_vm_application.obj" "obj/compat/mvdm_vm_io.obj" "obj/compat/mvdm_vm_capture.obj" "obj/compat/mvdm_softpc_termination.obj" "obj/compat/wow_hard_error.obj" "obj/compat/physical_mapping.obj" "obj/compat/mvdm_firmware.obj" "obj/compat/wow_start_glass.obj" "obj/adapter-softpc/mvdm_softpc_ccpu_wait.obj" "obj/compat/softpc_xt_storage.obj" "obj/adapter-softpc/mvdm_keyboard_history.obj" "obj/adapter-softpc/mvdm_softpc_event_thread.obj" "obj/compat/mvdm_softpc_worker_exit.obj")
worker_archive("vdmredir-dll-bindings.lib"
  "${vdmredir_dll_objects}" "obj/compat/mvdm_redirector_guest_copy.obj" "obj/compat/remote_browser.obj" "obj/compat/remote_rap.obj")
worker_archive("original-opennt-netlib.lib"
  "obj/opennt-netlib/ntstatus.obj" "obj/opennt-netlib/copystr.obj" "obj/opennt-netlib/allocstr.obj" "obj/opennt-netlib/initoem.obj" "obj/opennt-netlib/memalloc.obj" "obj/opennt-netlib/time.obj" "obj/opennt-netlib/timezone.obj" "obj/opennt-netlib/xlatesvc.obj" "obj/opennt-netlib/svcinfo.obj" "obj/opennt-netlib/packstr.obj")
worker_archive("original-opennt-xactsrv.lib"
  "obj/opennt-xactsrv/apiwksta.obj" "obj/opennt-xactsrv/apiuse.obj" "obj/opennt-xactsrv/apimsg.obj" "obj/opennt-xactsrv/apisvc.obj" "obj/opennt-xactsrv/apisrv.obj" "obj/opennt-xactsrv/xssubs.obj" "obj/opennt-xactsrv/strucsiz.obj" "obj/opennt-xactsrv/examine.obj" "obj/opennt-xactsrv/ascii.obj" "obj/opennt-xactsrv/pointer.obj" "obj/opennt-xactsrv/convert.obj" "obj/opennt-xactsrv/xsunicod.obj" "obj/opennt-xactsrv/auxdata.obj" "obj/opennt-xactsrv/parmnum.obj")
worker_archive("original-opennt-base-vdm.lib"
  "obj/opennt-base-vdm/vdm.obj")
worker_archive("original-opennt-rtl-x86.lib"
  "obj/opennt-rtl/environ.obj" "obj/opennt-rtl/error.obj" "obj/opennt-rtl-x86/largeint.obj" "obj/opennt-rtl-x86/movemem.obj")
worker_archive("worker-shell.lib"
  "obj/worker/package_layout.obj")
worker_archive("session.lib"
  "obj/ntvdm/session/session.obj" "obj/compat/host_context.obj" "obj/compat/wow_worker_state.obj" "obj/compat/guest_memory_lease.obj" "obj/compat/wow_user_runtime.obj" "obj/compat/mvdm_softpc_observation.obj" "obj/compat/wow_shared_clock.obj" "obj/compat/wow_window_graph.obj")
worker_archive("wow-worker-bindings.lib"
  "obj/adapter-wow-worker/wow_gdi_alias.obj" "obj/adapter-wow-worker/wow_user_session_binding.obj")
worker_link("rtl-x86-fixture.exe" "/nologo /out:$out" "kernel32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/rtl_x86_fixture.obj" "original-opennt-rtl-x86.lib")
worker_link("environment-projection-fixture.exe" "/nologo /out:$out" "kernel32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/environment_projection_fixture.obj" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-win32-bindings.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib")
worker_archive("mvdm-softpc-effective-address.lib"
  "obj/compat/mvdm_softpc_effective_address.obj")
worker_unit("obj/compat/console_activation.obj" "${NTVDM_REPO}/src/compat/console_activation.c" "${profile_22}")
worker_unit("obj/compat/console_services.obj" "${NTVDM_REPO}/src/compat/console_services.c" "${profile_24}")
worker_unit("obj/compat/console_graphics.obj" "${NTVDM_REPO}/src/compat/console_graphics.c" "${profile_22}")
worker_unit("obj/compat/console_bitmap.obj" "${NTVDM_REPO}/src/compat/console_bitmap.c" "${profile_22}")
worker_unit("obj/compat/v7_pointer.obj" "${NTVDM_REPO}/src/compat/v7_pointer.c" "${profile_22}")
worker_unit("obj/compat/console_text.obj" "${NTVDM_REPO}/src/compat/console_text.c" "${profile_22}")
worker_unit("obj/compat/console_legacy.obj" "${NTVDM_REPO}/src/compat/console_legacy.c" "${profile_22}")
worker_archive("softpc-win32-bindings.lib"
  "obj/adapter-win32/console_client.obj" "obj/adapter-win32/worker_lifecycle.obj" "obj/adapter-win32/command_process_compat.obj" "obj/adapter-win32/wow_private_unavailable.obj" "obj/adapter-win32/wow_hard_error_dialog.obj" "obj/adapter-win32/wow_hard_error_decoder.obj" "obj/adapter-win32/mvdm_base_vdm_environment.obj" "obj/opennt-abi-host-compat/opennt_support_rtl.obj" "obj/opennt-abi-host-compat/console_grid.obj" "obj/adapter-win32/console_compat.obj" "obj/adapter-win32/ntioapi_facade.obj" "obj/adapter-win32/dem_restart_hint.obj" "obj/adapter-win32/invalid_virtual_instance.obj" "obj/compat/console_graphics.obj" "obj/compat/console_bitmap.obj" "obj/compat/v7_pointer.obj" "obj/compat/console_text.obj" "obj/compat/console_legacy.obj" "obj/compat/console_services.obj" "obj/compat/console_activation.obj")
worker_unit("obj/compat/command_services.obj" "${NTVDM_REPO}/src/compat/command_services.c" "${profile_24}")
worker_unit("obj/compat/command_process.obj" "${NTVDM_REPO}/src/compat/command_process.c" "${profile_24}")
worker_unit("obj/compat/vdm_control.obj" "${NTVDM_REPO}/src/compat/vdm_control.c" "${profile_24}")
worker_archive("worker-command-bindings.lib"
  "obj/adapter-basesrv/mvdm_command_guest_state.obj" "obj/adapter-basesrv/mvdm_command_launch.obj" "obj/adapter-basesrv/mvdm_command_launch_request.obj" "obj/adapter-basesrv/mvdm_command_native_child.obj" "obj/adapter-basesrv/mvdm_command_binding.obj" "obj/compat/command_services.obj" "obj/compat/command_process.obj")
worker_archive("monitor-bindings.lib"
  "obj/adapter-monitor/mvdm_vdm_tib.obj" "obj/compat/vdm_control.obj")
worker_archive("kernel-vdm-printer.lib"
  "obj/kernel-vdm/mvdm_monitor_printer.obj")
worker_archive("debugger-bindings.lib"
  "obj/compat/dbg_registers.obj" "obj/mvdm-debugger/dbg.obj")
worker_archive("softpc-ccpu-vector-defaults.lib"
  "obj/ccpu/fmstubs_vector_defaults.obj")
worker_archive("softpc-activity-check.lib"
  "obj/ccpu/fmstubs_activity_check.obj")
worker_group("original-softpc-candidate" "original-ccpu386.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-support.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-redir.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-softpc-host-roots.lib" "original-opennt-netlib.lib" "original-opennt-xactsrv.lib" "original-opennt-rtl-x86.lib" "softpc-bindings.lib" "vdmredir-dll-bindings.lib" "worker-shell.lib" "worker-command-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib")
worker_group("product-programs" "run16.exe" "ntsrv.exe" "ntvdm.exe" "ntvwm.exe" "ntcon.exe" "ntmon.exe" "VDMREDIR.dll")
worker_unit("obj/tests/ccpu_halt_reset_test.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_halt_reset_test.c" "${profile_1}")
worker_unit("obj/tests/ccpu_host_fixture_seams.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_host_fixture_seams.c" "${profile_1}")
worker_unit("obj/tests/ccpu_bounded_execution_fixture_seams.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_bounded_execution_fixture_seams.c" "${profile_1}")
worker_unit("obj/tests/ccpu_monitor_boundary_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_monitor_boundary_fixture.c" "${profile_1}")
worker_link("ccpu-monitor-boundary-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_monitor_boundary_fixture.obj" "obj/host/nt_inthk.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_link("ccpu-halt-reset-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_halt_reset_test.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_throughput_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_throughput_fixture.c" "${profile_1}")
worker_link("ccpu-throughput-fixture.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_throughput_fixture.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/physical_mapping_observation_test.obj" "${NTVDM_REPO}/test/compat/physical_mapping_test.c" "${profile_1}")
worker_link("physical-mapping-observation-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/physical_mapping_observation_test.obj" "obj/compat/physical_mapping.obj" "session.lib")
worker_unit("obj/tests/ccpu_debug_match_test.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_debug_match_test.c" "${profile_1}")
worker_link("ccpu-debug-match-test.exe" "/nologo /machine:x86 /opt:ref /out:$out" "legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_debug_match_test.obj" "obj/ccpu/c_debug.obj")
worker_unit("obj/tests/debug_register_binding_test.obj" "${NTVDM_REPO}/test/mvdm-host/debug_register_binding_test.c" "${profile_1}")
worker_link("debug-register-binding-test.exe" "/nologo /machine:x86 /opt:ref /out:$out" "legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/debug_register_binding_test.obj" "obj/compat/dpmi_services.obj" "obj/compat/mvdm_cpu_access.obj")
worker_unit("obj/tests/debugger_event_transport_test.obj" "${NTVDM_REPO}/test/mvdm-host/debugger_event_transport_test.c" "${profile_1}")
worker_link("debugger-event-transport-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/debugger_event_transport_test.obj")
worker_unit("obj/tests/debugger_module_event_test.obj" "${NTVDM_REPO}/test/mvdm-host/debugger_module_event_test.c" "${profile_1}")
worker_link("debugger-module-event-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/debugger_module_event_test.obj")
worker_unit("obj/tests/oem_command_uppercase_test.obj" "${NTVDM_REPO}/test/mvdm-host/oem_command_uppercase_test.c" "${profile_1}")
worker_link("oem-command-uppercase-test.exe" "/nologo /machine:x86 /opt:ref /out:$out" "legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/oem_command_uppercase_test.obj")
worker_unit("obj/tests/debugger_worker_module_observer.obj" "${NTVDM_REPO}/test/mvdm-host/debugger_worker_module_observer.c" "${profile_1}")
worker_link("debugger-worker-module-observer.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/debugger_worker_module_observer.obj")
worker_unit("obj/tests/s44_bop_vdd.obj" "${NTVDM_REPO}/test/mvdm-host/s44_test_vdd.c" "${profile_70}")
worker_unit("obj/tests/s44_entry_hook_vdd.obj" "${NTVDM_REPO}/test/mvdm-host/s44_test_vdd.c" "${profile_71}")
worker_unit("obj/tests/s44_memory_vdd.obj" "${NTVDM_REPO}/test/mvdm-host/s44_test_vdd.c" "${profile_72}")
worker_unit("obj/tests/s44_terminate_vdd.obj" "${NTVDM_REPO}/test/mvdm-host/s44_test_vdd.c" "${profile_73}")
worker_link("s44-bop-vdd.dll" "/nologo /dll /machine:x86 /entry:VDDInitialize@12 /export:VDDInitialize=_VDDInitialize@12 /export:S44BOPInit=_S44BOPInit@0 /export:S44BOPDispatch=_S44BOPDispatch@0 /out:$out" "kernel32.lib user32.lib"
  INPUTS "obj/tests/s44_bop_vdd.obj" "ntvdm.lib" "common-root.lib")
worker_link("s44-entry-hook-vdd.dll" "/nologo /dll /machine:x86 /entry:VDDInitialize@12 /export:VDDInitialize=_VDDInitialize@12 /export:S44BOPInit=_S44BOPInit@0 /export:S44BOPDispatch=_S44BOPDispatch@0 /out:$out" "kernel32.lib user32.lib"
  INPUTS "obj/tests/s44_entry_hook_vdd.obj" "ntvdm.lib" "common-root.lib")
worker_link("s44-memory-vdd.dll" "/nologo /dll /machine:x86 /entry:VDDInitialize@12 /export:VDDInitialize=_VDDInitialize@12 /export:S44BOPInit=_S44BOPInit@0 /export:S44BOPDispatch=_S44BOPDispatch@0 /out:$out" "kernel32.lib user32.lib"
  INPUTS "obj/tests/s44_memory_vdd.obj" "ntvdm.lib" "common-root.lib")
worker_link("s44-terminate-vdd.dll" "/nologo /dll /machine:x86 /entry:VDDInitialize@12 /export:VDDInitialize=_VDDInitialize@12 /export:S44BOPInit=_S44BOPInit@0 /export:S44BOPDispatch=_S44BOPDispatch@0 /out:$out" "kernel32.lib user32.lib"
  INPUTS "obj/tests/s44_terminate_vdd.obj" "ntvdm.lib" "common-root.lib")
worker_unit("obj/tests/ntvdm_export_live_vdd.obj" "${NTVDM_REPO}/test/mvdm-host/ntvdm_export_live_vdd.c" "${profile_1}")
worker_link("ntvdm-export-live-vdd.dll" "/nologo /dll /machine:x86 /entry:VDDInitialize@12 /export:VDDInitialize=_VDDInitialize@12 /out:$out" "kernel32.lib"
  INPUTS "obj/tests/ntvdm_export_live_vdd.obj" "ntvdm.lib" "common-root.lib")
worker_group("debugger-event-tests" "debugger-event-transport-test.exe" "debugger-module-event-test.exe" "debugger-worker-module-observer.exe")
worker_group("dpmi-debug-tests" "ccpu-debug-match-test.exe" "debug-register-binding-test.exe" "debugger-event-transport-test.exe" "debugger-module-event-test.exe" "ccpu-halt-reset-test.exe")
worker_unit("obj/tests/ccpu_thread_lifecycle_test.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_thread_lifecycle_test.c" "${profile_1}")
worker_link("ccpu-thread-lifecycle-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_thread_lifecycle_test.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/original_external_memory_test.obj" "${NTVDM_REPO}/test/mvdm-host/original_external_memory_test.c" "${profile_1}")
worker_link("original-external-memory-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/original_external_memory_test.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_bounded_execution_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_bounded_execution_fixture.c" "${profile_1}")
worker_link("ccpu-high-linear-page-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_bounded_execution_fixture.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_bit_matrix_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_bounded_execution_fixture.c" "${profile_74}")
worker_link("ccpu-bit-matrix-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_bit_matrix_fixture.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_bit_fault_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_bit_fault_fixture.c" "${profile_1}")
worker_link("ccpu-bit-fault-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_bit_fault_fixture.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_x87_span_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_x87_span_fixture.c" "${profile_1}")
worker_link("ccpu-x87-span-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_x87_span_fixture.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_x87_page_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_x87_page_fixture.c" "${profile_1}")
worker_link("ccpu-x87-page-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_x87_page_fixture.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_bit_memory_oracle.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_bit_memory_oracle.c" "${profile_1}")
worker_link("ccpu-bit-native-oracle.exe" "/nologo /out:$out" "kernel32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_bit_memory_oracle.obj")
worker_unit("obj/tests/ccpu_far_exception_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_far_exception_fixture.c" "${profile_1}")
worker_link("ccpu-far-exception-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_far_exception_fixture.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_stack_transition_test.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_stack_transition_test.c" "${profile_1}")
worker_link("ccpu-stack-transition-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_stack_transition_test.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_fist64_test.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_fist64_test.c" "${profile_1}")
worker_link("ccpu-fist64-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_fist64_test.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/ccpu_bios_access_test.obj" "${NTVDM_REPO}/test/mvdm-host/ccpu_bios_access_test.c" "${profile_1}")
worker_link("ccpu-bios-access-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/ccpu_bios_access_test.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/keyboard_controller_pic_test.obj" "${NTVDM_REPO}/test/mvdm-host/keyboard_controller_pic_test.c" "${profile_1}")
worker_link("keyboard-controller-pic-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/keyboard_controller_pic_test.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/pic_reset_eoi_test.obj" "${NTVDM_REPO}/test/mvdm-host/pic_reset_eoi_test.c" "${profile_1}")
worker_link("pic-reset-eoi-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/pic_reset_eoi_test.obj" "obj/tests/ccpu_bounded_execution_fixture_seams.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/wow_page_domain_fixture.obj" "${NTVDM_REPO}/test/mvdm-host/wow_page_domain_fixture.c" "${profile_1}")
worker_link("cpu40-descriptor-domain-fixture.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/cpu40_descriptor_domain_fixture.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_unit("obj/tests/wow_user_client_view_layout_fixture.obj" "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_user_client_view_layout_fixture.c" "${profile_75}")
worker_link("wow-user-client-view-layout-test.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/wow_user_client_view_layout_fixture.obj")
worker_link("cvidc-vector-binding-fixture.exe" "/nologo /force:multiple /map:$out.map /out:$out" "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/cvidc_vector_binding_fixture.obj" "obj/tests/cvidc_sas_fixture.obj" "obj/tests/ccpu_host_fixture_seams.obj" "worker-base.lib" "worker-shell.lib" "worker-command-bindings.lib" "original-softpc-host-fixture-roots.lib" "original-softpc-support.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-opennt-base-vdm.lib" "original-opennt-rtl-x86.lib" "softpc-fixture-bindings.lib" "vdmredir-dll-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib" "softpc-activity-check.lib" "original-ccpu386.lib" "obj/host/softpc-resource.res" "obj/compat/mvdm_vm_text.obj" "obj/compat/console_video_buffer.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/worker/stub.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-crt-compat.lib" "compat-worker-thread.lib" "lib-thread.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib")
worker_link("x87-layout-fixture.exe" "/nologo /out:$out" "kernel32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS "obj/tests/x87_layout_fixture.obj")
worker_link("VDMREDIR.dll" "/nologo /dll /entry:mvdm_vdmredir_dll_entry@12 /alternatename:_call_ica_hw_interrupt@12=call_ica_hw_interrupt /def:${NTVDM_REPO}/src/mvdm-host/vdmredir/vdmredir.def /implib:$out.lib /out:$out /map:$out.map" "kernel32.lib advapi32.lib netapi32.lib rpcrt4.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS ${redir_objects} "obj/redir/vdmredir.res" "ntvdm.lib" "vdmredir-dll-bindings.lib" "original-opennt-netlib.lib" "original-opennt-xactsrv.lib" "original-opennt-rtl-x86.lib" "common-opennt-compat.lib" "common-root.lib")
worker_link("original-softpc-forced-closure.dll" "/nologo /dll /noentry /force:unresolved /force:multiple /out:$out /implib:$out.lib /wholearchive:original-ccpu386.lib /wholearchive:original-softpc-bios.lib /wholearchive:original-softpc-keymouse.lib /wholearchive:original-softpc-system.lib /wholearchive:original-softpc-disks.lib /wholearchive:original-softpc-support.lib /wholearchive:original-softpc-video.lib /wholearchive:original-softpc-cvidc.lib /wholearchive:original-softpc-comms.lib /wholearchive:original-softpc-dos.lib /wholearchive:original-mvdm-dem.lib /wholearchive:original-mvdm-command.lib /wholearchive:original-mvdm-xms.lib /wholearchive:original-mvdm-dpmi32.lib /wholearchive:original-mvdm-host-suballoc.lib /wholearchive:original-mvdm-host-oemuni.lib /wholearchive:original-softpc-base-trace.lib /wholearchive:original-softpc-host-roots.lib /wholearchive:softpc-bindings.lib /wholearchive:softpc-win32-bindings.lib worker-base.lib /wholearchive:monitor-bindings.lib /wholearchive:debugger-bindings.lib /wholearchive:session.lib /wholearchive:mvdm-softpc-effective-address.lib softpc-ccpu-vector-defaults.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib" ""
  INPUTS "original-ccpu386.lib" "original-softpc-bios.lib" "original-softpc-keymouse.lib" "original-softpc-system.lib" "original-softpc-disks.lib" "original-softpc-support.lib" "original-softpc-video.lib" "original-softpc-cvidc.lib" "original-softpc-comms.lib" "original-softpc-dos.lib" "original-mvdm-dem.lib" "original-mvdm-command.lib" "original-mvdm-xms.lib" "original-mvdm-dpmi32.lib" "original-mvdm-host-suballoc.lib" "original-mvdm-host-oemuni.lib" "original-softpc-base-trace.lib" "original-softpc-host-roots.lib" "softpc-bindings.lib" "worker-shell.lib" "worker-command-bindings.lib" "softpc-win32-bindings.lib" "monitor-bindings.lib" "kernel-vdm-printer.lib" "debugger-bindings.lib" "session.lib" "mvdm-softpc-effective-address.lib" "softpc-ccpu-vector-defaults.lib"
  NO_LINK_INPUTS)
worker_group("ntcon-source-closure" "obj/frontend/main.obj" "obj/frontend/session_arguments.obj" "obj/frontend/session_service.obj" "obj/frontend/frontend_session.obj" "frontend-window.lib" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/frontend/console_channel.obj" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "obj/opennt-abi-host-compat/console_grid.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_group("ntmon-source-closure" "obj/monitor/main.obj" "obj/monitor/trace_view.obj" "obj/monitor/stub.obj" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib")
worker_group("run16-source-closure" "obj/run16/entry.obj" "obj/common/application_search.obj" "obj/run16/image_classification.obj" "obj/common/native_image.obj" "obj/run16/launch_options.obj" "obj/run16/frontend_scope.obj" "frontend-client.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-native-hook-context.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "ntcon.exe")
worker_group("ntsrv-source-closure" "obj/basesrv/entry.obj" "obj/basesrv/stub.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-x86.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib")
worker_unit("obj/guest-memory-coordination.obj" "${CMAKE_BINARY_DIR}/guest-memory-coordination.c" "${profile_76}")
