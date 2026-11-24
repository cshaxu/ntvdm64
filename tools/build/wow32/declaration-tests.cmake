add_library(user_declarations OBJECT EXCLUDE_FROM_ALL "${NTVDM_REPO}/test/opennt-host/user_declarations.c")
target_compile_options(user_declarations PRIVATE /we4133)
target_include_directories(user_declarations PRIVATE "${NTVDM_REPO}/src" "${NTVDM_REPO}/src/compat/include")
add_custom_target(wow-user-declarations DEPENDS user_declarations)
ntvdm_source_projection("user-capacity-overflow.c"
  "${NTVDM_REPO}/src/non-mvdm/windows/core/ntuser/inc/user.h"
  "#define FNID_END " "#define FNID_DDE_BIT " "#define FNID_START 0\n" TRUE)
add_library(user_capacity_overflow OBJECT EXCLUDE_FROM_ALL "${CMAKE_BINARY_DIR}/user-capacity-overflow.c")
add_custom_target(wow-user-capacity-overflow DEPENDS user_capacity_overflow)
