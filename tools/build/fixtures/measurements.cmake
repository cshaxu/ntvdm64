fixture(window-render LINKDEFAULT src/lib-nxvm/lib/kvm-window/render.c
  test/observation/measure-window-render.c)
set_property(TARGET window-render_objects PROPERTY COMPILE_OPTIONS
  /nologo /TC /W4 /WX /O2 /std:c11 /D_CRT_SECURE_NO_WARNINGS)
target_include_directories(window-render_objects PRIVATE
  "${NTVDM_REPO}/src/ntcon-exe" "${NTVDM_REPO}/src/lib-nxvm")
foreach(kind IN ITEMS worker-primitives worker-primitives-high-resolution)
  set(publisher src/compat/publication.c)
  if(kind STREQUAL "worker-primitives-high-resolution")
    set(publisher test/observation/publication_high_resolution_measured.c)
  endif()
  fixture(${kind} LINK ${publisher} src/compat/mvdm_softpc_mouse_input.c
    test/observation/measure-worker-primitives.c)
  set_property(TARGET ${kind}_objects PROPERTY COMPILE_OPTIONS /nologo /TC /W4 /WX /O2)
endforeach()
