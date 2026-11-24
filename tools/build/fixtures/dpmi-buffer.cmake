if(NTVDM_ARCH STREQUAL "x86")
  set(dpmi "${NTVDM_REPO}/src/mvdm-host/dpmi32")
  ntvdm_source_projection(dpmi-buffer-before.inc "${dpmi}/buffer.c"
    "PUCHAR\nDpmiMapAndCopyBuffer(" "USHORT\nDpmiSegmentToSelector(" "" TRUE)
  ntvdm_source_projection(dpmi-buffer-after.inc "${dpmi}/buffer.c"
    "PUCHAR\nDpmiAllocateBuffer(" "" "" TRUE)
  ntvdm_source_projection(dpmi-state.inc "${dpmi}/savestat.c"
    "typedef struct _SavedState {" "" "" TRUE)
  fixture(dpmi-buffer-state CL test/mvdm-host/dpmi_buffer_state_fixture.c)
  set_property(TARGET dpmi-buffer-state_objects PROPERTY COMPILE_OPTIONS /nologo /W3 /TC /Od)
  target_include_directories(dpmi-buffer-state_objects PRIVATE "${CMAKE_BINARY_DIR}")
endif()
