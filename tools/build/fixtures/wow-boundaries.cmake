# Independent current boundary contracts; no private historical build graph.
fixture(wow-gdi-alias CL test/compat/wow_gdi_alias_fixture.c src/compat/wow_gdi_alias.c)
set_property(TARGET wow-gdi-alias_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX)
set_property(TARGET wow-gdi-alias_objects PROPERTY FIXTURE_LIBRARIES gdi32.lib)
if(NTVDM_ARCH STREQUAL "x86")
  fixture(wow-hard-error-decoder CL src/ntvdm-exe/win32/wow_hard_error_decoder.c
    test/adapter-mvdm-host-out/wow/wow_hard_error_decoder_fixture.c)
  target_include_directories(wow-hard-error-decoder_objects PRIVATE "${NTVDM_REPO}/src/ntvdm-exe/win32")
  set_property(TARGET wow-hard-error-decoder_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX /DWIN32)
  fixture(wow-hook-native-binding CL test/compat/wow_hook_native_binding_fixture.c)
  target_include_directories(wow-hook-native-binding_objects PRIVATE "${NTVDM_REPO}/src/compat/include")
  set_property(TARGET wow-hook-native-binding_objects PROPERTY COMPILE_OPTIONS /nologo /TC /O2 /W4)
endif()
