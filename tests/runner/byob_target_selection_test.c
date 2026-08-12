#include "byob_target_selection.h"

#include <windows.h>
#include <stdio.h>

int main(void)
{
    wchar_t root[MAX_PATH], target[MAX_PATH], outside[MAX_PATH];
    byob_profile_selection selection = {0};
    if (GetTempPathW(MAX_PATH, root) == 0u || GetTempFileNameW(root, L"n64", 0u, root) == 0u ||
        !DeleteFileW(root) || !CreateDirectoryW(root, NULL)) return 1;
    swprintf(target, MAX_PATH, L"%ls\\TARGET.COM", root);
    swprintf(outside, MAX_PATH, L"%ls\\OTHER.COM", root);
    wcscpy(selection.target.file_name, L"TARGET.COM");
    wcscpy(selection.target_placement.path, L"\\TARGET.COM");
    selection.target_placement.drive_index = 2u;
    selection.has_target_placement = 1u;
    if (!byob_target_selection_matches(root, &selection, target) ||
        byob_target_selection_matches(root, &selection, outside)) return 1;
    wcscpy(selection.target.file_name, L"TARGET.EXE");
    if (byob_target_selection_matches(root, &selection, target)) return 1;
    RemoveDirectoryW(root);
    puts("byob-target-selection-test: profile-pinned target path guard verified");
    return 0;
}
