# Link the exact production membership object; only fixture profiles differ.
set(input_tests)
foreach(name IN ITEMS wow_native_input_group_fixture wow_native_input_focus_fixture
    wow_input_membership_fixture wow_input_lifetime_fixture)
  add_library(${name}_object OBJECT "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/${name}.c")
  set_target_properties(${name}_object PROPERTIES EXCLUDE_FROM_ALL TRUE)
  target_compile_options(${name}_object PRIVATE /nologo /MT /O2 /Gz /W4)
  target_include_directories(${name}_object PRIVATE "${NTVDM_REPO}/src"
    "${NTVDM_REPO}/src/wow32-dll/include" "${NTVDM_REPO}/src/compat/include")
  if(name STREQUAL "wow_input_lifetime_fixture")
    target_compile_options(${name}_object PRIVATE /WX)
  endif()
  set(extra)
  if(name MATCHES "^wow_input_")
    list(APPEND extra "$<TARGET_OBJECTS:obj_wow32-dll_wow_input_membership_obj>")
  endif()
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${name}.exe"
    COMMAND "${CMAKE_LINKER}" /nologo /MACHINE:X86 /OPT:REF
      "/OUT:${CMAKE_BINARY_DIR}/${name}.exe" "/MAP:${CMAKE_BINARY_DIR}/${name}.map"
      $<TARGET_OBJECTS:${name}_object> ${extra} kernel32.lib user32.lib
    DEPENDS ${name}_object $<TARGET_OBJECTS:${name}_object> ${extra}
    BYPRODUCTS "${CMAKE_BINARY_DIR}/${name}.map" COMMAND_EXPAND_LISTS VERBATIM)
  list(APPEND input_tests "${CMAKE_BINARY_DIR}/${name}.exe")
endforeach()
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/input-membership-object.txt"
  CONTENT "$<TARGET_OBJECTS:obj_wow32-dll_wow_input_membership_obj>\n")
add_custom_target(native-input-contracts DEPENDS ${input_tests})
