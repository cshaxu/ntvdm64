/*
 * SH-11 bounded caller fixture. This is not a CPU adapter: it provides just
 * enough passive register and guest-memory observation for the original
 * demdir.c routines to exercise the contained OEM facade.
 */
#include <windows.h>
#include <string.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

BOOL ntdos64_oemuni_configure_root(LPCSTR);
VOID ntdos64_oemuni_reset(VOID);
VOID demCreateDir(VOID);
VOID demDeleteDir(VOID);
VOID demSetCurrentDir(VOID);

static UCHAR guest_memory[4096];
static USHORT fixture_bx;
static USHORT fixture_dx;
static USHORT fixture_si;
static USHORT fixture_ds;
static UCHAR fixture_al;
static ULONG fixture_cf;
static CHAR fixture_error_drive;

USHORT getBX(VOID) { return fixture_bx; }
USHORT getDX(VOID) { return fixture_dx; }
USHORT getSI(VOID) { return fixture_si; }
USHORT getDS(VOID) { return fixture_ds; }
UCHAR getAL(VOID) { return fixture_al; }
VOID setCF(ULONG value) { fixture_cf = value; }

UCHAR *Sim32pGetVDMPointer(ULONG address, UCHAR protected_mode) {
    UNREFERENCED_PARAMETER(protected_mode);
    if (address >= sizeof(guest_memory)) return NULL;
    return guest_memory + address;
}

VOID demClientError(HANDLE file, CHAR drive) {
    UNREFERENCED_PARAMETER(file);
    fixture_error_drive = drive;
}

static VOID reset_fixture(LPCSTR path) {
    ZeroMemory(guest_memory, sizeof(guest_memory));
    memcpy(guest_memory + 0x100, path, strlen(path) + 1);
    fixture_bx = 0;
    fixture_si = 0;
    fixture_ds = 0;
    fixture_dx = 0x100;
    fixture_cf = 0xFFFFFFFF;
    fixture_error_drive = 0;
}

int main(void) {
    CHAR temp[MAX_PATH];
    CHAR root[MAX_PATH];

    if (!GetTempPathA(MAX_PATH, temp) || !GetTempFileNameA(temp, "nod", 0, root)) return 1;
    if (!DeleteFileA(root) || !CreateDirectoryA(root, NULL)) return 2;
    if (!ntdos64_oemuni_configure_root(root)) { RemoveDirectoryA(root); return 3; }

    reset_fixture("C:\\created");
    demCreateDir();
    if (fixture_cf != 0 || fixture_error_drive != 0 ||
        GetFileAttributesA(root) == INVALID_FILE_ATTRIBUTES) goto failure;

    reset_fixture("C:\\created");
    demSetCurrentDir();
    if (fixture_cf != 0 || fixture_error_drive != 0) goto failure;

    reset_fixture("C:\\created");
    demDeleteDir();
    if (fixture_cf != 0 || fixture_error_drive != 0) goto failure;

    reset_fixture("C:\\..\\escape");
    demCreateDir();
    if (fixture_error_drive != 'C') goto failure;

    ntdos64_oemuni_reset();
    if (!RemoveDirectoryA(root)) return 4;
    return 0;

failure:
    ntdos64_oemuni_reset();
    RemoveDirectoryA(root);
    return 5;
}
