# Independently authored Win95 diagnostics; never modify original guest images.
if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
find_program(NTVDM_EDITBIN editbin REQUIRED)
set(win95_images)
foreach(kind IN ITEMS loader menu dos_restart)
  if(kind STREQUAL "dos_restart")
    set(image DOSRET)
  else()
    set(image MENU95)
  endif()
  set(output "${CMAKE_BINARY_DIR}/win95-${kind}")
  file(MAKE_DIRECTORY "${output}")
  add_library(win95_${kind}_object OBJECT EXCLUDE_FROM_ALL
    "${NTVDM_REPO}/test/observation/win95_${kind}_probe.c")
  target_compile_options(win95_${kind}_object PRIVATE
    /nologo /TC /O1 /Oi /GS- /Zl /arch:IA32)
  target_compile_definitions(win95_${kind}_object PRIVATE
    _WIN32_WINNT=0x0400 WINVER=0x0400)
  add_custom_command(OUTPUT "${output}/${image}.EXE"
    COMMAND "${CMAKE_LINKER}" /nologo /nodefaultlib /entry:entry
      /subsystem:windows /osversion:4.0 /machine:x86 /dynamicbase:no
      /nxcompat:no /safeseh:no /stack:0x100000,0x1000
      "/out:${output}/${image}.EXE" "/map:${output}/${image}.MAP"
      $<TARGET_OBJECTS:win95_${kind}_object>
      kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib uuid.lib
    COMMAND "${NTVDM_EDITBIN}" /nologo /subsystem:windows,4.0 "${output}/${image}.EXE"
    DEPENDS win95_${kind}_object $<TARGET_OBJECTS:win95_${kind}_object>
      "${NTVDM_EDITBIN}"
    BYPRODUCTS "${output}/${image}.MAP" COMMAND_EXPAND_LISTS VERBATIM)
  list(APPEND win95_images "${output}/${image}.EXE")
endforeach()
add_custom_target(win95-probes DEPENDS ${win95_images})
