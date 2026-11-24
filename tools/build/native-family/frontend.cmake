# Explicit source and link composition; no reference graph is consumed.
set(native_flags "${family_flags} /I \"${NTVDM_REPO}/src/lib-nxvm\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/compat/include\" /I \"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc\" /I obj/basesrv")
family_unit("obj/frontend/main.obj" "${NTVDM_REPO}/src/ntcon-exe/main.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/session_arguments.obj" "${NTVDM_REPO}/src/ntcon-exe/session_arguments.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/session_service.obj" "${NTVDM_REPO}/src/ntcon-exe/session_service.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/frontend_session.obj" "${NTVDM_REPO}/src/ntcon-exe/frontend_session.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/window/window_frame.obj" "${NTVDM_REPO}/src/ntcon-exe/window_frame.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/window/window_controller.obj" "${NTVDM_REPO}/src/ntcon-exe/window_controller.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/window/window_input_queue.obj" "${NTVDM_REPO}/src/ntcon-exe/window_input_queue.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/window/window_keyboard.obj" "${NTVDM_REPO}/src/ntcon-exe/window_keyboard.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
family_unit("obj/frontend/window/window_mouse.obj" "${NTVDM_REPO}/src/ntcon-exe/window_mouse.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
include(nxvm.cmake)
family_archive("frontend-window.lib"
  ${nxvm_objects} "obj/frontend/window/window_frame.obj" "obj/frontend/window/window_controller.obj" "obj/frontend/window/window_input_queue.obj" "obj/frontend/window/window_keyboard.obj" "obj/frontend/window/window_mouse.obj")
family_unit("obj/frontend/console_frontend.obj" "${NTVDM_REPO}/src/ntcon-exe/console_frontend.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend-base/channel.obj" "${NTVDM_REPO}/src/frontend-base/channel.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/frontend-base/service.obj" "${NTVDM_REPO}/src/frontend-base/service.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/compat/console_video_buffer.obj" "${NTVDM_REPO}/src/compat/console_video_buffer.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_archive("frontend-base.lib"
  "obj/frontend-base/channel.obj" "obj/frontend-base/service.obj" "obj/compat/console_video_buffer.obj")
family_unit("obj/frontend/console_channel.obj" "${NTVDM_REPO}/src/ntcon-exe/console_channel.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/run16/support.obj" "${NTVDM_REPO}/src/compat/opennt_support_rtl.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /Gy /O2")
family_unit("obj/run16/rpc_client_core.obj" "${NTVDM_REPO}/src/common/base_srv/base_rpc_client_core.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/common/frontend_rpc_client.obj" "${NTVDM_REPO}/src/common/base_srv/frontend_client.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("base-rpc-client-core.lib"
  "obj/run16/rpc_client_core.obj" "obj/common/frontend_rpc_client.obj")
family_unit("obj/run16/stub.obj" "obj/basesrv/service_c.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-abi-host-compat/console_grid.obj" "${NTVDM_REPO}/src/compat/console_grid.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/command.obj" "${NTVDM_REPO}/src/common/base_srv/base_command.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/values.obj" "${NTVDM_REPO}/src/common/base_srv/base_values.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/payload.obj" "${NTVDM_REPO}/src/common/base_srv/base_payload.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/startup.obj" "${NTVDM_REPO}/src/common/base_srv/base_startup.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/opennt-base-bindings/config.obj" "${NTVDM_REPO}/src/compat/base_vdm_config.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/capture-heap.obj" "${NTVDM_REPO}/src/compat/base_capture_heap.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/process.obj" "${NTVDM_REPO}/src/compat/base_console_handle.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/classifier-path.obj" "${NTVDM_REPO}/src/compat/base_classifier_path.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_unit("obj/opennt-base-bindings/config-command.obj" "${CMAKE_BINARY_DIR}/generated/base_vdm_config.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_unit("obj/common/native_image.obj" "${NTVDM_REPO}/src/common/native_image.c" "${native_flags} /we4013 /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/inc\" /I \"${NTVDM_REPO}/src/non-mvdm/base/win32/server\" /O2" "obj/basesrv/service.h")
family_archive("common-base-srv-bindings.lib"
  "obj/opennt-base-bindings/command.obj" "obj/opennt-base-bindings/values.obj" "obj/opennt-base-bindings/payload.obj" "obj/opennt-base-bindings/startup.obj" "obj/opennt-base-bindings/config.obj" "obj/opennt-base-bindings/capture-heap.obj" "obj/opennt-base-bindings/process.obj" "obj/opennt-base-bindings/classifier-path.obj" "obj/opennt-base-bindings/config-command.obj" "obj/common/native_image.obj")
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
family_image("ntcon.exe" "/nologo /machine:x64 /subsystem:console /entry:wmainCRTStartup /opt:ref"
  "obj/frontend/main.obj" "obj/frontend/session_arguments.obj" "obj/frontend/session_service.obj" "obj/frontend/frontend_session.obj" "frontend-window.lib" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/frontend/console_channel.obj" "obj/run16/support.obj" "base-rpc-client-core.lib" "obj/run16/stub.obj" "obj/opennt-abi-host-compat/console_grid.obj" "common-base-srv-bindings.lib" "common-base-srv-transport.lib" "common-rpc.lib" "lib-rpc.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "common-codec.lib" "common-console.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_base_channel.obj" "${NTVDM_REPO}/test/frontend-base/channel_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("frontend-base-channel-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_base_channel.obj" "frontend-base.lib" "lib-pipe.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_base_service.obj" "${NTVDM_REPO}/test/frontend-base/service_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("frontend-base-service-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_base_service.obj" "frontend-base.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_base_video.obj" "${NTVDM_REPO}/test/frontend-base/video_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("frontend-base-video-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_base_video.obj" "frontend-base.lib" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_session_arguments.obj" "${NTVDM_REPO}/test/app/frontend_session_arguments_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("frontend-session-arguments-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_session_arguments.obj" "obj/frontend/session_arguments.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_window_library.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_library_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /O2" "obj/basesrv/service.h")
family_unit("obj/compat/native_console_text_frame.obj" "${NTVDM_REPO}/src/compat/native_console_text_frame.c" "${family_flags} /I \"${NTVDM_REPO}/src\" /we4013 /O2")
family_image("frontend-window-library-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_window_library.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_window_controller.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_controller_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /O2" "obj/basesrv/service.h")
family_image("frontend-window-controller-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_window_controller.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_window_keyboard.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_keyboard_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /O2" "obj/basesrv/service.h")
family_image("frontend-window-keyboard-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_window_keyboard.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_window_mouse.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_mouse_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /O2" "obj/basesrv/service.h")
family_image("frontend-window-mouse-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_window_mouse.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_window_lifetime.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_lifetime_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /O2" "obj/basesrv/service.h")
family_image("frontend-window-lifetime-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_window_lifetime.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_window_payload.obj" "${NTVDM_REPO}/test/ntcon-exe/frontend_window_payload_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /I \"${CMAKE_BINARY_DIR}/frontend-font\" /O2" "obj/basesrv/service.h")
family_image("frontend-window-payload-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_window_payload.obj" "frontend-window.lib" "frontend-base.lib" "obj/compat/native_console_text_frame.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/console_frontend_test.obj" "${NTVDM_REPO}/test/app/console_frontend_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("console-frontend-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/console_frontend_test.obj" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/console_video.obj" "${NTVDM_REPO}/test/app/console_video_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("console-video-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/console_video.obj" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/frontend_text_handoff.obj" "${NTVDM_REPO}/test/app/frontend_text_handoff_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("frontend-text-handoff-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/frontend_text_handoff.obj" "obj/frontend/frontend_session.obj" "frontend-window.lib" "obj/frontend/console_frontend.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/console_channel_lifetime.obj" "${NTVDM_REPO}/test/app/console_channel_lifetime_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("console-channel-lifetime-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/console_channel_lifetime.obj" "frontend-base.lib" "obj/frontend/frontend_session.obj" "frontend-window.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "obj/opennt-abi-host-compat/console_grid.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/console_frame_failure.obj" "${NTVDM_REPO}/test/app/console_frame_failure_test.c" "${native_flags} /std:c11 /we4013 /I \"${NTVDM_REPO}/src/ntcon-exe\" /O2" "obj/basesrv/service.h")
family_image("console-frame-failure-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/console_frame_failure.obj" "frontend-base.lib" "frontend-window.lib" "lib-pipe.lib" "lib-process-observation.lib" "lib-file.lib" "obj/opennt-abi-host-compat/console_grid.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
family_unit("obj/tests/console_pointer_dispatch_test.obj" "${NTVDM_REPO}/test/app/console_pointer_dispatch_test.c" "${native_flags} /we4013 /O2" "obj/basesrv/service.h")
family_image("console-pointer-contract-test.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  "obj/tests/console_pointer_dispatch_test.obj" "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj" "rpcrt4.lib" "ntdll.lib" "kernel32.lib" "shell32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "legacy_stdio_definitions.lib")
add_custom_target(product ALL DEPENDS "${CMAKE_BINARY_DIR}/ntcon.exe")

# VT fidelity uses the same current frontend objects as the product.
set(vt_objects)
foreach(source IN ITEMS test/observation/native_vt_fidelity_test.c
    src/ntvwm-exe/console_state.c src/compat/native_console_diagnostics.c
    src/common/native_process.c src/lib-win32/process-launch/attributes.c
    src/lib-win32/process-launch/process.c)
  get_filename_component(stem "${source}" NAME_WE)
  set(object "obj/tests/vt-${stem}.obj")
  family_unit("${object}" "${NTVDM_REPO}/${source}"
    "${native_flags} /std:c11 /DWIN32_LEAN_AND_MEAN /O2" "obj/basesrv/service.h")
  list(APPEND vt_objects "${object}")
endforeach()
family_image("native-vt-fidelity.exe" "/nologo /machine:x64 /subsystem:console /opt:ref"
  ${vt_objects} "obj/compat/native_console_text_frame.obj"
  "obj/frontend/frontend_session.obj" "obj/frontend/console_frontend.obj"
  "frontend-base.lib" "obj/opennt-abi-host-compat/console_grid.obj" "frontend-window.lib"
  "lib-process-observation.lib" "common-codec.lib"
  "kernel32.lib" "user32.lib" "gdi32.lib" "advapi32.lib" "shell32.lib" "legacy_stdio_definitions.lib")
