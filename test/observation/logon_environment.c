/* Test-owned current-token environment; stdout is an exact UTF-16 block. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <userenv.h>
#include <wchar.h>

int main(int argc, char **argv)
{
    HANDLE token;
    WCHAR *block, *end;
    DWORD bytes, written;
    int result = 1;
    (void)argv;
    if (argc != 1) return 2;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE,
            &token)) return 3;
    if (!CreateEnvironmentBlock((void **)&block, token, FALSE)) {
        CloseHandle(token);
        return 4;
    }
    CloseHandle(token);
    end = block;
    while (*end) end += wcslen(end) + 1;
    ++end;
    if (end == block + 1) ++end;
    if ((SIZE_T)(end - block) <= MAXDWORD / sizeof(WCHAR)) {
        bytes = (DWORD)((end - block) * sizeof(WCHAR));
        if (WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), block, bytes, &written,
                NULL) && written == bytes) result = 0;
    }
    DestroyEnvironmentBlock(block);
    return result;
}
