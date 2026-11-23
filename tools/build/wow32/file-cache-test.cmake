# Preserve the existing native COFF import substitution and GNU fixture link.
# The source object is the actual product object; no mirror source projection.
find_program(NTVDM_TEST_OBJCOPY NAMES objcopy.exe objcopy)
find_program(NTVDM_TEST_I686_GCC NAMES i686-w64-mingw32-gcc.exe i686-w64-mingw32-gcc)
if(NOT NTVDM_TEST_OBJCOPY OR NOT NTVDM_TEST_I686_GCC)
  add_custom_target(wow-file-cache
    COMMAND "${CMAKE_COMMAND}" -E echo "wow-file-cache requires objcopy and i686-w64-mingw32-gcc"
    COMMAND "${CMAKE_COMMAND}" -E false)
  return()
endif()
set(cache_object "${CMAKE_BINARY_DIR}/wkfileio-test.obj")
set(cache_test "${CMAKE_BINARY_DIR}/wow-file-cache.exe")
add_custom_command(OUTPUT "${cache_object}"
  COMMAND "${NTVDM_TEST_OBJCOPY}"
    "--redefine-sym=__imp__CreateFileMappingA@24=_cache_imp_Create"
    "--redefine-sym=__imp__MapViewOfFile@20=_cache_imp_Map"
    "--redefine-sym=__imp__CloseHandle@4=_cache_imp_Close"
    "--redefine-sym=__imp__UnmapViewOfFile@4=_cache_imp_Unmap"
    $<TARGET_OBJECTS:original_wkfileio> "${cache_object}"
  DEPENDS original_wkfileio $<TARGET_OBJECTS:original_wkfileio>
    "${NTVDM_TEST_OBJCOPY}" COMMAND_EXPAND_LISTS VERBATIM)
add_custom_command(OUTPUT "${cache_test}"
  COMMAND "${NTVDM_TEST_I686_GCC}" -O2 -ffunction-sections -fdata-sections
    "${NTVDM_REPO}/test/integration/wow_file_cache_test.c" "${cache_object}"
    -Wl,--gc-sections -o "${cache_test}"
  DEPENDS "${cache_object}" "${NTVDM_REPO}/test/integration/wow_file_cache_test.c"
    "${NTVDM_TEST_I686_GCC}" VERBATIM)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/file-cache-production-object.txt"
  CONTENT "$<TARGET_OBJECTS:original_wkfileio>\n")
add_custom_target(wow-file-cache DEPENDS "${cache_test}")
