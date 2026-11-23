if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
set(source "${NTVDM_REPO}/test/observation/win16_clipboard_probe.asm")
set(native "${NTVDM_REPO}/test/observation/wow_clipboard_native_bridge.c")
set(outputs)
foreach(kind normal return exit)
  foreach(dib 0 1)
    set(flags)
    if(kind STREQUAL "return")
      list(APPEND flags -d CALLBACK_RETURN_ONLY)
    elseif(kind STREQUAL "exit")
      list(APPEND flags -d OWNER_TASK_EXIT)
    endif()
    if(dib)
      list(APPEND flags -d CLIPBOARD_DIB)
    endif()
    set(root "${CMAKE_BINARY_DIR}/guest-probes/clipboard/${kind}-${dib}")
    file(MAKE_DIRECTORY "${root}")
    add_custom_command(OUTPUT "${root}/CLIPPRB.EXE" "${root}/clipprobe.lst"
      COMMAND "${NTVDM_NASM}" -f bin -o "${root}/CLIPPRB.EXE" -l "${root}/clipprobe.lst"
        ${flags} "${source}"
      DEPENDS "${source}" "${NTVDM_NASM}" VERBATIM)
    list(APPEND outputs "${root}/CLIPPRB.EXE" "${root}/clipprobe.lst")
  endforeach()
endforeach()
foreach(dib 0 1)
  set(root "${CMAKE_BINARY_DIR}/guest-probes/clipboard/native-${dib}")
  file(MAKE_DIRECTORY "${root}")
  set(flags)
  if(dib)
    list(APPEND flags /DCLIPBOARD_DIB)
  endif()
  add_custom_command(OUTPUT "${root}/GENHOST.DLL" "${root}/GENHOST.lib"
    COMMAND "${CMAKE_C_COMPILER}" /nologo /LD /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS
      ${flags} "/Fo${root}/cliphost.obj" "/Fe${root}/GENHOST.DLL" "${native}"
      /link "/IMPLIB:${root}/GENHOST.lib" user32.lib /EXPORT:ClipboardBridge=_ClipboardBridge@12
    DEPENDS "${native}" "${CMAKE_C_COMPILER}"
    BYPRODUCTS "${root}/cliphost.obj" "${root}/GENHOST.exp"
    WORKING_DIRECTORY "${root}" VERBATIM)
  list(APPEND outputs "${root}/GENHOST.DLL" "${root}/GENHOST.lib")
endforeach()
set(root "${CMAKE_BINARY_DIR}/guest-probes/clipboard")
add_custom_command(OUTPUT "${root}/CLIPRET.EXE"
  COMMAND "${CMAKE_C_COMPILER}" /nologo /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS
    /DCLIPBOARD_RETIREMENT_WITNESS "/Fo${root}/retirement.obj" "/Fe${root}/CLIPRET.EXE"
    "${native}" /link user32.lib
  DEPENDS "${native}" "${CMAKE_C_COMPILER}"
  BYPRODUCTS "${root}/retirement.obj" WORKING_DIRECTORY "${root}" VERBATIM)
add_custom_target(wow-clipboard-probes DEPENDS ${outputs} "${root}/CLIPRET.EXE")
