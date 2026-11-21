add_executable(mvdm-command-guest-launch-test command_guest_launch.c
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_command_launch.c")
target_include_directories(mvdm-command-guest-launch-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/compat/include")
target_compile_options(mvdm-command-guest-launch-test PRIVATE /W4 /WX)
target_link_libraries(mvdm-command-guest-launch-test PRIVATE ntdll user32)
add_test(NAME mvdm-command-guest-launch COMMAND mvdm-command-guest-launch-test)
