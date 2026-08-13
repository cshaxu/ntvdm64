#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "bx_ntvdm_adapter_runtime.h"
#include "bx_ntvdm_host_session.h"
#include "byob_profile.h"
#include "byob_identity.h"

static const char sha256_abc[] =
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

static int write_file(const wchar_t *path, const char *bytes)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);
    DWORD written = 0u;
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, bytes, (DWORD)strlen(bytes), &written, 0) ||
        written != strlen(bytes)) { CloseHandle(file); return 0; }
    CloseHandle(file);
    return 1;
}

static int run_t97_path_search(void)
{
    wchar_t root[MAX_PATH], ntio[MAX_PATH], ntdos[MAX_PATH], command[MAX_PATH], target[MAX_PATH], profile[MAX_PATH];
    char json[4096];
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x7c00u };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_read_action_v1 dta_action;
    bx_ntvdm_guest_gather_read_action_v1 gather_action;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_multi_write_transaction_v1 multi_write;
    bx_ntvdm_bulk_result_transaction_v1 bulk;
    uint8_t *read_bytes = 0, *gather_bytes = 0;
    const uint8_t *payload = 0;
    uint64_t payload_bytes = 0u;
    int failed = 0;
    if (GetTempPathW(MAX_PATH, root) == 0 || GetTempFileNameW(root, L"n64", 0u, root) == 0 ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0)) return 1;
    swprintf(ntio, MAX_PATH, L"%ls\\NTIO.SYS", root); swprintf(ntdos, MAX_PATH, L"%ls\\NTDOS.SYS", root);
    swprintf(command, MAX_PATH, L"%ls\\COMMAND.COM", root); swprintf(target, MAX_PATH, L"%ls\\TARGET.COM", root);
    swprintf(profile, MAX_PATH, L"%ls\\profile.json", root);
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v4\",\"profile\":\"nt4-en-us-command-smoke-v4\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"t97-runtime\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_target_placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":33},\"target\":{\"attributes\":32,\"dos_time\":2,\"dos_date\":34},\"config\":{\"attributes\":32,\"dos_time\":3,\"dos_date\":35},\"autoexec\":{\"attributes\":32,\"dos_time\":4,\"dos_date\":36}}}", sha256_abc,sha256_abc,sha256_abc,sha256_abc);
    failed |= !write_file(ntio,"abc") || !write_file(ntdos,"abc") || !write_file(command,"abc") ||
        !write_file(target,"abc") || !write_file(profile,json) ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_KIND",L"com") ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_TAIL",L"");
    bx_ntvdm_adapter_runtime_v1_reset();
    failed |= !bx_ntvdm_adapter_runtime_v1_install(profile,root) ||
        bx_ntvdm_adapter_runtime_v1_install_diagnostic() !=
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_NONE;
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_KIND",0); SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_TAIL",0);
    bx_ntvdm_cpu_state_v1_initialize(&state,BX_NTVDM_CPU_EXECUTION_REAL);
    state.ds=0x100u; state.esi=0u; state.eax=0x100u; state.edx=0x200u; state.ecx=0x300u;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x1bu},4u);
    failed |= !bx_ntvdm_adapter_runtime_v3_dispatch(&event,&state,&window,&dta_action) ||
        !bx_ntvdm_adapter_runtime_v3_take_pending_guest_read(&event,&state,&dta_action,&read_bytes) || read_bytes==0;
    if (read_bytes) { read_bytes[0]=0x40u; read_bytes[1]=0u; }
    failed |= !bx_ntvdm_adapter_runtime_v3_complete_guest_read(&event,&state,&dta_action,read_bytes,2u,&result);
    state.edx=0x20u; state.ecx=0u;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x09u},4u);
    failed |= !bx_ntvdm_adapter_runtime_v4_dispatch(&event,&state,&window,&gather_action) ||
        gather_action.disposition!=BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ || gather_action.total_bytes!=134u ||
        !bx_ntvdm_adapter_runtime_v4_take_pending_guest_gather_read(&event,&state,&gather_action,&gather_bytes) || gather_bytes==0;
    if (gather_bytes) { memset(gather_bytes,0,134u); memcpy(gather_bytes,"C:\\COMMAND.COM",15u); gather_bytes[130]=0x10u; gather_bytes[131]=0x01u; gather_bytes[132]=0x34u; gather_bytes[133]=0x12u; }
    failed |= !bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read(&event,&state,&gather_action,gather_bytes,134u,&result) ||
        result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME || !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event,&state,&multi_write,&payload,&payload_bytes) ||
        payload_bytes!=30u || memcmp(payload+17u,"COMMAND.COM",11u)!=0;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x0bu},4u);
    failed |= !bx_ntvdm_adapter_runtime_v4_dispatch(&event,&state,&window,&gather_action) || gather_action.total_bytes!=45u ||
        !bx_ntvdm_adapter_runtime_v4_take_pending_guest_gather_read(&event,&state,&gather_action,&gather_bytes) || gather_bytes==0;
    if (gather_bytes) { memcpy(gather_bytes,payload,43u); gather_bytes[43]=0x34u; gather_bytes[44]=0x12u; }
    failed |= !bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read(&event,&state,&gather_action,gather_bytes,45u,&result) ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event,&state,&multi_write,&payload,&payload_bytes) || payload_bytes!=8u ||
        multi_write.result.cpu_delta.gpr16_values[0]!=0x12u;
    state.ds=0x100u; state.edx=0x60u;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x54u,0x0cu},4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event,&state,&window,&result) ||
        result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event,&state,&multi_write,&payload,&payload_bytes) ||
        multi_write.writes.write_count!=1u || payload_bytes!=14u ||
        memcmp(payload,"C:\\CONFIG.SYS",14u)!=0;
    state.esi=0x80u; state.eax=0u; state.ebx=0u;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x12u},4u);
    failed |= !bx_ntvdm_adapter_runtime_v3_dispatch(&event,&state,&window,&dta_action) ||
        !bx_ntvdm_adapter_runtime_v3_take_pending_guest_read(&event,&state,&dta_action,&read_bytes) || read_bytes==0;
    if (read_bytes) { memset(read_bytes,0,128u); memcpy(read_bytes,"C:\\CONFIG.SYS",14u); }
    failed |= !bx_ntvdm_adapter_runtime_v3_complete_guest_read(&event,&state,&dta_action,read_bytes,128u,&result) ||
        result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values!=0u;
    state.eax=result.cpu_delta.gpr16_values[0]; state.ebp=result.cpu_delta.gpr16_values[5];
    state.ecx=20u; state.edx=0x90u; state.eflags=0x40u;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x16u},4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event,&state,&window,&result) ||
        result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_bulk_result(&event,&state,&bulk,&payload,&payload_bytes) ||
        payload_bytes!=13u || memcmp(payload,"REM NTVDM64\r\n",13u)!=0;
    /* `50:42` is a distinct source-derived fast-read provider. ZF clear
       requests the caller-supplied BX:SI position, proving this is not
       accidentally routed through the ordinary-read continuation. */
    state.ebx=0u; state.esi=0u; state.ecx=20u; state.edx=0x90u; state.eflags=0u;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x42u},4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event,&state,&window,&result) ||
        result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME || result.cpu_delta.gpr16_write_mask!=1u ||
        result.cpu_delta.gpr16_values[0]!=13u || result.eflags_values!=0u ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_bulk_result(&event,&state,&bulk,&payload,&payload_bytes) ||
        payload_bytes!=13u || bulk.guest_physical_address!=0x1090u ||
        memcmp(payload,"REM NTVDM64\r\n",13u)!=0;
    bx_ntvdm_instruction_window_v1_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x02u},4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event,&state,&window,&result) ||
        result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values!=0u;
    bx_ntvdm_adapter_runtime_v1_reset(); DeleteFileW(profile); DeleteFileW(target); DeleteFileW(command); DeleteFileW(ntdos); DeleteFileW(ntio); RemoveDirectoryW(root);
    return failed;
}

static int run_t181_v5_install(void)
{
    wchar_t root[MAX_PATH], ntio[MAX_PATH], ntdos[MAX_PATH], command[MAX_PATH], target[MAX_PATH], quit[MAX_PATH], profile[MAX_PATH];
    char json[4096];
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x732u };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_multi_write_transaction_v1 multi_write;
    bx_ntvdm_adapter_runtime_diagnostic_state_v1 diagnostic;
    const uint8_t *payload = 0;
    uint64_t payload_bytes = 0u;
    int failed = 0;
    if (GetTempPathW(MAX_PATH, root) == 0 || GetTempFileNameW(root, L"n64", 0u, root) == 0 ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0)) return 1;
    swprintf(ntio,MAX_PATH,L"%ls\\NTIO.SYS",root); swprintf(ntdos,MAX_PATH,L"%ls\\NTDOS.SYS",root);
    swprintf(command,MAX_PATH,L"%ls\\COMMAND.COM",root); swprintf(target,MAX_PATH,L"%ls\\TARGET.COM",root);
    swprintf(quit,MAX_PATH,L"%ls\\QUIT.COM",root); swprintf(profile,MAX_PATH,L"%ls\\profile.json",root);
    snprintf(json,sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v5\",\"profile\":\"nt4-en-us-command-smoke-v5\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"t181-install\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"terminal-quit\",\"file_name\":\"QUIT.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"06a37dff559df7325de8b003f4df53c188f733e0ca312aad961c34dae48d7b83\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},\"guest_declared_targets\":[{\"role\":\"target\",\"placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2}},{\"role\":\"terminal-quit\",\"placement\":{\"path\":\"\\\\QUIT.COM\",\"drive_index\":2}}],\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"target\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"terminal-quit\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"config\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"autoexec\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1}}}",sha256_abc,sha256_abc,sha256_abc,sha256_abc);
    failed |= !write_file(ntio,"abc") || !write_file(ntdos,"abc") || !write_file(command,"abc") ||
        !write_file(target,"abc") || !write_file(quit,"\xc4\xc4\xfe") || !write_file(profile,json) ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_PLAN",L"2,2,c,00");
    bx_ntvdm_adapter_runtime_v1_reset();
    failed |= !bx_ntvdm_adapter_runtime_v1_install(profile,root) ||
        bx_ntvdm_adapter_runtime_v1_install_diagnostic()!=BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_NONE;
    failed |= !bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state(&diagnostic) ||
        diagnostic.version != BX_NTVDM_ADAPTER_RUNTIME_DIAGNOSTIC_STATE_V1_VERSION ||
        diagnostic.installed != 1u || diagnostic.has_boot_namespace_provider != 1u ||
        diagnostic.pending_kind != BX_NTVDM_HOST_PENDING_NONE;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.ds = 0x8dc8u;
    state.edx = 0x33d9u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x54u, 0x0cu }, 4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x736u ||
        !bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state(&diagnostic) ||
        diagnostic.installed != 1u || diagnostic.has_boot_namespace_provider != 1u ||
        diagnostic.pending_kind != BX_NTVDM_HOST_PENDING_MULTI_WRITE ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes) ||
        multi_write.writes.write_count != 1u ||
        multi_write.writes.writes[0].guest_physical_address != 0x91059u ||
        payload_bytes != 14u || memcmp(payload, "C:\\CONFIG.SYS", 14u) != 0;
    bx_ntvdm_adapter_runtime_v1_reset();
    failed |= !write_file(quit,"bad") || bx_ntvdm_adapter_runtime_v1_install(profile,root) != 0 ||
        bx_ntvdm_adapter_runtime_v1_install_diagnostic()!=BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_PROFILE_OR_IMAGE;
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_PLAN",0); bx_ntvdm_adapter_runtime_v1_reset();
    DeleteFileW(profile); DeleteFileW(quit); DeleteFileW(target); DeleteFileW(command); DeleteFileW(ntdos); DeleteFileW(ntio); RemoveDirectoryW(root);
    return failed;
}

int main(int argc, char **argv)
{
    wchar_t root[MAX_PATH], ntio[MAX_PATH], ntdos[MAX_PATH], command[MAX_PATH], target[MAX_PATH], profile[MAX_PATH], evidence[MAX_PATH];
    char json[4096];
    const uint8_t marker[] = { 0xc4u, 0xc4u, 0x50u, 0x11u, 0xf4u };
    const uint8_t ud2[] = { 0x0fu, 0x0bu };
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_cpu_result_v2 read_result;
    bx_ntvdm_guest_gather_read_action_v1 gather_action;
    bx_ntvdm_host_transaction_v1 transaction;
    bx_ntvdm_multi_write_transaction_v1 multi_write;
    bx_ntvdm_observation_transaction_v1 observation;
    bx_ntvdm_startup_snapshot_transaction_v1 snapshot;
    bx_ntvdm_startup_plan_v1 startup_plan;
    bx_ntvdm_host_drive_snapshot_v1 drive_snapshot, drive_snapshot_copy;
    const uint8_t *payload = 0;
    uint8_t observation_bytes[4] = { 0u, 0u, 0u, 0u };
    uint8_t *observation_output = 0;
    uint8_t *snapshot_output = 0;
    uint8_t *gather_output = 0;
    uint64_t payload_bytes = 0u, resume_rip = 0u;
    uint64_t observation_output_bytes = 0u, observation_digest = 0u;
    uint64_t snapshot_output_bytes = 0u, snapshot_digest = 0u;
    char evidence_text[4096] = { 0 };
    HANDLE evidence_file = INVALID_HANDLE_VALUE;
    DWORD evidence_read = 0u;
    int failed = 0;

    if (argc == 2 && strcmp(argv[1], "--t97-path-search") == 0)
        return run_t97_path_search();
    if (argc == 2 && strcmp(argv[1], "--t181-v5-install") == 0)
        return run_t181_v5_install();

    if (argc == 3) {
        wchar_t supplied_profile[MAX_PATH], supplied_root[MAX_PATH];
        if (MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, supplied_profile,
                MAX_PATH) == 0 ||
            MultiByteToWideChar(CP_UTF8, 0, argv[2], -1, supplied_root,
                MAX_PATH) == 0) return 2;
        bx_ntvdm_adapter_runtime_v1_reset();
        if (!bx_ntvdm_adapter_runtime_v1_install(supplied_profile,
                supplied_root)) {
            byob_profile_result validation = byob_profile_validate_file(
                supplied_profile, supplied_root);
            const wchar_t *names[] = { L"NTIO.SYS", L"NTDOS.SYS", L"COMMAND.COM" };
            const wchar_t *hashes[] = {
                L"cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937",
                L"957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84",
                L"908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43" };
            uint64_t sizes[] = { 33792u, 858u, 384u };
            unsigned index;
            printf("bx-ntvdm-adapter-runtime-test: supplied profile rejected=%s\n",
                byob_profile_result_name(validation));
            printf("install-diagnostic=%d\n", (int)
                bx_ntvdm_adapter_runtime_v1_install_diagnostic());
            for (index = 0u; index < 3u; ++index)
                printf("component[%u]=%d\n", index, (int)byob_verify_component(
                    supplied_root, names[index], sizes[index], hashes[index]));
            return 3;
        }
        if (!bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan(
                &startup_plan, &payload, &payload_bytes)) return 4;
        printf("bx-ntvdm-adapter-runtime-test: supplied profile plan bytes=%llu\n",
            (unsigned long long)payload_bytes);
        return 0;
    }

    if (GetTempPathW(MAX_PATH, root) == 0 || GetTempFileNameW(root, L"n64", 0u, root) == 0 ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0)) return 1;
    swprintf(ntio, MAX_PATH, L"%ls\\NTIO.SYS", root);
    swprintf(ntdos, MAX_PATH, L"%ls\\NTDOS.SYS", root);
    swprintf(command, MAX_PATH, L"%ls\\COMMAND.COM", root);
    swprintf(target, MAX_PATH, L"%ls\\TARGET.COM", root);
    swprintf(profile, MAX_PATH, L"%ls\\profile.json", root);
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v2\",\"profile\":\"nt4-en-us-command-smoke-v2\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"runtime-test\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},\"host_drive_inventory\":{\"types\":[2,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]},\"machine_observations\":[{\"id\":7,\"physical_address\":1808,\"byte_count\":4}],\"machine_observation_trigger\":{\"observation_id\":7,\"exception_vector\":6,\"instruction_bytes\":\"0f0b\",\"resume_bytes\":2}}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_file(ntio, "abc") || !write_file(ntdos, "abc") ||
        !write_file(command, "abc") || !write_file(profile, json);
    bx_ntvdm_adapter_runtime_v1_reset();
    failed |= !bx_ntvdm_adapter_runtime_v1_install(profile, root);
    failed |= !bx_ntvdm_adapter_runtime_v1_copy_host_drive_snapshot(
        &drive_snapshot) || !bx_ntvdm_host_drive_snapshot_v1_valid(
        &drive_snapshot);
    memset(&drive_snapshot, 0, sizeof(drive_snapshot));
    failed |= !bx_ntvdm_adapter_runtime_v1_copy_host_drive_snapshot(
        &drive_snapshot_copy) || !bx_ntvdm_host_drive_snapshot_v1_valid(
        &drive_snapshot_copy);
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC; event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event); event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 0u; event.vector = 6u; event.error_code = 0u; event.reserved0 = 0u; event.fault_rip = 0x7c00u;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.edi = 0x08b0u;
    bx_ntvdm_instruction_window_v1_capture(&window, marker, sizeof(marker));
    failed |= !bx_ntvdm_adapter_runtime_v1_dispatch(&event, &state, &window, &result) ||
        result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME || result.resume_rip != 0x7c04u;
    failed |= !bx_ntvdm_adapter_runtime_v1_take_pending_write(&event, &state,
        &transaction, &payload,
        &payload_bytes, &resume_rip) || payload_bytes != 3u || memcmp(payload, "abc", 3u) != 0 ||
        transaction.guest_write.guest_physical_address != 0x8b00u || resume_rip != 0x7c04u;
    state.ds = 0x1000u; state.edx = 0x60u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x54u, 0x0cu }, 4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
        bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes);
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x54u, 0x0du }, 4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
        bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes);
    state.eax = 0xabcd5500u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x50u, 0x0du }, 4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        read_result.cpu_delta.gpr16_write_mask != 1u ||
        read_result.cpu_delta.gpr16_values[0] != 0x5503u;
    state.eax = 2u; state.ds = 0x1000u; state.esi = 0x70u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x54u, 0x04u }, 4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes) ||
        multi_write.writes.write_count != 1u || payload_bytes != 4u ||
        multi_write.writes.writes[0].guest_physical_address != 0x10070u ||
        memcmp(payload, "C:\\", 4u) != 0 ||
        (read_result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u;
    state.eax = 1u;
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        read_result.cpu_delta.gpr16_write_mask != 1u ||
        read_result.cpu_delta.gpr16_values[0] != 0u ||
        (read_result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes);
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x50u, 0x46u }, 4u);
    state.es = 0x100u;
    state.ebp = 0x200u;
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        read_result.resume_rip != 0x7c04u ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes) ||
        multi_write.writes.write_count != 4u || payload_bytes != 12u ||
        multi_write.result.cpu_delta.gpr16_values[6] != 0x242u ||
        bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &multi_write, &payload, &payload_bytes);
    bx_ntvdm_instruction_window_v1_capture(&window, ud2, sizeof(ud2));
    failed |= !bx_ntvdm_adapter_runtime_v1_dispatch(&event, &state, &window,
        &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
        result.resume_rip != 0x7c02u;
    failed |= !bx_ntvdm_adapter_runtime_v1_take_pending_observation(&event,
        &state, &observation, &observation_output, &observation_output_bytes) ||
        observation.guest_read.address != 1808u ||
        observation.guest_read.length != sizeof(observation_bytes) ||
        observation_output == 0 ||
        observation_output_bytes != sizeof(observation_bytes);
    failed |= !bx_ntvdm_adapter_runtime_v1_complete_profile_observation(
        &observation, observation_output, observation_output_bytes,
        &observation_digest) || observation_digest == 0u;
    failed |= bx_ntvdm_adapter_runtime_v1_complete_profile_observation(
        &observation, observation_output, observation_output_bytes,
        &observation_digest) != 0;
    failed |= bx_ntvdm_adapter_runtime_v1_take_pending_observation(&event,
        &state, &observation, &observation_output, &observation_output_bytes) != 0;
    bx_ntvdm_adapter_runtime_v1_reset();
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v1\",\"profile\":\"nt4-en-us-command-smoke-v1\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"runtime-snapshot-test\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null,\"machine_observations\":[{\"id\":11,\"physical_address\":64,\"byte_count\":4},{\"id\":12,\"physical_address\":1812,\"byte_count\":4}],\"machine_startup_snapshot_trigger\":{\"observation_ids\":[11,12],\"exception_vector\":6,\"instruction_bytes\":\"0f0b\",\"resume_bytes\":2},\"machine_startup_snapshot_evidence_file\":\"snapshot-evidence.json\",\"machine_startup_plan\":true,\"machine_startup_entry\":\"ntio-v0\",\"machine_startup_handoff\":{\"exception_vector\":6,\"instruction_bytes\":\"0f0b\",\"cs\":51840,\"ip\":3}}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_file(profile, json) ||
        !bx_ntvdm_adapter_runtime_v1_install(profile, root);
    event.fault_rip = 3u;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.cs = 0xca80u;
    state.eip = 3u;
    bx_ntvdm_instruction_window_v1_capture(&window, ud2, sizeof(ud2));
    state.cs = 0xc801u;
    failed |= bx_ntvdm_adapter_runtime_v1_take_deferred_startup_plan(&event,
        &state, &window, &startup_plan, &payload, &payload_bytes);
    state.cs = 0xca80u;
    failed |= !bx_ntvdm_adapter_runtime_v1_take_deferred_startup_plan(&event,
        &state, &window, &startup_plan, &payload, &payload_bytes) ||
        payload_bytes != 3u || startup_plan.entry_cpu.cs != 0x70u;
    failed |= bx_ntvdm_adapter_runtime_v1_take_deferred_startup_plan(&event,
        &state, &window, &startup_plan, &payload, &payload_bytes);
    failed |= !bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan(
        &startup_plan, &payload, &payload_bytes) || payload_bytes != 3u ||
        startup_plan.payload_write.guest_physical_address != 0x700u ||
        startup_plan.preserved_state_address != 0x714u ||
        startup_plan.entry_cpu.cs != 0x70u || startup_plan.entry_cpu.edx != 0u;
    failed |= bx_ntvdm_adapter_runtime_v1_prepare_execution_plan_from_environment(
        &startup_plan, &payload, &payload_bytes) != 1 || payload_bytes != 3u ||
        startup_plan.payload_write.guest_physical_address != 0x700u;
    bx_ntvdm_adapter_runtime_v1_reset();
    if (!SetEnvironmentVariableW(L"NTDOS64_ADAPTER_PROFILE", profile) ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_ROOT", root)) failed = 1;
    failed |= bx_ntvdm_adapter_runtime_v1_prepare_deferred_startup_from_environment() != 1;
    failed |= bx_ntvdm_adapter_runtime_v1_prepare_execution_plan_from_environment(
        &startup_plan, &payload, &payload_bytes) != 1 || payload_bytes != 3u ||
        startup_plan.payload_write.guest_physical_address != 0x700u;
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_PROFILE", 0);
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_ROOT", 0);
    event.fault_rip = 0x7c00u;
    bx_ntvdm_instruction_window_v1_capture(&window, ud2, sizeof(ud2));
    failed |= !bx_ntvdm_adapter_runtime_v4_dispatch(&event, &state, &window,
        &gather_action) ||
        gather_action.disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        gather_action.range_count != 2u || gather_action.total_bytes != 8u ||
        gather_action.cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        !bx_ntvdm_adapter_runtime_v4_take_pending_guest_gather_read(&event,
            &state, &gather_action, &gather_output) || gather_output == 0;
    if (gather_output != 0) memset(gather_output, 0xa5,
        gather_action.total_bytes);
    failed |= !bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read(&event,
        &state, &gather_action, gather_output, gather_action.total_bytes,
        &read_result) || read_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        read_result.resume_rip != 0x7c02u;
    failed |= !bx_ntvdm_adapter_runtime_v1_dispatch(&event, &state, &window,
        &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
        result.resume_rip != 0x7c02u;
    failed |= !bx_ntvdm_adapter_runtime_v1_take_pending_startup_snapshot(&event,
        &state, &snapshot, &snapshot_output, &snapshot_output_bytes) ||
        snapshot.range_count != 2u || snapshot.output_bytes != 8u ||
        snapshot_output == 0 || snapshot_output_bytes != 8u;
    if (snapshot_output != 0 && snapshot_output_bytes == 8u) {
        snapshot_output[0] = 1u; snapshot_output[1] = 2u;
        snapshot_output[2] = 3u; snapshot_output[3] = 4u;
        snapshot_output[4] = 5u; snapshot_output[5] = 6u;
        snapshot_output[6] = 7u; snapshot_output[7] = 8u;
    }
    failed |= !bx_ntvdm_adapter_runtime_v1_complete_profile_startup_snapshot(
        &snapshot, snapshot_output, snapshot_output_bytes, &snapshot_digest) ||
        snapshot_digest == 0u;
    swprintf(evidence, MAX_PATH, L"%ls\\snapshot-evidence.json", root);
    evidence_file = CreateFileW(evidence, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (evidence_file == INVALID_HANDLE_VALUE || !ReadFile(evidence_file,
        evidence_text, sizeof(evidence_text) - 1u, &evidence_read, 0) ||
        strstr(evidence_text, "ntdos64-startup-snapshot-evidence-v1") == 0 ||
        strstr(evidence_text, "\"bytes_hex\":\"01020304\"") == 0 ||
        strstr(evidence_text, "\"bytes_hex\":\"05060708\"") == 0) failed = 1;
    if (evidence_file != INVALID_HANDLE_VALUE) CloseHandle(evidence_file);
    failed |= bx_ntvdm_adapter_runtime_v1_complete_profile_startup_snapshot(
        &snapshot, snapshot_output, snapshot_output_bytes, &snapshot_digest) != 0;
    bx_ntvdm_adapter_runtime_v1_reset();
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v3\",\"profile\":\"nt4-en-us-command-smoke-v3\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"runtime-test\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"target\",\"file_name\":\"TARGET.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_target_placement\":{\"path\":\"\\\\TARGET.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}}}",
        sha256_abc, sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_file(target, "abc") || !write_file(profile, json);
    failed |= bx_ntvdm_adapter_runtime_v1_install(profile, root) != 0 ||
        bx_ntvdm_adapter_runtime_v1_install_diagnostic() !=
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_LAUNCH_DECLARATION;
    if (!SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_KIND", L"com") ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_TAIL", L"/c smoke")) failed = 1;
    bx_ntvdm_adapter_runtime_v1_reset();
    failed |= !bx_ntvdm_adapter_runtime_v1_install(profile, root) ||
        bx_ntvdm_adapter_runtime_v1_install_diagnostic() !=
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_NONE;
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_KIND", 0);
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_TAIL", 0);
    bx_ntvdm_adapter_runtime_v1_reset();
    DeleteFileW(evidence); DeleteFileW(profile); DeleteFileW(target); DeleteFileW(command); DeleteFileW(ntdos); DeleteFileW(ntio); RemoveDirectoryW(root);
    if (failed) return 1;
    puts("bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified");
    return 0;
}
