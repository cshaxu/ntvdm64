ntvdm_source_projection(native_input_geometry.inc "${NTVDM_REPO}/src/ntvwm-exe/presentation.c"
  "DWORD ntvwm_presentation_input(" "DWORD ntvwm_presentation_capture(" "" FALSE)
set(window_diagnostics src/compat/native_console_diagnostics.c
  src/lib-win32/process-observation/console_members.c)
fixture(native-input-geometry CL test/ntvwm-exe/native_input_geometry_test.c)
fixture(native-window-sizing CL test/observation/native_window_sizing_probe.c)
fixture(ntvwm-carrier-guard CL test/observation/ntvwm_carrier_guard_test.c
  src/ntvwm-exe/console_state.c ${window_diagnostics})
fixture(ntvwm-carrier-order CL test/app/ntvwm_carrier_order_test.c)
fixture(ntvwm-close-priority CL test/app/ntvwm_close_priority_test.c src/worker-base/shutdown_close.c)
fixture(ntvwm-console-state CL test/observation/ntvwm_console_state_test.c
  src/ntvwm-exe/console_state.c ${window_diagnostics})
fixture(ntvwm-palette-geometry CL test/app/ntvwm_palette_geometry_test.c ${window_diagnostics})
fixture(native-console-capture CL test/app/native_console_capture_test.c
  src/ntvwm-exe/console_state.c ${window_diagnostics})
set(window_tests native-input-geometry native-window-sizing ntvwm-carrier-guard
  ntvwm-carrier-order ntvwm-close-priority ntvwm-console-state ntvwm-palette-geometry native-console-capture)
foreach(name IN LISTS window_tests)
  set_property(TARGET ${name}_objects PROPERTY COMPILE_OPTIONS /nologo /O2 /W4)
  set_property(TARGET ${name}_objects PROPERTY FIXTURE_LIBRARIES user32.lib)
endforeach()
foreach(name IN ITEMS native-input-geometry native-window-sizing ntvwm-carrier-guard)
  target_compile_options(${name}_objects PRIVATE /WX)
endforeach()
target_compile_options(native-input-geometry_objects PRIVATE /TC /std:c11)
target_include_directories(native-input-geometry_objects PRIVATE "${CMAKE_BINARY_DIR}")
set_property(TARGET native-input-geometry_objects PROPERTY FIXTURE_LIBRARIES /STACK:8388608)
foreach(name IN ITEMS ntvwm-carrier-guard ntvwm-palette-geometry native-console-capture)
  target_compile_definitions(${name}_objects PRIVATE _WIN32_WINNT=0x0A00)
endforeach()
set_property(TARGET ntvwm-console-state_objects PROPERTY COMPILE_OPTIONS /nologo /W4)
set_property(TARGET ntvwm-console-state_objects PROPERTY FIXTURE_LIBRARIES "user32.lib;/incremental:no")
set(font_tool "${NTVDM_REPO}/build/.ninja/build-tools/ntvdm-font.exe")
set(font_rom "${NTVDM_REPO}/src/mvdm-guest/softpc.new/roms/v7vga.rom")
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/native_pc_font.h"
  COMMAND "${font_tool}" "${font_rom}" "${CMAKE_BINARY_DIR}/native_pc_font.h"
  DEPENDS "${font_tool}" "${font_rom}" VERBATIM)
foreach(name IN ITEMS ntvwm-carrier-order ntvwm-close-priority)
  target_sources(${name}_objects PRIVATE "${CMAKE_BINARY_DIR}/native_pc_font.h")
  target_include_directories(${name}_objects PRIVATE "${CMAKE_BINARY_DIR}")
  target_compile_options(${name}_objects PRIVATE /GL /Gy)
  set_property(TARGET ${name}_objects PROPERTY FIXTURE_LIBRARIES "user32.lib;/LTCG")
endforeach()
add_custom_target(window-contracts DEPENDS ${window_tests})
