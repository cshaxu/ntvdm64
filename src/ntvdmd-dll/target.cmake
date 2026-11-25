# The XP host debugger is a separate x86 image that imports the worker ABI.
set(ntvdmd_source "${NTVDM_REPO}/src/mvdm-host/dbgdll")
set(ntvdmd_objects)
foreach(unit IN ITEMS dbgdll brkpt data excptn module segment)
  worker_unit("obj/ntvdmd/${unit}.obj" "${ntvdmd_source}/${unit}.c"
    "/I \"${ntvdmd_source}\" ${profile_1} /Di386 /DWIN_32 /DDEBUG")
  list(APPEND ntvdmd_objects "obj/ntvdmd/${unit}.obj")
endforeach()
set(ntvdmd_version_dir "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc")
worker_resource("obj/ntvdmd/ntvdmd.res" "${ntvdmd_source}/ntvdmd.rc"
  "/nologo /I \"${ntvdmd_version_dir}\""
  "${ntvdmd_version_dir}/ntverp.h" "${ntvdmd_version_dir}/common.ver")
# LINK names the import-library thunks after the undecorated PE exports.
set(ntvdmd_register_imports " /alternatename:_VdmMapFlat@12=VdmMapFlat")
foreach(reg IN ITEMS AX EAX EBX ECX EDX ESI EDI ESP EBP EIP EFLAGS ES)
  string(APPEND ntvdmd_register_imports " /alternatename:_set${reg}@4=set${reg}")
endforeach()
worker_link("NTVDMD.DLL" "/nologo /dll /machine:x86 /subsystem:console /base:0x18000000 /entry:DbgDllInitialize@12${ntvdmd_register_imports} /map:$out.map /def:${ntvdmd_source}/ntvdmd.def /out:$out"
  "kernel32.lib advapi32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib"
  INPUTS ${ntvdmd_objects} "obj/ntvdmd/ntvdmd.res" "ntvdm.lib"
  DEPENDS "${ntvdmd_source}/ntvdmd.def")

set(NTVDMD_SP3_REFERENCE "" CACHE FILEPATH "Expanded English Professional SP3 NTVDMD.DLL")
add_custom_target(verify-ntvdmd-retail
  COMMAND "${CMAKE_COMMAND}" "-DREFERENCE=${NTVDMD_SP3_REFERENCE}"
    "-DIMAGE=${CMAKE_BINARY_DIR}/NTVDMD.DLL" -P "${CMAKE_CURRENT_LIST_DIR}/verify-retail.cmake"
  DEPENDS "${CMAKE_BINARY_DIR}/NTVDMD.DLL"
  VERBATIM)
