# Exact current-source test projections from the former generator. No copied
# product algorithm is maintained here; source edits trigger reconfiguration.
function(control_slice out source start end reverse_end)
  file(READ "${source}" text)
  string(REPLACE "\r\n" "\n" text "${text}")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
  string(FIND "${text}" "${start}" begin)
  string(FIND "${text}" "${start}" last REVERSE)
  if(begin LESS 0 OR NOT begin EQUAL last)
    message(FATAL_ERROR "Ambiguous control projection start: ${source}: ${start}")
  endif()
  string(SUBSTRING "${text}" ${begin} -1 tail)
  if("${end}" STREQUAL "")
    string(LENGTH "${tail}" length)
  elseif(reverse_end)
    string(FIND "${tail}" "${end}" length REVERSE)
  else()
    string(FIND "${tail}" "${end}" length)
  endif()
  if(length LESS 1)
    message(FATAL_ERROR "Missing control projection end: ${source}: ${end}")
  endif()
  string(SUBSTRING "${tail}" 0 ${length} body)
  string(SUBSTRING "${text}" 0 ${begin} prefix)
  string(REGEX MATCHALL "\n" newlines "${prefix}")
  list(LENGTH newlines line)
  math(EXPR line "${line}+1")
  set(${out} "${body}" PARENT_SCOPE)
  set(${out}_line ${line} PARENT_SCOPE)
endfunction()
set(button "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/btnctl.c")
control_slice(globals "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clglobal.c"
  "CRITICAL_SECTION gcsLookaside;" "#ifdef _JANUS_" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/control-globals.c" CONTENT "#include <windows.h>\n#line ${globals_line} \"${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clglobal.c\"\n${globals}" @ONLY)
control_slice(provider "${button}" "/* Image-attach composition only:" "#line 15" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/button-resource-provider.c"
  CONTENT "#include \"${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_button_resource_provider.h\"\n#line ${provider_line} \"${button}\"\n${provider}" @ONLY)
set(dialog "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/dlgmgr.c")
set(keyboard "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/dlgmgr2.c")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/keyboard-message-table.c"
  CONTENT "#include \"control-client/default-precomp.h\"\n#define PROTOS_ONLY\n#include \"${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/inc/messages.h\"\n" @ONLY)
control_slice(keyboard_helpers "${keyboard}" "void xxxRemoveDefaultButton(" "FUNCLOG2(LOG_GENERAL, BOOL, DUMMYCALLINGTYPE, IsDialogMessageA" FALSE)
control_slice(keyboard_lookup "${keyboard}" "PWND _FindDlgItem(" "" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/dialog-keyboard-helpers.c"
  CONTENT "#include \"control-client/precomp.h\"\n#line ${keyboard_helpers_line} \"${keyboard}\"\n${keyboard_helpers}\n#line ${keyboard_lookup_line} \"${keyboard}\"\n${keyboard_lookup}" @ONLY)
set(layout_marker "/* Original-layout operations; caller retains its task and window borrow. */")
control_slice(layout "${dialog}" "${layout_marker}" "" TRUE)
string(REGEX MATCHALL "BOOL WINAPI wow_original_dialog_" operations "${layout}")
list(LENGTH operations count)
string(STRIP "${layout}" stripped)
if(NOT count EQUAL 2 OR NOT stripped MATCHES "}$")
  message(FATAL_ERROR "Dialog layout projection changed")
endif()
control_slice(memory "${dialog}" "/* XP SP1 client/client.c registered dialog notification; original callback owner. */"
  "/* Copyright (c) 1985-95, Microsoft Corporation." FALSE)
control_slice(pointer "${dialog}" "VOID SetDialogPointer(" "/* Copyright (c) 1985-92" FALSE)
control_slice(lookup "${dialog}" "PWND _GetDlgItem(" "BOOL WINAPI wow_original_dialog_set_pointer(" FALSE)
control_slice(setter "${dialog}" "BOOL WINAPI wow_original_dialog_set_pointer(" "${layout_marker}" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/dialog-memory-original.c"
  CONTENT "#include \"wow_edit_memory_bindings.h\"\n${memory}" @ONLY)
control_slice(modal "${dialog}" "INT_PTR DialogBox2(" "void\nRepositionRect(" FALSE)
control_slice(modal_foreground "${dialog}" "#define IsInForegroundQueue(hwnd)" "/***************************************************************************\\" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/dialog-modal-original.c"
  CONTENT "#include \"${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/dialog-lifetime/precomp.h\"\n#line ${modal_foreground_line} \"${dialog}\"\n${modal_foreground}\n#line ${modal_line} \"${dialog}\"\n${modal}" @ONLY)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/dialog-pointer-original.c" CONTENT "${pointer}" @ONLY)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/dialog-lookup-original.c"
  CONTENT "#include \"control-client/precomp.h\"\n${lookup}" @ONLY)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/dialog-finalization-layout.c"
  CONTENT "#include \"wow_user_client_view_layout.h\"\n#define WOW_ORIGINAL_DIALOG_LAYOUT\n#include \"compat/include/opennt_user_multipass.h\"\n#undef WOW_ORIGINAL_DIALOG_LAYOUT\n${pointer}${setter}${layout}" @ONLY)

# XP moved PSMTextOut and its default implementation to the client owner.
control_slice(text_lpk_defaults "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clglobal.c"
  "FPLPKTABBEDTEXTOUT fpLpkTabbedTextOut" "#line 112" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/text-lpk-defaults.c"
  CONTENT "#include \"control-drawing/precomp.h\"\n#line ${text_lpk_defaults_line} \"${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clglobal.c\"\n${text_lpk_defaults}" @ONLY)
control_slice(mnemonic_client "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/drawtext.c"
  "void PSMTextOut(" "" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/mnemonic-client.c"
  CONTENT "#include \"${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/mnemonic-drawing/precomp.h\"\n#line ${mnemonic_client_line} \"${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/drawtext.c\"\n${mnemonic_client}" @ONLY)

control_slice(prefix_body "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/rtl/drawtext.c"
  "LONG GetPrefixCount(" "\n/***************************************************************************" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/text-prefix.c"
  CONTENT "#include \"control-drawing/precomp.h\"\n#line ${prefix_body_line} \"${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/rtl/drawtext.c\"\n${prefix_body}" @ONLY)
control_slice(mnemonic_default "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clglobal.c"
  "FPLPKPSMTEXTOUT fpLpkPSMTextOut" "FPLPKDRAWTEXTEX" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/mnemonic-default.c"
  CONTENT "#include \"control-drawing/precomp.h\"\n#line ${mnemonic_default_line} \"${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clglobal.c\"\n${mnemonic_default}" @ONLY)

set(class_binding "${NTVDM_REPO}/src/compat/wow_standard_class_bindings.c")
control_slice(class_atoms "${class_binding}" "static const struct { BYTE cleanup, client; } class_indices[]" "static BOOL process_control(" FALSE)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/class-atoms.c"
  CONTENT "#include <windows.h>\n#include \"wow_standard_class_bindings.h\"\n#line ${class_atoms_line} \"${class_binding}\"\n${class_atoms}" @ONLY)
