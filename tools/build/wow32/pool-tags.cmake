# XP pool-tag contract: existing listmung plus original template and batch.
set(pool_tag_source "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/kernel")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ptag.h"
  COMMAND cmd /d /c "${CMAKE_CURRENT_LIST_DIR}/generate-pool-tags.cmd"
    "$<TARGET_FILE:listmung>" "${pool_tag_source}" "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS listmung "${pool_tag_source}/ptag.lst" "${pool_tag_source}/ptag.tpl"
    "${pool_tag_source}/ptag.bat" "${CMAKE_CURRENT_LIST_DIR}/generate-pool-tags.cmd"
  VERBATIM)
add_custom_target(pool-tags-generated DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/ptag.h")
foreach(unit obj_opennt-user_taskman_obj obj_opennt-user_queue_obj obj_opennt-user_cleanup_obj)
  add_dependencies(${unit} pool-tags-generated)
  target_include_directories(${unit} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endforeach()
