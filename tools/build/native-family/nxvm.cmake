include("${CMAKE_CURRENT_LIST_DIR}/../NxvmImports.cmake")
set(nxvm_objects)
foreach(path IN LISTS nxvm_sources)
  string(REGEX REPLACE "\\.c$" ".obj" object "obj/frontend/window/${path}")
  family_unit("${object}" "${NTVDM_REPO}/src/${path}"
    "${family_flags} /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/lib-nxvm\" /std:c11 /we4013 /DWIN32_LEAN_AND_MEAN /O2")
  list(APPEND nxvm_objects "${object}")
endforeach()
