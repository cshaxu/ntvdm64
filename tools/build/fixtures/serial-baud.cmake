# Current-source projections, with test-only device/memory providers.
if(NTVDM_ARCH STREQUAL "x86")
  ntvdm_source_projection(serial_baud_arm.inc
    "${NTVDM_REPO}/src/mvdm-host/softpc.new/host/src/nt_com.c"
    "case HOST_COM_BAUD:" "/*:::::::::::::::::::::::::::::::::::::::: Process DTR line requests */" "" TRUE)
  fixture(serial-baud CL test/mvdm-host/serial_baud_test.c)
  set_property(TARGET serial-baud_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX)
  target_compile_definitions(serial-baud_objects PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_include_directories(serial-baud_objects PRIVATE "${CMAKE_BINARY_DIR}")
endif()
