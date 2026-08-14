#include <wchar.h>
#include <stdio.h>

#include "byob_profile.h"

int wmain(int argc, wchar_t **argv)
{
    byob_profile_selection selection;
    if (argc != 3) return 2;
    { byob_profile_result result = byob_profile_validate_file_select(argv[1], argv[2], &selection);
      if (result != BYOB_PROFILE_ACCEPTED) { fprintf(stderr, "%s\n", byob_profile_result_name(result)); return 3; } }
    if (
        selection.guest_display_state !=
            BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1 ||
        selection.target.bytes != 882u ||
        wcscmp(selection.target.file_name, L"TARGET.EXE") != 0 ||
        selection.declared_target_count != 1u ||
        selection.has_machine_external_initial_state != 1u ||
        wcscmp(selection.machine_external_initial_state_evidence_sha256,
            L"168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195") != 0 ||
        wcscmp(selection.declared_targets[0].placement.path, L"\\TARGET.EXE") != 0 ||
        selection.declared_targets[0].placement.drive_index != 2u)
        return 1;
    return 0;
}
