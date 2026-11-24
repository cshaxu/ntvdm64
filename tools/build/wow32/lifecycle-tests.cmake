function(wow_lifecycle_tests)
# Reuse product object identities; no Ninja command parser or duplicate compiler driver.
get_target_property(lifecycle_flags obj_compat_wow_user_task_lifecycle_obj COMPILE_FLAGS)
get_target_property(layout_flags obj_opennt-user_clglobal_obj COMPILE_FLAGS)
string(REGEX MATCHALL "/I[ ]*\"[^\"]+\"" layout_includes "${layout_flags}")
string(JOIN " " layout_includes ${layout_includes})
string(APPEND lifecycle_flags " ${layout_includes} /I \"${NTVDM_REPO}/src/wow32-dll/include\"")
string(REPLACE "/Gz" "/Gd" lifecycle_flags "${lifecycle_flags}")
foreach(pair IN ITEMS "seams|wow_combined_fixture_seams.c"
    "imports|wow_worker_static_imports.c" "parent|wow_page_domain_parent_stub.c"
    "task|wow_user_task_lifecycle_fixture.c")
  string(REPLACE "|" ";" pair "${pair}")
  list(GET pair 0 kind)
  list(GET pair 1 file)
  set(flags "${lifecycle_flags}")
  if(kind STREQUAL "parent")
    string(APPEND flags " /DWOW_TASK_CLASS_PROJECTION_FIXTURE")
  endif()
  wow_unit(lifecycle_${kind} "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/${file}" "${flags}")
  set_property(TARGET lifecycle_${kind} PROPERTY EXCLUDE_FROM_ALL TRUE)
endforeach()
get_target_property(redir_flags original_wkfileio COMPILE_FLAGS)
wow_unit(lifecycle_wkfileio "${NTVDM_REPO}/src/mvdm-host/wow32/wkfileio.c" "${redir_flags} /DVdmRedirLoaded=wow_fixture_provider_VdmRedirLoaded")
set_property(TARGET lifecycle_wkfileio PROPERTY EXCLUDE_FROM_ALL TRUE)
get_target_property(lifecycle_provider wow32 SOURCES)
list(REMOVE_ITEM lifecycle_provider "$<TARGET_OBJECTS:original_wkfileio>" "$<TARGET_OBJECTS:obj_wow32_res>")
list(APPEND lifecycle_provider "$<TARGET_OBJECTS:lifecycle_wkfileio>")
add_library(lifecycle_provider STATIC EXCLUDE_FROM_ALL ${lifecycle_provider})
set_target_properties(lifecycle_provider PROPERTIES LINKER_LANGUAGE C)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lifecycle-provider-objects.txt" CONTENT "$<JOIN:${lifecycle_provider},\n>\n")
get_filename_component(lifecycle_worker "${NTVDM_PARENT_IMPORT_LIBRARY}" DIRECTORY)
# The CMD entry builds these exact worker-owned objects before this target.
if(EXISTS "${lifecycle_worker}/lifecycle-inputs.txt")
  file(STRINGS "${lifecycle_worker}/lifecycle-inputs.txt" lifecycle_worker_inputs)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${lifecycle_worker}/lifecycle-inputs.txt")
  set(lifecycle_cases task native-send native-reply native-nested native-seen native-identity native-foreign native-indirect)
  ntvdm_source_projection("lifecycle-after.h" "${NTVDM_REPO}/src/compat/wow_user_task_lifecycle.c"
    "#define WOW_NATIVE_CALL_AFTER(" "#define WOW_NATIVE_CALL_RESULT(" "" FALSE)
  ntvdm_source_projection("lifecycle-message.h" "${NTVDM_REPO}/src/compat/wow_user_task_lifecycle.c"
    "#define WOW_NATIVE_MESSAGE(" "WOW_NATIVE_MESSAGE(SendMessageA," "" FALSE)
  file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/wow_native_fixture_call.h"
    CONTENT "#include \"lifecycle-after.h\"\n#include \"lifecycle-message.h\"\n" @ONLY)
  foreach(kind IN LISTS lifecycle_cases)
    set(fixture lifecycle_task)
    if(NOT kind STREQUAL "task")
      set(flags "${lifecycle_flags} /DWOW_NATIVE_SEND_FIXTURE /I \"${CMAKE_BINARY_DIR}\"")
      if(NOT kind STREQUAL "native-send")
        string(REPLACE "native-" "" option "${kind}")
        string(TOUPPER "${option}" option)
        string(APPEND flags " /DWOW_NATIVE_${option}_FIXTURE")
      endif()
      set(fixture lifecycle_${kind})
      wow_unit(${fixture} "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/wow_user_task_lifecycle_fixture.c" "${flags}")
      set_property(TARGET ${fixture} PROPERTY EXCLUDE_FROM_ALL TRUE)
    endif()
    set(output "${CMAKE_BINARY_DIR}/lifecycle-${kind}.exe")
  set(lifecycle_direct "$<TARGET_OBJECTS:${fixture}>" "$<TARGET_OBJECTS:lifecycle_parent>"
    "$<TARGET_OBJECTS:lifecycle_imports>" "$<TARGET_OBJECTS:lifecycle_seams>"
    "$<TARGET_OBJECTS:obj_wow32-system-import-abi-aliases_obj>"
    "$<TARGET_OBJECTS:obj_wow32-parent-import-abi-aliases_obj>")
  add_custom_command(OUTPUT "${output}"
    COMMAND "${CMAKE_LINKER}" /nologo /NODEFAULTLIB:ntvdm.lib "/out:${output}" "/map:${output}.map"
      ${lifecycle_direct} "$<TARGET_FILE:lifecycle_provider>" ${lifecycle_worker_inputs}
      "${CMAKE_BINARY_DIR}/legacy-wow-user32.lib" "${CMAKE_BINARY_DIR}/legacy-wow-kernel32.lib"
      "${CMAKE_BINARY_DIR}/legacy-wow-ntdll.lib" version.lib comdlg32.lib rpcrt4.lib kernel32.lib
      user32.lib gdi32.lib shell32.lib advapi32.lib ntdll.lib oldnames.lib libcmt.lib libvcruntime.lib libucrt.lib
    DEPENDS lifecycle_provider ${fixture} lifecycle_parent lifecycle_imports lifecycle_seams wow-imports
      ${lifecycle_direct} "$<TARGET_FILE:lifecycle_provider>" ${lifecycle_worker_inputs}
    COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(wow-lifecycle-${kind} DEPENDS "${output}")
  endforeach()
endif()
endfunction()
wow_lifecycle_tests()
