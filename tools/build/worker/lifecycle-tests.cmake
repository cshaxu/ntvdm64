# Shared actual worker inputs for combined native WOW lifecycle fixtures.
set(lifecycle_worker_inputs
  obj/worker/stub.obj obj/adapter-wow-worker/wow_gdi_alias.obj
  obj/compat/wow_user_runtime.obj obj/adapter-wow-worker/wow_user_session_binding.obj
  obj/compat/guest_memory_lease.obj obj/ntvdm/session/session.obj
  obj/host/softpc-resource.res
worker-base.lib
ntvdm-vm-frontend.lib
frontend-base.lib
base-rpc-client-core.lib
base-rpc-client-launch.lib
common-root.lib
common-rpc.lib
lib-rpc.lib
common-native-hook-installer.lib
common-native-hook-context.lib
lib-process-launch.lib
lib-process-hook.lib
lib-process-image.lib
lib-handle.lib
lib-file.lib
lib-crt-compat.lib
compat-worker-thread.lib
lib-thread.lib
lib-pipe.lib
lib-process-observation.lib
common-codec.lib
common-console.lib
opennt-base-client.lib
opennt-base-bindings.lib
common-base-srv-bindings.lib
broker-transport.lib
common-base-srv-transport.lib
worker-shell.lib
worker-command-bindings.lib
original-softpc-host-fixture-roots.lib
original-softpc-support.lib
original-softpc-bios.lib
original-softpc-keymouse.lib
original-softpc-system.lib
original-softpc-disks.lib
original-softpc-video.lib
original-softpc-cvidc.lib
original-softpc-comms.lib
original-softpc-dos.lib
original-mvdm-dem.lib
original-mvdm-command.lib
original-mvdm-xms.lib
original-mvdm-dpmi32.lib
original-mvdm-host-suballoc.lib
original-mvdm-host-oemuni.lib
original-softpc-base-trace.lib
original-opennt-base-vdm.lib
original-opennt-rtl-x86.lib
softpc-fixture-bindings.lib
vdmredir-dll-bindings.lib
softpc-win32-bindings.lib
monitor-bindings.lib
kernel-vdm-printer.lib
debugger-bindings.lib
session.lib
mvdm-softpc-effective-address.lib
softpc-ccpu-vector-defaults.lib
softpc-activity-check.lib
original-ccpu386.lib
)
get_target_property(page_flags unit_obj_compat_mvdm_softpc_wow_page_domain_obj COMPILE_FLAGS)
foreach(leaf IN ITEMS publish_menu publish_menus retire_menu publish_named_class
    retire_class release_class set_expected_windows_version exchange_client_context)
  string(APPEND page_flags " /Dmvdm_softpc_wow_page_domain_${leaf}=wow_fixture_real_page_${leaf}")
endforeach()
worker_unit("obj/tests/lifecycle/page-native.obj"
  "${NTVDM_REPO}/src/compat/mvdm_softpc_wow_page_domain.c" "${page_flags}")
list(APPEND lifecycle_worker_inputs obj/tests/lifecycle/page-native.obj)
worker_paths(lifecycle_inputs ${lifecycle_worker_inputs})
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lifecycle-inputs.txt" CONTENT "$<JOIN:${lifecycle_inputs},\n>\n")
worker_group(wow-lifecycle-inputs ${lifecycle_worker_inputs})
set(inputs ${lifecycle_worker_inputs})
list(REMOVE_ITEM inputs obj/tests/lifecycle/page-native.obj)
list(APPEND inputs obj/compat/mvdm_softpc_wow_page_domain.obj)
worker_paths(paths ${inputs})
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lifecycle-real-inputs.txt" CONTENT "$<JOIN:${paths},\n>\n")
worker_group(wow-lifecycle-real-inputs ${inputs})
foreach(kind IN ITEMS retirement startup)
  set(flags "${page_flags}")
  if(kind STREQUAL "retirement")
    set(leaves handle_metadata)
  else()
    set(leaves client_desktop_ready guest_shared_info)
  endif()
  foreach(leaf IN LISTS leaves)
    string(APPEND flags " /Dmvdm_softpc_wow_page_domain_${leaf}=wow_fixture_real_page_${leaf}")
  endforeach()
  set(object "obj/tests/lifecycle/page-${kind}.obj")
  worker_unit("${object}" "${NTVDM_REPO}/src/compat/mvdm_softpc_wow_page_domain.c" "${flags}")
  set(inputs ${lifecycle_worker_inputs})
  list(REMOVE_ITEM inputs obj/tests/lifecycle/page-native.obj)
  list(APPEND inputs "${object}")
  worker_paths(paths ${inputs})
  file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lifecycle-${kind}-inputs.txt" CONTENT "$<JOIN:${paths},\n>\n")
  worker_group(wow-lifecycle-${kind}-inputs ${inputs})
endforeach()
