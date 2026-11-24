# Current-source projections, with test-only device/memory providers.
if(NTVDM_ARCH STREQUAL "x86")
  ntvdm_source_projection(serial_baud_arm.inc
    "${NTVDM_REPO}/src/mvdm-host/softpc.new/host/src/nt_com.c"
    "case HOST_COM_BAUD:" "/*:::::::::::::::::::::::::::::::::::::::: Process DTR line requests */" "" TRUE)
  fixture(serial-baud CL test/mvdm-host/serial_baud_test.c)
  set_property(TARGET serial-baud_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX)
  target_compile_definitions(serial-baud_objects PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_include_directories(serial-baud_objects PRIVATE "${CMAKE_BINARY_DIR}")
  fixture(serial-medium CL test/mvdm-host/nt_com_mock_fixture.c)
  target_compile_definitions(serial-medium_objects PRIVATE
    WIN32 WINNT NTVDM CPU_40_STYLE NEW_CPU CCPU C_VID SPC386 SIM32 V7VGA ANSI PROD
    terminate=mvdm_softpc_terminate)
  target_compile_options(serial-medium_objects PRIVATE
    "/FI${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h")
  target_include_directories(serial-medium_objects PRIVATE
    "${NTVDM_REPO}/src/compat/include"
    "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/inc"
    "${NTVDM_REPO}/src/mvdm-host/softpc.new/host/inc"
    "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc"
    "${NTVDM_REPO}/src/non-mvdm/public/internal/base/inc"
    "${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc"
    "${NTVDM_REPO}/src/mvdm-host/inc")
  set_property(TARGET serial-medium_objects PROPERTY FIXTURE_LIBRARIES
    "kernel32.lib;user32.lib;libcmt.lib;libvcruntime.lib;libucrt.lib")
endif()
