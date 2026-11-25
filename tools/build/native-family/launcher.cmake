# Explicit source and link composition; no reference graph is consumed.
set(native_flags "${family_flags} /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I obj/basesrv")
family_unit("obj/run16/entry.obj" "${NTVDM_REPO}/src/run16-exe/main.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/common/application_search.obj" "${NTVDM_REPO}/src/common/application_search.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/run16/image_classification.obj" "${NTVDM_REPO}/src/run16-exe/image_classification.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/common/native_image.obj" "${NTVDM_REPO}/src/common/native_image.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/run16/launch_options.obj" "${NTVDM_REPO}/src/run16-exe/launch_options.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/run16/frontend_scope.obj" "${NTVDM_REPO}/src/run16-exe/frontend_scope.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/native_request_client.obj" "${NTVDM_REPO}/src/run16-exe/native_request_client.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/bootstrap_client.obj" "${NTVDM_REPO}/src/run16-exe/bootstrap_client.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("frontend-client.lib"
  "obj/frontend/native_request_client.obj" "obj/frontend/bootstrap_client.obj")
family_unit("obj/run16/support.obj" "${NTVDM_REPO}/src/compat/opennt_support_rtl.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /Gy /O2")
family_unit("obj/run16/rpc_client_core.obj" "${NTVDM_REPO}/src/common/base_srv/base_rpc_client_core.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/common/frontend_rpc_client.obj" "${NTVDM_REPO}/src/common/base_srv/frontend_client.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("base-rpc-client-core.lib"
  "obj/run16/rpc_client_core.obj" "obj/common/frontend_rpc_client.obj")
family_unit("obj/run16/rpc_client_launch.obj" "${NTVDM_REPO}/src/common/base_srv/base_rpc_client_launch.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /FI \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h\" /O2" "obj/basesrv/service.h")
family_unit("obj/compat/base_dispatch.obj" "${NTVDM_REPO}/src/compat/base_dispatch.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("base-rpc-client-launch.lib"
  "obj/run16/rpc_client_launch.obj" "obj/compat/base_dispatch.obj")
family_unit("obj/run16/stub.obj" "obj/basesrv/service_c.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-client/client.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c" "${native_flags} /we4013 /Gy /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /DOPENNT_BASE_CLIENT_VDM_COMMANDS /DOPENNT_BASE_CLIENT_VDM_ENVIRONMENT /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-client/classifier.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c" "${native_flags} /we4013 /Gy /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /DOPENNT_BASE_CLIENT_CLASSIFIER /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-client/capture.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntdll/csrutil.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("opennt-base-client.lib"
  "obj/opennt-base-client/client.obj" "obj/opennt-base-client/classifier.obj" "obj/opennt-base-client/capture.obj")
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
family_unit("obj/opennt-rtl/environ.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/environ.c" "${native_flags} /DNTOS_KERNEL_RUNTIME /FI \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h\" /O2" "obj/basesrv/service.h")
family_archive("original-opennt-rtl-native.lib"
  "obj/opennt-rtl/error.obj" "obj/opennt-rtl/environ.obj")
family_unit("obj/common/native-hook/context.obj" "${NTVDM_REPO}/src/common/native-hook/context.c" "${native_flags} /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/common/native-hook/packet.obj" "${NTVDM_REPO}/src/common/native-hook/packet.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-native-hook-context.lib"
  "obj/common/native-hook/context.obj" "obj/common/native-hook/packet.obj")
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
family_unit("obj/lib-win32/file/identity.obj" "${NTVDM_REPO}/src/lib-win32/file/identity.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/file/native_path.obj" "${NTVDM_REPO}/src/lib-win32/file/native_path.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-file.lib"
  "obj/lib-win32/file/identity.obj" "obj/lib-win32/file/native_path.obj")
family_unit("obj/common/native_launch.obj" "${NTVDM_REPO}/src/common/codec/native_launch.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-codec.lib"
  "obj/common/native_launch.obj")
family_unit("obj/common/console_client.obj" "${NTVDM_REPO}/src/compat/console_transport.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("common-console.lib"
  "obj/common/console_client.obj")
family_unit("obj/lib-win32/process-hook/staging.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/staging.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/helper.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/helper.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/payload.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/payload.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/startup_hooks.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/startup_hooks.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/minhook/src/buffer.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/buffer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/minhook/src/trampoline.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/trampoline.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_unit("obj/lib-win32/process-hook/minhook/src/hde/hde32.obj" "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/hde/hde64.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /std:c11 /O2 /Gy /we4013 /DWIN32_LEAN_AND_MEAN /O2")
family_archive("lib-process-hook.lib"
  "obj/lib-win32/process-hook/staging.obj" "obj/lib-win32/process-hook/helper.obj" "obj/lib-win32/process-hook/payload.obj" "obj/lib-win32/process-hook/startup_hooks.obj" "obj/lib-win32/process-hook/minhook/src/buffer.obj" "obj/lib-win32/process-hook/minhook/src/trampoline.obj" "obj/lib-win32/process-hook/minhook/src/hde/hde32.obj")
family_unit("obj/lib-win32/process-image/image.obj" "${NTVDM_REPO}/src/lib-win32/process-image/image.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-process-image.lib"
  "obj/lib-win32/process-image/image.obj")
family_image("run16.exe" "/nologo /machine:x64 /subsystem:console /entry:wWinMainCRTStartup /opt:ref"
  "obj/run16/entry.obj" "obj/common/application_search.obj" "obj/run16/image_classification.obj" "obj/common/native_image.obj" "obj/run16/launch_options.obj" "obj/run16/frontend_scope.obj" "frontend-client.lib" "obj/run16/support.obj" "base-rpc-client-core.lib" "base-rpc-client-launch.lib" "obj/run16/stub.obj" "opennt-base-client.lib" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "original-opennt-rtl-native.lib" "common-native-hook-context.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/run16_image_classification.obj" "${NTVDM_REPO}/test/observation/run16_image_classification_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("run16-image-classification-test.exe" "/nologo /machine:x64 /subsystem:console /entry:wmainCRTStartup /opt:ref"
  "obj/tests/run16_image_classification.obj" "obj/run16/image_classification.obj" "obj/common/native_image.obj" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "obj/run16/support.obj" "original-opennt-rtl-native.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/application_search.obj" "${NTVDM_REPO}/test/common/application_search_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("application-search-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/application_search.obj" "obj/common/application_search.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/native_capture.obj" "${NTVDM_REPO}/test/adapter-basesrv/native_capture_test.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /O2" "obj/basesrv/service.h")
family_image("native-capture-test.exe" "/nologo /machine:x64 /subsystem:console /entry:mainCRTStartup /opt:ref"
  "obj/tests/native_capture.obj" "obj/opennt-base-client/capture.obj" "obj/opennt-base-bindings/capture-heap.obj" "obj/run16/support.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_scope_lifetime.obj" "${NTVDM_REPO}/test/app/frontend_scope_lifetime_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/session_service.obj" "${NTVDM_REPO}/src/ntcon-exe/session_service.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend-base/channel.obj" "${NTVDM_REPO}/src/frontend-base/channel.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend-base/service.obj" "${NTVDM_REPO}/src/frontend-base/service.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/compat/console_video_buffer.obj" "${NTVDM_REPO}/src/compat/console_video_buffer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("frontend-base.lib"
  "obj/frontend-base/channel.obj" "obj/frontend-base/service.obj" "obj/compat/console_video_buffer.obj")
family_image("frontend-scope-lifetime-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_scope_lifetime.obj" "obj/run16/frontend_scope.obj" "obj/frontend/session_service.obj" "frontend-base.lib" "common-native-hook-context.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-hook.lib" "lib-file.lib" "lib-process-image.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/rtl_x86_fixture.obj" "${NTVDM_REPO}/test/opennt-host/rtl_x86_fixture.c" "${native_flags} /DOPENNT_ENVIRONMENT_ONLY /O2" "obj/basesrv/service.h")
family_image("rtl-x86-fixture.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/rtl_x86_fixture.obj" "original-opennt-rtl-native.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
add_custom_target(product ALL DEPENDS "${CMAKE_BINARY_DIR}/run16.exe")
