include("${CMAKE_CURRENT_LIST_DIR}/../SourceProjection.cmake")
ntvdm_source_projection(extension/native-extension-owner.inc
  "${NTVDM_REPO}/src/mvdm-host/wow32/wcall16.c"
  "/* DIVERGENCE(MVDM-HOST-DIV-910)" "" "" FALSE)
get_target_property(extension_flags original_wcall16 COMPILE_FLAGS)
foreach(kind IN ITEMS extension mm-body rejection original-frame)
  if(kind STREQUAL "rejection")
    set(sources src/mvdm-host/wow32/wcall16.c
      test/adapter-mvdm-host-out/wow/wow_public_callback_rejection_fixture.c)
  elseif(kind STREQUAL "original-frame")
    set(sources src/mvdm-host/wow32/wcall16.c src/compat/wow_callback_frame_lease.c
      src/compat/wow_user_native_extension.c test/adapter-mvdm-host-out/wow/wow_callback_abort_fixture.c)
  else()
    set(sources src/compat/wow_user_native_extension.c
      test/adapter-mvdm-host-out/wow/wow_native_extension_boundary_fixture.c)
    if(kind STREQUAL "mm-body")
      list(APPEND sources src/mvdm-host/wow32/wmmedia.c)
    endif()
  endif()
  set(providers)
  set(objects)
  foreach(source IN LISTS sources)
    get_filename_component(stem "${source}" NAME_WE)
    set(extra "")
    if(kind STREQUAL "rejection")
      set(extra "/DCallBack16=wow_test_reject_callback")
    elseif(kind STREQUAL "mm-body" AND stem STREQUAL "wmmedia")
      set(extra "/DWMM32CallProc32=wow_original_WMM32CallProc32")
    elseif(kind STREQUAL "mm-body" AND stem STREQUAL "wow_native_extension_boundary_fixture")
      set(extra "/DWOW_EXTENSION_ORIGINAL_MM")
    endif()
    if(source MATCHES "^src/compat/")
      get_target_property(flags obj_compat_${stem}_obj COMPILE_FLAGS)
    elseif(stem STREQUAL "wmmedia")
      get_target_property(flags original_wmmedia COMPILE_FLAGS)
    else()
      set(flags "${extension_flags}")
    endif()
    set(target "extension_${kind}_${stem}")
    wow_unit(${target} "${NTVDM_REPO}/${source}"
      "${flags} /Gy /I \"${CMAKE_BINARY_DIR}/extension\" ${extra}")
    set_property(TARGET ${target} PROPERTY EXCLUDE_FROM_ALL TRUE)
    list(APPEND providers ${target})
    list(APPEND objects "$<TARGET_OBJECTS:${target}>")
  endforeach()
  set(output "${CMAKE_BINARY_DIR}/extension-${kind}.exe")
  add_custom_command(OUTPUT "${output}"
    COMMAND "${CMAKE_LINKER}" /nologo /MACHINE:X86 /OPT:REF
      "/out:${output}" "/map:${output}.map" ${objects}
      kernel32.lib user32.lib ntdll.lib legacy_stdio_definitions.lib
    DEPENDS ${providers} ${objects} COMMAND_EXPAND_LISTS VERBATIM)
  add_custom_target(wow-extension-${kind} DEPENDS "${output}")
endforeach()
add_custom_target(wow-extension-contracts DEPENDS wow-extension-extension
  wow-extension-mm-body wow-extension-rejection wow-extension-original-frame)
