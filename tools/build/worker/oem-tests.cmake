# Opt-in OEM/COMMAND tests share the current worker's compiler and RTL owners.
set(oem_test_dir "${CMAKE_BINARY_DIR}/oem-tests")
file(MAKE_DIRECTORY "${oem_test_dir}")
function(oem_projection output source start end include_end)
  set(source "${NTVDM_REPO}/${source}")
  file(READ "${source}" text)
  string(REPLACE "\r\n" "\n" text "${text}")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
  string(FIND "${text}" "${start}" begin)
  if(begin LESS 0)
    message(FATAL_ERROR "Missing OEM projection start: ${source}")
  endif()
  string(SUBSTRING "${text}" ${begin} -1 body)
  string(FIND "${body}" "${end}" length)
  if(length LESS 1)
    message(FATAL_ERROR "Missing OEM projection end: ${source}")
  endif()
  if(include_end)
    string(LENGTH "${end}" extra)
    math(EXPR length "${length}+${extra}")
  endif()
  string(SUBSTRING "${body}" 0 ${length} body)
  file(CONFIGURE OUTPUT "${oem_test_dir}/${output}" CONTENT "${body}" @ONLY)
  file(SHA256 "${source}" hash)
  file(CONFIGURE OUTPUT "${oem_test_dir}/${output}.sha256" CONTENT "${hash}\n" @ONLY)
endfunction()
oem_projection(dem-create-body.inc src/mvdm-host/dos/dem/demfile.c
  "VOID demCreateCommon (flCreateType)" "BOOL IsCdRomFile (PSTR pszPath)" FALSE)
oem_projection(dem-media-body.inc src/mvdm-host/dos/dem/demgset.c
  "BOOL\nGetMediaId(" "/* demGetDPB" FALSE)
oem_projection(dem-media-type.inc src/mvdm-host/dos/dem/dosdef.h
  "#pragma pack(1)\n\ntypedef struct _VOLINFO" "typedef VOLINFO *PVOLINFO;" TRUE)
oem_projection(cmd-temp-body.inc src/mvdm-host/dos/command/cmdredir.c
  "BOOL cmdCreateTempFile (phTempFile,ppszTempFile)" "\n}" TRUE)
oem_projection(cmd-tail-body.inc src/mvdm-host/dos/command/cmdmisc.c
  "    pSrc = strrchr(VDMInfo.AppName" "    if (DosEnvCreated)" FALSE)

set(oem_test_profile "/nologo /TC /c /MT /W4 /DWIN32 /DWINNT /FI\"${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h\"")
foreach(path IN ITEMS src src/mvdm-host/inc src/mvdm-host/oemuni
    src/ntvdm-exe/softpc/include src/compat/include src/non-mvdm/public/sdk/inc
    src/non-mvdm/public/internal/base/inc)
  string(APPEND oem_test_profile " /I\"${NTVDM_REPO}/${path}\"")
endforeach()
string(APPEND oem_test_profile " /I\"${oem_test_dir}\"")
function(oem_test name source profile)
  set(object "obj/tests/oem/${name}.obj")
  worker_unit("${object}" "${NTVDM_REPO}/${source}" "${profile}")
  worker_link("${name}.exe" "/nologo /out:$out /map:$out.map"
    "kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib"
    INPUTS "${object}" ${ARGN} "obj/opennt-abi-host-compat/opennt_support_rtl.obj"
      "original-opennt-rtl-x86.lib")
  set_property(GLOBAL APPEND PROPERTY oem_tests "${CMAKE_BINARY_DIR}/${name}.exe")
endfunction()
foreach(kind IN ITEMS family failure)
  oem_test("oemuni-${kind}-fixture" "test/mvdm/oemuni/oemuni_${kind}_fixture.c"
    "${oem_test_profile}" "obj/oemuni/file.obj" "obj/oemuni/process.obj")
endforeach()
foreach(kind IN ITEMS expand_failure dbcs font font_retry media)
  string(REPLACE "_" "-" name "${kind}")
  oem_test("oemuni-${name}-fixture" "test/mvdm/oemuni/oemuni_${kind}_fixture.c" "${oem_test_profile}")
endforeach()
oem_test(oem-support-fixture test/mvdm-host/t282_s11_oem_support_fixture.c
  "${oem_test_profile}" "obj/oemuni/file.obj" "obj/oemuni/process.obj")
oem_test(cmd-temp-fixture test/mvdm-host/dos/command/temp_file_fixture.c "${oem_test_profile}")
oem_test(cmd-tail-fixture test/mvdm-host/dos/command/command_tail_fixture.c "${oem_test_profile}")
oem_test(cmd-svc-probe test/mvdm-host/dos/command/command_service_fixture.c
  "${oem_test_profile}" "obj/command/cmddisp.obj")
oem_test(oemuni-config-path-fixture test/mvdm/oemuni/oemuni_config_path_fixture.c
  "${oem_test_profile}" "obj/compat/pif_path.obj")
oem_test(event-startup-fixture test/observation/event_thread_startup_failure.c
  "${profile_1}" "obj/adapter-softpc/mvdm_softpc_event_thread.obj")
oem_test(oemuni-pif-fault-fixture test/mvdm/oemuni/oemuni_pif_fault_fixture.c
  "${profile_1}" "obj/oemuni/process.obj")
oem_test(oemuni-pif-root-fixture test/mvdm/oemuni/oemuni_pif_root_fixture.c
  "${profile_1}" "obj/ntvdm/session/session.obj" "obj/compat/guest_memory_lease.obj"
  "obj/adapter-softpc/mvdm_softpc_firmware.obj" "obj/common/system_root.obj"
  "obj/common/guest_environment.obj" "obj/host/nt_pif.obj" "obj/oemuni/process.obj"
  "obj/compat/host_context.obj" "obj/compat/pif_path.obj")
oem_test(oemuni-pif-parser-fixture test/mvdm/oemuni/oemuni_pif_parser_fixture.c
  "${profile_1}" "obj/host/nt_pif.obj" "obj/oemuni/process.obj")
oem_test(oemuni-pif-caller-fixture test/mvdm/oemuni/oemuni_pif_caller_fixture.c
  "${profile_1} /I\"${NTVDM_REPO}/src/mvdm-host/dos/command\""
  "obj/command/cmdpif.obj" "obj/oemuni/file.obj" "obj/oemuni/process.obj")
get_property(oem_tests GLOBAL PROPERTY oem_tests)
add_custom_target(oem-tests DEPENDS ${oem_tests})
