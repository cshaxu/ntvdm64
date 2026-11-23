# Existing complete callback wrapper; only its one product TU is substituted.
wow_diagnostic_provider(callback-abort original_wcall16
  "${NTVDM_REPO}/test/observation/wow_callback_abort_provider.c"
  "${CMAKE_BINARY_DIR}/wow32-x86.def")
