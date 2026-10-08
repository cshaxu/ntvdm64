/* Independently authored serializer for the fixed PIF / Windows 386 / NT 3.1
 * records documented by the selected OpenNT ABI header.  It intentionally
 * does not import or retain an existing PIF template, whose old paths are not
 * portable installation state. */
#define WINNT 1
#include <windows.h>
#include <pif.h>
#include <string.h>
#include "pif_writer.h"

static BOOL put_oem(char *destination, size_t capacity, const wchar_t *source)
{
    int written;
    ZeroMemory(destination, capacity);
    written = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, source, -1,
                                  destination, (int)capacity, NULL, NULL);
    if (written == 0 || (size_t)written >= capacity) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    return TRUE;
}

static BOOL write_exact(HANDLE file, const void *bytes, DWORD length)
{
    DWORD written = 0;
    return WriteFile(file, bytes, length, &written, NULL) && written == length;
}

BOOL win31_write_pif(const wchar_t *path, const wchar_t *title,
                     const wchar_t *program, const wchar_t *directory,
                     const wchar_t *arguments, const wchar_t *config,
                     const wchar_t *autoexec)
{
    STDPIF standard = {0};
    PIFEXTHDR first = {0}, w386_header = {0}, nt_header = {0};
    W386PIF30 w386 = {0};
    WNTPIF31 nt = {0};
    HANDLE file;
    BYTE sum = 0;
    DWORD i;

    if (!path || !title || !program || !directory || !arguments || !config || !autoexec) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!put_oem(standard.appname, sizeof(standard.appname), title) ||
        !put_oem(standard.startfile, sizeof(standard.startfile), program) ||
        !put_oem(standard.defpath, sizeof(standard.defpath), directory) ||
        !put_oem(standard.params, sizeof(standard.params), arguments) ||
        !put_oem(w386.PfW386params, sizeof(w386.PfW386params), arguments) ||
        !put_oem(nt.nt31Prop.achConfigFile, sizeof(nt.nt31Prop.achConfigFile), config) ||
        !put_oem(nt.nt31Prop.achAutoexecFile, sizeof(nt.nt31Prop.achAutoexecFile), autoexec))
        return FALSE;

    standard.cPages = 1;
    standard.highVector = 0xff;
    strcpy_s(first.extsig, sizeof(first.extsig), STDHDRSIG);
    first.extnxthdrfloff = (WORD)(sizeof(standard) + sizeof(first));
    first.extsizebytes = (WORD)sizeof(standard);
    strcpy_s(w386_header.extsig, sizeof(w386_header.extsig), W386HDRSIG30);
    w386_header.extnxthdrfloff = (WORD)(first.extnxthdrfloff + sizeof(w386_header));
    w386_header.extfileoffset = (WORD)(w386_header.extnxthdrfloff + sizeof(nt_header));
    w386_header.extsizebytes = (WORD)sizeof(w386);
    strcpy_s(nt_header.extsig, sizeof(nt_header.extsig), WNTHDRSIG31);
    nt_header.extnxthdrfloff = LASTHDRPTR;
    nt_header.extfileoffset = (WORD)(w386_header.extfileoffset + sizeof(w386));
    nt_header.extsizebytes = (WORD)sizeof(nt);
    w386.PfFPriority = 100;
    w386.PfBPriority = 50;
    w386.PfMaxXmsK = 1024;
    nt.wInternalRevision = WNTPIF31_VERSION;

    /* The checksum covers the serialized fixed record, starting at byte 2. */
    for (i = 2; i < sizeof(standard); ++i) sum = (BYTE)(sum + ((BYTE *)&standard)[i]);
    standard.id = sum;
    file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!write_exact(file, &standard, sizeof(standard)) ||
        !write_exact(file, &first, sizeof(first)) ||
        !write_exact(file, &w386_header, sizeof(w386_header)) ||
        !write_exact(file, &nt_header, sizeof(nt_header)) ||
        !write_exact(file, &w386, sizeof(w386)) ||
        !write_exact(file, &nt, sizeof(nt))) {
        DWORD error = GetLastError();
        CloseHandle(file);
        DeleteFileW(path);
        SetLastError(error ? error : ERROR_WRITE_FAULT);
        return FALSE;
    }
    return CloseHandle(file);
}
