cmake_minimum_required(VERSION 3.25)
set(NTVDM_BINARY_DIR "${CMAKE_BINARY_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/../BuildTree.cmake")
project(ntvdm_build_tools LANGUAGES C CXX)
if(NOT MSVC)
  message(FATAL_ERROR "Build tools require the configured MSVC toolchain")
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
add_executable(ntvdm-font font.c)
target_compile_options(ntvdm-font PRIVATE /W4 /WX)
target_compile_definitions(ntvdm-font PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(ntvdm-font PRIVATE bcrypt)
add_executable(ntvdm-cvidc cvidc.cpp)
target_compile_features(ntvdm-cvidc PRIVATE cxx_std_17)
target_compile_options(ntvdm-cvidc PRIVATE /W4 /WX /EHsc)
add_executable(ntvdm-tib tib.cpp)
target_compile_features(ntvdm-tib PRIVATE cxx_std_17)
target_compile_options(ntvdm-tib PRIVATE /W4 /WX /EHsc)
add_executable(ntvdm-package-id package-id.c package-manifest.c)
target_compile_options(ntvdm-package-id PRIVATE /W4 /WX)
target_link_libraries(ntvdm-package-id PRIVATE ole32)

# CMD CALL-label offsets require CRLF. Authored template remains LF.
get_filename_component(NTVDM_STAGE_REPO "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
file(TO_NATIVE_PATH "${NTVDM_STAGE_REPO}" NTVDM_STAGE_REPO)
configure_file(stage-system32.cmd.in stage-system32.cmd @ONLY NEWLINE_STYLE CRLF)
configure_file(refresh-package.cmd.in refresh-package.cmd @ONLY NEWLINE_STYLE CRLF)
configure_file(stage-runtime.cmd.in stage-runtime.cmd @ONLY NEWLINE_STYLE CRLF)
# Match the published UTF-8 text composition, excluding input BOMs.
file(READ "${NTVDM_STAGE_REPO}/src/lib-win32/process-hook/LICENSE.win16style" hook_license)
file(READ "${NTVDM_STAGE_REPO}/src/lib-win32/process-hook/minhook/LICENSE.txt" minhook_license)
string(ASCII 239 187 191 utf8_bom)
string(REGEX REPLACE "^${utf8_bom}" "" hook_license "${hook_license}")
string(REGEX REPLACE "^${utf8_bom}" "" minhook_license "${minhook_license}")
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/hook-license.txt" CONTENT "${hook_license}\n" NEWLINE_STYLE CRLF)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/minhook-license.txt" CONTENT "${minhook_license}" NEWLINE_STYLE UNIX)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${NTVDM_STAGE_REPO}/src/lib-win32/process-hook/LICENSE.win16style"
  "${NTVDM_STAGE_REPO}/src/lib-win32/process-hook/minhook/LICENSE.txt")

configure_file(prepare-wow-diagnostic.cmd.in prepare-wow-diagnostic.cmd @ONLY NEWLINE_STYLE CRLF)

configure_file(prepare-test-inputs.cmd.in prepare-test-inputs.cmd @ONLY NEWLINE_STYLE CRLF)

# XP's original generator reads its original CodePairs object.
add_executable(ntvdm-generr "${NTVDM_STAGE_REPO}/src/non-mvdm/sdktools/generr/generr.c")
target_compile_definitions(ntvdm-generr PRIVATE _CRT_SECURE_NO_WARNINGS)
add_library(ntvdm-rtl-error-pairs OBJECT "${NTVDM_STAGE_REPO}/src/non-mvdm/base/ntos/rtl/generr.c")
target_include_directories(ntvdm-rtl-error-pairs PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
  "${NTVDM_STAGE_REPO}/src/non-mvdm/public/internal/base/inc")
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/rtl/error.h"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/rtl"
  COMMAND ntvdm-generr $<TARGET_OBJECTS:ntvdm-rtl-error-pairs> "${CMAKE_BINARY_DIR}/rtl/error.h"
  DEPENDS ntvdm-generr ntvdm-rtl-error-pairs $<TARGET_OBJECTS:ntvdm-rtl-error-pairs>
  COMMAND_EXPAND_LISTS VERBATIM)
add_custom_target(ntvdm-rtl-error-header ALL DEPENDS "${CMAKE_BINARY_DIR}/rtl/error.h")
