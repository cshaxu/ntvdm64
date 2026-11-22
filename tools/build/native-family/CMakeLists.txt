cmake_minimum_required(VERSION 3.25)
set(NTVDM_BINARY_DIR "${CMAKE_BINARY_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/../BuildTree.cmake")
project(ntvdm_native_family LANGUAGES C)
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "Native components require the MSVC AMD64 environment")
endif()
if(NOT NTVDM_COMPONENT MATCHES "^(worker|frontend|monitor|launcher|service)$")
  message(FATAL_ERROR "Select one native product component")
endif()
set(CMAKE_C_FLAGS "" CACHE STRING "Explicit per-unit profiles" FORCE)
set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "Explicit per-unit profiles" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
set(CMAKE_C_COMPILER_LAUNCHER "${NTVDM_COMPILER_LAUNCHER}")
get_filename_component(msvc_bin "${CMAKE_C_COMPILER}" DIRECTORY)
find_program(NTVDM_LIB NAMES lib.exe PATHS "${msvc_bin}" NO_DEFAULT_PATH REQUIRED)
find_program(NTVDM_MIDL NAMES midl.exe REQUIRED)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/obj/basesrv")
set(rpc "${CMAKE_BINARY_DIR}/obj/basesrv")
add_custom_command(OUTPUT "${rpc}/service_c.c" "${rpc}/service_s.c" "${rpc}/service.h"
  COMMAND "${NTVDM_MIDL}" /nologo /env x64 /target NT100 /prefix client Client_
    /prefix server Server_ /acf "${NTVDM_REPO}/src/common/protocol/service.acf"
    /out "${rpc}" /h service.h /cstub service_c.c /sstub service_s.c
    "${NTVDM_REPO}/src/common/protocol/service.idl"
  DEPENDS "${NTVDM_REPO}/src/common/protocol/service.idl"
    "${NTVDM_REPO}/src/common/protocol/service.acf" VERBATIM)
add_custom_command(OUTPUT "${rpc}/rpc_management_c.c" "${rpc}/rpc_management.h"
  COMMAND "${NTVDM_MIDL}" /nologo /env x64 /target NT100
    /acf "${NTVDM_REPO}/src/common/protocol/rpc_management.acf"
    /client stub /server none /out "${rpc}" /h rpc_management.h /cstub rpc_management_c.c
    "${NTVDM_REPO}/src/common/protocol/rpc_management.idl"
  DEPENDS "${NTVDM_REPO}/src/common/protocol/rpc_management.idl"
    "${NTVDM_REPO}/src/common/protocol/rpc_management.acf" VERBATIM)
if(NTVDM_COMPONENT MATCHES "^(worker|frontend)$")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/frontend-font")
  set(font "${CMAKE_BINARY_DIR}/frontend-font/native_pc_font.h")
  add_custom_command(OUTPUT "${font}"
    COMMAND "${NTVDM_REPO}/build/.ninja/build-tools/ntvdm-font.exe"
      "${NTVDM_REPO}/src/mvdm-guest/softpc.new/roms/v7vga.rom" "${font}"
    DEPENDS "${NTVDM_REPO}/build/.ninja/build-tools/ntvdm-font.exe"
      "${NTVDM_REPO}/src/mvdm-guest/softpc.new/roms/v7vga.rom" VERBATIM)
endif()

# The original configuration body remains live source, not a reference-tree
# artifact. Keep the same two bounded, column-zero function selections.
if(NOT NTVDM_COMPONENT STREQUAL "monitor")
  include("${CMAKE_CURRENT_LIST_DIR}/../OriginalVdmConfig.cmake")
endif()

function(family_unit output source flags)
  string(MAKE_C_IDENTIFIER "unit_${output}" unit)
  if(NOT IS_ABSOLUTE "${source}")
    set(source "${CMAKE_BINARY_DIR}/${source}")
  endif()
  set(original_allowed "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/error.c")
  if(NTVDM_COMPONENT STREQUAL "launcher")
    list(APPEND original_allowed
      "${NTVDM_REPO}/src/non-mvdm/base/ntos/rtl/environ.c"
      "${NTVDM_REPO}/src/non-mvdm/base/win32/client/vdm.c"
      "${NTVDM_REPO}/src/non-mvdm/base/ntdll/csrutil.c")
  elseif(NTVDM_COMPONENT STREQUAL "service")
    list(APPEND original_allowed
      "${NTVDM_REPO}/src/non-mvdm/base/win32/server/srvvdm.c"
      "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/server/exports.c")
  endif()
  if(source MATCHES "/src/mvdm-host/" OR
      (source MATCHES "/src/non-mvdm/" AND NOT source IN_LIST original_allowed))
    message(FATAL_ERROR "Unadmitted original source: ${source}")
  endif()
  add_library(${unit} OBJECT "${source}")
  set_target_properties(${unit} PROPERTIES EXCLUDE_FROM_ALL TRUE COMPILE_FLAGS "${flags}")
  set(dependencies)
  foreach(dependency IN LISTS ARGN)
    if(NOT IS_ABSOLUTE "${dependency}")
      set(dependency "${CMAKE_BINARY_DIR}/${dependency}")
    endif()
    list(APPEND dependencies "${dependency}")
  endforeach()
  if(flags MATCHES "frontend-font")
    list(APPEND dependencies "${font}")
  endif()
  if(dependencies)
    set_property(SOURCE "${source}" APPEND PROPERTY OBJECT_DEPENDS "${dependencies}")
  endif()
  get_filename_component(directory "${CMAKE_BINARY_DIR}/${output}" DIRECTORY)
  file(MAKE_DIRECTORY "${directory}")
  # Existing fixture and archive interfaces consume these object names.
  # CMake owns compilation/dependencies; this is only a stable output alias.
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${output}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_OBJECTS:${unit}>" "${CMAKE_BINARY_DIR}/${output}"
    DEPENDS ${unit} "$<TARGET_OBJECTS:${unit}>" VERBATIM)
  set_property(GLOBAL APPEND PROPERTY family_outputs "${output}")
  if(EXISTS "${source}")
    file(SHA256 "${source}" hash)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
    set_property(GLOBAL APPEND PROPERTY family_manifest
      "{\"path\":\"${source}\",\"sha256\":\"${hash}\",\"object\":\"${output}\"}")
  endif()
endfunction()

function(family_archive output)
  set(inputs)
  foreach(input IN LISTS ARGN)
    list(APPEND inputs "${CMAKE_BINARY_DIR}/${input}")
  endforeach()
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${output}"
    COMMAND "${NTVDM_LIB}" /nologo "/out:${output}" ${inputs}
    DEPENDS ${inputs} VERBATIM)
  set_property(GLOBAL APPEND PROPERTY family_outputs "${output}")
endfunction()

function(family_image output flags)
  get_property(outputs GLOBAL PROPERTY family_outputs)
  set(inputs)
  set(dependencies)
  foreach(input IN LISTS ARGN)
    if(input IN_LIST outputs)
      set(input "${CMAKE_BINARY_DIR}/${input}")
      list(APPEND dependencies "${input}")
    endif()
    list(APPEND inputs "${input}")
  endforeach()
  separate_arguments(options NATIVE_COMMAND "${flags}")
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${output}"
    COMMAND "${CMAKE_LINKER}" ${options} "/out:${output}" "/map:${output}.map" ${inputs}
    DEPENDS ${dependencies} VERBATIM)
  string(MAKE_C_IDENTIFIER "image_${output}" target)
  add_custom_target(${target} DEPENDS "${CMAKE_BINARY_DIR}/${output}")
endfunction()

set(family_flags "/nologo /c /MT /W4 /we4013 /we4311 /we4302 /Gy /showIncludes /DWINNT /D_NO_CRT_STDIO_INLINE /D_WIN32_WINNT=0x0A00")
include("${NTVDM_COMPONENT}.cmake")
get_property(manifest GLOBAL PROPERTY family_manifest)
list(JOIN manifest ",\n" manifest)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/source-manifest.json"
  CONTENT "{\"architecture\":\"AMD64\",\"scope\":\"${NTVDM_COMPONENT}\",\"sources\":[\n${manifest}\n]}\n" @ONLY)
if(NOT NTVDM_VSDEVCMD)
  set(NTVDM_VSDEVCMD "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat")
endif()
configure_file("../hook/run-ninja.cmd.in" "${CMAKE_BINARY_DIR}/run-ninja.cmd" @ONLY)
