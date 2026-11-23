# Authored message witnesses. The mask selects the existing test features;
# runtime combination validation remains with the integration runner.
if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
set(NTVDM_MESSAGE_MASK "0" CACHE STRING "Message witness feature mask")
if(NOT NTVDM_MESSAGE_MASK MATCHES "^(0|[1-9][0-9]*)$" OR NTVDM_MESSAGE_MASK GREATER 8191)
  message(FATAL_ERROR "Message mask must be 0..8191")
endif()
set(features EXTENSION_WITNESS THREAD_CLIENT_VIEW M4_STRUCTURES M4_REPLY_PAYLOAD
  M4_TIMEOUT_PAYLOAD M4_COPYDATA_PROC32 CROSS_WORKER PENDING_SEND CONSTRUCTION
  CLASSIC_DDE CLASSIC_DDE_TASK_LOSS SYNC_DDE_DIAGNOSTIC MESSAGE_TRANSACTION)
set(guest_flags)
set(native_flags)
set(bit 1)
foreach(feature IN LISTS features)
  math(EXPR selected "${NTVDM_MESSAGE_MASK} & ${bit}")
  math(EXPR bit "${bit} * 2")
  if(NOT selected)
    continue()
  endif()
  if(NOT feature STREQUAL "MESSAGE_TRANSACTION")
    list(APPEND guest_flags "-D${feature}")
  endif()
  if(feature STREQUAL "CONSTRUCTION")
    list(APPEND native_flags /DMESSAGE_CONSTRUCTION)
  elseif(feature MATCHES "^(M4_.*|CLASSIC_DDE|CLASSIC_DDE_TASK_LOSS|MESSAGE_TRANSACTION)$")
    list(APPEND native_flags "/D${feature}")
  endif()
endforeach()
set(root "${CMAKE_BINARY_DIR}/guest-probes/message/${NTVDM_MESSAGE_MASK}")
file(MAKE_DIRECTORY "${root}")
set(source "${NTVDM_REPO}/test/observation/win16_message_probe.asm")
set(outputs)
foreach(role SEND RECV)
  set(flags ${guest_flags})
  if(role STREQUAL "RECV")
    list(PREPEND flags -DRECEIVER)
  else()
    math(EXPR transaction "${NTVDM_MESSAGE_MASK} & 4096")
    if(transaction)
      list(APPEND flags -DMESSAGE_TRANSACTION)
    endif()
  endif()
  add_custom_command(OUTPUT "${root}/MSG${role}.EXE" "${root}/${role}.lst"
    COMMAND "${NTVDM_NASM}" -f bin -o "${root}/MSG${role}.EXE"
      -l "${root}/${role}.lst" ${flags} "${source}"
    DEPENDS "${source}" "${NTVDM_NASM}" VERBATIM)
  list(APPEND outputs "${root}/MSG${role}.EXE" "${root}/${role}.lst")
endforeach()
set(peer "${NTVDM_REPO}/test/observation/wow_message_peer.c")
add_custom_command(OUTPUT "${root}/MSGHOST.DLL" "${root}/MSGHOST.lib"
  COMMAND "${CMAKE_C_COMPILER}" /nologo /LD /MT /O2 /W4 /WX
    /D_CRT_SECURE_NO_WARNINGS ${native_flags} "/Fo${root}/peer.obj" "${peer}"
    /link "/OUT:${root}/MSGHOST.DLL" "/IMPLIB:${root}/MSGHOST.lib"
    /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12 user32.lib
  DEPENDS "${peer}" "${NTVDM_REPO}/test/observation/wow_reply_payload_lifetime.h"
    "${CMAKE_C_COMPILER}"
  BYPRODUCTS "${root}/peer.obj" "${root}/MSGHOST.exp"
  WORKING_DIRECTORY "${root}" VERBATIM)
add_custom_target(win16-message-guest DEPENDS ${outputs})
add_custom_target(win16-message DEPENDS win16-message-guest "${root}/MSGHOST.DLL" "${root}/MSGHOST.lib")
