# Explicit source and link composition; no reference graph is consumed.
set(native_flags "${family_flags} /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc\" /I obj/basesrv")
family_unit("obj/worker-base/connection.obj" "${NTVDM_REPO}/src/worker-base/connection.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/worker-base/shutdown_close.obj" "${NTVDM_REPO}/src/worker-base/shutdown_close.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/worker-base/publication.obj" "${NTVDM_REPO}/src/compat/publication.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/worker-base/input_watch.obj" "${NTVDM_REPO}/src/worker-base/input_watch.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/worker-base/input_readiness.obj" "${NTVDM_REPO}/src/compat/input_readiness.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/worker-base/task_observation.obj" "${NTVDM_REPO}/src/worker-base/task_observation.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("worker-base.lib"
  "obj/worker-base/connection.obj" "obj/worker-base/shutdown_close.obj" "obj/worker-base/publication.obj" "obj/worker-base/input_watch.obj" "obj/worker-base/input_readiness.obj" "obj/worker-base/task_observation.obj")
family_unit("obj/ntvwm/next_command.obj" "${NTVDM_REPO}/src/ntvwm-exe/next_command.c" "${native_flags} /we4013 /I frontend-font /O2" "obj/basesrv/service.h")
family_unit("obj/ntvwm/main.obj" "${NTVDM_REPO}/src/ntvwm-exe/main.c" "${native_flags} /we4013 /I \"${CMAKE_BINARY_DIR}/frontend-font\" /I frontend-font /O2" "obj/basesrv/service.h")
family_unit("obj/ntvwm/presentation.obj" "${NTVDM_REPO}/src/ntvwm-exe/presentation.c" "${native_flags} /we4013 /I frontend-font /O2" "obj/basesrv/service.h")
family_unit("obj/compat/native_console_publication.obj" "${NTVDM_REPO}/src/compat/native_console_publication.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/compat/native_console_text_frame.obj" "${NTVDM_REPO}/src/compat/native_console_text_frame.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/ntvwm/console_state.obj" "${NTVDM_REPO}/src/ntvwm-exe/console_state.c" "${native_flags} /we4013 /I \"${CMAKE_BINARY_DIR}/frontend-font\" /I frontend-font /O2" "obj/basesrv/service.h")
family_unit("obj/compat/native_console_diagnostics.obj" "${NTVDM_REPO}/src/compat/native_console_diagnostics.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/ntvwm/execution.obj" "${NTVDM_REPO}/src/ntvwm-exe/execution.c" "${native_flags} /we4013 /I \"${CMAKE_BINARY_DIR}/frontend-font\" /I frontend-font /O2" "obj/basesrv/service.h")
family_unit("obj/common/native_process.obj" "${NTVDM_REPO}/src/common/native_process.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/native-hook/installer.obj" "${NTVDM_REPO}/src/common/native-hook/installer.c" "${native_flags} /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/common/native_image.obj" "${NTVDM_REPO}/src/common/native_image.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("common-native-hook-installer.lib"
  "obj/common/native-hook/installer.obj" "obj/common/native_image.obj")
family_unit("obj/common/native-hook/context.obj" "${NTVDM_REPO}/src/common/native-hook/context.c" "${native_flags} /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/common/native-hook/packet.obj" "${NTVDM_REPO}/src/common/native-hook/packet.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-native-hook-context.lib"
  "obj/common/native-hook/context.obj" "obj/common/native-hook/packet.obj")
family_unit("obj/lib-win32/process-launch/attributes.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/attributes.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/process-launch/process.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/process.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/process-launch/command_line.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/command_line.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-process-launch.lib"
  "obj/lib-win32/process-launch/attributes.obj" "obj/lib-win32/process-launch/process.obj" "obj/lib-win32/process-launch/command_line.obj")
family_unit("obj/lib-win32/process-image/image.obj" "${NTVDM_REPO}/src/lib-win32/process-image/image.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-process-image.lib"
  "obj/lib-win32/process-image/image.obj")
family_unit("obj/lib-win32/process-hook/staging.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/staging.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/helper.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/helper.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/payload.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/payload.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/startup_hooks.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/startup_hooks.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/minhook/src/buffer.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/buffer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/minhook/src/trampoline.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/trampoline.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/minhook/src/hde/hde32.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/hde/hde64.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_archive("lib-process-hook.lib"
  "obj/lib-win32/process-hook/staging.obj" "obj/lib-win32/process-hook/helper.obj" "obj/lib-win32/process-hook/payload.obj" "obj/lib-win32/process-hook/startup_hooks.obj" "obj/lib-win32/process-hook/minhook/src/buffer.obj" "obj/lib-win32/process-hook/minhook/src/trampoline.obj" "obj/lib-win32/process-hook/minhook/src/hde/hde32.obj")
family_unit("obj/lib-win32/handle/duplicate.obj" "${NTVDM_REPO}/src/lib-win32/handle/duplicate.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-handle.lib"
  "obj/lib-win32/handle/duplicate.obj")
family_unit("obj/lib-win32/file/identity.obj" "${NTVDM_REPO}/src/lib-win32/file/identity.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/file/native_path.obj" "${NTVDM_REPO}/src/lib-win32/file/native_path.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-file.lib"
  "obj/lib-win32/file/identity.obj" "obj/lib-win32/file/native_path.obj")
family_unit("obj/run16/support.obj" "${NTVDM_REPO}/src/compat/opennt_support_rtl.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /Gy /O2")
family_unit("obj/run16/rpc_client_core.obj" "${NTVDM_REPO}/src/common/base_srv/base_rpc_client_core.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/common/frontend_rpc_client.obj" "${NTVDM_REPO}/src/common/base_srv/frontend_client.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("base-rpc-client-core.lib"
  "obj/run16/rpc_client_core.obj" "obj/common/frontend_rpc_client.obj")
family_unit("obj/run16/stub.obj" "obj/basesrv/service_c.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/command.obj" "${NTVDM_REPO}/src/common/base_srv/base_command.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/values.obj" "${NTVDM_REPO}/src/common/base_srv/base_values.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/payload.obj" "${NTVDM_REPO}/src/common/base_srv/base_payload.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/startup.obj" "${NTVDM_REPO}/src/common/base_srv/base_startup.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/config.obj" "${NTVDM_REPO}/src/compat/base_vdm_config.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/capture-heap.obj" "${NTVDM_REPO}/src/compat/base_capture_heap.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/process.obj" "${NTVDM_REPO}/src/compat/base_console_handle.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/classifier-path.obj" "${NTVDM_REPO}/src/compat/base_classifier_path.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/config-command.obj" "${CMAKE_BINARY_DIR}/generated/base_vdm_config.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("common-base-srv-bindings.lib"
  "obj/opennt-base-bindings/command.obj" "obj/opennt-base-bindings/values.obj" "obj/opennt-base-bindings/payload.obj" "obj/opennt-base-bindings/startup.obj" "obj/opennt-base-bindings/config.obj" "obj/opennt-base-bindings/capture-heap.obj" "obj/opennt-base-bindings/process.obj" "obj/opennt-base-bindings/classifier-path.obj" "obj/opennt-base-bindings/config-command.obj" "obj/common/native_image.obj")
family_unit("obj/broker-transport/rpc_scope.obj" "${NTVDM_REPO}/src/common/base_srv/scope.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_payload.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_payload.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_message.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_message.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-base-srv-transport.lib"
  "obj/broker-transport/rpc_scope.obj" "obj/broker-transport/vdm_payload.obj" "obj/broker-transport/vdm_message.obj")
family_unit("obj/opennt-rtl/error.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/error.c" "${native_flags} /I \"${NTVDM_REPO}/build/.ninja/build-tools/rtl\" /DNTOS_KERNEL_RUNTIME /DNTOS_KERNEL_RUNTIME /FI \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h\" /O2" "obj/basesrv/service.h" "${NTVDM_REPO}/build/.ninja/build-tools/rtl/error.h")
family_archive("original-opennt-rtl-native.lib"
  "obj/opennt-rtl/error.obj")
family_unit("obj/common/system_root.obj" "${NTVDM_REPO}/src/compat/system_root.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/common/guest_environment.obj" "${NTVDM_REPO}/src/compat/guest_environment.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("common-root.lib"
  "obj/common/system_root.obj" "obj/common/guest_environment.obj")
family_unit("obj/common/rpc_memory_crt.obj" "${NTVDM_REPO}/src/common/rpc/memory_crt.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/native_command.obj" "${NTVDM_REPO}/src/common/rpc/native_command.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/frontend_control.obj" "${NTVDM_REPO}/src/common/rpc/frontend_control.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/worker_control.obj" "${NTVDM_REPO}/src/common/rpc/worker_control.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/management.obj" "${NTVDM_REPO}/src/common/rpc/management.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/dos_observation.obj" "${NTVDM_REPO}/src/common/rpc/dos_observation.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/interface_query.obj" "${NTVDM_REPO}/src/common/rpc/interface_query.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h" "obj/basesrv/rpc_management.h")
family_unit("obj/common/rpc_management_stub.obj" "obj/basesrv/rpc_management_c.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h" "obj/basesrv/rpc_management.h")
family_archive("common-rpc.lib"
  "obj/common/rpc_memory_crt.obj" "obj/common/native_command.obj" "obj/common/frontend_control.obj" "obj/common/worker_control.obj" "obj/common/management.obj" "obj/common/dos_observation.obj" "obj/common/interface_query.obj" "obj/common/rpc_management_stub.obj")
family_unit("obj/lib-win32/rpc/local_binding.obj" "${NTVDM_REPO}/src/lib-win32/rpc/local_binding.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/rpc/peer.obj" "${NTVDM_REPO}/src/lib-win32/rpc/peer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-rpc.lib"
  "obj/lib-win32/rpc/local_binding.obj" "obj/lib-win32/rpc/peer.obj")
family_unit("obj/lib-win32/pipe/pipe_transfer.obj" "${NTVDM_REPO}/src/lib-win32/pipe/pipe_transfer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/pipe/pipe_pair.obj" "${NTVDM_REPO}/src/lib-win32/pipe/pipe_pair.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-pipe.lib"
  "obj/lib-win32/pipe/pipe_transfer.obj" "obj/lib-win32/pipe/pipe_pair.obj")
family_unit("obj/lib-win32/process-observation/observation.obj" "${NTVDM_REPO}/src/lib-win32/process-observation/observation.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/process-observation/console_members.obj" "${NTVDM_REPO}/src/lib-win32/process-observation/console_members.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-process-observation.lib"
  "obj/lib-win32/process-observation/observation.obj" "obj/lib-win32/process-observation/console_members.obj")
family_unit("obj/common/native_launch.obj" "${NTVDM_REPO}/src/common/codec/native_launch.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-codec.lib"
  "obj/common/native_launch.obj")
family_unit("obj/common/console_client.obj" "${NTVDM_REPO}/src/compat/console_transport.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("common-console.lib"
  "obj/common/console_client.obj")
family_image("ntvwm.exe" "/nologo /machine:x64 /subsystem:console /entry:wmainCRTStartup /opt:ref"
  "worker-base.lib" "obj/ntvwm/next_command.obj" "obj/ntvwm/main.obj" "obj/ntvwm/presentation.obj" "obj/compat/native_console_publication.obj" "obj/compat/native_console_text_frame.obj" "obj/ntvwm/console_state.obj" "obj/compat/native_console_diagnostics.obj" "obj/ntvwm/execution.obj" "obj/common/native_process.obj" "common-native-hook-installer.lib" "common-native-hook-context.lib" "lib-process-launch.lib" "lib-process-image.lib" "lib-process-hook.lib" "lib-handle.lib" "lib-file.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-native.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/ntvwm_execution_lifetime.obj" "${NTVDM_REPO}/test/observation/ntvwm_execution_lifetime_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("ntvwm-execution-lifetime-test.exe" "/nologo /machine:x64 /subsystem:console /entry:wmainCRTStartup /opt:ref"
  "obj/tests/ntvwm_execution_lifetime.obj" "obj/ntvwm/execution.obj" "obj/common/native_process.obj" "common-codec.lib" "common-native-hook-installer.lib" "common-native-hook-context.lib" "obj/run16/support.obj" "original-opennt-rtl-native.lib" "lib-process-launch.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/common_worker_control.obj" "${NTVDM_REPO}/test/common/rpc/worker_control_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("common-worker-control-test.exe" "/nologo /machine:x64 /subsystem:console /entry:mainCRTStartup /opt:ref"
  "obj/tests/common_worker_control.obj" "common-rpc.lib" "lib-rpc.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/ntvwm_presentation.obj" "${NTVDM_REPO}/test/observation/ntvwm_presentation_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/tests/ntvwm_presentation_provider.obj" "${NTVDM_REPO}/src/ntvwm-exe/presentation.c" "${native_flags} /we4013 /Dntvwm_capture_begin=fixture_capture_begin /O2" "obj/basesrv/service.h")
family_unit("obj/frontend-base/channel.obj" "${NTVDM_REPO}/src/frontend-base/channel.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend-base/service.obj" "${NTVDM_REPO}/src/frontend-base/service.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/compat/console_video_buffer.obj" "${NTVDM_REPO}/src/compat/console_video_buffer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("frontend-base.lib"
  "obj/frontend-base/channel.obj" "obj/frontend-base/service.obj" "obj/compat/console_video_buffer.obj")
family_image("ntvwm-presentation-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/ntvwm_presentation.obj" "obj/tests/ntvwm_presentation_provider.obj" "obj/compat/native_console_publication.obj" "obj/ntvwm/console_state.obj" "obj/compat/native_console_diagnostics.obj" "obj/compat/native_console_text_frame.obj" "frontend-base.lib" "worker-base.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
add_custom_target(product ALL DEPENDS "${CMAKE_BINARY_DIR}/ntvwm.exe")
