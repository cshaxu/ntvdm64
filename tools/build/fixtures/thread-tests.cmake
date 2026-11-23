ntvdm_source_projection(softpc_thread_exit.inc
  "${NTVDM_REPO}/src/mvdm-host/softpc.new/host/src/nt_thred.c"
  "VOID\nWINAPI\nhost_ExitThread(" "" "" FALSE)
fixture(nt-thread-alert-fixture LINK test/adapter-mvdm-host-out/nt_thread_alert_compat_fixture.c)
target_include_directories(nt-thread-alert-fixture_objects PRIVATE
  "${NTVDM_REPO}/src/compat/include" "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc")
set_property(TARGET nt-thread-alert-fixture_objects PROPERTY FIXTURE_LIBRARIES ntdll.lib)
fixture(softpc-event-thread-shutdown-fixture LINK src/compat/mvdm_softpc_event_thread.c
  test/compat/softpc_event_thread_shutdown_fixture.c)
set_property(TARGET softpc-event-thread-shutdown-fixture_objects PROPERTY FIXTURE_LIBRARIES ntdll.lib)
set(thread-start-session-fixture_link_byproducts
  "${CMAKE_BINARY_DIR}/thread-start-session-fixture.lib"
  "${CMAKE_BINARY_DIR}/thread-start-session-fixture.exp")
fixture(thread-start-session-fixture LINK ${resource_session}
  src/compat/thread_start.c src/lib-win32/thread/thread.c
  test/adapter-mvdm-host-out/thread_start_session_fixture.c)
target_compile_options(thread-start-session-fixture_objects PRIVATE /O2)
target_include_directories(thread-start-session-fixture_objects PRIVATE
  "${CMAKE_BINARY_DIR}" "${NTVDM_REPO}/src/compat/include")
set_property(TARGET thread-start-session-fixture_objects PROPERTY FIXTURE_LIBRARIES
  "/export:session_thread_current;/export:opennt_create_cdecl_thread_named;/implib:thread-start-session-fixture.lib")
add_library(thread_module_objects OBJECT
  "${NTVDM_REPO}/src/vdmredir-dll/source/mvdm_redirector_dll_entry.c"
  "${NTVDM_REPO}/test/adapter-mvdm-host-out/thread_start_module_fixture.c")
set_target_properties(thread_module_objects PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_include_directories(thread_module_objects PRIVATE "${NTVDM_REPO}/src")
target_compile_options(thread_module_objects PRIVATE /nologo /TC /O2 /W4)
if(NTVDM_ARCH STREQUAL "x86")
  set(thread_entry mvdm_vdmredir_dll_entry@12)
else()
  set(thread_entry mvdm_vdmredir_dll_entry)
endif()
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/thread-module.dll"
  COMMAND "${CMAKE_LINKER}" /nologo /dll "/entry:${thread_entry}"
    /export:thread_module_run /out:thread-module.dll
    $<TARGET_OBJECTS:thread_module_objects> thread-start-session-fixture.lib
    kernel32.lib libcmt.lib libvcruntime.lib libucrt.lib
  DEPENDS thread_module_objects $<TARGET_OBJECTS:thread_module_objects>
    "${CMAKE_BINARY_DIR}/thread-start-session-fixture.exe"
  BYPRODUCTS "${CMAKE_BINARY_DIR}/thread-module.lib" "${CMAKE_BINARY_DIR}/thread-module.exp"
  COMMAND_EXPAND_LISTS VERBATIM)
add_custom_target(thread-lifecycle DEPENDS nt-thread-alert-fixture
  softpc-event-thread-shutdown-fixture thread-start-session-fixture
  "${CMAKE_BINARY_DIR}/thread-module.dll")
