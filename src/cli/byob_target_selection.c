#include "byob_target_selection.h"

#include <windows.h>
#include <wchar.h>

int byob_target_selection_matches(const wchar_t *byob_root,
    const byob_profile_selection *selection, const wchar_t *target_path)
{
    wchar_t declared[MAX_PATH];
    wchar_t expected[MAX_PATH];
    wchar_t actual[MAX_PATH];
    DWORD declared_length, actual_length;

    if (byob_root == NULL || selection == NULL || target_path == NULL ||
        !selection->has_target_placement || selection->target.file_name[0] == L'\0' ||
        swprintf(declared, MAX_PATH, L"%ls\\%ls", byob_root,
            selection->target.file_name) < 0) return 0;
    declared_length = GetFullPathNameW(declared, MAX_PATH, expected, NULL);
    actual_length = GetFullPathNameW(target_path, MAX_PATH, actual, NULL);
    return declared_length != 0u && declared_length < MAX_PATH &&
        actual_length != 0u && actual_length < MAX_PATH &&
        CompareStringOrdinal(expected, -1, actual, -1, TRUE) == CSTR_EQUAL;
}
