# Diagnostic-only projection: preserve bodies and instrument their public calls.
set(source "${NTVDM_REPO}/src/compat/mvdm_softpc_wow_page_domain.c")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
file(READ "${source}" original)
set(body "${original}")
foreach(name collect_observed_classes collect_retired_windows copy_window_view project_window_graph)
  set(old "int mvdm_softpc_wow_page_domain_${name}(")
  string(REGEX MATCHALL "int mvdm_softpc_wow_page_domain_${name}\\(" matches "${body}")
  list(LENGTH matches count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "Page diagnostic boundary changed: ${name}")
  endif()
  string(REPLACE "${old}" "static int page_cost_body_${name}(" body "${body}")
endforeach()
set(restored "${body}")
foreach(name collect_observed_classes collect_retired_windows copy_window_view project_window_graph)
  string(REPLACE "static int page_cost_body_${name}("
    "int mvdm_softpc_wow_page_domain_${name}(" restored "${restored}")
endforeach()
if(NOT restored STREQUAL original)
  message(FATAL_ERROR "Page diagnostic reversal failed")
endif()
set(wrappers [=[
#undef qsort
int mvdm_softpc_wow_page_domain_collect_observed_classes(void) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_collect_observed_classes();
 page_cost_end(PAGE_COST_CLASS,started,0);return result;
}
int mvdm_softpc_wow_page_domain_collect_retired_windows(void) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_collect_retired_windows();
 page_cost_end(PAGE_COST_RETIRED,started,0);return result;
}
int mvdm_softpc_wow_page_domain_copy_window_view(ULONG capacity,mvdm_softpc_wow_window_observation *rows,ULONG *count) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_copy_window_view(capacity,rows,count);
 page_cost_end(PAGE_COST_COPY,started,result?*count:0);return result;
}
int mvdm_softpc_wow_page_domain_project_window_graph(ULONG count,const mvdm_softpc_wow_window_observation *rows) {
 LONGLONG started=page_cost_begin(); int result=page_cost_body_project_window_graph(count,rows);
 page_cost_end(PAGE_COST_GRAPH,started,count);return result;
}
]=])
set(root "${CMAKE_BINARY_DIR}/diagnostics/page-cost")
file(MAKE_DIRECTORY "${root}")
file(CONFIGURE OUTPUT "${root}/page-domain-profile.c" CONTENT
  "#include \"${NTVDM_REPO}/test/observation/wow_page_cost_instrument.h\"\n#define qsort page_cost_qsort\n${body}\n${wrappers}" @ONLY NEWLINE_STYLE LF)
get_target_property(flags unit_obj_compat_mvdm_softpc_wow_page_domain_obj COMPILE_FLAGS)
get_filename_component(source_directory "${source}" DIRECTORY)
string(APPEND flags " /I\"${source_directory}\"")
get_source_file_property(dependencies "${source}" OBJECT_DEPENDS)
if(NOT dependencies)
  set(dependencies)
endif()
set(object "diagnostics/page-cost/page-domain-profile.obj")
worker_unit("${object}" "${root}/page-domain-profile.c" "${flags}" ${dependencies})
get_property(members GLOBAL PROPERTY archive_softpc_bindings_lib)
set(original_member "obj/compat/mvdm_softpc_wow_page_domain.obj")
list(FIND members "${original_member}" index)
if(index LESS 0)
  message(FATAL_ERROR "Page domain archive member missing")
endif()
list(REMOVE_AT members ${index})
list(INSERT members ${index} "${object}")
set(library "diagnostics/page-cost/softpc-bindings.lib")
worker_archive("${library}" ${members})
get_target_property(inputs image_ntvdm_exe WORKER_LINK_INPUTS)
get_target_property(dependencies image_ntvdm_exe WORKER_LINK_DEPENDS)
list(FIND inputs softpc-bindings.lib index)
if(index LESS 0)
  message(FATAL_ERROR "Page domain archive absent from product")
endif()
list(REMOVE_AT inputs ${index})
list(INSERT inputs ${index} "${library}")
get_property(prefix GLOBAL PROPERTY image_ntvdm_exe_prefix)
get_property(suffix GLOBAL PROPERTY image_ntvdm_exe_suffix)
worker_link("diagnostics/page-cost/ntvdm.exe"
  "${prefix} /implib:diagnostics/page-cost/ntvdm.lib" "${suffix}"
  INPUTS ${inputs} DEPENDS ${dependencies} TIB_CHECK)
file(GENERATE OUTPUT "${root}/members.txt" CONTENT "$<JOIN:${members},\n>\n")
file(GENERATE OUTPUT "${root}/inputs.txt" CONTENT "$<JOIN:${inputs},\n>\n")
add_custom_target(wow-page-cost DEPENDS "${root}/ntvdm.exe")
