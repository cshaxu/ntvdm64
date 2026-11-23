static void wow_fixture_unload_marker(const char *message, DWORD size)
{
    char path[MAX_PATH]; DWORD length, written; HANDLE file;
    length = GetEnvironmentVariableA("MVDM_TEST_WOW_UNLOAD_MARKER", path, sizeof(path));
    if (length && length < sizeof(path)) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            WriteFile(file, message, size, &written, NULL);
            CloseHandle(file);
        }
    }
}
