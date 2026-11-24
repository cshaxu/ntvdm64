if(NTVDM_ARCH STREQUAL "x86")
  set(source "${NTVDM_REPO}/src/mvdm-host/wow32/waccel.c")
  ntvdm_source_projection(accel-create32.inc "${source}"
    "HACCEL CreateAccel32(VPVOID vpAccel16, DWORD cbAccel16)"
    "//*****************************************************************************\n// CreateAccel16 -" "" TRUE)
  ntvdm_source_projection(accel-alias.inc "${source}"
    "LPACCELALIAS SetupAccelAlias("
    "//*****************************************************************************\n// GetAccelHandle32 -" "" TRUE)
  foreach(kind IN ITEMS call alias)
    fixture(wow-accelerator-${kind} LINK
      test/adapter-mvdm-host-out/wow/wow_accelerator_${kind}_fixture.c)
    set_property(TARGET wow-accelerator-${kind}_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX)
    target_include_directories(wow-accelerator-${kind}_objects PRIVATE "${CMAKE_BINARY_DIR}")
  endforeach()
  set_property(TARGET wow-accelerator-call_objects PROPERTY FIXTURE_LIBRARIES user32.lib)
  add_custom_target(wow-accelerator DEPENDS wow-accelerator-call wow-accelerator-alias)
endif()
