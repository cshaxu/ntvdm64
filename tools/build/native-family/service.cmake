# Explicit source and link composition; no reference graph is consumed.
set(native_flags "${family_flags} /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I obj/basesrv /we4312 /FI \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\"")
family_unit("obj/basesrv/entry.obj" "${NTVDM_REPO}/src/ntsrv-exe/main.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/basesrv/stub.obj" "obj/basesrv/service_s.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/run16/support.obj" "${NTVDM_REPO}/src/compat/opennt_support_rtl.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /Gy /O2")
family_unit("obj/opennt-base-server/srvvdm.obj" "${NTVDM_REPO}/src/non-mvdm/base/win32/server/srvvdm.c" "${native_flags} /DNULL=0 /we4013 /D_CSRSRV_ /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-server/exports.obj" "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/server/exports.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("opennt-base-server.lib"
  "obj/opennt-base-server/srvvdm.obj" "obj/opennt-base-server/exports.obj")
family_unit("obj/opennt-base-bindings/service.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/base_service.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/service-core.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/service_core.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/worker-registry.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/worker_registry.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/frontend-registry.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/frontend_registry.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/native-commands.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/native_commands.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/service-lifecycle.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/lifecycle.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/service-management.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/management.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/task-observation.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/task_observation.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/worker-spawn.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/worker_spawn.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/dispatch.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/base_dispatch.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/resources.obj" "${NTVDM_REPO}/src/compat/base_resource.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/resource-scope.obj" "${NTVDM_REPO}/src/compat/base_resource_scope.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/streams.obj" "${NTVDM_REPO}/src/compat/base_stream.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/waits.obj" "${NTVDM_REPO}/src/compat/base_wait.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/registry.obj" "${NTVDM_REPO}/src/compat/base_process.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/reservation.obj" "${NTVDM_REPO}/src/ntsrv-exe/opennt/source/base_reservation.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/request.obj" "${NTVDM_REPO}/src/compat/base_request.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/interactive.obj" "${NTVDM_REPO}/src/compat/base_interactive.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("opennt-base-bindings.lib"
  "obj/opennt-base-bindings/service.obj" "obj/opennt-base-bindings/service-core.obj" "obj/opennt-base-bindings/worker-registry.obj" "obj/opennt-base-bindings/frontend-registry.obj" "obj/opennt-base-bindings/native-commands.obj" "obj/opennt-base-bindings/service-lifecycle.obj" "obj/opennt-base-bindings/service-management.obj" "obj/opennt-base-bindings/task-observation.obj" "obj/opennt-base-bindings/worker-spawn.obj" "obj/opennt-base-bindings/dispatch.obj" "obj/opennt-base-bindings/resources.obj" "obj/opennt-base-bindings/resource-scope.obj" "obj/opennt-base-bindings/streams.obj" "obj/opennt-base-bindings/waits.obj" "obj/opennt-base-bindings/registry.obj" "obj/opennt-base-bindings/reservation.obj" "obj/opennt-base-bindings/request.obj" "obj/opennt-base-bindings/interactive.obj")
family_unit("obj/opennt-base-bindings/command.obj" "${NTVDM_REPO}/src/common/base_srv/base_command.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/values.obj" "${NTVDM_REPO}/src/common/base_srv/base_values.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/payload.obj" "${NTVDM_REPO}/src/common/base_srv/base_payload.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/startup.obj" "${NTVDM_REPO}/src/common/base_srv/base_startup.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/config.obj" "${NTVDM_REPO}/src/compat/base_vdm_config.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/capture-heap.obj" "${NTVDM_REPO}/src/compat/base_capture_heap.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/process.obj" "${NTVDM_REPO}/src/compat/base_console_handle.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/classifier-path.obj" "${NTVDM_REPO}/src/compat/base_classifier_path.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/opennt-base-bindings/config-command.obj" "${CMAKE_BINARY_DIR}/generated/base_vdm_config.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/common/native_image.obj" "${NTVDM_REPO}/src/common/native_image.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-base-srv-bindings.lib"
  "obj/opennt-base-bindings/command.obj" "obj/opennt-base-bindings/values.obj" "obj/opennt-base-bindings/payload.obj" "obj/opennt-base-bindings/startup.obj" "obj/opennt-base-bindings/config.obj" "obj/opennt-base-bindings/capture-heap.obj" "obj/opennt-base-bindings/process.obj" "obj/opennt-base-bindings/classifier-path.obj" "obj/opennt-base-bindings/config-command.obj" "obj/common/native_image.obj")
family_unit("obj/broker-transport/rpc_security.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/rpc_security.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_receipt.obj" "${NTVDM_REPO}/src/compat/vdm_receipt.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/broker-transport/vdm_delivery.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/vdm_delivery.c" "${native_flags} /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /we4013 /O2" "obj/basesrv/service.h")
family_archive("broker-transport.lib"
  "obj/broker-transport/rpc_security.obj" "obj/broker-transport/vdm_receipt.obj" "obj/broker-transport/vdm_delivery.obj")
family_unit("obj/broker-transport/rpc_scope.obj" "${NTVDM_REPO}/src/common/base_srv/scope.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_payload.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_payload.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_message.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_message.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-base-srv-transport.lib"
  "obj/broker-transport/rpc_scope.obj" "obj/broker-transport/vdm_payload.obj" "obj/broker-transport/vdm_message.obj")
family_unit("obj/opennt-rtl/error.obj" "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/error.c" "${native_flags} /I \"${NTVDM_REPO}/build/.ninja/build-tools/rtl\" /DNTOS_KERNEL_RUNTIME /DNTOS_KERNEL_RUNTIME /FI \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h\" /O2" "obj/basesrv/service.h" "${NTVDM_REPO}/build/.ninja/build-tools/rtl/error.h")
family_archive("original-opennt-rtl-native.lib"
  "obj/opennt-rtl/error.obj")
family_unit("obj/common/system_root.obj" "${NTVDM_REPO}/src/compat/system_root.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/common/guest_environment.obj" "${NTVDM_REPO}/src/compat/guest_environment.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
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
family_unit("obj/lib-win32/rpc/local_binding.obj" "${NTVDM_REPO}/src/lib-win32/rpc/local_binding.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/lib-win32/rpc/peer.obj" "${NTVDM_REPO}/src/lib-win32/rpc/peer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("lib-rpc.lib"
  "obj/lib-win32/rpc/local_binding.obj" "obj/lib-win32/rpc/peer.obj")
family_unit("obj/lib-win32/pipe/pipe_transfer.obj" "${NTVDM_REPO}/src/lib-win32/pipe/pipe_transfer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/lib-win32/pipe/pipe_pair.obj" "${NTVDM_REPO}/src/lib-win32/pipe/pipe_pair.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("lib-pipe.lib"
  "obj/lib-win32/pipe/pipe_transfer.obj" "obj/lib-win32/pipe/pipe_pair.obj")
family_unit("obj/lib-win32/process-observation/observation.obj" "${NTVDM_REPO}/src/lib-win32/process-observation/observation.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/lib-win32/process-observation/console_members.obj" "${NTVDM_REPO}/src/lib-win32/process-observation/console_members.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("lib-process-observation.lib"
  "obj/lib-win32/process-observation/observation.obj" "obj/lib-win32/process-observation/console_members.obj")
family_unit("obj/lib-win32/file/identity.obj" "${NTVDM_REPO}/src/lib-win32/file/identity.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/lib-win32/file/native_path.obj" "${NTVDM_REPO}/src/lib-win32/file/native_path.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("lib-file.lib"
  "obj/lib-win32/file/identity.obj" "obj/lib-win32/file/native_path.obj")
family_unit("obj/common/native_launch.obj" "${NTVDM_REPO}/src/common/codec/native_launch.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-codec.lib"
  "obj/common/native_launch.obj")
family_unit("obj/common/console_client.obj" "${NTVDM_REPO}/src/compat/console_transport.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("common-console.lib"
  "obj/common/console_client.obj")
family_unit("obj/lib-win32/process-launch/attributes.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/attributes.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/lib-win32/process-launch/process.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/process.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_unit("obj/lib-win32/process-launch/command_line.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/command_line.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("lib-process-launch.lib"
  "obj/lib-win32/process-launch/attributes.obj" "obj/lib-win32/process-launch/process.obj" "obj/lib-win32/process-launch/command_line.obj")
family_unit("obj/lib-win32/handle/duplicate.obj" "${NTVDM_REPO}/src/lib-win32/handle/duplicate.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4312 /we4013 /O2")
family_archive("lib-handle.lib"
  "obj/lib-win32/handle/duplicate.obj")
family_image("ntsrv.exe" "/nologo /machine:x64 /subsystem:windows /entry:mainCRTStartup /opt:ref"
  "obj/basesrv/entry.obj" "obj/basesrv/stub.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-native.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/base_service_reservation_test.obj" "${NTVDM_REPO}/test/adapter-basesrv/base_service_reservation_test.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /Dterminate=mvdm_softpc_terminate /D_CSRSRV_ /O2" "obj/basesrv/service.h")
family_unit("obj/tests/base_service_fixture.obj" "${NTVDM_REPO}/test/adapter-basesrv/base_service_fixture.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /Dterminate=mvdm_softpc_terminate /D_CSRSRV_ /O2" "obj/basesrv/service.h")
family_image("basesrv-service-reservation-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/base_service_reservation_test.obj" "obj/tests/base_service_fixture.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-native.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/basesrv_idle_policy.obj" "${NTVDM_REPO}/test/adapter-basesrv/idle_policy_test.c" "${native_flags} /O2" "obj/basesrv/service.h")
family_image("basesrv-idle-policy-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/basesrv_idle_policy.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/base_reservation_test.obj" "${NTVDM_REPO}/test/adapter-basesrv/base_reservation_test.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /Dterminate=mvdm_softpc_terminate /O2" "obj/basesrv/service.h")
family_image("basesrv-reservation-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/base_reservation_test.obj" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/native_service_layout.obj" "${NTVDM_REPO}/test/adapter-basesrv/native_service_layout_test.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /Dterminate=mvdm_softpc_terminate /D_CSRSRV_ /O2" "obj/basesrv/service.h")
family_image("native-service-layout-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/native_service_layout.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/native_observation_fixture.obj" "${NTVDM_REPO}/test/adapter-basesrv/native_observation_fixture.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /Dterminate=mvdm_softpc_terminate /D_CSRSRV_ /O2" "obj/basesrv/service.h")
family_image("native-observation-fixture.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/native_observation_fixture.obj" "obj/tests/base_service_fixture.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-native.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/dos_observation_fixture.obj" "${NTVDM_REPO}/test/adapter-basesrv/dos_observation_fixture.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/ntsrv-exe/opennt/include\" /Dterminate=mvdm_softpc_terminate /D_CSRSRV_ /O2" "obj/basesrv/service.h")
family_image("dos-observation-fixture.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/dos_observation_fixture.obj" "obj/tests/base_service_fixture.obj" "obj/run16/support.obj" "opennt-base-server.lib" "opennt-base-bindings.lib" "common-base-srv-bindings.lib" "broker-transport.lib" "common-base-srv-transport.lib" "original-opennt-rtl-native.lib" "common-root.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "lib-process-observation.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
add_custom_target(product ALL DEPENDS "${CMAKE_BINARY_DIR}/ntsrv.exe")
