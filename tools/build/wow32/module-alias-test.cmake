# Reuse the actual product alias owner; the fixture only supplies allocation.
get_target_property(module_alias_flags original_wole2 COMPILE_FLAGS)
wow_unit(module_alias_fixture
  "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_module_alias_fixture.c"
  "${module_alias_flags}")
set_property(TARGET module_alias_fixture PROPERTY EXCLUDE_FROM_ALL TRUE)
set(output "${CMAKE_BINARY_DIR}/module-alias.exe")
add_custom_command(OUTPUT "${output}"
  COMMAND "${CMAKE_LINKER}" /nologo /machine:x86 /subsystem:console
    "/out:${output}" "/map:${output}.map"
    $<TARGET_OBJECTS:module_alias_fixture> $<TARGET_OBJECTS:original_wole2>
    kernel32.lib ntdll.lib oldnames.lib
  DEPENDS module_alias_fixture original_wole2
    $<TARGET_OBJECTS:module_alias_fixture> $<TARGET_OBJECTS:original_wole2>
  COMMAND_EXPAND_LISTS VERBATIM)
add_custom_target(wow-module-alias DEPENDS "${output}")
