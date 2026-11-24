get_target_property(public_memory_flags original_wcall16 COMPILE_FLAGS)
foreach(flag IN ITEMS /TC /MT /O2 /Gz)
  string(FIND "${public_memory_flags}" "${flag}" found)
  if(found LESS 0)
    message(FATAL_ERROR "Public memory ABI production profile lacks ${flag}")
  endif()
endforeach()
string(REPLACE "/Gz" "/Gd" public_memory_flags "${public_memory_flags}")
wow_unit(wow_public_memory_abi_fixture "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_public_memory_abi_fixture.c" "${public_memory_flags}")
set_property(TARGET wow_public_memory_abi_fixture PROPERTY EXCLUDE_FROM_ALL TRUE)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/public-memory-abi-object.txt" CONTENT "$<TARGET_OBJECTS:wow_public_memory_abi_fixture>\n")
add_custom_target(wow-public-memory-abi DEPENDS wow_public_memory_abi_fixture)

add_library(wow_registry_shutdown_fixture OBJECT "${NTVDM_REPO}/test/compat/wow_registry_shutdown_fixture.c")
set_property(TARGET wow_registry_shutdown_fixture PROPERTY EXCLUDE_FROM_ALL TRUE)
target_compile_options(wow_registry_shutdown_fixture PRIVATE /nologo /W4)
target_include_directories(wow_registry_shutdown_fixture PRIVATE "${NTVDM_REPO}/src")
set(output "${CMAKE_BINARY_DIR}/registry-shutdown.exe")
add_custom_command(OUTPUT "${output}"
  COMMAND "${CMAKE_C_COMPILER}" /nologo /MT "/Fe${output}"
    $<TARGET_OBJECTS:wow_registry_shutdown_fixture> $<TARGET_OBJECTS:obj_compat_wow_registry_binding_obj>
    /link kernel32.lib "/MAP:${output}.map"
  DEPENDS wow_registry_shutdown_fixture obj_compat_wow_registry_binding_obj
    $<TARGET_OBJECTS:wow_registry_shutdown_fixture> $<TARGET_OBJECTS:obj_compat_wow_registry_binding_obj>
  COMMAND_EXPAND_LISTS VERBATIM)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/registry-shutdown-provider.txt" CONTENT "$<TARGET_OBJECTS:obj_compat_wow_registry_binding_obj>\n")
add_custom_target(wow-registry-shutdown DEPENDS "${output}")

# Independent ticket completion contract; reuse the actual selected product object.
add_library(wow_task_close_fixture OBJECT "${NTVDM_REPO}/test/wow32-dll/source/wow_task_close_result_fixture.c")
set_property(TARGET wow_task_close_fixture PROPERTY EXCLUDE_FROM_ALL TRUE)
target_compile_options(wow_task_close_fixture PRIVATE /nologo /W4)
target_include_directories(wow_task_close_fixture PRIVATE "${NTVDM_REPO}/src")
set(output "${CMAKE_BINARY_DIR}/task-close-result.exe")
add_custom_command(OUTPUT "${output}"
  COMMAND "${CMAKE_C_COMPILER}" /nologo /MT "/Fe${output}"
    $<TARGET_OBJECTS:wow_task_close_fixture> $<TARGET_OBJECTS:obj_wow32-dll_wow_task_termination_obj>
    /link kernel32.lib "/MAP:${output}.map"
  DEPENDS wow_task_close_fixture obj_wow32-dll_wow_task_termination_obj
    $<TARGET_OBJECTS:wow_task_close_fixture> $<TARGET_OBJECTS:obj_wow32-dll_wow_task_termination_obj>
  COMMAND_EXPAND_LISTS VERBATIM)
add_custom_target(wow-task-close-result DEPENDS "${output}")
