add_executable(mvdm-command-capture-test
  command_capture.c
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/command/source/mvdm_command_native_child.c"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_command.c"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_guest_location.c"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/frontend/vm_text.c"
  "${CMAKE_SOURCE_DIR}/src/common/frontend-base/video.c")
target_include_directories(mvdm-command-capture-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/command/include"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/include")
target_compile_options(mvdm-command-capture-test PRIVATE /W4 /WX)
target_compile_features(mvdm-command-capture-test PRIVATE c_std_11)
add_test(NAME mvdm-command-capture COMMAND mvdm-command-capture-test)

add_executable(mvdm-command-guest-launch-test command_guest_launch.c
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/command/source/mvdm_command_launch.c")
target_include_directories(mvdm-command-guest-launch-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/compat/include"
  "${CMAKE_SOURCE_DIR}/src/non-mvdm/public/sdk/inc")
target_compile_options(mvdm-command-guest-launch-test PRIVATE /W4 /WX)
target_link_libraries(mvdm-command-guest-launch-test PRIVATE ntdll user32)
add_test(NAME mvdm-command-guest-launch COMMAND mvdm-command-guest-launch-test)
