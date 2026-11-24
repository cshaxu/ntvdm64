# Retained public-host and actual font-provider contracts, x86 only.
if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
fixture(wow-font-glyph CL test/adapter-mvdm-host-out/wow/wow_glyph_outline_rotation_fixture.c src/compat/wow_fonts.c)
foreach(kind IN ITEMS etm etm-native tracking tracking-search)
  if(kind MATCHES "^etm")
    set(source test/adapter-mvdm-host-out/wow/wow_gdi_etm_fixture.c)
  else()
    set(source test/adapter-mvdm-host-out/wow/wow_font_tracking_fixture.c)
  endif()
  fixture(wow-font-${kind} CL "${source}")
endforeach()
target_compile_definitions(wow-font-etm-native_objects PRIVATE WOW_ETM_NATIVE)
target_compile_definitions(wow-font-tracking-search_objects PRIVATE WOW_FONT_NATIVE_SEARCH)
foreach(kind IN ITEMS glyph etm etm-native tracking tracking-search)
  set_property(TARGET wow-font-${kind}_objects PROPERTY COMPILE_OPTIONS
    /nologo /TC /W4 /Gz /external:anglebrackets /external:W0)
  set_property(TARGET wow-font-${kind}_objects PROPERTY FIXTURE_LIBRARIES gdi32.lib user32.lib kernel32.lib)
endforeach()
target_compile_options(wow-font-glyph_objects PRIVATE /WX)
add_custom_target(wow-font-contracts DEPENDS wow-font-glyph wow-font-etm wow-font-etm-native wow-font-tracking wow-font-tracking-search)
fixture(wow-metafile-host CL test/adapter-mvdm-host-out/wow/wow_metafile_host_fixture.c)
set_property(TARGET wow-metafile-host_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX)
set_property(TARGET wow-metafile-host_objects PROPERTY FIXTURE_LIBRARIES gdi32.lib user32.lib kernel32.lib)

# Session rejection/rollback and independently linkable inner runtime.
fixture(wow-session-binding CL
  test/adapter-mvdm-host-out/wow/wow_user_session_binding_fixture.c
  src/compat/wow_user_runtime.c src/compat/wow_worker_state.c
  src/compat/wow_user_session_binding.c src/ntvdm-exe/session/session.c
  src/compat/host_context.c src/compat/guest_memory_lease.c)
fixture(wow-runtime-independent CL
  test/compat/wow_user_runtime_fixture.c src/compat/wow_user_runtime.c)
foreach(name IN ITEMS wow-session-binding wow-runtime-independent)
  set_property(TARGET ${name}_objects PROPERTY COMPILE_OPTIONS /nologo /W4)
  target_include_directories(${name}_objects PRIVATE "${NTVDM_REPO}/src/compat/include")
endforeach()
add_custom_target(wow-session-contracts DEPENDS wow-session-binding wow-runtime-independent)
