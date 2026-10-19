# Independently authored acceptance peers only. Never a runtime component or
# a substitute WOW provider. Objects/libraries belong to this architecture's
# existing stable Ninja tree; callers copy only test media to disposable runs.
add_executable(wow_message_external_peer EXCLUDE_FROM_ALL
  "${CMAKE_SOURCE_DIR}/tests/observation/wow_message_peer.c")
target_compile_definitions(wow_message_external_peer PRIVATE
  M4_STRUCTURES MESSAGE_EXTERNAL_PROGRAM _CRT_SECURE_NO_WARNINGS)
target_compile_options(wow_message_external_peer PRIVATE /O2 /W4 /WX)
target_link_libraries(wow_message_external_peer PRIVATE user32)
set_target_properties(wow_message_external_peer PROPERTIES
  OUTPUT_NAME MSGPEER RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
if(NTVDM_ARCHITECTURE STREQUAL "x86")
  foreach(mode IN ITEMS pending nonblock)
    add_library(wow_message_${mode}_host SHARED EXCLUDE_FROM_ALL
      "${CMAKE_SOURCE_DIR}/tests/observation/wow_message_peer.c")
    target_compile_definitions(wow_message_${mode}_host PRIVATE
      M4_STRUCTURES MESSAGE_PENDING_SEND _CRT_SECURE_NO_WARNINGS)
    if(mode STREQUAL "nonblock")
      target_compile_definitions(wow_message_${mode}_host PRIVATE MESSAGE_NONBLOCK_CONTROL)
    endif()
    target_compile_options(wow_message_${mode}_host PRIVATE /O2 /W4 /WX)
    target_link_options(wow_message_${mode}_host PRIVATE
      /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12)
    target_link_libraries(wow_message_${mode}_host PRIVATE user32)
    set_target_properties(wow_message_${mode}_host PROPERTIES
      OUTPUT_NAME MSGHOST_${mode} RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
  endforeach()
  add_library(wow_message_cross_host SHARED EXCLUDE_FROM_ALL
    "${CMAKE_SOURCE_DIR}/tests/observation/wow_message_peer.c")
  target_compile_definitions(wow_message_cross_host PRIVATE
    M4_STRUCTURES MESSAGE_CROSS_WORKER _CRT_SECURE_NO_WARNINGS)
  target_compile_options(wow_message_cross_host PRIVATE /O2 /W4 /WX)
  target_link_options(wow_message_cross_host PRIVATE
    /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12)
  target_link_libraries(wow_message_cross_host PRIVATE user32)
  set_target_properties(wow_message_cross_host PROPERTIES
    OUTPUT_NAME MSGHOSTCROSS RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
  add_library(wow_message_host_structured SHARED EXCLUDE_FROM_ALL
    "${CMAKE_SOURCE_DIR}/tests/observation/wow_message_peer.c")
  target_compile_definitions(wow_message_host_structured PRIVATE
    M4_STRUCTURES _CRT_SECURE_NO_WARNINGS)
  target_compile_options(wow_message_host_structured PRIVATE /O2 /W4 /WX)
  target_link_options(wow_message_host_structured PRIVATE
    /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12)
  target_link_libraries(wow_message_host_structured PRIVATE user32)
  set_target_properties(wow_message_host_structured PROPERTIES
    OUTPUT_NAME MSGHOSTLOCAL RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
  add_library(wow_message_external_host SHARED EXCLUDE_FROM_ALL
    "${CMAKE_SOURCE_DIR}/tests/observation/wow_message_peer.c")
  target_compile_definitions(wow_message_external_host PRIVATE
    M4_STRUCTURES MESSAGE_EXTERNAL_HOST _CRT_SECURE_NO_WARNINGS)
  target_compile_options(wow_message_external_host PRIVATE /O2 /W4 /WX)
  target_link_options(wow_message_external_host PRIVATE
    /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12)
  target_link_libraries(wow_message_external_host PRIVATE user32)
  set_target_properties(wow_message_external_host PROPERTIES
    OUTPUT_NAME MSGHOST RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
endif()
add_executable(wow_pending_native_control EXCLUDE_FROM_ALL
  "${CMAKE_SOURCE_DIR}/tests/observation/wow_pending_send_native_control.c")
target_compile_options(wow_pending_native_control PRIVATE /O2 /W4 /WX)
target_link_libraries(wow_pending_native_control PRIVATE user32)
set_target_properties(wow_pending_native_control PROPERTIES
  OUTPUT_NAME MSGDEATH RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
add_executable(wow_cross_worker_probe EXCLUDE_FROM_ALL
  "${CMAKE_SOURCE_DIR}/tests/observation/wow_cross_worker_probe.c")
target_compile_options(wow_cross_worker_probe PRIVATE /O2 /W4 /WX)
target_link_libraries(wow_cross_worker_probe PRIVATE user32)
set_target_properties(wow_cross_worker_probe PROPERTIES
  OUTPUT_NAME MSGXHOST RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
add_executable(wow_message_observer EXCLUDE_FROM_ALL
  "${CMAKE_SOURCE_DIR}/tests/observation/console_startup_observer.c")
target_include_directories(wow_message_observer PRIVATE "${CMAKE_SOURCE_DIR}/src/ntcon-exe")
target_compile_definitions(wow_message_observer PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(wow_message_observer PRIVATE /O2 /std:c11 /W4 /WX /wd4201)
target_link_libraries(wow_message_observer PRIVATE user32 dbghelp)
set_target_properties(wow_message_observer PROPERTIES
  OUTPUT_NAME console-startup-observer RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/message-probes")
