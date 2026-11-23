# Independently authored payloads; never part of a product release.
fixture(native-observation-probe CL test/integration/native_observation_probe.c)
set_property(TARGET native-observation-probe_objects PROPERTY COMPILE_OPTIONS /nologo /W4 /std:c11)
target_compile_definitions(native-observation-probe_objects PRIVATE _WIN32_WINNT=0x0A00)
set_property(TARGET native-observation-probe_objects PROPERTY FIXTURE_LIBRARIES "/subsystem:console;/incremental:no")
set(probes native-observation-probe)
foreach(kind IN ITEMS cui gui)
  fixture(mixed-batch-${kind} CL test/observation/mixed_batch_probe.c)
  set_property(TARGET mixed-batch-${kind}_objects PROPERTY COMPILE_OPTIONS /nologo /W4 /we4013 /we4311 /we4302)
  target_compile_definitions(mixed-batch-${kind}_objects PRIVATE UNICODE _UNICODE)
  if(kind STREQUAL "gui")
    target_compile_definitions(mixed-batch-${kind}_objects PRIVATE BATCH_GUI)
    set_property(TARGET mixed-batch-${kind}_objects PROPERTY FIXTURE_LIBRARIES
      "/subsystem:windows;/entry:wWinMainCRTStartup;shell32.lib")
  else()
    set_property(TARGET mixed-batch-${kind}_objects PROPERTY FIXTURE_LIBRARIES
      "/subsystem:console;/entry:wmainCRTStartup")
  endif()
  list(APPEND probes mixed-batch-${kind})
endforeach()
if(NTVDM_ARCH STREQUAL "x86")
  fixture(native-guest-output CL test/observation/native_guest_output_test.c)
  set_property(TARGET native-guest-output_objects PROPERTY COMPILE_OPTIONS /nologo /W4)
  set_property(TARGET native-guest-output_objects PROPERTY FIXTURE_LIBRARIES
    "/incremental:no;user32.lib")
  list(APPEND probes native-guest-output)
  add_library(unpaired-native-object OBJECT EXCLUDE_FROM_ALL
    "${NTVDM_REPO}/test/observation/run16_unpaired_native_test.c")
  target_compile_options(unpaired-native-object PRIVATE /nologo /W4)
  foreach(kind IN ITEMS cui gui)
    set(options /subsystem:console)
    if(kind STREQUAL "gui")
      set(options /subsystem:windows /entry:wmainCRTStartup)
    endif()
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/unpaired-${kind}.exe"
      COMMAND "${CMAKE_LINKER}" /nologo ${options} "/out:unpaired-${kind}.exe"
        $<TARGET_OBJECTS:unpaired-native-object> kernel32.lib
      DEPENDS unpaired-native-object $<TARGET_OBJECTS:unpaired-native-object>
      COMMAND_EXPAND_LISTS VERBATIM)
    list(APPEND probes "${CMAKE_BINARY_DIR}/unpaired-${kind}.exe")
  endforeach()
else()
  fixture(dos-observation-pif CL test/observation/dos_observation_pif.c)
  set_property(TARGET dos-observation-pif_objects PROPERTY COMPILE_OPTIONS /nologo /W4)
  target_include_directories(dos-observation-pif_objects PRIVATE "${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc")
  add_custom_target(dos-observation-probes DEPENDS guest-observation dos-observation-pif)
  fixture(mixed-batch-pif CL test/observation/mixed_batch_pif.c)
  set_property(TARGET mixed-batch-pif_objects PROPERTY COMPILE_OPTIONS /nologo /W4 /we4013)
  target_include_directories(mixed-batch-pif_objects PRIVATE "${NTVDM_REPO}/src/non-mvdm/public/internal/windows/inc")
  list(APPEND probes mixed-batch-pif)
  fixture(private-mouse-probe CL test/observation/win101_mouse_window_probe.c)
  fixture(console-live-snapshot CL test/observation/console_live_snapshot.c)
  foreach(name IN ITEMS private-mouse-probe console-live-snapshot)
    set_property(TARGET ${name}_objects PROPERTY COMPILE_OPTIONS /nologo /std:c11)
    target_include_directories(${name}_objects PRIVATE "${NTVDM_REPO}/src/lib-nxvm")
    set_property(TARGET ${name}_objects PROPERTY FIXTURE_LIBRARIES user32.lib)
  endforeach()
  add_library(private-mouse-hook-object OBJECT EXCLUDE_FROM_ALL
    "${NTVDM_REPO}/test/observation/private-mouse-input.c")
  target_compile_options(private-mouse-hook-object PRIVATE /nologo /std:c11)
  target_include_directories(private-mouse-hook-object PRIVATE "${NTVDM_REPO}/src"
    "${NTVDM_REPO}/src/lib-nxvm" "${NTVDM_REPO}/src/ntcon-exe")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/mouse-tools")
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/mouse-tools/mouse-hook.dll"
    COMMAND "${CMAKE_LINKER}" /nologo /DLL /EXPORT:MouseInputHook
      /OUT:mouse-tools/mouse-hook.dll /IMPLIB:mouse-tools/mouse-hook.lib
      $<TARGET_OBJECTS:private-mouse-hook-object> user32.lib
    DEPENDS private-mouse-hook-object $<TARGET_OBJECTS:private-mouse-hook-object>
    BYPRODUCTS "${CMAKE_BINARY_DIR}/mouse-tools/mouse-hook.lib" "${CMAKE_BINARY_DIR}/mouse-tools/mouse-hook.exp"
    COMMAND_EXPAND_LISTS VERBATIM)
  foreach(pair IN ITEMS "private-mouse-probe|probe" "console-live-snapshot|console-snapshot")
    string(REPLACE "|" ";" parts "${pair}")
    list(GET parts 0 source)
    list(GET parts 1 name)
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/mouse-tools/${name}.exe"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${source}.exe" "mouse-tools/${name}.exe"
      DEPENDS "${CMAKE_BINARY_DIR}/${source}.exe" VERBATIM)
  endforeach()
  add_custom_target(private-mouse-tools DEPENDS "${CMAKE_BINARY_DIR}/mouse-tools/mouse-hook.dll"
    "${CMAKE_BINARY_DIR}/mouse-tools/probe.exe" "${CMAKE_BINARY_DIR}/mouse-tools/console-snapshot.exe")
  list(APPEND probes private-mouse-tools)
endif()
add_custom_target(native-payload-probes DEPENDS ${probes})
