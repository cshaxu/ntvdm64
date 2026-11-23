# Selected original CCPU declaration/profile checks. Negative targets are
# explicit, expected compiler failures and are never part of product/all.
get_target_property(cpu_profile unit_obj_ccpu_c_neg64_obj COMPILE_FLAGS)
if(NOT cpu_profile MATCHES "/DCPU_40_STYLE" OR NOT cpu_profile MATCHES "/DSPC386"
   OR NOT cpu_profile MATCHES "/DANSI" OR cpu_profile MATCHES "/G[zr]")
  message(FATAL_ERROR "CPU contracts require the selected ANSI/cdecl CCPU profile")
endif()
# Current original NT/SDK headers expose anonymous unions and historical
# declaration/linkage warnings. Keep signature C4113 fatal, as the negative
# pointer tests require, while excluding only these known header diagnostics.
set(abi_checks "/WX /wd4005 /wd4201 /wd4273 /wd4028")
foreach(pair IN ITEMS "ccpu-neg64|ccpu_neg64_test|CCPU_NEG64_WRONG_ABI"
    "ccpu-leaf|ccpu_leaf_declarations|CPU_LEAF_ABI_NEGATIVE")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 name)
  list(GET parts 1 source)
  list(GET parts 2 negative)
  worker_unit("obj/tests/${name}.obj" "${NTVDM_REPO}/test/mvdm-host/${source}.c"
    "${cpu_profile} /O2 ${abi_checks}")
  worker_unit("obj/tests/${name}-negative.obj" "${NTVDM_REPO}/test/mvdm-host/${source}.c"
    "${cpu_profile} /O2 ${abi_checks} /D${negative}=1")
  worker_group("${name}-negative" "obj/tests/${name}-negative.obj")
endforeach()
worker_link("ccpu-neg64.exe" "/nologo /machine:x86 /subsystem:console /out:$out /map:$out.map"
  "kernel32.lib legacy_stdio_definitions.lib"
  INPUTS "obj/tests/ccpu-neg64.obj" "obj/ccpu/c_neg64.obj")
worker_group(ccpu-neg64 "ccpu-neg64.exe")
worker_group(ccpu-leaf "obj/tests/ccpu-leaf.obj")

set(notification_source "${NTVDM_REPO}/src/mvdm-host/softpc.new/base/ccpu386/c_main.c")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${notification_source}")
file(READ "${notification_source}" body)
string(REPLACE "\r\n" "\n" body "${body}")
set(projection "")
foreach(name IN ITEMS CPU_HW_INT_MASK CPU_SIGIO_EXCEPTION_MASK CPU_SAD_EXCEPTION_MASK
    CPU_RESET_EXCEPTION_MASK CPU_SIGALRM_EXCEPTION_MASK)
  string(REGEX MATCH "#define[ \t]+${name}[ \t]+\\(1 << [0-9]+\\)[ \t]*" part "${body}")
  if(NOT part)
    message(FATAL_ERROR "Missing notification mask: ${name}")
  endif()
  string(APPEND projection "${part}\n")
endforeach()
foreach(name IN ITEMS clear_any_thingies c_cpu_event_snapshot c_cpu_raise_event c_cpu_take_event)
  string(REGEX MATCH "(LOCAL|GLOBAL) (VOID|IUM32|IBOOL) ${name} IFN[01]\\([^\n]*\\)\n\\{" start "${body}")
  if(NOT start)
    message(FATAL_ERROR "Missing notification function: ${name}")
  endif()
  string(FIND "${body}" "${start}" begin)
  string(FIND "${body}" "${start}" last REVERSE)
  if(NOT begin EQUAL last)
    message(FATAL_ERROR "Ambiguous notification function: ${name}")
  endif()
  string(SUBSTRING "${body}" ${begin} -1 tail)
  string(FIND "${tail}" "\n}" end)
  if(end LESS 0)
    message(FATAL_ERROR "Missing notification function end: ${name}")
  endif()
  math(EXPR end "${end}+2")
  string(SUBSTRING "${tail}" 0 ${end} part)
  string(APPEND projection "${part}\n")
endforeach()
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/notification_body.inc" CONTENT "${projection}" @ONLY)
worker_unit("obj/tests/ccpu-notification.obj"
  "${NTVDM_REPO}/test/mvdm-host/ccpu_notification_fixture.c"
  "/nologo /W4 /MT /TC /I\"${CMAKE_BINARY_DIR}\"" "notification_body.inc")
worker_link("ccpu-notification.exe" "/nologo /out:$out" "kernel32.lib"
  INPUTS "obj/tests/ccpu-notification.obj")
worker_group(ccpu-notification "ccpu-notification.exe")
worker_unit("obj/tests/native-int-prefix.obj"
  "${NTVDM_REPO}/test/mvdm-host/native_int_prefix_control.c" "/nologo /W3 /MT /O2")
worker_link("native-int-prefix.exe" "/nologo /out:$out" "kernel32.lib"
  INPUTS "obj/tests/native-int-prefix.obj")
worker_group(native-int-prefix "native-int-prefix.exe")
worker_group(cpu-contracts "ccpu-neg64.exe" "obj/tests/ccpu-leaf.obj"
  "ccpu-notification.exe" "native-int-prefix.exe")
