# Independently authored Win16 witnesses; never rebuild original guest media.
if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
guest_probe(win16 DDINIT.EXE win16_ddeml_init_probe.asm)
guest_probe(win16 DDREG.EXE win16_ddeml_register_probe.asm)
guest_probe(win16 DDREG-self.EXE win16_ddeml_register_probe.asm -DSELF_CALLBACK_CONTROL)
guest_probe(win16 DDWORD.EXE win16_ddeml_window_word_probe.asm)
guest_probe(win16 DDWORD-self.EXE win16_ddeml_window_word_probe.asm -DSELF_CALLBACK_CONTROL)
guest_probe(win16 MMTPROBE.DLL win16_multimedia_timer_library.asm)
guest_probe(win16 MMTPROB.EXE win16_multimedia_timer_probe.asm)
guest_probe(win16 MMTPROB-load.EXE win16_multimedia_timer_probe.asm -DLOAD_ONLY)
guest_probe(win16 MMTPROB-no-timer.EXE win16_multimedia_timer_probe.asm -DNO_TIMER)
get_property(outputs GLOBAL PROPERTY guest_win16)
foreach(pair IN ITEMS "PDCANCEL|win16_printdlg_cancel_probe" "PDGPROBE|win16_printdlg_default_probe"
    "RAWPROBE|win16_raw_spool_probe" "THPROBE|win16_toolhelp_probe"
    "DISCOVER|win16_discovery_probe" "DCPROBE|win16_dc_probe"
    "DLGPROBE|win16_dialog_probe" "CDLPROBE|win16_common_dialog_probe")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 stem)
  set(source "${NTVDM_REPO}/test/observation/${stem}.asm")
  set(output "${CMAKE_BINARY_DIR}/guest-probes/win16/${name}")
  set(flags)
  if(name MATCHES "^(DLGPROBE|CDLPROBE)$")
    list(APPEND flags -d COMMON_DIALOG_ROUNDS=2)
  endif()
  add_custom_command(OUTPUT "${output}.EXE" "${output}.lst"
    COMMAND "${NTVDM_NASM}" -f bin -o "${output}.EXE" -l "${output}.lst" ${flags} "${source}"
    DEPENDS "${source}" "${NTVDM_NASM}" VERBATIM)
  list(APPEND outputs "${output}.EXE" "${output}.lst")
endforeach()
add_custom_target(win16-probes DEPENDS ${outputs})
fixture(wow-discovery-peer CL test/observation/wow_discovery_peer.c)
set_property(TARGET wow-discovery-peer_objects PROPERTY COMPILE_OPTIONS /nologo /O2 /W4 /WX)
target_compile_definitions(wow-discovery-peer_objects PRIVATE _CRT_SECURE_NO_WARNINGS)
set_property(TARGET wow-discovery-peer_objects PROPERTY FIXTURE_LIBRARIES user32.lib)
add_custom_target(win16-discovery DEPENDS win16-probes wow-discovery-peer)
fixture(wow-callback-cost-unit CL test/observation/wow_callback_cost_fixture.c)
set_property(TARGET wow-callback-cost-unit_objects PROPERTY COMPILE_OPTIONS /nologo /O2 /W4)
target_compile_definitions(wow-callback-cost-unit_objects PRIVATE _CRT_SECURE_NO_WARNINGS)
set_property(TARGET wow-callback-cost-unit_objects PROPERTY FIXTURE_LIBRARIES kernel32.lib)

# The caller's fresh evidence directory is embedded by NASM, not patched later.
set(NTVDM_PRINT_OUTPUT_DIR "" CACHE STRING "Print witness output directory")
if(NOT NTVDM_PRINT_OUTPUT_DIR STREQUAL "")
  set(output "${CMAKE_BINARY_DIR}/guest-probes/win16/PRNPROBE")
  set(source "${NTVDM_REPO}/test/observation/win16_print_probe.asm")
  add_custom_command(OUTPUT "${output}.EXE" "${output}.lst"
    COMMAND "${NTVDM_NASM}" "-DOUTPUT_DIR=\"${NTVDM_PRINT_OUTPUT_DIR}\""
      -f bin -o "${output}.EXE" -l "${output}.lst" "${source}"
    DEPENDS "${source}" "${NTVDM_NASM}" VERBATIM)
  add_custom_target(win16-print DEPENDS "${output}.EXE" "${output}.lst")
endif()
