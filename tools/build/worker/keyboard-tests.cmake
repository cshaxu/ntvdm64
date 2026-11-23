# Original COMMAND keyboard reader with the same production objects as the
# former standalone fixture; authored observer and PIF helpers share its entry.
worker_unit("obj/tests/keyboard-system-root.obj" "${NTVDM_REPO}/src/compat/system_root.c"
  "/nologo /MT /W4 /DNtvdmGetSystemDirectoryA=keyboard_fixture_system_directory")
foreach(pair IN ITEMS "s45-keyboard-profile-builder|test/observation/s45_keyboard_profile_builder.c"
    "s45-pif-detached-observer|test/observation/pif_detached_observer.c"
    "s45-cmdkeyb-shadow-fixture|test/mvdm-host/s45_cmdkeyb_shadow_fixture.c")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 source)
  set(flags "/nologo /MT /W4")
  set(inputs "obj/tests/${name}.obj")
  set(libraries "kernel32.lib")
  if(name STREQUAL "s45-keyboard-profile-builder")
    string(APPEND flags " /I\"${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc\"")
  elseif(name STREQUAL "s45-pif-detached-observer")
    string(APPEND libraries " user32.lib")
  else()
    # Keep the fixture's instrumented system-directory input; registry package
    # lookup uses the real implementation, with only that overlapping name renamed.
    list(APPEND inputs "obj/command/cmdkeyb.obj" "obj/adapter-softpc/mvdm_shadow_registry.obj"
      "obj/tests/keyboard-system-root.obj")
    string(APPEND libraries " advapi32.lib user32.lib legacy_stdio_definitions.lib")
  endif()
  worker_unit("obj/tests/${name}.obj" "${NTVDM_REPO}/${source}" "${flags}")
  worker_link("${name}.exe" "/nologo /map:$out.map /out:$out" "${libraries}" INPUTS ${inputs})
  list(APPEND keyboard_images "${name}.exe")
endforeach()
worker_group(command-keyboard-shadow ${keyboard_images})
