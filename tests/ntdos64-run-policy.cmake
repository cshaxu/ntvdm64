if(NOT DEFINED RUNNER OR NOT EXISTS "${RUNNER}")
    message(FATAL_ERROR "RUNNER must name the fixed T235 executable")
endif()
if(NOT DEFINED ENGINE_PROBE OR NOT EXISTS "${ENGINE_PROBE}")
    message(FATAL_ERROR "ENGINE_PROBE must name runner-engine-probe.exe")
endif()
if(NOT DEFINED ENV{ComSpec} OR "$ENV{ComSpec}" STREQUAL "")
    message(FATAL_ERROR "ComSpec is required for the PE handoff check")
endif()

execute_process(COMMAND "${RUNNER}" RESULT_VARIABLE no_args)
if(NOT no_args EQUAL 2)
    message(FATAL_ERROR "no-argument policy exit was ${no_args}, expected 2")
endif()
execute_process(COMMAND "${RUNNER}" "$ENV{ComSpec}" /d /s /c "exit 37" RESULT_VARIABLE pe_exit)
if(NOT pe_exit EQUAL 37)
    message(FATAL_ERROR "PE handoff exit was ${pe_exit}, expected 37")
endif()

set(fake_dos "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy.com")
set(fake_pif "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy.pif")
file(WRITE "${fake_dos}" "MZ")
file(WRITE "${fake_pif}" "MZ")
execute_process(
    COMMAND "${RUNNER}" --engine "${ENGINE_PROBE}"
        "${fake_dos}" /c smoke
    RESULT_VARIABLE engine_exit)
execute_process(
    COMMAND "${RUNNER}" --engine "${ENGINE_PROBE}"
        "${fake_pif}"
    RESULT_VARIABLE pif_engine_exit)
file(REMOVE "${fake_dos}" "${fake_pif}")
if(NOT engine_exit EQUAL 47)
    message(FATAL_ERROR "engine fixed-bundle handoff exit was ${engine_exit}, expected 47")
endif()
if(NOT pif_engine_exit EQUAL 47)
    message(FATAL_ERROR "PIF fixed-bundle handoff exit was ${pif_engine_exit}, expected 47")
endif()

execute_process(
    COMMAND "${RUNNER}" --include-drives c "$ENV{ComSpec}" /d /s /c "exit 0"
    RESULT_VARIABLE removed_drive_option_exit)
if(NOT removed_drive_option_exit EQUAL 2)
    message(FATAL_ERROR "removed drive-option exit was ${removed_drive_option_exit}, expected 2")
endif()
