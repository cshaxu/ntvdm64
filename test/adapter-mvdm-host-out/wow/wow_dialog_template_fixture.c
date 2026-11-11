#include "dialog-template/precomp.h"
#include <stdio.h>

/* Supplemental original-source fixture, unrun until the complete S3 group.
 * The records below are authored test inputs. The actual original clres.c
 * walker and modal entry are linked with WOW_ORIGINAL_DIALOG_TEMPLATE.
 * InternalDialogBox is a boundary double, not guest/modal acceptance. */
static __declspec(align(4)) BYTE bytes[128];
static DWORD version;
static unsigned version_calls, manager_calls, failures;
static DWORD manager_bytes;
static UINT manager_flags;
static HWND manager_owner;
static LPARAM manager_parameter;
#define MODULE ((HINSTANCE)(ULONG_PTR)0x12340001)
#define OWNER ((HWND)(ULONG_PTR)0x2345)
#define CHECK(x) do { if (!(x)) { ++failures; \
    printf("FAIL line %d: %s\n", __LINE__, #x); } } while (0)

DWORD dialog_fixture_version(HINSTANCE module)
{
    CHECK(module == MODULE);
    ++version_calls;
    return version;
}

int InternalDialogBox(HANDLE module, LPDLGTEMPLATE resource, DWORD count,
    HWND owner, DLGPROC procedure, LONG parameter, UINT flags)
{
    CHECK(module == MODULE && (BYTE *)resource == bytes && procedure == NULL);
    ++manager_calls;
    manager_bytes = count;
    manager_flags = flags;
    manager_owner = owner;
    manager_parameter = parameter;
    return -29;
}

static void word_at(unsigned offset, WORD value)
{ CopyMemory(bytes + offset, &value, sizeof(value)); }

static void standard(void)
{ ZeroMemory(bytes, sizeof(bytes)); }

static void extended(void)
{
    ZeroMemory(bytes, sizeof(bytes));
    ((LPDLGTEMPLATE2)bytes)->wDlgVer = 1;
    ((LPDLGTEMPLATE2)bytes)->wSignature = 0xffff;
}

int main(void)
{
    unsigned calls;
    standard();
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 24);
    CHECK(version_calls == 0);
    ((LPDLGTEMPLATE)bytes)->style = 0x4000; /* outside DS_VALID40 */
    version = 0x030a;
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 24);
    version = 0x0400;
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 0);
    calls = manager_calls;
    CHECK(DialogBoxIndirectParamAorW(MODULE, (LPCDLGTEMPLATEW)bytes,
        OWNER, NULL, 81, 0xffff) == -1 && manager_calls == calls);

    standard();
    ((LPDLGTEMPLATE)bytes)->style = DS_SETFONT;
    word_at(24, 8); word_at(26, 'Z'); /* font point size and UTF-16 name */
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 32);
    standard();
    ((LPDLGTEMPLATE)bytes)->cdit = 1;
    word_at(42, 0xffff); word_at(44, 0x81); /* EDIT class ordinal */
    word_at(46, 'x');
    word_at(50, 6); /* Original walker advances payload plus this WORD. */
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 60);
    CHECK(DialogBoxIndirectParamAorW(MODULE, (LPCDLGTEMPLATEW)bytes,
        OWNER, NULL, 81, 0xffff) == -29);
    CHECK(manager_bytes == 60 && manager_flags ==
        (SCDLG_CLIENT | SCDLG_ANSI | SCDLG_16BIT));
    CHECK(manager_owner == OWNER && manager_parameter == 81);

    extended();
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 32);
    ((LPDLGTEMPLATE2)bytes)->style = DS_SETFONT;
    word_at(32, 8); word_at(34, 400); bytes[36] = 1;
    bytes[37] = DEFAULT_CHARSET; word_at(38, 'Z');
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 44);
    extended();
    ((LPDLGTEMPLATE2)bytes)->cDlgItems = 1;
    word_at(56, 0xffff); word_at(58, 0x81);
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 64);
    calls = version_calls;
    ((LPDLGTEMPLATE2)bytes)->style = 0x4000;
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 0);
    CHECK(version_calls == calls); /* Extended validation does not query version. */
    extended();
    ((LPDLGTEMPLATE2)bytes)->wDlgVer = 2;
    CHECK(GetSizeDialogTemplate(MODULE, (LPCDLGTEMPLATE)bytes) == 0);
    printf("dialog_template_failures=%u\n", failures);
    return failures != 0;
}
