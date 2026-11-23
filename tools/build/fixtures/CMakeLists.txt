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

ntvdm_guest_memory_projection(guest-memory-coordination.c "#line 1960")

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
    set(tail /link /OPT:REF "/MAP:${name}.exe.map")
  else()
    set(link "${CMAKE_LINKER}" /nologo /OPT:REF "/OUT:${name}.exe" "/MAP:${name}.exe.map")
    set(tail kernel32.lib)
  endif()
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${name}.exe"
    COMMAND ${link} $<TARGET_OBJECTS:${name}_objects> ${tail}
    DEPENDS ${name}_objects $<TARGET_OBJECTS:${name}_objects>
    BYPRODUCTS "${CMAKE_BINARY_DIR}/${name}.exe.map"
    COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(${name} DEPENDS "${CMAKE_BINARY_DIR}/${name}.exe")
endfunction()
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
