cmake_minimum_required(VERSION 3.25)
set(NTVDM_BINARY_DIR "${CMAKE_BINARY_DIR}")
include(tools/build/BuildTree.cmake)
project(ntvdm64_host_tools LANGUAGES C CXX)
if(NOT MSVC)
  message(FATAL_ERROR "The admitted host source islands require MSVC /MT; GCC is not a product profile")
endif()
if(NOT NTVDM_ARCHITECTURE MATCHES "^(x86|x64)$")
  message(FATAL_ERROR "Select NTVDM_ARCHITECTURE=x86 or x64")
endif()
if((NTVDM_ARCHITECTURE STREQUAL "x86" AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
   OR (NTVDM_ARCHITECTURE STREQUAL "x64" AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8))
  message(FATAL_ERROR "Use the matching Visual Studio developer environment; do not mix architecture trees")
endif()
# FORCE also clears an old launcher when a non-cache preset is selected.
set(CMAKE_C_COMPILER_LAUNCHER "${NTVDM_COMPILER_LAUNCHER}" CACHE STRING "Repository-local optional cache" FORCE)
set(CMAKE_CXX_COMPILER_LAUNCHER "${NTVDM_COMPILER_LAUNCHER}" CACHE STRING "Repository-local optional cache" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
include(tests/build/wow-message-probes.cmake)

# The original source-manifest composition remains its existing owner. CMake
# selects stable subtrees and exposes their real Ninja targets, not a second
# handwritten inventory of MVDM or native-component sources.
find_program(NTVDM_POWERSHELL NAMES pwsh REQUIRED)
find_program(NTVDM_NODE NAMES node REQUIRED)
set(_worker_tree "${CMAKE_BINARY_DIR}/worker-reference-x86")
set(_native_args)
if(NTVDM_ARCHITECTURE STREQUAL "x86")
  set(_worker_tree "${CMAKE_BINARY_DIR}/worker")
  # The existing image importer deliberately accepts build-owned producers
  # only. Identity-preserving generated prerequisites satisfy that contract;
  # never relax it or read/copy an installed Windows directory.
  foreach(_pair Worker/ntvwm Frontend/ntcon Monitor/ntmon Launcher/run16 Service/ntsrv)
    string(REPLACE "/" ";" _parts "${_pair}")
    list(GET _parts 0 _option)
    list(GET _parts 1 _image)
    configure_file("assets/release/${_image}.exe" "native-inputs/${_image}.exe" COPYONLY)
    list(APPEND _native_args "-Native${_option}" "${CMAKE_BINARY_DIR}/native-inputs/${_image}.exe")
  endforeach()
endif()
set(_cache_args)
if(NTVDM_USE_CCACHE)
  list(APPEND _cache_args -CompilerCache "${NTVDM_CCACHE_EXECUTABLE}")
endif()
function(ntvdm_generate script)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tools/build/${script}"
    "${CMAKE_SOURCE_DIR}/tools/build/Initialize-NinjaTree.ps1")
  execute_process(COMMAND "${NTVDM_POWERSHELL}" -NoProfile -ExecutionPolicy Bypass
    -File "${CMAKE_SOURCE_DIR}/tools/build/${script}" ${ARGN} ${_cache_args}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMAND_ERROR_IS_FATAL ANY)
endfunction()
ntvdm_generate(New-T310OriginalSoftpcNinja.ps1 -Architecture x86
  -RepositoryRoot "${CMAKE_SOURCE_DIR}" -BuildRoot "${_worker_tree}"
  -NodeExecutable "${NTVDM_NODE}"
  -NinjaExecutable "${CMAKE_MAKE_PROGRAM}" ${_native_args})
if(NTVDM_ARCHITECTURE STREQUAL "x86")
  add_custom_target(worker COMMAND cmd /d /c "${_worker_tree}/run-ninja-parallel.cmd" ntvdm.exe
    BYPRODUCTS "${_worker_tree}/ntvdm.lib"
    USES_TERMINAL)
  # Its generator reads the actual parent's import library; configure only
  # after that producer has run, never borrow an older library to configure.
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/wow32/build.ninja"
    COMMAND "${NTVDM_POWERSHELL}" -NoProfile -ExecutionPolicy Bypass -File
      "${CMAKE_SOURCE_DIR}/tools/build/New-T404S5Wow32ProviderNinja.ps1"
      -Architecture x86 -RepositoryRoot "${CMAKE_SOURCE_DIR}"
      -BuildRoot "${CMAKE_BINARY_DIR}/wow32"
      -ParentImportLibrary "${_worker_tree}/ntvdm.lib" ${_cache_args}
    DEPENDS "${_worker_tree}/ntvdm.lib"
      "${CMAKE_SOURCE_DIR}/tools/build/New-T404S5Wow32ProviderNinja.ps1"
      "${CMAKE_SOURCE_DIR}/src/mvdm/wow32/sources"
      "${CMAKE_SOURCE_DIR}/src/mvdm/wow32/wow32.def"
    VERBATIM)
  add_custom_target(wow32
    COMMAND cmd /d /c "${CMAKE_BINARY_DIR}/wow32/msvc-x86.cmd"
      "${CMAKE_MAKE_PROGRAM}" -C "${CMAKE_BINARY_DIR}/wow32" wow32.dll
    DEPENDS worker "${CMAKE_BINARY_DIR}/wow32/build.ninja" USES_TERMINAL)
else()
  foreach(_component Worker Frontend Monitor Launcher Service)
    string(TOLOWER "${_component}" _name)
    ntvdm_generate(New-NativeWorkerNinja.ps1 -RepositoryRoot "${CMAKE_SOURCE_DIR}"
      -BuildRoot "${CMAKE_BINARY_DIR}/${_name}" -Component "${_component}"
      -ReferenceGraph "${_worker_tree}/build.ninja")
    add_custom_target(native-${_name} COMMAND cmd /d /c
      "${CMAKE_BINARY_DIR}/${_name}/run-ninja.cmd" USES_TERMINAL)
  endforeach()
  ntvdm_generate(New-NativeHook64Ninja.ps1 -RepositoryRoot "${CMAKE_SOURCE_DIR}"
    -BuildRoot "${CMAKE_BINARY_DIR}/hook" -ReferenceGraph "${_worker_tree}/build.ninja")
  add_custom_target(hook64 COMMAND cmd /d /c "${CMAKE_BINARY_DIR}/hook/run-ninja.cmd" USES_TERMINAL)
endif()
