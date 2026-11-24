# Independent native USER reference contracts, not guest acceptance.
foreach(pair IN ITEMS "query|wow_native_query_contract.c" "queue|wow_native_queue_order_reference.c")
  string(REPLACE "|" ";" parts "${pair}")
  list(GET parts 0 kind)
  list(GET parts 1 source)
  wow_unit(native_${kind}_reference "${NTVDM_REPO}/test/observation/${source}"
    "/nologo /TC /MT /W4 /WX")
  set_property(TARGET native_${kind}_reference PROPERTY EXCLUDE_FROM_ALL TRUE)
  set(output "${CMAKE_BINARY_DIR}/native-${kind}.exe")
  add_custom_command(OUTPUT "${output}"
    COMMAND "${CMAKE_LINKER}" /nologo /machine:x86 "/out:${output}"
      $<TARGET_OBJECTS:native_${kind}_reference> kernel32.lib user32.lib gdi32.lib
    DEPENDS native_${kind}_reference $<TARGET_OBJECTS:native_${kind}_reference>
    COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(wow-native-${kind} DEPENDS "${output}")
endforeach()
# Selected product objects are inspected, never linked into the native control.
add_dependencies(wow-native-query original_wusercli original_wuwind original_walias original_wuser)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/native-query-objects.txt"
  CONTENT "$<TARGET_OBJECTS:original_wusercli>\n$<TARGET_OBJECTS:original_wuwind>\n$<TARGET_OBJECTS:original_walias>\n$<TARGET_OBJECTS:original_wuser>\n")
add_custom_target(wow-native-references DEPENDS wow-native-query wow-native-queue)
