# Bounded leaf tests reuse the selected XMS profile and original suballocator.
get_target_property(xms_flags unit_obj_xms_xmsblock_obj COMPILE_FLAGS)
worker_unit("obj/tests/xms/provider.obj" "${NTVDM_REPO}/src/mvdm-host/xms.486/xmsblock.c" "${xms_flags} /Gy")
set(xms_seams)
foreach(symbol IN ITEMS TerminateVDM mvdm_softpc_guest_memory_copy_from
    mvdm_softpc_guest_memory_range_valid c_sas_move_bytes_forward getAX getBX
    getDX setCX getBP setDX getSS Sim32pGetVDMPointer sas_overwrite_memory)
  string(APPEND xms_seams " /alternatename:_${symbol}=_fixture_unexpected")
endforeach()
foreach(kind IN ITEMS allocation move)
  if(kind STREQUAL "allocation")
    set(source xms_block_boundary_fixture.c)
  else()
    set(source xms_move_failure_fixture.c)
  endif()
  worker_unit("obj/tests/xms/${kind}.obj" "${NTVDM_REPO}/test/mvdm-host/xms/${source}" "${xms_flags} /Gy")
  worker_link("xms-${kind}-fixture.exe" "/nologo /OPT:REF /out:$out /map:$out.map ${xms_seams}"
    "kernel32.lib ntdll.lib legacy_stdio_definitions.lib"
    INPUTS "obj/tests/xms/${kind}.obj" "obj/tests/xms/provider.obj" "original-mvdm-host-suballoc.lib")
  worker_group(xms-${kind} "xms-${kind}-fixture.exe")
endforeach()
