if(NTVDM_ARCH STREQUAL "x86")
  ntvdm_source_projection(common_dialog_exact_body.h
    "${NTVDM_REPO}/src/compat/wow_common_dialog_calls.c"
    "#define COMMON_DIALOG_CALL(" "" "" TRUE)
  fixture(wow-common-dialog-boundary CL test/observation/wow_common_dialog_calls_fixture.c)
  fixture(wow-common-dialog-reference CL test/observation/wow_native_common_dialog_reference.c)
  foreach(kind IN ITEMS boundary reference)
    set_property(TARGET wow-common-dialog-${kind}_objects PROPERTY COMPILE_OPTIONS /nologo /TC /O2 /W4 /WX)
    set_property(TARGET wow-common-dialog-${kind}_objects PROPERTY FIXTURE_LIBRARIES user32.lib)
  endforeach()
  target_include_directories(wow-common-dialog-boundary_objects PRIVATE "${CMAKE_BINARY_DIR}")
  set_property(TARGET wow-common-dialog-reference_objects APPEND PROPERTY FIXTURE_LIBRARIES comdlg32.lib)
  add_custom_target(wow-common-dialog DEPENDS wow-common-dialog-boundary wow-common-dialog-reference)
endif()
