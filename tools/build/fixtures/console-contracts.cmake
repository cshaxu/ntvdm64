# Current console publication and controlled-input contracts.
fixture(console-graphics-publication LINK src/compat/publication.c
  src/compat/console_bitmap.c src/compat/v7_pointer.c src/compat/console_graphics.c
  test/ntvdm-exe/win32/console_graphics_publication_test.c)
set_property(TARGET console-graphics-publication_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX /O2)
target_include_directories(console-graphics-publication_objects PRIVATE "${NTVDM_REPO}/src/compat/include")
set_property(TARGET console-graphics-publication_objects PROPERTY FIXTURE_LIBRARIES gdi32.lib)
if(NTVDM_ARCH STREQUAL "x86")
  fixture(console-input-batch LINK test/app/console_input_batch_test.c
    test/app/console_input_batch_stubs.c src/compat/mvdm_keyboard_history.c
    src/compat/mvdm_softpc_mouse_context.c src/compat/console_transport.c
    src/compat/console_services.c src/compat/console_activation.c
    src/lib-win32/pipe/pipe_transfer.c src/lib-win32/handle/duplicate.c
    src/ntvdm-exe/win32/console_client.c src/compat/console_graphics.c
    src/compat/console_bitmap.c src/compat/v7_pointer.c src/compat/console_grid.c
    src/ntvdm-exe/session/session.c src/compat/host_context.c src/compat/guest_memory_lease.c)
  target_include_directories(console-input-batch_objects PRIVATE "${NTVDM_REPO}/src/compat/include")
  set(worker_base "${NTVDM_REPO}/build/.ninja/msvc-x86-release/worker/worker-base.lib")
  set_property(TARGET console-input-batch_objects PROPERTY FIXTURE_LIBRARIES "${worker_base};user32.lib;gdi32.lib")
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/console-input-batch.exe" APPEND DEPENDS "${worker_base}")
endif()
