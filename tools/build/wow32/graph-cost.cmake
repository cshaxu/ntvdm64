# Optional graph diagnostic, using the current product object and link owners.
set(source "${NTVDM_REPO}/src/compat/wow_user_object_bindings.c")
file(READ "${source}" original)
string(REPLACE "\r\n" "\n" original "${original}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
set(body "${original}")
set(graph_marks)
macro(graph_mark old new count)
  string(REPLACE "${old}" "" removed "${body}")
  string(LENGTH "${body}" before_length)
  string(LENGTH "${removed}" after_length)
  string(LENGTH "${old}" needle_length)
  math(EXPR found "(${before_length} - ${after_length}) / ${needle_length}")
  if(NOT found EQUAL ${count})
    message(FATAL_ERROR "Graph diagnostic boundary changed: ${old} (${found})")
  endif()
  list(LENGTH graph_marks mark_id)
  set(graph_old_${mark_id} "${old}")
  set(graph_new_${mark_id} "${new}")
  list(APPEND graph_marks ${mark_id})
  string(REPLACE "${old}" "${new}" body "${body}")
endmacro()
foreach(name IN ITEMS refresh_graph project_graph)
  if(name STREQUAL "refresh_graph")
    set(args "HWND callback_window, const HWND *payload, ULONG payload_count")
    set(call "callback_window, payload, payload_count")
    set(kind 3)
    set(items payload_count)
  else()
    set(args "const HWND *roots, ULONG root_count,\n    const HWND *payload, ULONG payload_count")
    set(call "roots, root_count, payload, payload_count")
    set(kind 2)
    set(items root_count)
  endif()
  set(old "static BOOL ${name}(${args})\n{")
  set(new "static BOOL graph_cost_body_${name}(${args});\nstatic BOOL ${name}(${args})\n{\n    BOOL result=FALSE; LONGLONG started=graph_cost_begin();\n    __try { result=graph_cost_body_${name}(${call}); }\n    __finally { graph_cost_end(${kind},started,${items}); }\n    return result;\n}\nstatic BOOL graph_cost_body_${name}(${args})\n{")
  graph_mark("${old}" "${new}" 1)
endforeach()
# Capture/normalization moved to their shared providers. Time the same calls
# from this graph transaction; do not instrument unrelated provider consumers.
graph_mark("wow_native_window_capture(" "graph_cost_capture(" 4)
graph_mark("wow_native_window_normalize_chains(" "graph_cost_normalize(" 1)
set(anchor "typedef mvdm_softpc_wow_window_observation window_observation;")
set(wrappers [=[
static BOOL graph_cost_capture(HWND window, window_observation *row)
{
    BOOL result=FALSE; LONGLONG started=graph_cost_begin();
    __try { result=wow_native_window_capture(window,row); }
    __finally { graph_cost_end(0,started,1); }
    return result;
}
static BOOL graph_cost_normalize(window_observation *rows, ULONG count,
    const ULONG *index, BOOL *capture_raced)
{
    BOOL result=FALSE; LONGLONG started=graph_cost_begin();
    __try { result=wow_native_window_normalize_chains(rows,count,index,capture_raced); }
    __finally { graph_cost_end(1,started,count); }
    return result;
}
]=])
graph_mark("${anchor}" "${anchor}\n${wrappers}" 1)
set(commit "    result = wow_window_words_project_observed(count, rows);")
graph_mark("${commit}" "    { LONGLONG started=graph_cost_begin();\n      __try { result = wow_window_words_project_observed(count, rows); }\n      __finally { graph_cost_end(4,started,count); }\n    }" 1)
set(restored "${body}")
list(REVERSE graph_marks)
foreach(id IN LISTS graph_marks)
  string(REPLACE "${graph_new_${id}}" "${graph_old_${id}}" restored "${restored}")
endforeach()
if(NOT restored STREQUAL original)
  message(FATAL_ERROR "Graph diagnostic reversal failed")
endif()
set(root "${CMAKE_BINARY_DIR}/diagnostics/graph-cost")
set(test "${NTVDM_REPO}/test/observation")
file(MAKE_DIRECTORY "${root}")
file(CONFIGURE OUTPUT "${root}/graph-profile.c" CONTENT
  "#include \"${test}/wow_graph_cost_instrument.h\"\n${body}" @ONLY NEWLINE_STYLE LF)
get_target_property(flags obj_compat_wow_user_object_bindings_obj COMPILE_FLAGS)
add_library(graph_cost_object OBJECT "${root}/graph-profile.c")
set_target_properties(graph_cost_object PROPERTIES EXCLUDE_FROM_ALL TRUE COMPILE_FLAGS "${flags}")
set(diagnostic_objects ${objects})
list(FIND diagnostic_objects "$<TARGET_OBJECTS:obj_compat_wow_user_object_bindings_obj>" index)
if(index LESS 0)
  message(FATAL_ERROR "Graph product object missing")
endif()
list(REMOVE_AT diagnostic_objects ${index})
list(INSERT diagnostic_objects ${index} "$<TARGET_OBJECTS:graph_cost_object>")
add_library(wow_graph_cost SHARED EXCLUDE_FROM_ALL ${diagnostic_objects})
set_target_properties(wow_graph_cost PROPERTIES LINKER_LANGUAGE C PREFIX ""
  OUTPUT_NAME WOW32 RUNTIME_OUTPUT_DIRECTORY "${root}" ARCHIVE_OUTPUT_DIRECTORY "${root}")
add_dependencies(wow_graph_cost wow-imports)
get_target_property(libraries wow32 LINK_LIBRARIES)
target_link_libraries(wow_graph_cost PRIVATE ${libraries})
target_link_options(wow_graph_cost PRIVATE "/map:${root}/WOW32.DLL.map" "/def:${CMAKE_BINARY_DIR}/wow32-x86.def")
set_property(TARGET wow_graph_cost APPEND PROPERTY LINK_DEPENDS "${CMAKE_BINARY_DIR}/wow32-x86.def")
file(GENERATE OUTPUT "${root}/objects.txt" CONTENT "$<JOIN:${diagnostic_objects},\n>\n")
file(GENERATE OUTPUT "${root}/graph-object.txt" CONTENT "$<TARGET_OBJECTS:graph_cost_object>\n")
add_custom_target(wow-graph-cost DEPENDS wow_graph_cost)
include("${NTVDM_REPO}/tools/build/fixtures/graph-probe.cmake")
ntvdm_graph_probe("${root}/wow_page_cost_probe.h")
