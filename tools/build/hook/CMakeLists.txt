cmake_minimum_required(VERSION 3.25)
set(NTVDM_BINARY_DIR "${CMAKE_BINARY_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/../BuildTree.cmake")
project(ntvdm_hook64 LANGUAGES C)
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "Hook64 requires the MSVC AMD64 environment")
endif()
set(CMAKE_C_FLAGS "/nologo /W4 /Gy" CACHE STRING "Hook compile profile" FORCE)
set(CMAKE_C_FLAGS_RELEASE "/O2" CACHE STRING "Hook release profile" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "Libraries are explicit per target" FORCE)
set(CMAKE_C_COMPILER_LAUNCHER "${NTVDM_COMPILER_LAUNCHER}")
# Preserve direct MSVC link semantics; CMake owns target dependency metadata.
set(CMAKE_C_CREATE_SHARED_LIBRARY "<CMAKE_LINKER> /nologo /dll /opt:ref <LINK_FLAGS> /out:<TARGET> /implib:<TARGET_IMPLIB> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> /nologo /entry:wmainCRTStartup /opt:ref <LINK_FLAGS> /out:<TARGET> <OBJECTS> <LINK_LIBRARIES>")
find_program(NTVDM_MIDL NAMES midl.exe REQUIRED)
set(rpc "${CMAKE_BINARY_DIR}/obj/basesrv")
file(MAKE_DIRECTORY "${rpc}")
add_custom_command(OUTPUT "${rpc}/service.h" "${rpc}/service_c.c" "${rpc}/service_s.c"
  COMMAND "${NTVDM_MIDL}" /nologo /env x64 /target NT100 /prefix client Client_
    /prefix server Server_ /acf "${NTVDM_REPO}/src/common/protocol/service.acf"
    /out "${rpc}" /h service.h /cstub service_c.c /sstub service_s.c
    "${NTVDM_REPO}/src/common/protocol/service.idl"
  DEPENDS "${NTVDM_REPO}/src/common/protocol/service.idl"
    "${NTVDM_REPO}/src/common/protocol/service.acf" VERBATIM)
add_custom_target(hook-rpc DEPENDS "${rpc}/service.h")
set(sources
  src/nthook-dll/entry.c src/common/rpc/memory_heap.c
  src/common/native-hook/context.c src/common/native-hook/packet.c
  src/common/native-hook/installer.c src/nthook-dll/create_process.c
  src/lib-win32/process-hook/staging.c src/lib-win32/process-hook/helper.c
  src/lib-win32/process-hook/payload.c src/lib-win32/process-hook/startup_hooks.c
  src/lib-win32/process-hook/minhook/src/buffer.c
  src/lib-win32/process-hook/minhook/src/trampoline.c
  src/lib-win32/process-hook/minhook/src/hde/hde64.c
  src/common/native_image.c src/lib-win32/process-image/image.c
  src/lib-win32/handle/duplicate.c src/lib-win32/file/identity.c
  src/nthook-dll/hook_classification.c src/non-mvdm/base/win32/client/vdm.c
  src/compat/base_classifier_path.c src/compat/opennt_support_rtl.c
  src/non-mvdm/base/ntos/rtl/error.c src/lib-win32/rpc/local_binding.c
  src/common/rpc/management.c src/common/base_srv/scope.c)
set(outer_includes "${rpc}" "${NTVDM_REPO}/src"
  "${NTVDM_REPO}/src/compat/include" "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc")
set(objects)
set(manifest "{\"architecture\":\"AMD64\",\"scope\":\"Hook DLL only; no worker\",\"sources\":[")
set(separator "")
set(index 0)
foreach(source IN LISTS sources)
  # Each existing unit keeps its source order and its own include boundary.
  set(unit "unit${index}")
  add_library(${unit} OBJECT "${NTVDM_REPO}/${source}")
  target_compile_definitions(${unit} PRIVATE _WIN32_WINNT=0x0A00)
  if(source MATCHES "^src/lib-win32/")
    target_include_directories(${unit} PRIVATE "${NTVDM_REPO}/src")
    if(source MATCHES "^src/lib-win32/process-hook/[^/]+[.]c$")
      target_compile_options(${unit} PRIVATE /std:c11)
    endif()
  elseif(NOT source MATCHES "^src/compat/")
    target_include_directories(${unit} PRIVATE ${outer_includes})
    add_dependencies(${unit} hook-rpc)
  endif()
  if(source STREQUAL "src/non-mvdm/base/win32/client/vdm.c")
    target_compile_definitions(${unit} PRIVATE OPENNT_BASE_CLIENT_CLASSIFIER)
  elseif(source STREQUAL "src/non-mvdm/base/ntos/rtl/error.c")
    target_compile_definitions(${unit} PRIVATE NTOS_KERNEL_RUNTIME)
    target_compile_options(${unit} PRIVATE "/FI${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h")
  endif()
  list(APPEND objects "$<TARGET_OBJECTS:${unit}>")
  if(source STREQUAL "src/lib-win32/process-image/image.c")
    set(image_object "$<TARGET_OBJECTS:${unit}>")
  endif()
  file(SHA256 "${NTVDM_REPO}/${source}" hash)
  string(APPEND manifest "${separator}{\"path\":\"${source}\",\"sha256\":\"${hash}\",\"target\":\"${unit}\"}")
  set(separator ",")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${NTVDM_REPO}/${source}")
  math(EXPR index "${index}+1")
endforeach()
string(APPEND manifest "]}\n")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/source-manifest.json" CONTENT "${manifest}" @ONLY)
add_library(observation-stub OBJECT "${rpc}/service_c.c")
target_compile_definitions(observation-stub PRIVATE _WIN32_WINNT=0x0A00)
target_include_directories(observation-stub PRIVATE ${outer_includes})
list(APPEND objects "$<TARGET_OBJECTS:observation-stub>")
add_library(nthook64 SHARED ${objects})
set_target_properties(nthook64 PROPERTIES LINKER_LANGUAGE C PREFIX ""
  ARCHIVE_OUTPUT_NAME nthook64-import)
target_link_options(nthook64 PRIVATE "/map:${CMAKE_BINARY_DIR}/nthook64.dll.map"
  "/def:${NTVDM_REPO}/src/nthook-dll/nthook64.def")
set_property(TARGET nthook64 APPEND PROPERTY LINK_DEPENDS "${NTVDM_REPO}/src/nthook-dll/nthook64.def")
target_link_libraries(nthook64 PRIVATE kernel32 user32 uxtheme ntdll advapi32 rpcrt4 legacy_stdio_definitions)

add_executable(process-image-machine-test EXCLUDE_FROM_ALL
  "${NTVDM_REPO}/test/lib-win32/process-image/image_machine_test.c"
  ${image_object})
target_include_directories(process-image-machine-test PRIVATE ${outer_includes})
target_compile_definitions(process-image-machine-test PRIVATE _WIN32_WINNT=0x0A00)
target_link_options(process-image-machine-test PRIVATE /subsystem:console)
target_link_libraries(process-image-machine-test PRIVATE kernel32 ntdll advapi32 user32 rpcrt4 legacy_stdio_definitions)
foreach(kind IN ITEMS install gui)
  add_executable(nthook-${kind}-test EXCLUDE_FROM_ALL
    "${NTVDM_REPO}/test/integration/nthook_install_test.c" ${objects})
  target_include_directories(nthook-${kind}-test PRIVATE ${outer_includes})
  target_compile_definitions(nthook-${kind}-test PRIVATE _WIN32_WINNT=0x0A00)
  target_compile_options(nthook-${kind}-test PRIVATE /std:c11)
  add_dependencies(nthook-${kind}-test hook-rpc)
  if(kind STREQUAL "gui")
    target_link_options(nthook-${kind}-test PRIVATE /subsystem:windows)
  else()
    target_link_options(nthook-${kind}-test PRIVATE /subsystem:console)
  endif()
  target_link_libraries(nthook-${kind}-test PRIVATE kernel32 ntdll advapi32 user32 rpcrt4 legacy_stdio_definitions)
endforeach()
if(NTVDM_LAUNCHER_FILE)
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/run16.exe"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${NTVDM_LAUNCHER_FILE}" "${CMAKE_BINARY_DIR}/run16.exe"
    DEPENDS "${NTVDM_LAUNCHER_FILE}" VERBATIM)
  add_custom_target(hook-test-launcher DEPENDS "${CMAKE_BINARY_DIR}/run16.exe")
  add_dependencies(nthook-install-test nthook64 nthook-gui-test hook-test-launcher)
endif()
if(NOT NTVDM_VSDEVCMD)
  set(NTVDM_VSDEVCMD "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat")
endif()
configure_file("${CMAKE_CURRENT_LIST_DIR}/run-ninja.cmd.in" "${CMAKE_BINARY_DIR}/run-ninja.cmd" @ONLY)
