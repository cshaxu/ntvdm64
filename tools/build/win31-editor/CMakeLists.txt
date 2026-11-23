cmake_minimum_required(VERSION 3.24)
project(win31_editor NONE)
get_filename_component(repo "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
find_program(NASM NAMES nasm.exe REQUIRED)
foreach(flag IN ITEMS MOUSE_MENU DOSONLY SAVE_WITNESS FIRST_DOS FIRST_DOS_BEFORE_SECOND)
  if(NOT "${${flag}}" MATCHES "^[01]$")
    message(FATAL_ERROR "Expected boolean ${flag}")
  endif()
endforeach()
if(DOSONLY AND (SAVE_WITNESS OR FIRST_DOS_BEFORE_SECOND))
  message(FATAL_ERROR "DOSONLY does not support save or first-editor sequence")
endif()
if(NOT IS_ABSOLUTE "${EDITOR_DEST}" OR EDITOR_DEST MATCHES "['\"\r\n]")
  message(FATAL_ERROR "Expected an absolute editor installation path")
endif()
set(editor "${repo}/test/integration/ntvdm64-386")
configure_file(config.inc.in config.inc @ONLY NEWLINE_STYLE LF)
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/controller.exe"
  COMMAND "${NASM}" -f bin -p "${CMAKE_BINARY_DIR}/config.inc"
    -o "${CMAKE_BINARY_DIR}/controller.exe" "${repo}/test/observation/win31_startup_contract.asm"
  DEPENDS "${CMAKE_BINARY_DIR}/config.inc" "${repo}/test/observation/win31_startup_contract.asm"
    "${editor}/editor-actions.inc" "${editor}/editor-routines.inc"
    "${editor}/editor-events.inc" "${editor}/editor-data.inc" "${NASM}" VERBATIM)
add_custom_target(controller ALL DEPENDS "${CMAKE_BINARY_DIR}/controller.exe")
