#include <wchar.h>

#include "byob_profile.h"

int wmain(int argc, wchar_t **argv)
{
    byob_profile_selection selection;
    if (argc != 3 ||
        byob_profile_validate_file_select(argv[1], argv[2], &selection) !=
            BYOB_PROFILE_ACCEPTED ||
        selection.guest_display_state !=
            BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1 ||
        selection.target.bytes != 882u ||
        wcscmp(selection.target.file_name, L"TARGET.EXE") != 0 ||
        selection.declared_target_count != 1u ||
        wcscmp(selection.declared_targets[0].placement.path, L"\\TARGET.EXE") != 0 ||
        selection.declared_targets[0].placement.drive_index != 2u)
        return 1;
    return 0;
}
