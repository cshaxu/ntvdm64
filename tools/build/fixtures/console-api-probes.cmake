# Private NTCON API observers. Never included in the product or publication.
if(NOT NTVDM_ARCH STREQUAL "x64")
  return()
endif()
add_library(console-api-input OBJECT EXCLUDE_FROM_ALL
  "${NTVDM_REPO}/test/observation/private-mouse-input.c")
target_compile_options(console-api-input PRIVATE /nologo /std:c11)
target_compile_definitions(console-api-input PRIVATE WIN31_CONSOLE_API_TRACE)
target_include_directories(console-api-input PRIVATE "${NTVDM_REPO}/src"
  "${NTVDM_REPO}/src/lib-nxvm" "${NTVDM_REPO}/src/ntcon-exe")
set(outputs)
foreach(kind IN ITEMS normal fault)
  set(target console-api-${kind})
  add_library(${target} OBJECT EXCLUDE_FROM_ALL
    "${NTVDM_REPO}/test/observation/win31_console_api_trace.c"
    "${NTVDM_REPO}/src/lib-win32/process-hook/late_guard.c"
    "${NTVDM_REPO}/src/lib-win32/process-hook/late_patch.c"
    "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/buffer.c"
    "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/trampoline.c"
    "${NTVDM_REPO}/src/lib-win32/process-hook/minhook/src/hde/hde64.c")
  target_compile_options(${target} PRIVATE /nologo /std:c11)
  target_include_directories(${target} PRIVATE "${NTVDM_REPO}/src")
  if(kind STREQUAL "fault")
    target_compile_definitions(${target} PRIVATE PROCESS_HOOK_LATE_GUARD_TEST)
  endif()
  set(directory "console-api/${kind}")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${directory}")
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${directory}/mouse-hook.dll"
    COMMAND "${CMAKE_LINKER}" /nologo /DLL /EXPORT:MouseInputHook
      "/OUT:${directory}/mouse-hook.dll" "/IMPLIB:${directory}/hook.lib"
      "/MAP:${directory}/mouse-hook.map"
      $<TARGET_OBJECTS:${target}> $<TARGET_OBJECTS:console-api-input> user32.lib kernel32.lib
    DEPENDS ${target} console-api-input $<TARGET_OBJECTS:${target}> $<TARGET_OBJECTS:console-api-input>
    BYPRODUCTS "${CMAKE_BINARY_DIR}/${directory}/hook.lib" "${CMAKE_BINARY_DIR}/${directory}/hook.exp"
    COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${directory}/probe.exe"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different private-mouse-probe.exe "${directory}/probe.exe"
    DEPENDS "${CMAKE_BINARY_DIR}/private-mouse-probe.exe" VERBATIM)
  list(APPEND outputs "${CMAKE_BINARY_DIR}/${directory}/mouse-hook.dll"
    "${CMAKE_BINARY_DIR}/${directory}/probe.exe")
endforeach()
add_custom_target(console-api-probes DEPENDS ${outputs})
