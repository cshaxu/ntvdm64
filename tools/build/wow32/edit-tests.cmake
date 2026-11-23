# These contracts link the selected production object, never a fixture copy.
add_library(edit_test_seams OBJECT EXCLUDE_FROM_ALL
  "${NTVDM_REPO}/artifacts/mvdm-audit/host-ch-scope/tabstop-release-seams.c")
target_compile_options(edit_test_seams PRIVATE /nologo /TC /O2 /MT)
set(edit_tests)
foreach(name IN ITEMS tabstop wordbreak)
  if(name STREQUAL "tabstop")
    set(source "${NTVDM_REPO}/test/integration/wow_edit_tabstop_test.c")
  else()
    set(source "${NTVDM_REPO}/artifacts/mvdm-audit/host-ch-scope/wordbreak-object-fixture.c")
  endif()
  add_library(edit_${name}_test OBJECT EXCLUDE_FROM_ALL "${source}")
  target_compile_options(edit_${name}_test PRIVATE /nologo /TC /O2 /MT)
  set(output "${CMAKE_BINARY_DIR}/edit-${name}.exe")
  add_custom_command(OUTPUT "${output}"
    COMMAND "${CMAKE_LINKER}" /nologo "/OUT:${output}" "/MAP:${output}.map"
      $<TARGET_OBJECTS:edit_${name}_test> $<TARGET_OBJECTS:edit_test_seams>
      $<TARGET_OBJECTS:original_wmsgem> kernel32.lib user32.lib
    DEPENDS edit_${name}_test edit_test_seams original_wmsgem
      $<TARGET_OBJECTS:edit_${name}_test> $<TARGET_OBJECTS:edit_test_seams>
      $<TARGET_OBJECTS:original_wmsgem>
    BYPRODUCTS "${output}.map" COMMAND_EXPAND_LISTS VERBATIM)
  list(APPEND edit_tests "${output}")
endforeach()
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/edit-production-object.txt"
  CONTENT "$<TARGET_OBJECTS:original_wmsgem>\n")
add_custom_target(wow-edit-contracts DEPENDS ${edit_tests})
