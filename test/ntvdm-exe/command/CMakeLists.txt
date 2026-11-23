set(_capture_coordination "${CMAKE_CURRENT_BINARY_DIR}/guest-memory-coordination.c")
include("${CMAKE_SOURCE_DIR}/tools/build/SourceProjection.cmake")
file(RELATIVE_PATH _capture_relative "${CMAKE_BINARY_DIR}" "${_capture_coordination}")
ntvdm_guest_memory_projection("${_capture_relative}" "#line 1960")
add_executable(mvdm-command-capture-test
  command_capture.c
  "${_capture_coordination}"
  "${CMAKE_SOURCE_DIR}/src/compat/guest_memory_lease.c"
  "${CMAKE_SOURCE_DIR}/src/compat/command_services.c"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/command/source/mvdm_command_binding.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_command_native_child.c"
  "${CMAKE_SOURCE_DIR}/src/compat/guest_location.c"
  "${CMAKE_SOURCE_DIR}/src/compat/host_context.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_capture.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_text.c"
  "${CMAKE_SOURCE_DIR}/src/compat/console_video_buffer.c")
target_include_directories(mvdm-command-capture-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/command/include"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/include")
target_compile_options(mvdm-command-capture-test PRIVATE /W4 /WX /Gy)
target_compile_features(mvdm-command-capture-test PRIVATE c_std_11)
target_link_options(mvdm-command-capture-test PRIVATE /OPT:REF)
target_link_libraries(mvdm-command-capture-test PRIVATE ntdll)
add_test(NAME mvdm-command-capture COMMAND mvdm-command-capture-test)
