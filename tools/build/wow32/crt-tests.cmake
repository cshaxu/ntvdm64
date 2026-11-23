# Standalone CRT ABI checks and the actual WOW32 provider, with negative links.
add_library(crt_test_object OBJECT EXCLUDE_FROM_ALL
  "${NTVDM_REPO}/test/lib-win32/crt-compat/crt_compat_test.c")
target_compile_options(crt_test_object PRIVATE /nologo /D_NO_CRT_STDIO_INLINE /D_CRT_SECURE_NO_WARNINGS)
add_library(crt_test_provider OBJECT EXCLUDE_FROM_ALL
  "${NTVDM_REPO}/src/lib-win32/crt-compat/crt_compat.c")
target_compile_options(crt_test_provider PRIVATE /nologo)
set(crt_root "${CMAKE_BINARY_DIR}/tests/crt")
file(MAKE_DIRECTORY "${crt_root}")
foreach(kind IN ITEMS standalone production)
  if(kind STREQUAL "standalone")
    set(provider crt_test_provider)
  else()
    set(provider obj_adapter-win32_crt_compat_obj)
  endif()
  add_custom_command(OUTPUT "${crt_root}/${kind}.exe"
    COMMAND "${CMAKE_LINKER}" /nologo "/OUT:${crt_root}/${kind}.exe"
      $<TARGET_OBJECTS:crt_test_object> $<TARGET_OBJECTS:${provider}>
    DEPENDS crt_test_object ${provider} $<TARGET_OBJECTS:crt_test_object> $<TARGET_OBJECTS:${provider}>
    COMMAND_EXPAND_LISTS VERBATIM)
endforeach()
add_custom_target(crt-contracts DEPENDS "${crt_root}/standalone.exe" "${crt_root}/production.exe")
add_custom_target(crt-missing
  COMMAND "${CMAKE_LINKER}" /nologo "/OUT:${crt_root}/missing.exe" $<TARGET_OBJECTS:crt_test_object>
  DEPENDS crt_test_object COMMAND_EXPAND_LISTS VERBATIM)
add_custom_target(crt-duplicate
  COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_OBJECTS:crt_test_provider> "${crt_root}/duplicate.obj"
  COMMAND "${CMAKE_LINKER}" /nologo "/OUT:${crt_root}/duplicate.exe"
    $<TARGET_OBJECTS:crt_test_object> $<TARGET_OBJECTS:crt_test_provider> "${crt_root}/duplicate.obj"
  DEPENDS crt_test_object crt_test_provider COMMAND_EXPAND_LISTS VERBATIM)
file(GENERATE OUTPUT "${crt_root}/production-object.txt" CONTENT "$<TARGET_OBJECTS:obj_adapter-win32_crt_compat_obj>\n")
