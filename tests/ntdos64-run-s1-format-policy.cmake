if(NOT DEFINED RUNNER OR NOT EXISTS "${RUNNER}")
    message(FATAL_ERROR "RUNNER must name the fixed T235 executable")
endif()
if(NOT DEFINED ENV{ComSpec} OR "$ENV{ComSpec}" STREQUAL "")
    message(FATAL_ERROR "ComSpec is required for the PE handoff check")
endif()

execute_process(COMMAND "${RUNNER}" RESULT_VARIABLE no_args)
if(NOT no_args EQUAL 2)
    message(FATAL_ERROR "no-argument policy exit was ${no_args}, expected 2")
endif()
execute_process(COMMAND "${RUNNER}" --engine first --engine second "$ENV{ComSpec}"
    RESULT_VARIABLE duplicate_engine)
if(NOT duplicate_engine EQUAL 2)
    message(FATAL_ERROR "duplicate engine exit was ${duplicate_engine}, expected 2")
endif()
execute_process(COMMAND "${RUNNER}" "$ENV{ComSpec}" /d /s /c "exit 37"
    RESULT_VARIABLE pe_exit)
if(NOT pe_exit EQUAL 37)
    message(FATAL_ERROR "PE host handoff exit was ${pe_exit}, expected 37")
endif()

set(fake_com "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-s1.com")
set(fake_exe "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-s1.exe")
set(fake_bat "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-s1.bat")
set(fake_pif "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-s1.pif")
# MZ-looking data makes suffix ownership observable: BAT/PIF must not be
# inspected or routed as host executables by the CLI.
file(WRITE "${fake_com}" "MZ")
file(WRITE "${fake_exe}" "MZ")
file(WRITE "${fake_bat}" "MZ")
file(WRITE "${fake_pif}" "MZ")
execute_process(COMMAND "${RUNNER}" "${fake_com}" RESULT_VARIABLE com_exit)
execute_process(COMMAND "${RUNNER}" "${fake_exe}" RESULT_VARIABLE exe_exit)
execute_process(COMMAND "${RUNNER}" "${fake_bat}" RESULT_VARIABLE bat_exit)
execute_process(COMMAND "${RUNNER}" "${fake_pif}" RESULT_VARIABLE pif_exit)
file(REMOVE "${fake_com}" "${fake_exe}" "${fake_bat}" "${fake_pif}")
foreach(result IN ITEMS com_exit exe_exit bat_exit pif_exit)
    if(NOT ${result} EQUAL 3)
        message(FATAL_ERROR "${result} was ${${result}}, expected COMMAND/DOS handoff disposition 3")
    endif()
endforeach()

execute_process(
    COMMAND "${RUNNER}" --exclude-drives c "$ENV{ComSpec}" /d /s /c "exit 0"
    RESULT_VARIABLE removed_drive_option_exit)
if(NOT removed_drive_option_exit EQUAL 2)
    message(FATAL_ERROR "removed drive-option exit was ${removed_drive_option_exit}, expected 2")
endif()
