#include "bx_ntvdm_profile_search_snapshot_v1.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    static const uint8_t command[] = { 1u };
    static const uint8_t config[] = { 2u };
    static const uint8_t autoexec[] = { 3u };
    static const uint8_t target[] = { 4u };
    bx_ntvdm_readonly_namespace_v1 resource_set;
    byob_profile_selection selection;
    bx_ntvdm_profile_search_snapshot_v1 snapshot;
    memset(&resource_set, 0, sizeof(resource_set));
    memset(&selection, 0, sizeof(selection));
    resource_set.drive_index = 2u;
    resource_set.files[0].bytes = command; resource_set.files[0].byte_count = sizeof(command);
    wcscpy(resource_set.files[0].path, L"\\COMMAND.COM");
    resource_set.files[1].bytes = config; resource_set.files[1].byte_count = sizeof(config);
    wcscpy(resource_set.files[1].path, L"\\CONFIG.SYS");
    resource_set.files[2].bytes = autoexec; resource_set.files[2].byte_count = sizeof(autoexec);
    wcscpy(resource_set.files[2].path, L"\\AUTOEXEC.BAT");
    resource_set.files[3].bytes = target; resource_set.files[3].byte_count = sizeof(target);
    resource_set.file_count = 4u;
    wcscpy(resource_set.files[3].path, L"\\TARGET.COM");
    selection.has_guest_search_metadata = 1u;
    selection.command_metadata.attributes = 0x20u;
    selection.config_metadata.dos_time = 1u;
    selection.autoexec_metadata.dos_date = 2u;
    selection.target_metadata.attributes = 0x21u;
    if (!bx_ntvdm_profile_search_snapshot_v1_initialize(&snapshot, &resource_set,
            &selection) || !bx_ntvdm_profile_search_snapshot_v1_valid(&snapshot) ||
        snapshot.drive_index != 2u || snapshot.entry_count != 4u ||
        wcscmp(snapshot.entries[0].dos_name, L"COMMAND.COM") != 0 ||
        snapshot.entries[0].attributes != 0x20u ||
        snapshot.entries[1].dos_time != 1u || snapshot.entries[2].dos_date != 2u ||
        snapshot.entries[3].attributes != 0x21u || snapshot.entries[3].byte_count != 1u)
        return 1;
    selection.has_guest_search_metadata = 0u;
    if (bx_ntvdm_profile_search_snapshot_v1_initialize(&snapshot, &resource_set,
            &selection)) return 1;
    puts("profile-search-snapshot-test: finite BYOB projection verified");
    return 0;
}
