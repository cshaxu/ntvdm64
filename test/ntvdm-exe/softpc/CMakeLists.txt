add_executable(mvdm-vm-notification-test
  mvdm_vm_notification_test.c
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_binding.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_notification.c"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_bridge.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_wake.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_continuation.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_guest_io.c"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_application.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_input_bop.c"
  mvdm_vm_io_test.c
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_executor.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_text.c"
  "${CMAKE_SOURCE_DIR}/src/compat/console_video_buffer.c")
target_include_directories(mvdm-vm-notification-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/include")
target_compile_options(mvdm-vm-notification-test PRIVATE /W4 /WX)
target_compile_features(mvdm-vm-notification-test PRIVATE c_std_11)
add_test(NAME mvdm-vm-notification COMMAND mvdm-vm-notification-test)

add_executable(mvdm-vm-execute-test mvdm_vm_execute_test.c
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_execute.c")
target_include_directories(mvdm-vm-execute-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/include")
target_compile_options(mvdm-vm-execute-test PRIVATE /W4 /WX)
add_test(NAME mvdm-vm-execute COMMAND mvdm-vm-execute-test)
set_tests_properties(mvdm-vm-execute PROPERTIES TIMEOUT 30)

add_executable(mvdm-vm-ingress-test mvdm_vm_ingress_test.c
  "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/mvdm_vm_ingress.c"
  "${CMAKE_SOURCE_DIR}/src/compat/mvdm_vm_command_entry.c")
target_include_directories(mvdm-vm-ingress-test PRIVATE
  "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/ntvdm-exe/softpc/include")
target_compile_options(mvdm-vm-ingress-test PRIVATE /W4 /WX)
add_test(NAME mvdm-vm-ingress COMMAND mvdm-vm-ingress-test)
