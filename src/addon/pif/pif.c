/* PIF.EXE: bounded reader/editor for the OpenNT PIF ABI. */
#define WINNT 1
/* pif.h otherwise declares old property-sheet APIs whose UI typedefs are not
 * part of this format-only tool.  The ABI structs remain available. */
#define NTVDM 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <pif.h>
#include <stdio.h>
#include <string.h>

typedef struct _PIF_VIEW {
    BYTE *bytes;
    DWORD size;
    STDPIF *standard;
    W286PIF30 *w286;
    W386PIF30 *w386;
    WNTPIF31 *nt;
} PIF_VIEW;

static void usage(void)
{
    fwprintf(stderr, L"Usage:\n"
                     L"  PIF.EXE show <file>\n"
                     L"  PIF.EXE create <file> --title T --program P --directory D --arguments A --config C --autoexec E [--close-on-exit]\n"
                     L"  PIF.EXE update <file> [--title T] [--program P] [--directory D] [--arguments A] [--config C] [--autoexec E] [--close-on-exit]\n");
}

static BOOL read_file(const wchar_t *path, PIF_VIEW *view)
{
    HANDLE file = INVALID_HANDLE_VALUE;
    LARGE_INTEGER length;
    DWORD read = 0;
    ZeroMemory(view, sizeof(*view));
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &length) ||
        length.QuadPart < (LONGLONG)sizeof(STDPIF) || length.QuadPart > 65535) goto failed;
    view->size = (DWORD)length.QuadPart;
    view->bytes = HeapAlloc(GetProcessHeap(), 0, view->size);
    if (!view->bytes || !ReadFile(file, view->bytes, view->size, &read, NULL) || read != view->size) goto failed;
    CloseHandle(file);
    view->standard = (STDPIF *)view->bytes;
    return TRUE;
failed:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    HeapFree(GetProcessHeap(), 0, view->bytes);
    ZeroMemory(view, sizeof(*view));
    return FALSE;
}

static void dispose(PIF_VIEW *view)
{
    HeapFree(GetProcessHeap(), 0, view->bytes);
    ZeroMemory(view, sizeof(*view));
}

static BOOL range_ok(const PIF_VIEW *view, DWORD offset, DWORD size)
{
    return offset <= view->size && size <= view->size - offset;
}

static BOOL parse(PIF_VIEW *view)
{
    DWORD position = sizeof(STDPIF), seen[64], count = 0;
    BYTE sum = 0;
    DWORD i;
    if (!view->standard || !range_ok(view, 0, sizeof(STDPIF))) return FALSE;
    for (i = 2; i < sizeof(STDPIF); ++i) sum = (BYTE)(sum + view->bytes[i]);
    /* A zero ID is the established no-checksum form used by legacy PIFs.
     * When a producer did supply the byte, it remains an integrity check. */
    if (view->standard->id != 0 && sum != view->standard->id) return FALSE;
    while (position != LASTHDRPTR) {
        PIFEXTHDR *header;
        DWORD data;
        if (!range_ok(view, position, sizeof(PIFEXTHDR)) || count == ARRAYSIZE(seen)) return FALSE;
        for (i = 0; i < count; ++i) if (seen[i] == position) return FALSE;
        seen[count++] = position;
        header = (PIFEXTHDR *)(view->bytes + position);
        data = header->extfileoffset;
        if (!range_ok(view, data, header->extsizebytes)) return FALSE;
        if (strcmp(header->extsig, W386HDRSIG30) == 0) {
            if (header->extsizebytes < sizeof(W386PIF30)) return FALSE;
            view->w386 = (W386PIF30 *)(view->bytes + data);
        } else if (strcmp(header->extsig, W286HDRSIG30) == 0) {
            if (header->extsizebytes < sizeof(W286PIF30)) return FALSE;
            view->w286 = (W286PIF30 *)(view->bytes + data);
        } else if (strcmp(header->extsig, WNTHDRSIG31) == 0) {
            if (header->extsizebytes < sizeof(WNTPIF31)) return FALSE;
            view->nt = (WNTPIF31 *)(view->bytes + data);
        }
        position = header->extnxthdrfloff;
    }
    return TRUE;
}

static BOOL put_oem(char *target, size_t capacity, const wchar_t *source)
{
    int result;
    if (!source) return TRUE;
    ZeroMemory(target, capacity);
    result = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, source, -1,
                                 target, (int)capacity, NULL, NULL);
    return result > 0 && (size_t)result < capacity;
}

static void checksum(PIF_VIEW *view)
{
    BYTE sum = 0;
    DWORD i;
    for (i = 2; i < sizeof(STDPIF); ++i) sum = (BYTE)(sum + view->bytes[i]);
    view->standard->id = sum;
}

static BOOL write_file(const wchar_t *path, const void *bytes, DWORD size)
{
    HANDLE file;
    DWORD written = 0;
    file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!WriteFile(file, bytes, size, &written, NULL) || written != size) {
        CloseHandle(file); DeleteFileW(path); return FALSE;
    }
    return CloseHandle(file);
}

static const wchar_t *option(int argc, wchar_t **argv, const wchar_t *name)
{
    int i;
    for (i = 0; i + 1 < argc; ++i) if (_wcsicmp(argv[i], name) == 0) return argv[i + 1];
    return NULL;
}

static BOOL switch_present(int argc, wchar_t **argv, const wchar_t *name)
{
    int i;
    for (i = 0; i < argc; ++i) if (_wcsicmp(argv[i], name) == 0) return TRUE;
    return FALSE;
}

static BOOL set_values(PIF_VIEW *view, int argc, wchar_t **argv)
{
    const wchar_t *title = option(argc, argv, L"--title");
    const wchar_t *program = option(argc, argv, L"--program");
    const wchar_t *directory = option(argc, argv, L"--directory");
    const wchar_t *arguments = option(argc, argv, L"--arguments");
    const wchar_t *config = option(argc, argv, L"--config");
    const wchar_t *autoexec = option(argc, argv, L"--autoexec");
    if ((!title || put_oem(view->standard->appname, sizeof(view->standard->appname), title)) &&
        (!program || put_oem(view->standard->startfile, sizeof(view->standard->startfile), program)) &&
        (!directory || put_oem(view->standard->defpath, sizeof(view->standard->defpath), directory)) &&
        (!arguments || (put_oem(view->standard->params, sizeof(view->standard->params), arguments) &&
                         (!view->w386 || put_oem(view->w386->PfW386params, sizeof(view->w386->PfW386params), arguments)))) &&
        (!config || (view->nt && put_oem(view->nt->nt31Prop.achConfigFile, sizeof(view->nt->nt31Prop.achConfigFile), config))) &&
        (!autoexec || (view->nt && put_oem(view->nt->nt31Prop.achAutoexecFile, sizeof(view->nt->nt31Prop.achAutoexecFile), autoexec)))) {
        if (switch_present(argc, argv, L"--close-on-exit")) view->standard->MSflags |= 0x10u;
        /* Preserve the legacy no-checksum convention on update.  Creation
         * explicitly seals its new record below. */
        if (view->standard->id != 0) checksum(view);
        return TRUE;
    }
    return FALSE;
}

static BOOL create(const wchar_t *path, int argc, wchar_t **argv)
{
    const wchar_t *title = option(argc, argv, L"--title"), *program = option(argc, argv, L"--program");
    const wchar_t *directory = option(argc, argv, L"--directory"), *arguments = option(argc, argv, L"--arguments");
    const wchar_t *config = option(argc, argv, L"--config"), *autoexec = option(argc, argv, L"--autoexec");
    BYTE bytes[sizeof(STDPIF) + 3 * sizeof(PIFEXTHDR) + sizeof(W386PIF30) + sizeof(WNTPIF31)] = {0};
    PIF_VIEW view = {0};
    PIFEXTHDR *standard_header, *w386_header, *nt_header;
    DWORD at;
    if (!title || !program || !directory || !arguments || !config || !autoexec) return FALSE;
    view.bytes = bytes; view.size = sizeof(bytes); view.standard = (STDPIF *)bytes;
    view.standard->cPages = 1; view.standard->highVector = 0xff;
    at = sizeof(STDPIF); standard_header = (PIFEXTHDR *)(bytes + at); at += sizeof(*standard_header);
    w386_header = (PIFEXTHDR *)(bytes + at); at += sizeof(*w386_header);
    nt_header = (PIFEXTHDR *)(bytes + at); at += sizeof(*nt_header);
    strcpy_s(standard_header->extsig, sizeof(standard_header->extsig), STDHDRSIG);
    standard_header->extnxthdrfloff = (WORD)((BYTE *)w386_header - bytes);
    standard_header->extsizebytes = (WORD)sizeof(STDPIF);
    strcpy_s(w386_header->extsig, sizeof(w386_header->extsig), W386HDRSIG30);
    w386_header->extnxthdrfloff = (WORD)((BYTE *)nt_header - bytes);
    w386_header->extfileoffset = (WORD)at; w386_header->extsizebytes = (WORD)sizeof(W386PIF30);
    view.w386 = (W386PIF30 *)(bytes + at); at += sizeof(W386PIF30);
    strcpy_s(nt_header->extsig, sizeof(nt_header->extsig), WNTHDRSIG31);
    nt_header->extnxthdrfloff = LASTHDRPTR; nt_header->extfileoffset = (WORD)at;
    nt_header->extsizebytes = (WORD)sizeof(WNTPIF31);
    view.nt = (WNTPIF31 *)(bytes + at); view.nt->wInternalRevision = WNTPIF31_VERSION;
    view.w386->PfFPriority = 100; view.w386->PfBPriority = 50; view.w386->PfMaxXmsK = 1024;
    if (!set_values(&view, argc, argv)) return FALSE;
    checksum(&view);
    return write_file(path, bytes, sizeof(bytes));
}

static void print_oem(const wchar_t *key, const char *value, size_t capacity)
{
    wchar_t converted[260];
    int length;
    while (capacity && (value[capacity - 1] == '\0' || value[capacity - 1] == ' ')) --capacity;
    length = MultiByteToWideChar(CP_OEMCP, 0, value, (int)capacity, converted, ARRAYSIZE(converted));
    wprintf(L"%ls=", key);
    if (length > 0) wprintf(L"%.*ls", length, converted);
    wprintf(L"\n");
}

static void inspect(const PIF_VIEW *view)
{
    const STDPIF *s = view->standard;
    wprintf(L"PIF_SIZE=%lu\nCHECKSUM=%ls\n", (unsigned long)view->size,
            s->id ? L"VALID" : L"NONE");
    print_oem(L"TITLE", s->appname, sizeof(s->appname));
    print_oem(L"PROGRAM", s->startfile, sizeof(s->startfile));
    print_oem(L"DIRECTORY", s->defpath, sizeof(s->defpath));
    print_oem(L"ARGUMENTS", s->params, sizeof(s->params));
    wprintf(L"STANDARD.MAXMEM_KB=%u\nSTANDARD.MINMEM_KB=%u\n"
            L"STANDARD.MS_FLAGS=0x%02X\nSTANDARD.SCREEN=0x%02X\n"
            L"STANDARD.CLOSE_ON_EXIT=%ls\n"
            L"STANDARD.PAGES=%u\nSTANDARD.LOW_VECTOR=0x%02X\n"
            L"STANDARD.HIGH_VECTOR=0x%02X\nSTANDARD.ROWS=%u\n"
            L"STANDARD.COLS=%u\nSTANDARD.SYSMEM=0x%04X\n"
            L"STANDARD.BEHAVIOR=0x%02X\nSTANDARD.SYS_FLAGS=0x%02X\n",
            s->maxmem, s->minmem, s->MSflags, s->screen,
            (s->MSflags & 0x10u) ? L"YES" : L"NO", s->cPages,
            s->lowVector, s->highVector, s->rows, s->cols, s->sysmem,
            s->behavior, s->sysflags);
    if (view->w286) {
        wprintf(L"W286.MAX_XMS_KB=%u\nW286.MIN_XMS_KB=%u\nW286.FLAGS=0x%04X\n",
                view->w286->PfMaxXmsK, view->w286->PfMinXmsK, view->w286->PfW286Flags);
    }
    if (view->w386) {
        wprintf(L"W386.MAXMEM_KB=%u\nW386.MINMEM_KB=%u\n"
                L"W386.FOREGROUND_PRIORITY=%u\nW386.BACKGROUND_PRIORITY=%u\n"
                L"W386.MAX_EMS_KB=%u\nW386.MIN_EMS_KB=%u\n"
                L"W386.MAX_XMS_KB=%u\nW386.MIN_XMS_KB=%u\n"
                L"W386.FLAGS=0x%08lX\nW386.VIDEO_FLAGS=0x%08lX\n",
                view->w386->PfW386maxmem, view->w386->PfW386minmem,
                view->w386->PfFPriority, view->w386->PfBPriority,
                view->w386->PfMaxEMMK, view->w386->PfMinEMMK,
                view->w386->PfMaxXmsK, view->w386->PfMinXmsK,
                (unsigned long)view->w386->PfW386Flags,
                (unsigned long)view->w386->PfW386Flags2);
        print_oem(L"W386.ARGUMENTS", view->w386->PfW386params, sizeof(view->w386->PfW386params));
    }
    if (view->nt) {
        wprintf(L"NT31.FLAGS=0x%08lX\n", (unsigned long)view->nt->nt31Prop.dwWNTFlags);
        print_oem(L"CONFIG", view->nt->nt31Prop.achConfigFile, sizeof(view->nt->nt31Prop.achConfigFile));
        print_oem(L"AUTOEXEC", view->nt->nt31Prop.achAutoexecFile, sizeof(view->nt->nt31Prop.achAutoexecFile));
    }
}

int wmain(int argc, wchar_t **argv)
{
    PIF_VIEW view;
    BOOL ok;
    if (argc < 3) { usage(); return 64; }
    if (_wcsicmp(argv[1], L"create") == 0) {
        ok = create(argv[2], argc - 3, argv + 3);
        if (!ok) fwprintf(stderr, L"PIF.EXE: could not create %ls\n", argv[2]);
        return ok ? 0 : 1;
    }
    if (!read_file(argv[2], &view) || !parse(&view)) {
        fwprintf(stderr, L"PIF.EXE: invalid PIF %ls\n", argv[2]);
        dispose(&view); return 1;
    }
    if (_wcsicmp(argv[1], L"show") == 0) { inspect(&view); dispose(&view); return 0; }
    if (_wcsicmp(argv[1], L"update") == 0) {
        ok = argc > 3 && set_values(&view, argc - 3, argv + 3) && write_file(argv[2], view.bytes, view.size);
        dispose(&view); if (!ok) fwprintf(stderr, L"PIF.EXE: could not edit %ls\n", argv[2]);
        return ok ? 0 : 1;
    }
    dispose(&view); usage(); return 64;
}
