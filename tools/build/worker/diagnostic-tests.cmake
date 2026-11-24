# Diagnostic images reuse the actual product graph. Only the explicitly
# selected original translation unit is wrapped; production inputs are intact.
worker_unit("obj/tests/frontend_frame_codepage.obj"
  "${NTVDM_REPO}/test/app/frontend_frame_codepage_test.c"
  "${profile_49} /std:c11 /I\"${NTVDM_REPO}/src/ntcon-exe\"" "obj/basesrv/service.h")
get_target_property(frame_inputs image_console_frame_failure_test_exe WORKER_LINK_INPUTS)
list(REMOVE_ITEM frame_inputs "obj/tests/console_frame_failure.obj")
list(PREPEND frame_inputs "obj/tests/frontend_frame_codepage.obj")
list(APPEND frame_inputs "obj/frontend/console_frontend.obj")
get_property(frame_prefix GLOBAL PROPERTY image_console_frame_failure_test_exe_prefix)
get_property(frame_suffix GLOBAL PROPERTY image_console_frame_failure_test_exe_suffix)
worker_link("frontend-frame-codepage-test.exe" "${frame_prefix}" "${frame_suffix}" INPUTS ${frame_inputs})
add_custom_target(frame-codepage DEPENDS "${CMAKE_BINARY_DIR}/frontend-frame-codepage-test.exe")

function(worker_trace_unit name member source)
  string(MAKE_C_IDENTIFIER "unit_${member}" original)
  if(NOT TARGET ${original})
    message(FATAL_ERROR "Missing diagnostic owner: ${member}")
  endif()
  get_target_property(flags ${original} COMPILE_FLAGS)
  get_target_property(original_sources ${original} SOURCES)
  get_source_file_property(dependencies "${original_sources}" OBJECT_DEPENDS)
  if(NOT dependencies)
    set(dependencies)
  endif()
  set(output "diagnostics/${name}/${source}.obj")
  worker_unit("${output}" "${NTVDM_REPO}/test/observation/${source}.c" "${flags}" ${dependencies})
  string(MAKE_C_IDENTIFIER "unit_${output}" unit)
  target_compile_definitions(${unit} PRIVATE
    "S35_ENVIRONMENT_TRACE_LOG=\"${CMAKE_BINARY_DIR}/diagnostics/${name}/events.txt\""
    "DPMI_TRACE_LOG=\"${CMAKE_BINARY_DIR}/diagnostics/${name}/events.txt\"" ${ARGN})
  set(trace_object "${output}" PARENT_SCOPE)
endfunction()

function(worker_trace name library member source)
  string(MAKE_C_IDENTIFIER "archive_${library}" archive)
  get_property(members GLOBAL PROPERTY ${archive})
  if(NOT member IN_LIST members)
    message(FATAL_ERROR "Diagnostic member absent from ${library}: ${member}")
  endif()
  worker_trace_unit("${name}" "${member}" "${source}" ${ARGN})
  list(FIND members "${member}" position)
  list(REMOVE_AT members ${position})
  list(INSERT members ${position} "${trace_object}")
  if(name STREQUAL "dpmi-xmem-failure")
    worker_trace_unit("${name}" "obj/dpmi/dpmi32.obj" dpmi_dispatch_trace DPMI_TRACE_XMEM_FAILURE)
    list(FIND members "obj/dpmi/dpmi32.obj" position)
    if(position LESS 0)
      message(FATAL_ERROR "Missing coupled XMEM dispatch owner")
    endif()
    list(REMOVE_AT members ${position})
    list(INSERT members ${position} "${trace_object}")
  endif()
  set(replacement "diagnostics/${name}/${library}")
  worker_archive("${replacement}" ${members})
  get_target_property(inputs image_ntvdm_exe WORKER_LINK_INPUTS)
  get_target_property(dependencies image_ntvdm_exe WORKER_LINK_DEPENDS)
  if(NOT library IN_LIST inputs)
    message(FATAL_ERROR "Diagnostic library absent from product link: ${library}")
  endif()
  set(replaced)
  foreach(input IN LISTS inputs)
    if(input STREQUAL library)
      list(APPEND replaced "${replacement}")
    else()
      list(APPEND replaced "${input}")
    endif()
  endforeach()
  get_property(prefix GLOBAL PROPERTY image_ntvdm_exe_prefix)
  get_property(suffix GLOBAL PROPERTY image_ntvdm_exe_suffix)
  worker_link("diagnostics/${name}/ntvdm.exe"
    "${prefix} /implib:diagnostics/${name}/ntvdm.lib" "${suffix}"
    INPUTS ${replaced} DEPENDS ${dependencies})
  set_property(GLOBAL APPEND PROPERTY diagnostic_images
    "${CMAKE_BINARY_DIR}/diagnostics/${name}/ntvdm.exe")
endfunction()

worker_trace(environment original-mvdm-command.lib obj/command/cmdenv.obj command_environment_trace)
worker_trace(wow-environment original-mvdm-command.lib obj/command/cmdmisc.obj wow_initial_environment_trace)
worker_trace(command-dispatch original-mvdm-command.lib obj/command/cmddisp.obj command_dispatch_trace)
worker_trace(thread-exit compat-worker-thread.lib obj/compat/thread_start.obj wow_thread_exit_trace)
worker_trace(exception original-softpc-host-roots.lib obj/host/nt_inthk.obj dpmi_exception_trace)
worker_trace(dpmi original-mvdm-dpmi32.lib obj/dpmi/dpmi32.obj dpmi_dispatch_trace)
worker_trace(dpmi-lifecycle original-mvdm-dpmi32.lib obj/dpmi/dpmi32.obj dpmi_dispatch_trace DPMI_TRACE_LIFECYCLE_ONLY)
worker_trace(dpmi-pointer original-mvdm-dpmi32.lib obj/dpmi/int21map.obj dpmi_pointer_trace)
worker_trace(dpmi-ioctl original-mvdm-dpmi32.lib obj/dpmi/int21map.obj dpmi_ioctl_boundary)
worker_trace(dpmi-xmem-failure original-mvdm-dpmi32.lib obj/dpmi/xmem.obj dpmi_xmem_failure)
worker_trace(command-client base-rpc-client-launch.lib obj/run16/rpc_client_launch.obj ntvdm_command_client_diagnostic)
# Preserve the existing DOS identity witness without scraping generated Ninja.
# The diagnostic archive replaces exactly the observer member in product order.
worker_trace(dos-identity softpc-bindings.lib obj/adapter-softpc/ntvdm_dos_observer.obj
  dos_identity_observed)
add_custom_target(dos-identity-observer
  DEPENDS "${CMAKE_BINARY_DIR}/diagnostics/dos-identity/ntvdm.exe")

# The continuation witness interposes two entries while retaining the real
# worker providers and their original compile flags.
set(continuation_dir "diagnostics/vm-continuation")
get_target_property(bridge_flags unit_obj_adapter_softpc_mvdm_vm_bridge_obj COMPILE_FLAGS)
get_target_property(bop_flags unit_obj_host_nt_bop_obj COMPILE_FLAGS)
worker_unit("${continuation_dir}/bridge.obj"
  "${NTVDM_REPO}/src/compat/mvdm_vm_bridge.c"
  "${bridge_flags} /Dmvdm_vm_bridge_bop=mvdm_vm_bridge_bop_direct")
worker_unit("${continuation_dir}/bop.obj"
  "${NTVDM_REPO}/src/mvdm-host/softpc.new/host/src/nt_bop.c"
  "${bop_flags} /DCmdDispatch=test_cmd_dispatch")
worker_unit("${continuation_dir}/witness.obj"
  "${NTVDM_REPO}/test/integration/ntvdm64-386/host-continuation.c"
  "${bridge_flags} /I\"${NTVDM_REPO}/src/ntvdm-exe/softpc/include\"")
get_target_property(continuation_inputs image_ntvdm_exe WORKER_LINK_INPUTS)
get_target_property(continuation_dependencies image_ntvdm_exe WORKER_LINK_DEPENDS)
get_property(continuation_prefix GLOBAL PROPERTY image_ntvdm_exe_prefix)
get_property(continuation_suffix GLOBAL PROPERTY image_ntvdm_exe_suffix)
worker_link("${continuation_dir}/ntvdm.exe"
  "${continuation_prefix} /implib:${continuation_dir}/ntvdm.lib" "${continuation_suffix}"
  INPUTS "${continuation_dir}/bop.obj" "${continuation_dir}/bridge.obj"
    "${continuation_dir}/witness.obj" ${continuation_inputs}
  DEPENDS ${continuation_dependencies})
add_custom_target(vm-continuation-observer
  DEPENDS "${CMAKE_BINARY_DIR}/${continuation_dir}/ntvdm.exe")

# Add the independently authored startup observer to the unmodified product
# closure, using the same startup-hook sources as the retired producer.
set(startup_objects)
foreach(source IN ITEMS startup_hooks.c minhook/src/buffer.c minhook/src/trampoline.c minhook/src/hde/hde32.c)
  get_filename_component(stem "${source}" NAME_WE)
  set(object "diagnostics/startup-hook/${stem}.obj")
  worker_unit("${object}" "${NTVDM_REPO}/src/lib-win32/process-hook/${source}"
    "${profile_59}")
  list(APPEND startup_objects "${object}")
endforeach()
foreach(spec IN ITEMS "win101-guard|win101_install_write_guard|win101_guard_entry"
    "win31-guard|win31_setup_write_guard|win101_guard_entry"
    "win101-palette|win101_palette_trace|win101_palette_trace_entry"
    "win31-console|win31_console_trace|win31_console_trace_entry")
  string(REPLACE "|" ";" parts "${spec}")
  list(GET parts 0 name)
  list(GET parts 1 source)
  list(GET parts 2 entry)
  set(object "diagnostics/${name}/observer.obj")
  worker_unit("${object}" "${NTVDM_REPO}/test/observation/${source}.c"
    "${profile_59}")
  get_target_property(inputs image_ntvdm_exe WORKER_LINK_INPUTS)
  get_target_property(dependencies image_ntvdm_exe WORKER_LINK_DEPENDS)
  get_property(prefix GLOBAL PROPERTY image_ntvdm_exe_prefix)
  get_property(suffix GLOBAL PROPERTY image_ntvdm_exe_suffix)
  worker_link("diagnostics/${name}/ntvdm.exe"
    "${prefix} /include:_${entry} /implib:diagnostics/${name}/ntvdm.lib" "${suffix}"
    INPUTS "${object}" ${startup_objects} ${inputs} DEPENDS ${dependencies})
  set_property(GLOBAL APPEND PROPERTY diagnostic_images "${CMAKE_BINARY_DIR}/diagnostics/${name}/ntvdm.exe")
endforeach()
get_property(images GLOBAL PROPERTY diagnostic_images)
add_custom_target(worker-diagnostics DEPENDS ${images})

add_custom_target(win101-control-fixtures DEPENDS
  "${CMAKE_BINARY_DIR}/frontend-request-client-test.exe"
  "${CMAKE_BINARY_DIR}/broker-frontend-bootstrap-test.exe"
  "${CMAKE_BINARY_DIR}/native-gui-startup-probe.exe")
