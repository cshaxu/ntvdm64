if(NOT DEFINED RUNNER OR NOT EXISTS "${RUNNER}")
    message(FATAL_ERROR "RUNNER must name ntdos64-run.exe")
endif()
if(NOT DEFINED SHIM OR NOT EXISTS "${SHIM}")
    message(FATAL_ERROR "SHIM must name ntdos64-bochs-engine.exe")
endif()
if(NOT DEFINED FAKE_BOCHS OR NOT EXISTS "${FAKE_BOCHS}")
    message(FATAL_ERROR "FAKE_BOCHS must name bochs-engine-probe.exe")
endif()

set(root "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-bochs-engine-policy-root")
set(profile "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-bochs-engine-policy.json")
set(bundle "${CMAKE_CURRENT_BINARY_DIR}/ntdos64-bochs-engine-policy-bundle")
file(MAKE_DIRECTORY "${root}" "${bundle}/ROM")
file(WRITE "${root}/NTIO.SYS" "abc")
file(WRITE "${root}/NTDOS.SYS" "abc")
file(WRITE "${root}/COMMAND.COM" "abc")
file(WRITE "${root}/TARGET.COM" "abc")
execute_process(
    COMMAND powershell.exe -NoProfile -NonInteractive -Command
        "[IO.File]::WriteAllBytes('${root}/QUIT.COM', [byte[]](0xC4,0xC4,0xFE))"
    RESULT_VARIABLE quit_write_result)
if(NOT quit_write_result EQUAL 0)
    message(FATAL_ERROR "could not create fixed QUIT.COM fixture")
endif()
set(abc_sha256 "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
set(quit_sha256 "06a37dff559df7325de8b003f4df53c188f733e0ca312aad961c34dae48d7b83")
file(WRITE "${profile}"
    "{\"schema\":\"ntdos64-byob-profile-v5\",\"profile\":\"nt4-en-us-command-smoke-v5\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"bochs-engine-policy-owned\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"${abc_sha256}\",\"version\":null},{\"role\":\"terminal-quit\",\"file_name\":\"QUIT.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"${quit_sha256}\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},\"guest_declared_targets\":[{\"role\":\"target\",\"placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2}},{\"role\":\"terminal-quit\",\"placement\":{\"path\":\"\\\\QUIT.COM\",\"drive_index\":2}}],\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"target\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"terminal-quit\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"config\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"autoexec\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1}}}")
file(COPY_FILE "${FAKE_BOCHS}" "${bundle}/ntdos64-bochs.exe")
file(WRITE "${bundle}/ROM/BIOS-bochs-latest" "fake-bios\n")
file(WRITE "${bundle}/ROM/VGABIOS-lgpl-latest" "fake-vga\n")
file(WRITE "${bundle}/ROM/ntdos64-post-handoff.rom" "fake-option-rom\n")
file(SHA256 "${bundle}/ntdos64-bochs.exe" engine_hash)
file(SHA256 "${bundle}/ROM/BIOS-bochs-latest" bios_hash)
file(SHA256 "${bundle}/ROM/VGABIOS-lgpl-latest" vga_hash)
file(SHA256 "${bundle}/ROM/ntdos64-post-handoff.rom" optionrom_hash)
string(TOUPPER "${engine_hash}" engine_hash)
string(TOUPPER "${bios_hash}" bios_hash)
string(TOUPPER "${vga_hash}" vga_hash)
string(TOUPPER "${optionrom_hash}" optionrom_hash)
file(WRITE "${bundle}/ntdos64-bochs-engine-bundle-v1.txt"
    "schema=ntdos64-bochs-engine-bundle-v1\nengine-sha256=${engine_hash}\nbios-sha256=${bios_hash}\nvga-sha256=${vga_hash}\noptionrom-sha256=${optionrom_hash}\n")

# The shim has a fixed ABI and fixed bundle members; neither a caller directive
# nor a partial bundle may reach the native parser.
execute_process(
    COMMAND "${SHIM}" --byob-profile "${profile}" --byob-root "${root}"
        --bochs "${bundle}/ntdos64-bochs.exe" --unexpected
    RESULT_VARIABLE directive_result)
if(NOT directive_result EQUAL 2)
    message(FATAL_ERROR "shim directive rejection exit was ${directive_result}, expected 2")
endif()
file(REMOVE "${bundle}/ROM/VGABIOS-lgpl-latest")
execute_process(
    COMMAND "${RUNNER}" --engine "${SHIM}" --bochs "${bundle}/ntdos64-bochs.exe"
        --byob-profile "${profile}" --byob-root "${root}" "${root}/TARGET.COM"
    RESULT_VARIABLE missing_result)
if(NOT missing_result EQUAL 3)
    message(FATAL_ERROR "missing fixed ROM exit was ${missing_result}, expected 3")
endif()
file(WRITE "${bundle}/ROM/VGABIOS-lgpl-latest" "fake-vga\n")
file(SHA256 "${bundle}/ROM/VGABIOS-lgpl-latest" vga_hash)
string(TOUPPER "${vga_hash}" vga_hash)
file(WRITE "${bundle}/ntdos64-bochs-engine-bundle-v1.txt"
    "schema=ntdos64-bochs-engine-bundle-v1\nengine-sha256=${engine_hash}\nbios-sha256=${bios_hash}\nvga-sha256=${vga_hash}\noptionrom-sha256=${optionrom_hash}\n")

execute_process(
    COMMAND "${RUNNER}" --engine "${SHIM}" --bochs "${bundle}/ntdos64-bochs.exe"
        --byob-profile "${profile}" --byob-root "${root}" "${root}/TARGET.COM"
    RESULT_VARIABLE result)
file(REMOVE "${profile}")
file(REMOVE_RECURSE "${root}" "${bundle}")
if(NOT result EQUAL 47)
    message(FATAL_ERROR "fixed Bochs bundle handoff exit was ${result}, expected 47")
endif()
