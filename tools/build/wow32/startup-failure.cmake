wow_diagnostic_provider(startup-failure obj_opennt-user_queue_obj
  "${NTVDM_REPO}/test/observation/wow_startup_failure_provider.c"
  "${CMAKE_BINARY_DIR}/wow32-x86.def")
wow_diagnostic_provider(shell-exception obj_compat_wow_shell_callback_obj
  "${NTVDM_REPO}/test/observation/wow_shell_exception_provider.c"
  "${CMAKE_BINARY_DIR}/wow32-x86.def")

# The witness inspects original TD/VDM stack state, unlike the leaf provider.
get_target_property(witness_flags original_wshell COMPILE_FLAGS)
set_target_properties(diagnostic_shell_exception_object PROPERTIES COMPILE_FLAGS "${witness_flags}")
