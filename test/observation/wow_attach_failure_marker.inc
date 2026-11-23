static BOOL wow_fixture_attach_failure(void)
{
    char path[MAX_PATH]; DWORD length, written; HANDLE file;
    length = GetEnvironmentVariableA("MVDM_TEST_WOW_ATTACH_MARKER", path, sizeof(path));
    if (length && length < sizeof(path)) {
        file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            WriteFile(file, "attach-failure", 14, &written, NULL);
            CloseHandle(file);
        }
    }
    return TRUE;
}
