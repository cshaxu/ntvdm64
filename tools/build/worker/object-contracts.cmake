# Eight current-object contracts share fixture production, not runtime state.
set(contract_common "/nologo /TC /c /W4 /MT /Od /D_CRT_SECURE_NO_WARNINGS")
set(contract_headers "/DNTVDM /DSPC386 /DANSI /I\"${NTVDM_REPO}/src/mvdm-host/softpc.new/host/inc\" /I\"${NTVDM_REPO}/src/mvdm-host/softpc.new/base/inc\"")
function(softpc_contract name negative size flags libraries)
  set(source "${NTVDM_REPO}/test/mvdm-host/${name}_object_contract_fixture.c")
  set(object "obj/tests/object-contract/${name}.obj")
  worker_unit("${object}" "${source}" "${contract_common} ${flags}")
  worker_link("${name}-object-contract.exe" "/nologo /out:$out /map:$out.map"
    "kernel32.lib ${libraries}" INPUTS "${object}" ${ARGN})
  worker_group("softpc-${name}" "${name}-object-contract.exe")
  worker_unit("obj/tests/object-contract/${name}-negative.obj" "${source}"
    "${contract_common} ${flags} /D${negative}=${size}")
  worker_group("softpc-${name}-negative" "obj/tests/object-contract/${name}-negative.obj")
  set_property(GLOBAL APPEND PROPERTY softpc_contracts "${CMAKE_BINARY_DIR}/${name}-object-contract.exe")
endfunction()
softpc_contract(qevnt MVDM_QEV_EXPECT_HANDLE_BYTES 4 "" "" obj/system/qevnt.obj)
softpc_contract(ios MVDM_IO_EXPECT_WORD_BYTES 4 "${contract_headers}" ""
  obj/support/ios.obj obj/ccpu/in.obj obj/ccpu/out.obj)
softpc_contract(gfi MVDM_GFI_EXPECT_TABLE_BYTES 4 "${contract_headers} /DPROD" ""
  obj/disks/gfi.obj obj/disks/gfi_mpty.obj)
softpc_contract(keycode MVDM_KEYCODE_EXPECT_EVENT_BYTES 4 "${contract_headers}" "user32.lib ntdll.lib"
  obj/host/nt_keycd.obj obj/host/nt_input.obj)
softpc_contract(nls MVDM_NLS_EXPECT_CAPACITY 4
  "/I\"${NTVDM_REPO}/src/mvdm-host/softpc.new/host/inc\"" "user32.lib"
  obj/host/nt_nls.obj obj/host/softpc-resource.res)
softpc_contract(umb MVDM_UMB_EXPECT_NODE_BYTES 4
  "${contract_headers} /DCPU_40_STYLE /DCCPU /DV7VGA /DPROD /I\"${NTVDM_REPO}/src/compat/include/generated/x86/prod\"" "" obj/host/nt_umb.obj)
softpc_contract(vdd MVDM_VDD_EXPECT_HANDLER_BYTES 4
  "${contract_headers} /DPROD /I\"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc\"" ""
  obj/host/nt_vdd.obj obj/support/ios.obj obj/ccpu/in.obj obj/ccpu/out.obj)
softpc_contract(niddb MVDM_NIDDB_EXPECT_POINTER_BYTES 8 "" "" obj/bios/virtual.obj)
get_property(softpc_contracts GLOBAL PROPERTY softpc_contracts)
add_custom_target(softpc-contracts DEPENDS ${softpc_contracts})

# Link the actual product-selected packstr object, not a copied algorithm.
worker_unit("obj/tests/netlib-packstr.obj" "${NTVDM_REPO}/test/opennt-host/netlib/packstr.c" "/nologo /MT /TC /c")
worker_link("netlib-packstr.exe" "/nologo /out:$out" "kernel32.lib"
  INPUTS "obj/tests/netlib-packstr.obj" "obj/opennt-netlib/packstr.obj")
worker_group(netlib-packstr "netlib-packstr.exe")

# Preserve both rejected kernel-i386 boundaries and their original diagnostics.
set(modesw_source "${NTVDM_REPO}/src/mvdm-host/dpmi32/modesw.c")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${modesw_source}")
file(READ "${modesw_source}" modesw_text)
string(REPLACE "\r\n" "\n" modesw_text "${modesw_text}")
set(modesw_index 0)
foreach(modesw_else IN ITEMS 109 171)
  string(REGEX MATCH "#if !defined\\(_X86_\\) \\|\\| defined\\(CCPU\\)[^#]*#line ${modesw_else}\n#else\n#line [0-9]+\n#error DIVERGENCE MVDM-HOST-DIV-619[^\n]*\n#line [0-9]+\n#endif" guard "${modesw_text}")
  if(NOT guard)
    message(FATAL_ERROR "Missing original modesw software-first branch at ${modesw_else}")
  endif()
  math(EXPR modesw_index "${modesw_index}+1")
  set(input "${CMAKE_BINARY_DIR}/generated/modesw-guard-${modesw_index}.c")
  file(GENERATE OUTPUT "${input}" CONTENT "${guard}\n")
  add_custom_target(modesw-guard-${modesw_index}
    COMMAND "${CMAKE_C_COMPILER}" /nologo /TC /E /D_X86_ /DCCPU "${input}" VERBATIM)
  add_custom_target(modesw-guard-${modesw_index}-negative
    COMMAND "${CMAKE_C_COMPILER}" /nologo /TC /E /D_X86_ "${input}" VERBATIM)
endforeach()
