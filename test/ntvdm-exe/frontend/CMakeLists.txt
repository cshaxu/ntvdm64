add_executable(mvdm-vm-text-test vm_text_test.c
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_text.c"
  "${CMAKE_SOURCE_DIR}/src/compat/native_console_text_frame.c"
  "${CMAKE_SOURCE_DIR}/src/compat/console_video_buffer.c")
target_include_directories(mvdm-vm-text-test PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_compile_options(mvdm-vm-text-test PRIVATE /W4 /WX)
target_compile_features(mvdm-vm-text-test PRIVATE c_std_11)
add_test(NAME mvdm-vm-text COMMAND mvdm-vm-text-test)
