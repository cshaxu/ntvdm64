if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
foreach(pair IN ITEMS "SHLPROBE|shell" "CBTPROB|hook" "WORDPROB|window_word"
    "CARETPRB|caret" "KEYPROBE|keyboard" "MEDPROBE|media")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 stem)
  guest_probe(shell "${name}.EXE" "win16_${stem}_probe.asm")
endforeach()
guest_probe(shell SHLCHILD.EXE win16_shell_probe.asm -DCHILD)
guest_probe(shell CBTPROB-stay.EXE win16_hook_probe.asm -DHOOK_LEAVE_INSTALLED)
guest_probe(shell S44BOP.COM s44_vdd_bop.asm -DS41_ISOLATED)
foreach(format 15 3 131 49153 49154)
  guest_probe(shell "CLIPREJ-${format}.EXE" win16_clipreject_probe.asm "-DFORMAT=${format}")
endforeach()
foreach(format 49153 49154)
  guest_probe(shell "CLIPREJ-${format}-ole.EXE" win16_clipreject_probe.asm
    "-DFORMAT=${format}" -DOLE_ROUNDTRIP)
endforeach()
foreach(drive RANGE 0 25)
  guest_probe(shell "CDPROBE-${drive}.COM" win16_optical_probe.asm "-DOPTICAL_DRIVE=${drive}")
endforeach()
get_property(outputs GLOBAL PROPERTY guest_shell)
set(root "${CMAKE_BINARY_DIR}/guest-probes/shell")
set(source "${NTVDM_REPO}/test/observation/wow_caret_native_observer.c")
add_custom_command(OUTPUT "${root}/CARHOST.DLL" "${root}/caret.lib"
  COMMAND "${CMAKE_C_COMPILER}" /nologo /LD /O2 "${source}" "/Fo${root}/caret.obj"
    /link user32.lib "/OUT:${root}/CARHOST.DLL" "/IMPLIB:${root}/caret.lib"
    /EXPORT:CaretObserve=_CaretObserve@12
  DEPENDS "${source}" "${CMAKE_C_COMPILER}"
  BYPRODUCTS "${root}/caret.obj" "${root}/caret.exp"
  WORKING_DIRECTORY "${root}" VERBATIM)
add_custom_target(win16-shell-probes DEPENDS ${outputs} "${root}/CARHOST.DLL" "${root}/caret.lib")

# The runtime runner verifies that this map's image matches its package input.
set(NTVDM_OLE_PROVIDER "" CACHE STRING "Exact WOW32 build directory")
if(NOT NTVDM_OLE_PROVIDER STREQUAL "")
  set(map "${NTVDM_OLE_PROVIDER}/wow32.dll.map")
  file(READ "${map}" text)
  string(REGEX MATCH "_wow_ole_convert_descriptor@8[ \t]+([0-9A-Fa-f]+)[ \t]+f" symbol "${text}")
  if(NOT symbol)
    message(FATAL_ERROR "Missing original OLE provider symbol")
  endif()
  set(address "${CMAKE_MATCH_1}")
  string(REGEX MATCH "Preferred load address is ([0-9A-Fa-f]+)" base "${text}")
  if(NOT base)
    message(FATAL_ERROR "Missing provider load address")
  endif()
  math(EXPR rva "0x${address} - 0x${CMAKE_MATCH_1}")
  if(rva LESS 0 OR rva GREATER 4294967295)
    message(FATAL_ERROR "Invalid provider RVA")
  endif()
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${map}")
  set(source "${NTVDM_REPO}/test/observation/wow_ole_descriptor_roundtrip.c")
  add_custom_command(OUTPUT "${root}/OLEHOST.DLL" "${root}/ole.lib"
    COMMAND "${CMAKE_C_COMPILER}" /nologo /LD /MT /O2 "/DOLE_PROVIDER_RVA=${rva}"
      "${source}" "/Fo${root}/ole.obj" /link "/OUT:${root}/OLEHOST.DLL"
      "/IMPLIB:${root}/ole.lib" /EXPORT:OleRoundtrip=_OleRoundtrip@12
    DEPENDS "${source}" "${map}" "${NTVDM_OLE_PROVIDER}/wow32.dll" "${CMAKE_C_COMPILER}"
    BYPRODUCTS "${root}/ole.obj" "${root}/ole.exp"
    WORKING_DIRECTORY "${root}" VERBATIM)
  add_custom_target(win16-ole-peer DEPENDS "${root}/OLEHOST.DLL" "${root}/ole.lib")
endif()
