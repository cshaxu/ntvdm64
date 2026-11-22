# Explicit source and link composition; no reference graph is consumed.
set(native_flags "${family_flags} /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I obj/basesrv")
family_unit("obj/monitor/main.obj" "${NTVDM_REPO}/src/ntmon-exe/main.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/monitor/trace_view.obj" "${NTVDM_REPO}/src/ntmon-exe/trace_view.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/monitor/stub.obj" "obj/basesrv/service_c.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/rpc_scope.obj" "${NTVDM_REPO}/src/common/base_srv/scope.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_payload.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_payload.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_message.obj" "${NTVDM_REPO}/src/common/base_srv/vdm_message.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("common-base-srv-transport.lib"
  "obj/broker-transport/rpc_scope.obj" "obj/broker-transport/vdm_payload.obj" "obj/broker-transport/vdm_message.obj")
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
family_image("ntmon.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/monitor/main.obj" "obj/monitor/trace_view.obj" "obj/monitor/stub.obj" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/monitor_rpc_test.obj" "${NTVDM_REPO}/test/adapter-basesrv/monitor_rpc_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/rpc_security.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/rpc_security.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/broker-transport/vdm_receipt.obj" "${NTVDM_REPO}/src/compat/vdm_receipt.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/broker-transport/vdm_delivery.obj" "${NTVDM_REPO}/src/ntsrv-exe/transport/vdm_delivery.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_archive("broker-transport.lib"
  "obj/broker-transport/rpc_security.obj" "obj/broker-transport/vdm_receipt.obj" "obj/broker-transport/vdm_delivery.obj")
family_unit("obj/lib-win32/process-launch/attributes.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/attributes.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/process-launch/process.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/process.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/lib-win32/process-launch/command_line.obj" "${NTVDM_REPO}/src/lib-win32/process-launch/command_line.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-process-launch.lib"
  "obj/lib-win32/process-launch/attributes.obj" "obj/lib-win32/process-launch/process.obj" "obj/lib-win32/process-launch/command_line.obj")
family_unit("obj/lib-win32/handle/duplicate.obj" "${NTVDM_REPO}/src/lib-win32/handle/duplicate.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("lib-handle.lib"
  "obj/lib-win32/handle/duplicate.obj")
family_image("monitor-rpc-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/monitor_rpc_test.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/monitor_layout.obj" "${NTVDM_REPO}/test/observation/monitor_layout_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("monitor-layout-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/monitor_layout.obj" "obj/monitor/trace_view.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/monitor_session.obj" "${NTVDM_REPO}/test/observation/monitor_session_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("monitor-session-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/monitor_session.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/common_management.obj" "${NTVDM_REPO}/test/common/rpc/management_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("common-management-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/common_management.obj" "common-rpc.lib" "lib-rpc.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/monitor_trace_view.obj" "${NTVDM_REPO}/test/observation/monitor_trace_view_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("monitor-trace-view-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/monitor_trace_view.obj" "obj/monitor/trace_view.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/monitor_live_view.obj" "${NTVDM_REPO}/test/observation/monitor_live_view_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("monitor-live-view-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/monitor_live_view.obj" "obj/monitor/trace_view.obj" "obj/monitor/stub.obj" "broker-transport.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-process-launch.lib" "lib-handle.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
add_custom_target(product ALL DEPENDS "${CMAKE_BINARY_DIR}/ntmon.exe")
