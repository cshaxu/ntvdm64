# Original COMMAND keyboard reader with the production keyboard and registry objects.
worker_unit("obj/tests/keyboard-system-root.obj" "${NTVDM_REPO}/src/compat/system_root.c"
  "/nologo /MT /W4 /DNtvdmGetSystemDirectoryA=keyboard_fixture_system_directory")
worker_unit("obj/tests/s45-cmdkeyb-shadow-fixture.obj"
  "${NTVDM_REPO}/test/mvdm-host/s45_cmdkeyb_shadow_fixture.c" "/nologo /MT /W4")

# Keep the instrumented system-directory input while the registry lookup uses its real implementation.
worker_link("s45-cmdkeyb-shadow-fixture.exe" "/nologo /map:$out.map /out:$out"
  "kernel32.lib advapi32.lib user32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/s45-cmdkeyb-shadow-fixture.obj" "obj/command/cmdkeyb.obj"
    "obj/adapter-softpc/mvdm_shadow_registry.obj" "obj/tests/keyboard-system-root.obj")
worker_group(command-keyboard-shadow s45-cmdkeyb-shadow-fixture.exe)
