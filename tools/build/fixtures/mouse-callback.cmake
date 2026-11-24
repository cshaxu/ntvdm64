# Current callback implementation, retaining independent nested guest frames.
if(NTVDM_ARCH STREQUAL "x86")
  set(mouse "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/keymouse/mouse_io.c")
  ntvdm_source_projection(mouse-callback-return.inc "${mouse}"
    "LOCAL void mouse_callback_push IFN1(word, value)"
    "LOCAL void do_mouse_function IFN4(word *,m1,word *,m2,word *,m3,word *,m4)"
    "" TRUE)
  ntvdm_source_projection(mouse-callback-enter.inc "${mouse}"
    "LOCAL void jump_to_user_subroutine IFN3(MOUSE_CALL_MASK,condition_mask,word,segment,word,offset)"
    "" "" TRUE)
  fixture(mouse-callback-reentry CL test/observation/fixtures/mouse_callback_reentry.c)
  target_include_directories(mouse-callback-reentry_objects PRIVATE "${CMAKE_BINARY_DIR}")
endif()
