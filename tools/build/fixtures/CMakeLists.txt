cmake_minimum_required(VERSION 3.25)
set(NTVDM_BINARY_DIR "${CMAKE_BINARY_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/../BuildTree.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../SourceProjection.cmake")
set(CMAKE_TRY_COMPILE_CONFIGURATION Release)
project(ntvdm_fixtures LANGUAGES C)
if(NOT MSVC OR NOT NTVDM_ARCH MATCHES "^(x86|x64)$")
  message(FATAL_ERROR "Select the MSVC x86 or x64 fixture toolchain")
endif()
if((NTVDM_ARCH STREQUAL "x86" AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4) OR
   (NTVDM_ARCH STREQUAL "x64" AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8))
  message(FATAL_ERROR "Fixture architecture/toolchain mismatch")
endif()
set(CMAKE_C_FLAGS "" CACHE STRING "Explicit fixture profile" FORCE)
set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "Explicit fixture profile" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
set(CMAKE_C_COMPILER_LAUNCHER "${NTVDM_COMPILER_LAUNCHER}")
include("${CMAKE_CURRENT_LIST_DIR}/guest-probes.cmake")

ntvdm_guest_memory_projection(guest-memory-coordination.c "#line 1960")
set_source_files_properties("${CMAKE_BINARY_DIR}/guest-memory-coordination.c"
  PROPERTIES COMPILE_OPTIONS /Gy)

function(fixture name link_mode)
  set(sources)
  foreach(source IN LISTS ARGN)
    if(NOT IS_ABSOLUTE "${source}")
      set(source "${NTVDM_REPO}/${source}")
    endif()
    list(APPEND sources "${source}")
  endforeach()
  add_library(${name}_objects OBJECT ${sources})
  set_target_properties(${name}_objects PROPERTIES EXCLUDE_FROM_ALL TRUE)
  target_compile_options(${name}_objects PRIVATE /nologo /TC /W4 /Gy)
  target_include_directories(${name}_objects PRIVATE
    "${NTVDM_REPO}/src" "${NTVDM_REPO}/src/ntvdm-exe/softpc/include")
  # Preserve each runner's original CL-driven or explicit LINK library policy.
  if(link_mode STREQUAL "CL")
    set(link "${CMAKE_C_COMPILER}" /nologo /MT "/Fe${name}.exe")
    set(tail /link /OPT:REF "/MAP:${name}.exe.map"
      "$<TARGET_PROPERTY:${name}_objects,FIXTURE_LIBRARIES>")
  else()
    set(link "${CMAKE_LINKER}" /nologo /OPT:REF "/OUT:${name}.exe" "/MAP:${name}.exe.map")
    set(tail kernel32.lib)
    if(link_mode STREQUAL "LINKDEFAULT")
      set(tail)
    endif()
  endif()
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${name}.exe"
    COMMAND ${link} $<TARGET_OBJECTS:${name}_objects> ${tail}
    DEPENDS ${name}_objects $<TARGET_OBJECTS:${name}_objects>
    BYPRODUCTS "${CMAKE_BINARY_DIR}/${name}.exe.map"
    COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(${name} DEPENDS "${CMAKE_BINARY_DIR}/${name}.exe")
endfunction()
if(NTVDM_ARCH STREQUAL "x86")
  foreach(pair IN ITEMS "S34N|command_native_child_fixture.c"
      "S34E|command_stream_echo_fixture.c"
      "command-stream-observer|command_stream_observer.c"
      "keymouse-capability-observer|keymouse_capability_observer.c")
    string(REPLACE "|" ";" parts "${pair}")
    list(GET parts 0 name)
    list(GET parts 1 source)
    fixture(${name} CL "test/observation/${source}")
    set_property(TARGET ${name}_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4)
    set_property(TARGET ${name}_objects PROPERTY FIXTURE_LIBRARIES kernel32.lib)
    if(NOT name STREQUAL "keymouse-capability-observer")
      target_compile_options(${name}_objects PRIVATE /WX /O2)
    endif()
    if(name STREQUAL "S34E" OR name STREQUAL "keymouse-capability-observer")
      set_property(TARGET ${name}_objects APPEND PROPERTY FIXTURE_LIBRARIES user32.lib)
    endif()
  endforeach()
  add_custom_target(command-tools DEPENDS S34N S34E command-stream-observer)
  add_dependencies(guest-keymouse keymouse-capability-observer)
endif()
set(memory_sources src/compat/guest_memory_lease.c
  src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c src/ntvdm-exe/session/session.c
  src/compat/host_context.c "${CMAKE_BINARY_DIR}/guest-memory-coordination.c"
  src/compat/guest_location.c)
fixture(guest_memory_lease_boundary_test CL test/compat/guest_memory_lease_boundary_test.c
  src/compat/guest_memory_lease.c)
fixture(guest_memory_lease_test CL test/session/guest_memory_lease_test.c ${memory_sources})
fixture(guest_memory_coordination_test CL test/mvdm-host/guest_memory_coordination_test.c ${memory_sources})
add_custom_target(guest-memory DEPENDS guest_memory_lease_boundary_test
  guest_memory_lease_test guest_memory_coordination_test)
fixture(redirector_async_publication_test LINK
  test/mvdm-host/vdmredir/redirector_async_publication_test.c ${memory_sources}
  src/compat/mvdm_redirector_async.c src/compat/thread_start.c src/lib-win32/thread/thread.c)
target_compile_definitions(redirector_async_publication_test_objects PRIVATE
  WIN_32 VDMREDIR_DLL NTVDM CPU_40_STYLE CCPU)
add_custom_target(redirector-async DEPENDS redirector_async_publication_test)

# The former T280 and T282 generators compiled the exact same source fixture.
fixture(guest_location_test LINKDEFAULT
  test/adapter-mvdm-host-out/softpc/t280_s20_mvdm_guest_location_fixture.c ${memory_sources})
target_compile_options(guest_location_test_objects PRIVATE /std:c11)
add_custom_target(guest-location DEPENDS guest_location_test)
fixture(wow_task_frame_test LINKDEFAULT
  test/adapter-mvdm-host-out/monitor/t291_s3_wow_task_frame_fixture.c ${memory_sources}
  src/ntvdm-exe/monitor/mvdm_wow_task_frame.c)
target_compile_options(wow_task_frame_test_objects PRIVATE /std:c11 /WX)
target_include_directories(wow_task_frame_test_objects PRIVATE "${NTVDM_REPO}/src/ntvdm-exe/monitor/include")
add_custom_target(wow-task-frame DEPENDS wow_task_frame_test)
fixture(wow_callback_frame_test LINKDEFAULT
  test/adapter-mvdm-host-out/wow/t335_s3_wow_callback_frame_lease_fixture.c ${memory_sources}
  src/compat/wow_callback_frame_lease.c src/compat/opennt_support_rtl.c)
target_compile_options(wow_callback_frame_test_objects PRIVATE /std:c11 /WX)
target_include_directories(wow_callback_frame_test_objects PRIVATE
  "${NTVDM_REPO}/src/wow32-dll/include" "${NTVDM_REPO}/src/compat/include"
  "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc")
add_custom_target(wow-callback-frame DEPENDS wow_callback_frame_test)

fixture(redirector_async_contract_test LINKDEFAULT
  test/mvdm-host/vdmredir/redirector_async_contract_fixture.c ${memory_sources}
  src/compat/mvdm_redirector_async.c)
target_compile_options(redirector_async_contract_test_objects PRIVATE /std:c11
  "/FI${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h"
  "/FI${NTVDM_REPO}/src/compat/include/thread_start_compat.h")
target_compile_definitions(redirector_async_contract_test_objects PRIVATE
  WIN32 WINNT NTVDM VDMREDIR_DLL CPU_40_STYLE CCPU)
foreach(path IN ITEMS src/mvdm-host/inc src/mvdm-host/vdmredir src/mvdm-host/dos/command
    src/non-mvdm/ds/netapi/netlib src/mvdm-host/softpc.new/base/inc src/mvdm-host/softpc.new/host/inc
    src/compat/include src/non-mvdm/public/internal/base/inc src/non-mvdm/public/internal/ds/inc
    src/non-mvdm/public/sdk/inc src/non-mvdm/public/ddk/inc src/non-mvdm/private/inc)
  target_include_directories(redirector_async_contract_test_objects PRIVATE "${NTVDM_REPO}/${path}")
endforeach()
add_custom_target(redirector-contract DEPENDS redirector_async_contract_test)
if(NTVDM_ARCH STREQUAL "x86")
  add_library(redirector_original_objects OBJECT
    "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrnmpipe.c"
    "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrputil.c"
    "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrdisp.c"
    "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrmisc.c"
    "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrmslot.c"
    "${NTVDM_REPO}/src/mvdm-host/dos/command/cmdredir.c"
    "${NTVDM_REPO}/src/non-mvdm/ds/netapi/netlib/ntstatus.c")
  set_target_properties(redirector_original_objects PROPERTIES EXCLUDE_FROM_ALL TRUE)
  foreach(property IN ITEMS COMPILE_OPTIONS COMPILE_DEFINITIONS INCLUDE_DIRECTORIES)
    get_target_property(profile redirector_async_contract_test_objects ${property})
    set_property(TARGET redirector_original_objects PROPERTY ${property} "${profile}")
  endforeach()
  add_dependencies(redirector-contract redirector_original_objects)
  fixture(xms_mapped_memory_test LINKDEFAULT
    test/adapter-mvdm-host-out/softpc/xms_mapped_memory_fixture.c ${memory_sources}
    src/mvdm-host/suballoc/suballoc.c src/mvdm-host/xms.486/xmsmemr.c)
  target_compile_options(xms_mapped_memory_test_objects PRIVATE /std:c11
    "/FI${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h")
  target_compile_definitions(xms_mapped_memory_test_objects PRIVATE WIN_32 CPU_40_STYLE)
  foreach(path IN ITEMS src/compat/include src/non-mvdm/public/sdk/inc src/mvdm-host/xms.486
      src/mvdm-host/inc src/non-mvdm/public/internal/base/inc src/non-mvdm/public/ddk/inc
      src/mvdm-host/softpc.new/host/inc src/mvdm-host/softpc.new/base/inc)
    target_include_directories(xms_mapped_memory_test_objects PRIVATE "${NTVDM_REPO}/${path}")
  endforeach()
  add_custom_target(xms-mapped DEPENDS xms_mapped_memory_test)
endif()

fixture(dem_file_lifecycle_test LINK
  test/adapter-mvdm-host-out/softpc/t365_s2_dem_file_lifecycle_lease_fixture.c ${memory_sources})
target_compile_options(dem_file_lifecycle_test_objects PRIVATE /std:c11)
add_custom_target(dem-file-lifecycle DEPENDS dem_file_lifecycle_test)
if(NTVDM_ARCH STREQUAL "x86")
  fixture(xms_startup_test LINKDEFAULT test/mvdm-host/xms/xms_init_fixture.c
    ${memory_sources} src/mvdm-host/xms.486/xms.c
    src/mvdm-host/xms.486/xmsmemr.c src/mvdm-host/suballoc/suballoc.c)
  target_compile_options(xms_startup_test_objects PRIVATE /std:c11
    "/FI${NTVDM_REPO}/src/non-mvdm/public/sdk/inc/nt.h")
  target_compile_definitions(xms_startup_test_objects PRIVATE WIN32 WINNT WIN_32 CPU_40_STYLE DEVL)
  foreach(path IN ITEMS src/compat/include src/non-mvdm/public/sdk/inc src/mvdm-host/inc
      src/mvdm-host/xms.486 src/mvdm-host/softpc.new/base/inc
      src/non-mvdm/public/internal/base/inc src/non-mvdm/public/ddk/inc)
    target_include_directories(xms_startup_test_objects PRIVATE "${NTVDM_REPO}/${path}")
  endforeach()
  add_custom_target(xms-startup DEPENDS xms_startup_test)
endif()
ntvdm_guest_memory_projection(guest-memory-core.c "#define MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK")
fixture(redirector_guest_copy_test LINKDEFAULT
  test/mvdm-host/vdmredir/redirector_guest_copy_fixture.c
  src/compat/mvdm_redirector_guest_copy.c src/ntvdm-exe/session/session.c
  src/compat/host_context.c src/compat/guest_memory_lease.c
  src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c "${CMAKE_BINARY_DIR}/guest-memory-core.c")
target_compile_options(redirector_guest_copy_test_objects PRIVATE /std:c11)
target_compile_definitions(redirector_guest_copy_test_objects PRIVATE WIN_32 VDMREDIR_DLL CPU_40_STYLE)
foreach(path IN ITEMS src/compat/include src/mvdm-host/inc src/mvdm-host/vdmredir
    src/mvdm-host/dos/command src/mvdm-host/softpc.new/base/inc src/mvdm-host/softpc.new/host/inc
    src/non-mvdm/ds/netapi/netlib src/non-mvdm/public/internal/base/inc
    src/non-mvdm/public/internal/ds/inc src/non-mvdm/public/internal/net/inc
    src/non-mvdm/ds/netapi/rpcxlate src/non-mvdm/public/sdk/inc
    src/non-mvdm/public/ddk/inc src/non-mvdm/private/inc)
  target_include_directories(redirector_guest_copy_test_objects PRIVATE "${NTVDM_REPO}/${path}")
endforeach()
add_library(redirector_query_objects OBJECT
  "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrnetapi.c"
  "${NTVDM_REPO}/src/mvdm-host/vdmredir/vrremote.c")
set_target_properties(redirector_query_objects PROPERTIES EXCLUDE_FROM_ALL TRUE)
foreach(property IN ITEMS COMPILE_OPTIONS COMPILE_DEFINITIONS INCLUDE_DIRECTORIES)
  get_target_property(profile redirector_guest_copy_test_objects ${property})
  set_property(TARGET redirector_query_objects PROPERTY ${property} "${profile}")
endforeach()
add_custom_target(redirector-guest-copy DEPENDS redirector_guest_copy_test redirector_query_objects)

# These historical profiles enabled /Gy only for the coordination unit.
foreach(target IN ITEMS guest_location_test wow_task_frame_test xms_mapped_memory_test
    dem_file_lifecycle_test xms_startup_test)
  if(TARGET ${target}_objects)
    get_target_property(profile ${target}_objects COMPILE_OPTIONS)
    list(REMOVE_ITEM profile /Gy)
    set_property(TARGET ${target}_objects PROPERTY COMPILE_OPTIONS "${profile}")
  endif()
endforeach()
