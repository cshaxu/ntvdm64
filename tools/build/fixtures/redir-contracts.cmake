if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
ntvdm_source_projection(dlc_worker_exact.h
  "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrdlc5c.c"
  "BOOLEAN\nInitializeDlcWorkerThread(\n    VOID\n    )\n\n/*++"
  "VOID\nVrDlcWorkerThread(\n    IN LPVOID Parameters\n    )\n\n/*++" "" FALSE)
ntvdm_source_projection(redir_queue_exact.h
  "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrinit.c"
  "VOID\nVrRaiseInterrupt(" "VOID\nVrCheckPmNetbiosAnr(\n    VOID\n    )\n\n/*++" "" FALSE)
ntvdm_source_projection(redirector_loader_selected.inc
  "${NTVDM_REPO}/src/mvdm-host/softpc.new/host/src/nt_bop.c"
  "ULONG DefaultVrInitialized(VOID);"
  "/*::::::::::::::::::::::::::::::::::::::::::::::::::::::: More MS BOP stubs */" "" FALSE)
foreach(pair IN ITEMS "dlc-event|dlc_worker_event_test.c" "redir-queue|redir_async_queue_test.c" "redir-loader|redirector_loader_test.c")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 source)
  fixture(${name} CL "test/integration/${source}")
  set_property(TARGET ${name}_objects PROPERTY COMPILE_OPTIONS /nologo /TC /O2)
  target_include_directories(${name}_objects PRIVATE "${CMAKE_BINARY_DIR}")
endforeach()
# Preserve the loader fixture's original default optimization profile.
set_property(TARGET redir-loader_objects PROPERTY COMPILE_OPTIONS /nologo /TC)
add_library(redir-loader-provider SHARED "${NTVDM_REPO}/test/integration/redirector_loader_dll.c")
set_target_properties(redir-loader-provider PROPERTIES EXCLUDE_FROM_ALL TRUE PREFIX "")
target_compile_options(redir-loader-provider PRIVATE /nologo /MT)
add_custom_target(redir-contracts DEPENDS dlc-event redir-queue redir-loader redir-loader-provider)
