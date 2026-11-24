if(NOT NTVDM_ARCH STREQUAL "x86")
  return()
endif()
fixture(wow-mine-host CL test/observation/wow_mine_gameplay_probe.c)
set_property(TARGET wow-mine-host_objects PROPERTY COMPILE_OPTIONS
  /nologo /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS)
set_property(TARGET wow-mine-host_objects PROPERTY FIXTURE_LIBRARIES user32.lib gdi32.lib dwmapi.lib)
set(root "${CMAKE_BINARY_DIR}/guest-probes/mine")
file(MAKE_DIRECTORY "${root}")
set(outputs)
foreach(kind IN ITEMS generic fault shell shell-ignore shell-abort shell-ignore-abort child)
  set(stem generic)
  set(flags)
  if(kind STREQUAL "fault")
    list(APPEND flags -DFAULT)
  elseif(NOT kind STREQUAL "generic")
    set(stem shell)
    if(kind MATCHES "ignore")
      list(APPEND flags -DIGNORE_EXCEPTION)
    endif()
    if(kind MATCHES "abort")
      list(APPEND flags -DCALLBACK_ABORT)
    endif()
    if(kind STREQUAL "child")
      list(APPEND flags -DCHILD)
    endif()
  endif()
  set(source "${NTVDM_REPO}/test/observation/win16_${stem}_probe.asm")
  add_custom_command(OUTPUT "${root}/${kind}.exe" "${root}/${kind}.lst"
    COMMAND "${NTVDM_NASM}" -f bin ${flags} -o "${root}/${kind}.exe"
      -l "${root}/${kind}.lst" "${source}"
    DEPENDS "${source}" "${NTVDM_NASM}" VERBATIM)
  list(APPEND outputs "${root}/${kind}.exe" "${root}/${kind}.lst")
endforeach()
foreach(rounds RANGE 1 32)
  set(source "${NTVDM_REPO}/test/observation/win16_common_dialog_probe.asm")
  add_custom_command(OUTPUT "${root}/dialog-${rounds}.exe" "${root}/dialog-${rounds}.lst"
    COMMAND "${NTVDM_NASM}" -f bin "-DCOMMON_DIALOG_ROUNDS=${rounds}"
      -o "${root}/dialog-${rounds}.exe" -l "${root}/dialog-${rounds}.lst" "${source}"
    DEPENDS "${source}" "${NTVDM_NASM}" VERBATIM)
  list(APPEND outputs "${root}/dialog-${rounds}.exe" "${root}/dialog-${rounds}.lst")
endforeach()
function(generic_host directory define exported)
  file(MAKE_DIRECTORY "${directory}")
  set(source "${NTVDM_REPO}/test/observation/wow_generic_callback_probe.c")
  add_custom_command(OUTPUT "${directory}/GENHOST.DLL" "${directory}/GENHOST.lib"
    COMMAND "${CMAKE_C_COMPILER}" /nologo /LD /MT /O2 "/D${define}"
      "/Fo${directory}/generic.obj" "${source}" /link "/OUT:${directory}/GENHOST.DLL"
      "/IMPLIB:${directory}/GENHOST.lib" "/EXPORT:${exported}"
      /EXPORT:ResolveGenericSegment=_ResolveGenericSegment@4
    DEPENDS "${source}" "${CMAKE_C_COMPILER}"
    BYPRODUCTS "${directory}/generic.obj" "${directory}/GENHOST.exp"
    WORKING_DIRECTORY "${directory}" VERBATIM)
endfunction()
generic_host("${root}" TEST_GUEST_EXCEPTION "RunGenericProbe=_RunGenericProbe@12")
add_custom_target(wow-mine-probes DEPENDS wow-mine-host ${outputs}
  "${root}/GENHOST.DLL" "${root}/GENHOST.lib")
foreach(kind IN ITEMS normal mismatch)
  set(define TEST_GENERIC_NORMAL)
  set(exported "RunGenericProbe=_RunGenericProbe@12")
  if(kind STREQUAL "mismatch")
    set(define TEST_GENERIC_CDECL)
    set(exported RunGenericProbe)
  endif()
  set(directory "${CMAKE_BINARY_DIR}/guest-probes/generic-${kind}")
  generic_host("${directory}" "${define}" "${exported}")
  add_custom_target(wow-generic-${kind} DEPENDS "${root}/generic.exe"
    "${directory}/GENHOST.DLL" "${directory}/GENHOST.lib")
endforeach()
