cmake_minimum_required(VERSION 3.25)
set(NTVDM_BINARY_DIR "${CMAKE_BINARY_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/../BuildTree.cmake")
project(ntvdm_worker LANGUAGES C ASM_MASM RC)
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "The CCPU worker requires the MSVC x86 environment")
endif()
foreach(language IN ITEMS C ASM_MASM RC)
  set(CMAKE_${language}_FLAGS "" CACHE STRING "Explicit source profiles" FORCE)
  set(CMAKE_${language}_FLAGS_RELEASE "" CACHE STRING "Explicit source profiles" FORCE)
endforeach()
# Runtime selection is explicit per unit, including original VCDEX's default.
set(CMAKE_MSVC_RUNTIME_LIBRARY "")
set(CMAKE_C_COMPILER_LAUNCHER "${NTVDM_COMPILER_LAUNCHER}")
get_filename_component(msvc_bin "${CMAKE_C_COMPILER}" DIRECTORY)
find_program(NTVDM_LIB NAMES lib.exe PATHS "${msvc_bin}" NO_DEFAULT_PATH REQUIRED)
find_program(NTVDM_MIDL NAMES midl.exe REQUIRED)
include(targets.cmake)
include(original-sources.cmake)
include(generated.cmake)
set(rpc "${CMAKE_BINARY_DIR}/obj/basesrv")
file(MAKE_DIRECTORY "${rpc}")
add_custom_command(OUTPUT "${rpc}/service_s.c" "${rpc}/service_c.c" "${rpc}/service.h"
  COMMAND "${NTVDM_MIDL}" /nologo /env win32 /target NT100 /prefix client Client_
    /prefix server Server_ /acf "${NTVDM_REPO}/src/common/protocol/service.acf"
    /out "${rpc}" /h service.h /cstub service_c.c /sstub service_s.c
    "${NTVDM_REPO}/src/common/protocol/service.idl"
  DEPENDS "${NTVDM_REPO}/src/common/protocol/service.idl"
    "${NTVDM_REPO}/src/common/protocol/service.acf" VERBATIM)
add_custom_command(OUTPUT "${rpc}/rpc_management_c.c" "${rpc}/rpc_management.h"
  COMMAND "${NTVDM_MIDL}" /nologo /env win32 /target NT100
    /acf "${NTVDM_REPO}/src/common/protocol/rpc_management.acf"
    /client stub /server none /out "${rpc}" /h rpc_management.h /cstub rpc_management_c.c
    "${NTVDM_REPO}/src/common/protocol/rpc_management.idl"
  DEPENDS "${NTVDM_REPO}/src/common/protocol/rpc_management.idl"
    "${NTVDM_REPO}/src/common/protocol/rpc_management.acf" VERBATIM)
include(native-inputs.cmake)
include(profiles.cmake)
include(nxvm.cmake)
include(source-groups.cmake)
include(units.cmake)
include(observation-tests.cmake)
include(oem-tests.cmake)
include(diagnostic-tests.cmake)
include(page-cost.cmake)
include(cpu-tests.cmake)
include(dpmi-tests.cmake)
include(keyboard-tests.cmake)
get_property(manifest GLOBAL PROPERTY worker_manifest)
list(JOIN manifest ",\n" manifest)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/source-manifest.json"
  CONTENT "{\"architecture\":\"x86\",\"sources\":[\n${manifest}\n]}\n" @ONLY)
if(NOT NTVDM_VSDEVCMD)
  set(NTVDM_VSDEVCMD "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat")
endif()
configure_file(run-ninja-parallel.cmd.in "${CMAKE_BINARY_DIR}/run-ninja-parallel.cmd" @ONLY)
configure_file("../wow32/msvc-x86.cmd.in" "${CMAKE_BINARY_DIR}/msvc-x86.cmd" @ONLY)
add_custom_target(product ALL DEPENDS "${CMAKE_BINARY_DIR}/ntvdm.exe"
  "${CMAKE_BINARY_DIR}/VCDEX.DLL" "${CMAKE_BINARY_DIR}/VDMREDIR.dll"
  "${CMAKE_BINARY_DIR}/nthook32.dll")
