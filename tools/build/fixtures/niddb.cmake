if(NTVDM_ARCH STREQUAL "x86")
  set(niddb_original "${NTVDM_REPO}/build/.ninja/msvc-x86-release/worker/obj/bios/virtual.obj")
  set(niddb_intercept "${CMAKE_BINARY_DIR}/niddb-intercept.obj")
  add_custom_command(OUTPUT "${niddb_intercept}"
    COMMAND objcopy --redefine-sym _malloc=_niddb_test_malloc
      --redefine-sym _calloc=_niddb_test_calloc --redefine-sym _free=_niddb_test_free
      "${niddb_original}" "${niddb_intercept}"
    DEPENDS "${niddb_original}" VERBATIM)
  fixture(niddb-no-retry CL test/mvdm-host/niddb_no_retry_fixture.c)
  set_property(TARGET niddb-no-retry_objects PROPERTY COMPILE_OPTIONS /nologo /TC /Od)
  set_property(TARGET niddb-no-retry_objects PROPERTY FIXTURE_LIBRARIES "${niddb_intercept};kernel32.lib")
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/niddb-no-retry.exe" APPEND DEPENDS "${niddb_intercept}")
endif()
