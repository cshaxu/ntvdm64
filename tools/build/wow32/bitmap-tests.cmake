# Current product object closure; synthetic bitmap input is not guest media.
get_target_property(bitmap_flags obj_compat_wow_bitmap_bindings_obj COMPILE_FLAGS)
get_target_property(resource_flags obj_opennt-user_clres_obj COMPILE_FLAGS)
wow_unit(bitmap_resource_owner "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/client/clres.c"
  "${resource_flags} /DWOW_BITMAP_FIXTURE_BINDINGS /FI \"${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/bitmap-native-fixture.h\"")
set_property(TARGET bitmap_resource_owner PROPERTY EXCLUDE_FROM_ALL TRUE)
add_library(bitmap_resource OBJECT EXCLUDE_FROM_ALL
  "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_bitmap_binding_fixture.rc")
target_include_directories(bitmap_resource PRIVATE "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow")
set_property(SOURCE "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_bitmap_binding_fixture.rc"
  APPEND PROPERTY OBJECT_DEPENDS "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/fixture-bitmap.bmp")
set(bitmap_outputs)
foreach(kind IN ITEMS binding binding_failure)
  wow_unit(bitmap_${kind} "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_bitmap_${kind}_fixture.c" "${bitmap_flags}")
  set_property(TARGET bitmap_${kind} PROPERTY EXCLUDE_FROM_ALL TRUE)
  set(providers bitmap_${kind})
  if(kind STREQUAL "binding")
    list(APPEND providers bitmap_resource bitmap_resource_owner obj_compat_wow_bitmap_bindings_obj
      obj_opennt-user_cldib_obj obj_adapter-win32_chartran_obj obj_compat_ntuser_rtl_compat_obj)
  endif()
  set(objects)
  foreach(provider IN LISTS providers)
    list(APPEND objects "$<TARGET_OBJECTS:${provider}>")
  endforeach()
  set(output "${CMAKE_BINARY_DIR}/bitmap-${kind}.exe")
  add_custom_command(OUTPUT "${output}"
    COMMAND "${CMAKE_LINKER}" /nologo /machine:x86 /subsystem:console /opt:ref
      "/out:${output}" "/map:${output}.map" ${objects}
      kernel32.lib user32.lib gdi32.lib ntdll.lib legacy_stdio_definitions.lib
    DEPENDS ${providers} ${objects}
    COMMAND_EXPAND_LISTS VERBATIM)
  list(APPEND bitmap_outputs "${output}")
endforeach()
add_custom_target(wow-bitmap DEPENDS ${bitmap_outputs})
