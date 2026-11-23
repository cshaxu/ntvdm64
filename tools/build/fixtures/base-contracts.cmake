# Tests link selected product objects, never duplicate provider builds.
function(base_contract name source object x64_owner)
  set(owner worker)
  if(NTVDM_ARCH STREQUAL "x64")
    set(owner "${x64_owner}")
  endif()
  set(product "${NTVDM_REPO}/build/.ninja/msvc-${NTVDM_ARCH}-release/${owner}/obj/opennt-base-bindings/${object}.obj")
  add_library(${name}_objects OBJECT "${NTVDM_REPO}/test/compat/${source}.c")
  set_target_properties(${name}_objects PROPERTIES EXCLUDE_FROM_ALL TRUE)
  target_compile_options(${name}_objects PRIVATE /nologo /W4 /WX)
  target_include_directories(${name}_objects PRIVATE "${NTVDM_REPO}/src")
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/${name}.exe"
    COMMAND "${CMAKE_C_COMPILER}" /nologo /MT "/Fe${CMAKE_BINARY_DIR}/${name}.exe"
      $<TARGET_OBJECTS:${name}_objects> "${product}" /link ntdll.lib
    DEPENDS ${name}_objects $<TARGET_OBJECTS:${name}_objects> "${product}"
    COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(${name} DEPENDS "${CMAKE_BINARY_DIR}/${name}.exe")
endfunction()
base_contract(base-classifier-path base_classifier_path classifier-path launcher)
target_compile_options(base-classifier-path_objects PRIVATE /external:W0)
target_include_directories(base-classifier-path_objects SYSTEM PRIVATE
  "${NTVDM_REPO}/src/compat/include" "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc")
base_contract(base-resource base_resource resources service)
base_contract(base-vdm-config base_vdm_config config service)
