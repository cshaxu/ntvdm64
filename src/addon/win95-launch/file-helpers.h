static BOOL read_all(const wchar_t *path, BYTE **output, DWORD *length)
{
    HANDLE file; DWORD read = 0; *output = NULL;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    *length = GetFileSize(file, NULL);
    if (*length == INVALID_FILE_SIZE || *length > 16777216u || !(*output = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)*length + 1)) ||
        !ReadFile(file, *output, *length, &read, NULL) || read != *length) { if (*output) HeapFree(GetProcessHeap(), 0, *output); *output = NULL; CloseHandle(file); return FALSE; }
    CloseHandle(file); return TRUE;
}
