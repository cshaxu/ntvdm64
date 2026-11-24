include("${CMAKE_CURRENT_LIST_DIR}/../SourceProjection.cmake")
ntvdm_source_projection("creation/creation-menu.inc"
  "${NTVDM_REPO}/src/compat/wow_window_creation_binding.c"
  "BOOL WINAPI wow_window_creation_menu(" "static void trace_creation("
  "#include \"wow_window_creation_binding.h\"\n#include \"wow_menu_resource_bindings.h\"\n#include \"wow_window_access_provider.h\"\n#include <stdio.h>\n" FALSE)
ntvdm_source_projection("creation/hook-owner.inc"
  "${NTVDM_REPO}/src/mvdm-host/wow32/wowhooks.c" "void WINAPI wow_hook_capture_context(" "" "" FALSE)
ntvdm_source_projection("creation/task-owner.inc"
  "${NTVDM_REPO}/src/mvdm-host/wow32/wkman.c" "BOOL WINAPI wow_original_idle_registered("
  "/* DIVERGENCE(MVDM-HOST-DIV-909)" "" FALSE)
ntvdm_source_projection("creation/task-cleanup-owner.inc"
  "${NTVDM_REPO}/src/mvdm-host/wow32/wkman.c" "BOOL WINAPI wow_original_task_cleanup_uninitialized(" "" "" FALSE)
set(creation_outputs)
foreach(kind IN ITEMS hook menu timer idle common-dialog)
  if(kind STREQUAL "hook")
    set(stem wow_hook_callback)
  elseif(kind STREQUAL "timer")
    set(stem wow_timer_callback)
  elseif(kind STREQUAL "idle")
    set(stem wow_idle_hook)
  elseif(kind STREQUAL "common-dialog")
    set(stem wow_common_dialog_callback)
  else()
    set(stem wow_creation_menu)
  endif()
  set(owner obj_compat_${stem}_obj)
  if(kind STREQUAL "idle")
    set(owner obj_compat_wow_idle_hook_binding_obj)
  endif()
  # Owner projections include original private headers; use their real profiles.
  if(kind STREQUAL "idle" OR kind STREQUAL "common-dialog")
    get_target_property(flags original_wkman COMPILE_FLAGS)
  elseif(kind STREQUAL "hook")
    get_target_property(flags original_wowhooks COMPILE_FLAGS)
  elseif(kind STREQUAL "menu")
    get_target_property(flags obj_compat_wow_window_creation_binding_obj COMPILE_FLAGS)
  else()
    get_target_property(flags ${owner} COMPILE_FLAGS)
  endif()
  wow_unit(creation_${kind} "${NTVDM_REPO}/test/adapter-mvdm-host-out/wow/${stem}_fixture.c"
    "${flags} /Gy /I \"${CMAKE_BINARY_DIR}/creation\"")
  set_property(TARGET creation_${kind} PROPERTY EXCLUDE_FROM_ALL TRUE)
  set(providers creation_${kind})
  if(NOT kind STREQUAL "menu")
    list(APPEND providers ${owner})
  endif()
  if(kind STREQUAL "common-dialog")
    list(APPEND providers obj_compat_wow_task_cleanup_entry_obj)
  endif()
  set(inputs)
  foreach(provider IN LISTS providers)
    list(APPEND inputs "$<TARGET_OBJECTS:${provider}>")
  endforeach()
  set(output "${CMAKE_BINARY_DIR}/creation-${kind}.exe")
  add_custom_command(OUTPUT "${output}"
    COMMAND "${CMAKE_LINKER}" /nologo /machine:x86 /OPT:REF "/out:${output}" "/map:${output}.map"
      ${inputs} kernel32.lib user32.lib
    DEPENDS ${providers} ${inputs} COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(wow-creation-${kind} DEPENDS "${output}")
  list(APPEND creation_outputs "${output}")
endforeach()
add_custom_target(wow-creation DEPENDS ${creation_outputs})
