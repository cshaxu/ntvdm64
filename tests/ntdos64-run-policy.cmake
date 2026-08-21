if(NOT DEFINED RUNNER OR NOT EXISTS "${RUNNER}")
    message(FATAL_ERROR "RUNNER must name ntdos64-run.exe")
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

execute_process(
    COMMAND "${RUNNER}" "$ENV{ComSpec}" /d /s /c "exit 37"
    RESULT_VARIABLE pe_exit)
if(NOT pe_exit EQUAL 37)
    message(FATAL_ERROR "PE handoff exit was ${pe_exit}, expected 37")
endif()

set(fake_dos "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy.com")
file(WRITE "${fake_dos}" "MZ")
execute_process(
    COMMAND "${RUNNER}" --engine "$ENV{ComSpec}" "${fake_dos}"
    RESULT_VARIABLE dos_exit)
if(NOT dos_exit EQUAL 3)
    message(FATAL_ERROR "DOS without BYOB profile exit was ${dos_exit}, expected 3")
endif()

set(byob_root "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy-root")
set(byob_manifest "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy.json")
file(MAKE_DIRECTORY "${byob_root}")
file(WRITE "${byob_root}/NTIO.SYS" "abc")
file(WRITE "${byob_root}/NTDOS.SYS" "abc")
file(WRITE "${byob_root}/COMMAND.COM" "abc")
file(WRITE "${byob_root}/TARGET.COM" "abc")
set(abc_sha256 "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
file(WRITE "${byob_manifest}"
    "{\"schema\":\"ntdos64-byob-profile-v3\",\"profile\":\"nt4-en-us-command-smoke-v3\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"runner-policy-owned\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_target_placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}}}")
execute_process(
    COMMAND "${RUNNER}" --engine "${ENGINE_PROBE}" --byob-profile "${byob_manifest}"
        --byob-root "${byob_root}" --include-drives c,d,e --exclude-drives e "${byob_root}/TARGET.COM" /c smoke
    RESULT_VARIABLE engine_exit)
file(REMOVE "${byob_manifest}" "${byob_root}/NTIO.SYS" "${byob_root}/NTDOS.SYS"
    "${byob_root}/COMMAND.COM" "${byob_root}/TARGET.COM")
file(REMOVE_RECURSE "${byob_root}")
file(REMOVE "${fake_dos}")
if(NOT engine_exit EQUAL 47)
    message(FATAL_ERROR "engine profile/root handoff exit was ${engine_exit}, expected 47")
endif()
execute_process(
    COMMAND "${RUNNER}" --include-drives c,,d "$ENV{ComSpec}" /d /s /c "exit 0"
    RESULT_VARIABLE malformed_drive_exit)
if(NOT malformed_drive_exit EQUAL 2)
    message(FATAL_ERROR "malformed drive-list exit was ${malformed_drive_exit}, expected 2")
endif()

set(fake_bat "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy.bat")
set(fake_pif "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-run-policy.pif")
# Deliberately MZ-looking bytes prove suffix classification is prior to any
# BAT/PIF content inspection or host image classification.
file(WRITE "${fake_bat}" "MZ")
file(WRITE "${fake_pif}" "MZ")
execute_process(COMMAND "${RUNNER}" "${fake_bat}" RESULT_VARIABLE bat_exit)
execute_process(COMMAND "${RUNNER}" "${fake_pif}" RESULT_VARIABLE pif_exit)
file(REMOVE "${fake_bat}" "${fake_pif}")
if(NOT bat_exit EQUAL 3)
    message(FATAL_ERROR "BAT COMMAND-input disposition was ${bat_exit}, expected 3")
endif()
if(NOT pif_exit EQUAL 3)
    message(FATAL_ERROR "PIF COMMAND-input disposition was ${pif_exit}, expected 3")
endif()
