# Independently authored diagnostic driver, not original guest firmware.
find_program(NTVDM_WLINK NAMES wlink.exe
  HINTS "${NTVDM_REPO}/build/.tools/openwatcom/binnt64")
if(NTVDM_WLINK)
  get_filename_component(linker_directory "${NTVDM_WLINK}" DIRECTORY)
  set(linker_dependencies "${NTVDM_WLINK}")
  foreach(name IN ITEMS wlinkd.dll wlink.lnk)
    if(EXISTS "${linker_directory}/${name}")
      list(APPEND linker_dependencies "${linker_directory}/${name}")
    endif()
  endforeach()
  set(directory "${CMAKE_BINARY_DIR}/interrupt-observer")
  set(source "${NTVDM_REPO}/test/integration/ntvdm64-386/interrupt-observer.asm")
  file(MAKE_DIRECTORY "${directory}")
  file(CONFIGURE OUTPUT "${directory}/observer.lnk" CONTENT
    "format windows vxd static\noption quiet\noption map=observer.map\nname observer.386\nfile observer.obj\nsegment _LTEXT shared resident\nsegment _LDATA shared resident\nexport observe_ddb.1\n" @ONLY)
  add_custom_command(OUTPUT "${directory}/observer.obj"
    COMMAND "${NTVDM_NASM}" -f obj "${source}" -o observer.obj -l observer.lst
    DEPENDS "${source}" "${NTVDM_NASM}" BYPRODUCTS "${directory}/observer.lst"
    WORKING_DIRECTORY "${directory}" VERBATIM)
  add_custom_command(OUTPUT "${directory}/observer.386"
    COMMAND "${NTVDM_WLINK}" @observer.lnk
    DEPENDS "${directory}/observer.obj" "${directory}/observer.lnk" ${linker_dependencies}
    BYPRODUCTS "${directory}/observer.map" WORKING_DIRECTORY "${directory}" VERBATIM)
  add_custom_target(interrupt-observer DEPENDS "${directory}/observer.386")
else()
  # Keep unrelated fixtures usable, but never silently skip a requested driver.
  add_custom_target(interrupt-observer
    COMMAND "${CMAKE_COMMAND}" -E echo "interrupt-observer requires OpenWatcom wlink.exe"
    COMMAND "${CMAKE_COMMAND}" -E false VERBATIM)
endif()
