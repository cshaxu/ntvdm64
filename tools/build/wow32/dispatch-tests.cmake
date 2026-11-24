include("${CMAKE_CURRENT_LIST_DIR}/../SourceProjection.cmake")
ntvdm_source_projection(dispatch/original-wndproc.inc
  "${NTVDM_REPO}/src/mvdm-host/wow32/wmdisp32.c"
  "LONG W32Win16WndProcEx(" "// The following functions are used to" "" FALSE)
get_target_property(dispatch_flags original_wow32 COMPILE_FLAGS)
wow_unit(dispatch_owner "${NTVDM_REPO}/src/mvdm-host/wow32/wow32.c"
  "${dispatch_flags} /Gy /Gw /GL")
wow_unit(dispatch_scope "${NTVDM_REPO}/src/compat/wow_user_thunk_scope.c"
  "${cflags} /Gy /Gw /GL /Dwow_user_invoke_thunk=fixture_unused_original_invoke")
wow_unit(dispatch_fixture "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_dispatch_exception_fixture.c"
  "${cflags} /Gy /Gw /GL /I \"${CMAKE_BINARY_DIR}/dispatch\"")
foreach(name IN ITEMS dispatch_owner dispatch_scope dispatch_fixture)
  set_property(TARGET ${name} PROPERTY EXCLUDE_FROM_ALL TRUE)
endforeach()
# Existing closed list only. The runner rejects retained unrelated owners.
set(dispatch_aliases
  "/alternatename:_AddProcessSharedList@0=_fixture_unreachable_dependency"
  "/alternatename:_RegisterWowBaseHandlers@4=_fixture_unreachable_dependency"
  "/alternatename:_SetFakeDialogClass@4=_fixture_unreachable_dependency"
  "/alternatename:_W32LocalUnlock@8=_fixture_unreachable_dependency"
  "/alternatename:_ExitVDM@8=_fixture_unreachable_dependency"
  "/alternatename:_CleanseSharedList@0=_fixture_unreachable_dependency"
  "/alternatename:_WU32ICBStoreHandle@8=_fixture_unreachable_dependency"
  "/alternatename:_W32GlobalFree16@4=_fixture_unreachable_dependency"
  "/alternatename:_NtvdmGetWindowsDirectoryA=_fixture_unreachable_dependency"
  "/alternatename:_W32LocalReAlloc@20=_fixture_unreachable_dependency"
  "/alternatename:_InitVisRgn@0=_fixture_unreachable_dependency"
  "/alternatename:_DBGNotifyDebugged=_fixture_unreachable_dependency"
  "/alternatename:_getMSW=_fixture_unreachable_dependency"
  "/alternatename:_WK32InitWowIsKnownDLL@4=_fixture_unreachable_dependency"
  "/alternatename:_W32LockResource@8=_fixture_unreachable_dependency"
  "/alternatename:_ShowStartGlass@4=_fixture_unreachable_dependency"
  "/alternatename:_InitThunkTableOffsets@0=_fixture_unreachable_dependency"
  "/alternatename:_W32GlobalAlloc16@8=_fixture_unreachable_dependency"
  "/alternatename:_W32Win16WndProcEx@24=_fixture_unreachable_dependency"
  "/alternatename:_W32InitHookState@4=_fixture_unreachable_dependency"
  "/alternatename:_Sim32pGetVDMPointer=_fixture_unreachable_dependency"
  "/alternatename:_W32GetExpWinVer@4=_fixture_unreachable_dependency"
  "/alternatename:_W32SizeofResource@8=_fixture_unreachable_dependency"
  "/alternatename:_W32LocalSize@8=_fixture_unreachable_dependency"
  "/alternatename:_ntvdm_shadow_reg_open_key_ex@20=_fixture_unreachable_dependency"
  "/alternatename:_ntvdm_shadow_reg_query_value_ex@24=_fixture_unreachable_dependency"
  "/alternatename:_InitCBFormats@0=_fixture_unreachable_dependency"
  "/alternatename:_W32LocalLock@8=_fixture_unreachable_dependency"
  "/alternatename:_W32FindResource@16=_fixture_unreachable_dependency"
  "/alternatename:_NtvdmGetSystemDirectoryA=_fixture_unreachable_dependency"
  "/alternatename:_W32UnlockResource@8=_fixture_unreachable_dependency"
  "/alternatename:_CreateSmallHeap@0=_fixture_unreachable_dependency"
  "/alternatename:_W32InitWOWSetupNames@0=_fixture_unreachable_dependency"
  "/alternatename:_W32LocalFree@8=_fixture_unreachable_dependency"
  "/alternatename:_W32EmptyClipboard@0=_fixture_unreachable_dependency"
  "/alternatename:_W32LoadResource@8=_fixture_unreachable_dependency"
  "/alternatename:_W32EditNextWord@20=_fixture_unreachable_dependency"
  "/alternatename:_W32DDEFreeGlobalMem32@4=_fixture_unreachable_dependency"
  "/alternatename:_W32InitDlg@8=_fixture_unreachable_dependency"
  "/alternatename:_W32LocalAlloc@12=_fixture_unreachable_dependency"
  "/alternatename:_W32FreeResource@8=_fixture_unreachable_dependency"
  "/alternatename:_WK32InitializeHungAppSupport@0=_fixture_unreachable_dependency"
  "/alternatename:_ntvdm_shadow_reg_close_key@4=_fixture_unreachable_dependency"
  "/alternatename:_W32ExitWowProcess@4=_fixture_unreachable_dependency"
  "/alternatename:_UserRegisterWowHandlers@8=_fixture_unreachable_dependency"
  "/alternatename:_InitParamMap@0=_fixture_unreachable_dependency"
  "/alternatename:_VdmLoadCritSec=_fixture_unreachable_data"
  "/alternatename:_tableoffsets=_fixture_unreachable_data"
  "/alternatename:_TimerList=_fixture_unreachable_data"
  "/alternatename:_vpDebugWOW=_fixture_unreachable_data"
  "/alternatename:_dwWOWBatchLimit=_fixture_unreachable_data"
)
set(dispatch_inputs $<TARGET_OBJECTS:dispatch_owner> $<TARGET_OBJECTS:dispatch_scope> $<TARGET_OBJECTS:dispatch_fixture>)
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/wow-dispatch-exception.exe"
  COMMAND "${CMAKE_LINKER}" /nologo /LTCG /OPT:REF
    "/out:${CMAKE_BINARY_DIR}/wow-dispatch-exception.exe"
    "/map:${CMAKE_BINARY_DIR}/wow-dispatch-exception.exe.map"
    ${dispatch_inputs} ${dispatch_aliases}
    kernel32.lib user32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib
  DEPENDS dispatch_owner dispatch_scope dispatch_fixture ${dispatch_inputs}
  COMMAND_EXPAND_LISTS VERBATIM)
add_custom_target(wow-dispatch-exception DEPENDS "${CMAKE_BINARY_DIR}/wow-dispatch-exception.exe")
