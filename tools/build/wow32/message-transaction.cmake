# Current diagnostic owner; historical sealed P21 objects are not current inputs.
set(root "${CMAKE_BINARY_DIR}/diagnostics/message-transaction")
configure_file("${NTVDM_REPO}/src/mvdm-host/wow32/wmdisp32.c"
  "${root}/message_original_owner.c" COPYONLY)
wow_diagnostic_provider(message-transaction original_wmdisp32
  "${NTVDM_REPO}/test/observation/wow_message_transaction_provider.c"
  "${CMAKE_BINARY_DIR}/wow32-x86.def")
target_include_directories(diagnostic_message_transaction_object PRIVATE "${root}")
