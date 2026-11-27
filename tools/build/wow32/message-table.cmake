# XP message metadata: original list preprocessor and listmung template path.
add_executable(listmung "${NTVDM_REPO}/src/non-mvdm/sdktools/listmung/listmung.c")
set_target_properties(listmung PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreaded)
target_compile_definitions(listmung PRIVATE _X86_)
set(message_source "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/inc")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/msgdef.h"
  COMMAND cmd /d /c "${CMAKE_CURRENT_LIST_DIR}/generate-message-table.cmd"
    "${CMAKE_C_COMPILER}" "$<TARGET_FILE:listmung>" "${message_source}" "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS listmung "${message_source}/msgdef.lst" "${message_source}/msgdef.tpl"
    "${CMAKE_CURRENT_LIST_DIR}/generate-message-table.cmd"
  VERBATIM)
add_custom_target(message-table-generated DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/msgdef.h")
