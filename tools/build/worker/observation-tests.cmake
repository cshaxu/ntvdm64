# Reuse the configured CCPU fixture providers; tests select no historical graph.
worker_unit("obj/tests/dem_declarations.obj"
  "${NTVDM_REPO}/test/adapter-mvdm-host-out/win32/t280_s9_dem_declaration_fixture.c"
  "${profile_8} /std:c11")
add_custom_target(dem-declarations DEPENDS "${CMAKE_BINARY_DIR}/obj/tests/dem_declarations.obj")
worker_unit("obj/tests/command_redirection_fixture.obj"
  "${NTVDM_REPO}/test/mvdm-host/dos/command/t290_s3_cmdredir_fixture.c"
  "${profile_1} /I\"${NTVDM_REPO}/src/mvdm-host/dos/command\"")
worker_link("command-redirection-fixture.exe" "/nologo /out:$out /map:$out.map"
  "kernel32.lib" INPUTS "obj/tests/command_redirection_fixture.obj"
  "obj/command/cmdredir.obj" "obj/compat/guest_memory_lease.obj"
  "obj/ntvdm/session/session.obj" "obj/compat/host_context.obj")
add_custom_target(command-redirection DEPENDS "${CMAKE_BINARY_DIR}/command-redirection-fixture.exe")
worker_unit("obj/tests/command_initialization_fixture.obj"
  "${NTVDM_REPO}/test/mvdm-host/command_initialization_fixture.c"
  "${profile_1} /I\"${NTVDM_REPO}/src/mvdm-host/dos/command\"")
worker_link("command-initialization-fixture.exe" "/nologo /out:$out /map:$out.map"
  "kernel32.lib" INPUTS "obj/tests/command_initialization_fixture.obj"
  "obj/command/cmd.obj" "obj/command/cmddata.obj")
add_custom_target(command-initialization DEPENDS "${CMAKE_BINARY_DIR}/command-initialization-fixture.exe")
get_target_property(host_fixture image_ccpu_bios_access_test_exe WORKER_LINK_INPUTS)
list(REMOVE_ITEM host_fixture "obj/tests/ccpu_bios_access_test.obj")
worker_unit("obj/tests/dem_directory_reset_test.obj"
  "${NTVDM_REPO}/test/mvdm-host/dem_directory_reset_test.c" "${profile_8} /Gy")
worker_link("dem-directory-reset-test.exe"
  "/nologo /OPT:REF /force:multiple /alternatename:_call_ica_hw_interrupt@12=_mvdm_public_ica_interrupt@12 /out:$out /map:$out.map"
  "kernel32.lib ntdll.lib user32.lib gdi32.lib advapi32.lib rpcrt4.lib shell32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/dem_directory_reset_test.obj" ${host_fixture})

set(redirector_profile "/nologo /TC /c /MT /W4 /DNTVDM /DCPU_40_STYLE /DCCPU /DWIN_32 /DVDMREDIR_DLL /I\"${NTVDM_REPO}/src\" /I\"${NTVDM_REPO}/src/compat/include\" /I\"${NTVDM_REPO}/src/ntvdm-exe/softpc/include\" /FI\"${NTVDM_REPO}/test/mvdm-host/vdmredir/redirector_oem_fixture_binding.h\"")
set(redirector_fixture)
foreach(source IN ITEMS
    src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c
    src/compat/mvdm_redirector_guest_copy.c src/compat/guest_location.c
    src/ntvdm-exe/session/session.c src/compat/host_context.c
    src/compat/guest_memory_lease.c)
  get_filename_component(stem "${source}" NAME_WE)
  set(object "obj/tests/redirector/${stem}.obj")
  worker_unit("${object}" "${NTVDM_REPO}/${source}" "${redirector_profile}")
  list(APPEND redirector_fixture "${object}")
endforeach()
worker_unit("obj/tests/redirector/coordination.obj"
  "${CMAKE_BINARY_DIR}/guest-memory-coordination-core.c" "${redirector_profile}")
list(APPEND redirector_fixture "obj/tests/redirector/coordination.obj")
worker_unit("obj/tests/redirector/capacity.obj"
  "${NTVDM_REPO}/test/mvdm-host/vdmredir/redirector_oem_capacity_test.c" "${redirector_profile}")
set(redirector_system "kernel32.lib ntdll.lib netapi32.lib user32.lib gdi32.lib advapi32.lib rpcrt4.lib shell32.lib legacy_stdio_definitions.lib")
worker_link("redirector-oem-capacity-test.exe" "/nologo /OPT:REF /out:$out /map:$out.map"
  "${redirector_system}" INPUTS ${redirector_fixture} "obj/tests/redirector/capacity.obj")
worker_unit("obj/tests/redirector/providers.obj"
  "${NTVDM_REPO}/test/mvdm-host/vdmredir/redirector_netapi_provider_test.c" "${redirector_profile}")
worker_unit("obj/tests/redirector/async.obj"
  "${NTVDM_REPO}/src/compat/mvdm_redirector_async.c" "${redirector_profile}")
worker_unit("obj/tests/redirector/vrnetapi.obj"
  "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrnetapi.c"
  "${profile_10} /Gy /FI\"${NTVDM_REPO}/test/mvdm-host/vdmredir/redirector_netapi_fixture_binding.h\"")
worker_unit("obj/tests/redirector/vrnmpipe.obj"
  "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrnmpipe.c"
  "${profile_10} /Gy /DMVDM_REDIRECTOR_WORKER_EXPORTS")
worker_archive("original-redir-observation.lib" ${redir_objects})
worker_link("redirector-netapi-provider-test.exe"
  "/nologo /OPT:REF /force:multiple /alternatename:_call_ica_hw_interrupt@12=_mvdm_public_ica_interrupt@12 /out:$out /map:$out.map"
  "${redirector_system}" INPUTS ${redirector_fixture}
  "obj/tests/redirector/providers.obj" "obj/tests/redirector/async.obj"
  "obj/tests/redirector/vrnetapi.obj" "obj/tests/redirector/vrnmpipe.obj"
  "original-redir-observation.lib" ${host_fixture}
  "original-opennt-netlib.lib" "original-opennt-xactsrv.lib")
add_custom_target(redirector-providers DEPENDS "${CMAKE_BINARY_DIR}/redirector-netapi-provider-test.exe")
add_custom_target(redirector-capacity DEPENDS "${CMAKE_BINARY_DIR}/redirector-oem-capacity-test.exe")
set(console_fixture ${host_fixture})
list(REMOVE_ITEM console_fixture "obj/tests/ccpu_bounded_execution_fixture_seams.obj")
string(REPLACE " legacy_stdio_definitions.lib" "" console_system "${redirector_system}")
foreach(pair IN ITEMS "console-display-toggle|console_display_toggle_fixture"
    "console-input-contract|console_input_contract_fixture")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 source)
  worker_unit("obj/tests/${source}.obj"
    "${NTVDM_REPO}/test/adapter-mvdm-host-out/win32/${source}.c" "${profile_1}")
  worker_link("${name}.exe" "/nologo /OPT:REF /out:$out /map:$out.map"
    "${console_system}" INPUTS "obj/tests/${source}.obj" ${console_fixture})
endforeach()
add_custom_target(console-contracts DEPENDS "${CMAKE_BINARY_DIR}/console-display-toggle.exe"
  "${CMAKE_BINARY_DIR}/console-input-contract.exe")
foreach(pair IN ITEMS "softpc-delay-heartbeat|softpc_delay_heartbeat_fixture|nt_timer"
    "softpc-ica-lock|softpc_ica_lock_fixture|nt_eoi")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 source)
  list(GET parts 2 original)
  worker_unit("obj/tests/${source}.obj" "${NTVDM_REPO}/test/mvdm-host/${source}.c" "${profile_1}")
  worker_link("${name}.exe" "/nologo /OPT:REF /out:$out /map:$out.map"
    "kernel32.lib ntdll.lib" INPUTS "obj/tests/${source}.obj" "obj/host/${original}.obj")
endforeach()
add_custom_target(host-thread-boundaries DEPENDS "${CMAKE_BINARY_DIR}/softpc-delay-heartbeat.exe"
  "${CMAKE_BINARY_DIR}/softpc-ica-lock.exe")
set(callback_profile "/I\"${NTVDM_REPO}/src/mvdm-host/wow32\" ${profile_1} /I\"${NTVDM_REPO}/src/wow32-dll/include\" /DWOW_ORIGINAL_THUNK_BUILD /Gy")
set(callback_objects)
foreach(source IN ITEMS t335_s3_original_callback_ccpu_fixture t335_s3_wcall16_unreachable_seams t335_s3_callback_test_boundary)
  set(extra)
  if(source STREQUAL "t335_s3_wcall16_unreachable_seams")
    set(extra /Gz)
  endif()
  worker_unit("obj/tests/callback/${source}.obj" "${NTVDM_REPO}/test/mvdm-host/${source}.c" "${callback_profile} ${extra}")
  list(APPEND callback_objects "obj/tests/callback/${source}.obj")
endforeach()
worker_unit("obj/tests/callback/wcall16.obj" "${NTVDM_REPO}/src/mvdm-host/wow32/wcall16.c" "${callback_profile} /Gz")
list(APPEND callback_objects "obj/tests/callback/wcall16.obj")
worker_unit("obj/tests/callback/lease.obj" "${NTVDM_REPO}/src/compat/wow_callback_frame_lease.c" "${callback_profile}")
list(APPEND callback_objects "obj/tests/callback/lease.obj")
set(callback_providers ${host_fixture})
set(callback_aliases)
foreach(symbol IN ITEMS getCS getIP getSS getSP getAX getBX getBP getSI getDI setSP setIP setSS)
  string(APPEND callback_aliases " /alternatename:_${symbol}=_c_${symbol}")
endforeach()
# The shared CCPU fixture already selects bounded seams before host archives.
# Preserve that fixture link policy; it is not a production link profile.
worker_link("original-callback-ccpu.exe" "/nologo /OPT:REF /force:multiple /alternatename:_CallBack16@16=_wow_original_CallBack16@16 ${callback_aliases} /out:$out /map:$out.map"
  "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib shell32.lib"
  INPUTS ${callback_objects} ${callback_providers})
add_custom_target(ccpu-callback DEPENDS "${CMAKE_BINARY_DIR}/original-callback-ccpu.exe"
  "${CMAKE_BINARY_DIR}/ccpu-high-linear-page-test.exe")
add_custom_target(command-launch DEPENDS "${CMAKE_BINARY_DIR}/ntvdm-command-launch-test.exe")

worker_group(dem-directory-reset "dem-directory-reset-test.exe")
