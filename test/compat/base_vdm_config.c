#include "compat/include/base_vdm_config.h"
#include <stdio.h>
#include <string.h>

static unsigned failures;
#define CHECK(x) do { if (!(x)) { ++failures; printf("line=%u\n", __LINE__); } } while (0)
static BOOL read_dos(char *out, DWORD *size)
{
    DWORD reserve = 0;
    BOOL result = BaseGetVDMKeyword(CMDLINE, out, size, DOSSIZE, &reserve);
    if (result) CHECK(reserve == 16UL * 1024 * 1024);
    return result;
}
static DWORD WINAPI independent_thread(void *unused)
{
    OPENNT_BASE_VDM_CONFIG config;
    char text[MAX_VDM_CFG_LINE];
    DWORD size = sizeof(text);
    (void)unused;
    if (read_dos(text, &size)) return 1;
    if (!OpenNtBaseInitializeVdmConfig(&config, "child.exe", "kernel")) return 2;
    if (OpenNtBaseBindVdmConfig(&config)) return 3;
    if (!read_dos(text, &size) || strcmp(text, "child.exe")) return 4;
    return OpenNtBaseBindVdmConfig(NULL) == &config ? 0 : 5;
}
int main(void)
{
    OPENNT_BASE_VDM_CONFIG first, second, invalid;
    char text[MAX_VDM_CFG_LINE], expected[MAX_VDM_CFG_LINE];
    DWORD size, reserve, code;
    HANDLE thread;
    size = sizeof(text);
    CHECK(!read_dos(text, &size));
    CHECK(!OpenNtBaseInitializeVdmConfig(&invalid, "bad\"path", "kernel"));
    CHECK(!invalid.ImageLength);
    CHECK(OpenNtBaseInitializeVdmConfig(&first, "O:\\package with spaces\\ntvdm.exe", "wowkernel"));
    CHECK(OpenNtBaseInitializeVdmConfig(&second, "second.exe", "kernel"));
    CHECK(OpenNtBaseBindVdmConfig(&first) == NULL);
    size = sizeof(text);
    CHECK(read_dos(text, &size));
    CHECK(!strcmp(text, first.Dos) && size == strlen(first.Dos));
    CHECK(OpenNtBaseVdmImageEnd(text) == text + first.ImageLength);
    CHECK(OpenNtBaseVdmImageEnd("different.exe") == NULL);
    size = sizeof(text); reserve = 0;
    CHECK(BaseGetVDMKeyword(WOWCMDLINE, text, &size, WOWSIZE, &reserve));
    CHECK(!strcmp(text, first.Wow));
    memset(text, 'x', sizeof(text)); memcpy(expected, text, sizeof(text));
    size = first.ImageLength;
    CHECK(!read_dos(text, &size) && !memcmp(text, expected, sizeof(text)));
    CHECK(OpenNtBaseBindVdmConfig(&second) == &first);
    size = sizeof(text); CHECK(read_dos(text, &size) && !strcmp(text, "second.exe"));
    CHECK(OpenNtBaseBindVdmConfig(&first) == &second);
    thread = CreateThread(NULL, 0, independent_thread, NULL, 0, NULL);
    CHECK(thread != NULL);
    if (thread) {
        CHECK(WaitForSingleObject(thread, 10000) == WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread, &code) && code == 0);
        CloseHandle(thread);
    }
    size = sizeof(text); CHECK(read_dos(text, &size) && !strcmp(text, first.Dos));
    CHECK(OpenNtBaseBindVdmConfig(NULL) == &first);
    size = sizeof(text); CHECK(!read_dos(text, &size));
    printf("BASE_VDM_CONFIG failures=%u\n", failures);
    return failures ? 1 : 0;
}
