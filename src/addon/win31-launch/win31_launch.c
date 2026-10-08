/* Installed Windows 3.1 launch preparation.  The two guest-image changes are
 * deliberately identity-bound to the owner-approved retail hashes; this is
 * not a general binary patch facility. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "pif_writer.h"

#pragma comment(lib, "bcrypt.lib")
#define KRNL_RETAIL "FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980"
#define KRNL_CANDIDATE "88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181"
#define WIN_RETAIL "6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5"
#define WIN_CANDIDATE "C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8"
#define MOUSE31_RELEASE "D6BA5380A2EDCDB33E0C36850FB6BDD0542D78EC03E982DBD5114A462B0A2F1A"

static BOOL join(wchar_t *out, DWORD cap, const wchar_t *a, const wchar_t *b) {
    return swprintf_s(out, cap, L"%ls%ls%ls", a, a[wcslen(a)-1] == L'\\' ? L"" : L"\\", b) >= 0;
}
static BOOL sha(const wchar_t *path, char out[65]) {
    BCRYPT_ALG_HANDLE a=NULL; BCRYPT_HASH_HANDLE h=NULL; BYTE d[32],buf[8192],*o=NULL; DWORD n=0,r=0,got; HANDLE f=INVALID_HANDLE_VALUE; BOOL ok=FALSE; NTSTATUS s; unsigned i;
    f=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL); if(f==INVALID_HANDLE_VALUE) goto done;
    s=BCryptOpenAlgorithmProvider(&a,BCRYPT_SHA256_ALGORITHM,NULL,0); if(s<0) goto done;
    if(BCryptGetProperty(a,BCRYPT_OBJECT_LENGTH,(PUCHAR)&n,sizeof(n),&r,0)<0) goto done;
    o=(BYTE*)HeapAlloc(GetProcessHeap(),0,n); if(!o || BCryptCreateHash(a,&h,o,n,NULL,0,0)<0) goto done;
    for (;;) {
        if (!ReadFile(f, buf, sizeof(buf), &got, NULL)) goto done;
        if (got == 0) break;
        if (BCryptHashData(h, buf, got, 0) < 0) goto done;
    }
    if(BCryptFinishHash(h,d,sizeof(d),0)<0) goto done;
    for(i=0;i<32;i++) sprintf_s(out+i*2,65-i*2,"%02X",d[i]); ok=TRUE;
done: if(h) BCryptDestroyHash(h); if(a) BCryptCloseAlgorithmProvider(a,0); if(o) HeapFree(GetProcessHeap(),0,o); if(f!=INVALID_HANDLE_VALUE) CloseHandle(f); return ok;
}

static BOOL ordinary_file(const wchar_t *path)
{
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
}

static BOOL has_hash(const wchar_t *path, const char *expected)
{
    char actual[65];
    return ordinary_file(path) && sha(path, actual) && !_stricmp(actual, expected);
}
static BOOL read_all(const wchar_t *p, BYTE **out, DWORD *len) {
    HANDLE f = CreateFileW(p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    DWORD got = 0;

    if (f == INVALID_HANDLE_VALUE) return FALSE;
    *len = GetFileSize(f, NULL);
    if (*len == INVALID_FILE_SIZE ||
        !(*out = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)*len + 1)) ||
        !ReadFile(f, *out, *len, &got, NULL) || got != *len) {
        CloseHandle(f);
        return FALSE;
    }
    CloseHandle(f);
    return TRUE;
}
static BOOL write_all(const wchar_t *p,const BYTE *b,DWORD n){HANDLE f=CreateFileW(p,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);DWORD w;BOOL ok=f!=INVALID_HANDLE_VALUE&&WriteFile(f,b,n,&w,NULL)&&w==n; if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);return ok;}
static DWORD u16(const BYTE*b,DWORD p){return b[p]|((DWORD)b[p+1]<<8);} static DWORD u32(const BYTE*b,DWORD p){return u16(b,p)|u16(b,p+2)<<16;}
static BOOL backup_once(const wchar_t *target,const wchar_t *backup,const char *expected){char h[65]; if(GetFileAttributesW(backup)==INVALID_FILE_ATTRIBUTES && !CopyFileW(target,backup,TRUE))return FALSE; return sha(backup,h)&&_stricmp(h,expected)==0;}
static BOOL existing_backup(const wchar_t *backup,const char *expected){char h[65];return GetFileAttributesW(backup)!=INVALID_FILE_ATTRIBUTES&&sha(backup,h)&&_stricmp(h,expected)==0;}
static BOOL replace_candidate(const wchar_t *target,BYTE *image,DWORD len,const char *expected){wchar_t temp[MAX_PATH];char h[65]={0}; if(swprintf_s(temp,MAX_PATH,L"%ls.T437",target)<0||!write_all(temp,image,len)||!sha(temp,h)||_stricmp(h,expected)!=0){fwprintf(stderr,L"WIN31LAUNCH: candidate identity check failed for %ls (got %hs)\n",target,h);DeleteFileW(temp);return FALSE;} if(!MoveFileExW(temp,target,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)){fwprintf(stderr,L"WIN31LAUNCH: cannot replace %ls (error %lu)\n",target,GetLastError());DeleteFileW(temp);return FALSE;}return TRUE;}
static BOOL patch_krnl(const wchar_t *target, const wchar_t *backup)
{
    static const BYTE before[] = { 0x33,0xdb,0x3c,0x0a,0x73,0x22,0x3c,0x03,0x77,0x12,0xbb,0x38,0,0x80,0xfc,0,0x74,0x16,0xbb,0x35,0,0x80,0xfc,0x1f,0x76,0x0e,0xeb,0,0x50,0xe8,0x23,0,0x8b,0xd8,0x58,0x83,0xfb,0xff,0x74,0x0b };
    BYTE after[sizeof(before)], *image = NULL;
    DWORD length;
    char hash[65];
    if (!sha(target, hash)) { fwprintf(stderr,L"WIN31LAUNCH: cannot hash KRNL386.EXE\n"); return FALSE; }
    if (!_stricmp(hash, KRNL_CANDIDATE)) {
        if (!existing_backup(backup, KRNL_RETAIL)) fwprintf(stderr,L"WIN31LAUNCH: candidate KRNL386 lacks a retail recovery copy\n");
        return existing_backup(backup, KRNL_RETAIL);
    }
    if (_stricmp(hash, KRNL_RETAIL)) { fwprintf(stderr,L"WIN31LAUNCH: unsupported KRNL386 identity %hs\n",hash); return FALSE; }
    if (!backup_once(target, backup, KRNL_RETAIL)) { fwprintf(stderr,L"WIN31LAUNCH: cannot preserve retail KRNL386\n"); return FALSE; }
    if (!read_all(target, &image, &length)) { fwprintf(stderr,L"WIN31LAUNCH: cannot read retail KRNL386\n"); return FALSE; }
    if (length < 0xcfed + sizeof(before) || memcmp(image + 0xcfed, before, sizeof(before))) {
        fwprintf(stderr,L"WIN31LAUNCH: retail KRNL386 byte contract mismatch\n");
        HeapFree(GetProcessHeap(), 0, image); return FALSE;
    }
    memset(after, 0x90, sizeof(after));
    after[0] = 0xbb; after[1] = 0x21; after[2] = 0; after[3] = 0xe9; after[4] = 0x22; after[5] = 0;
    if (!replace_candidate(target, (memcpy(image + 0xcfed, after, sizeof(after)), image), length, KRNL_CANDIDATE)) {
        HeapFree(GetProcessHeap(), 0, image); return FALSE;
    }
    HeapFree(GetProcessHeap(), 0, image); return TRUE;
}
/* T436's retained byte-for-byte candidate comparison establishes the only
 * permitted WIN386 delta at this retail identity.  Keep the former LE walk in
 * history/evidence; this installed-tree tool uses the stronger exact-image
 * contract, then verifies the complete candidate hash before replacement. */
static BOOL patch_win386(const wchar_t *target,const wchar_t *backup)
{
    static const BYTE before[10]={0x6a,0,0x6a,4,0xcd,0x20,0xa9,0,1,0};
    static const BYTE after[10]={0xba,0x21,0,0,0,0xe9,0x23,1,0,0};
    BYTE *image=NULL; DWORD length; char hash[65];
    if(!sha(target,hash)) return FALSE;
    if(!_stricmp(hash,WIN_CANDIDATE)) return existing_backup(backup,WIN_RETAIL);
    if(_stricmp(hash,WIN_RETAIL)||!backup_once(target,backup,WIN_RETAIL)||!read_all(target,&image,&length)||length<407671u||memcmp(image+407661u,before,sizeof(before))){if(image)HeapFree(GetProcessHeap(),0,image);return FALSE;}
    memcpy(image+407661u,after,sizeof(after));
    if(!replace_candidate(target,image,length,WIN_CANDIDATE)){HeapFree(GetProcessHeap(),0,image);return FALSE;}
    HeapFree(GetProcessHeap(),0,image); return TRUE;
}
static BOOL text(const wchar_t*p,const wchar_t*s){int n=WideCharToMultiByte(CP_ACP,0,s,-1,NULL,0,NULL,NULL);BYTE*b=(BYTE*)HeapAlloc(GetProcessHeap(),0,n);BOOL ok=b&&WideCharToMultiByte(CP_ACP,0,s,-1,(char*)b,n,NULL,NULL)&&write_all(p,b,(DWORD)n-1);if(b)HeapFree(GetProcessHeap(),0,b);return ok;}
static BOOL set_mouse_driver(const wchar_t *root, const wchar_t *driver)
{
    wchar_t ini[MAX_PATH] = L"", temporary[MAX_PATH] = L""; BYTE *bytes = NULL; DWORD length;
    char *begin, *section_end, *line, *line_end = NULL, ansi[MAX_PATH], *rewritten;
    DWORD wrote; HANDLE file;
    if (!join(ini, MAX_PATH, root, L"SYSTEM.INI") || !join(temporary, MAX_PATH, root, L"SYSTEM.INI.T437") ||
        !read_all(ini, &bytes, &length) || length > 1024u * 1024u ||
        !WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, driver, -1, ansi, MAX_PATH, NULL, NULL)) goto fail;
    begin = strstr((char *)bytes, "[boot]"); if (!begin) goto fail;
    section_end = strstr(begin + 6, "\n["); if (!section_end) section_end = (char *)bytes + length;
    for (line = begin; line < section_end; line = line_end + 1) {
        line_end = strchr(line, '\n'); if (!line_end || line_end > section_end) line_end = section_end;
        if (!_strnicmp(line, "mouse.drv=", 10)) break;
        if (line_end == section_end) break;
    }
    if (line >= section_end || _strnicmp(line, "mouse.drv=", 10)) goto fail;
    rewritten = (char *)HeapAlloc(GetProcessHeap(), 0, length + strlen(ansi) + 4); if (!rewritten) goto fail;
    CopyMemory(rewritten, bytes, (SIZE_T)(line - (char *)bytes));
    sprintf_s(rewritten + (line - (char *)bytes), length + strlen(ansi) + 4 - (SIZE_T)(line - (char *)bytes), "mouse.drv=%s\r\n%s", ansi, line_end + (line_end < section_end ? 1 : 0));
    file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, rewritten, (DWORD)strlen(rewritten), &wrote, NULL) || wrote != strlen(rewritten) || !CloseHandle(file) || !MoveFileExW(temporary, ini, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) { if (file != INVALID_HANDLE_VALUE) CloseHandle(file); HeapFree(GetProcessHeap(),0,rewritten); goto fail; }
    HeapFree(GetProcessHeap(),0,rewritten); HeapFree(GetProcessHeap(),0,bytes); return TRUE;
fail: if (bytes) HeapFree(GetProcessHeap(),0,bytes); DeleteFileW(temporary); return FALSE;
}

static BOOL preflight(const wchar_t *root, const wchar_t *driver,
                      const wchar_t *patch, const wchar_t *system)
{
    wchar_t path[MAX_PATH];
    wchar_t backup[MAX_PATH];
    char hash[65];
    DWORD attributes = GetFileAttributesW(root);
    DWORD patch_attributes = GetFileAttributesW(patch);

    if (attributes == INVALID_FILE_ATTRIBUTES ||
        !(attributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (attributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
        (patch_attributes != INVALID_FILE_ATTRIBUTES &&
         (!(patch_attributes & FILE_ATTRIBUTE_DIRECTORY) ||
          (patch_attributes & FILE_ATTRIBUTE_REPARSE_POINT))) ||
        !has_hash(driver, MOUSE31_RELEASE) ||
        !join(path, MAX_PATH, root, L"WIN.COM") || !ordinary_file(path) ||
        !join(path, MAX_PATH, root, L"SYSTEM.INI") || !ordinary_file(path) ||
        !join(path, MAX_PATH, system, L"KRNL386.EXE") || !ordinary_file(path) ||
        !sha(path, hash) || (_stricmp(hash, KRNL_RETAIL) && _stricmp(hash, KRNL_CANDIDATE))) {
        return FALSE;
    }
    if (!_stricmp(hash, KRNL_CANDIDATE)) {
        if (!join(backup, MAX_PATH, patch, L"KRNL386.ORIG") || !has_hash(backup, KRNL_RETAIL)) return FALSE;
    }
    if (!join(path, MAX_PATH, system, L"WIN386.EXE") || !ordinary_file(path) ||
        !sha(path, hash) || (_stricmp(hash, WIN_RETAIL) && _stricmp(hash, WIN_CANDIDATE))) {
        return FALSE;
    }
    if (!_stricmp(hash, WIN_CANDIDATE)) {
        if (!join(backup, MAX_PATH, patch, L"WIN386.ORIG") || !has_hash(backup, WIN_RETAIL)) return FALSE;
    }
    return TRUE;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t root[MAX_PATH], patch[MAX_PATH], system[MAX_PATH], path[MAX_PATH];
    wchar_t kernel[MAX_PATH], config[MAX_PATH], autoexec[MAX_PATH], driver[MAX_PATH];
    wchar_t command[MAX_PATH];
    int mode;

    if (argc != 5 || _wcsicmp(argv[1], L"--root") || _wcsicmp(argv[3], L"--driver")) {
        fwprintf(stderr, L"Usage: WIN31LAUNCH.EXE --root <installed-win31> --driver <MOUSE31.DRV>\n");
        return 64;
    }
    if (!GetFullPathNameW(argv[2], MAX_PATH, root, NULL) ||
        !GetFullPathNameW(argv[4], MAX_PATH, driver, NULL) ||
        !join(patch, MAX_PATH, root, L"PATCH") ||
        !join(system, MAX_PATH, root, L"SYSTEM") ||
        !preflight(root, driver, patch, system)) {
        fwprintf(stderr, L"WIN31LAUNCH: unsupported tree, linked input, candidate identity, or driver\n");
        return 1;
    }
    if (!CreateDirectoryW(patch, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) return 1;
    if (!join(kernel, MAX_PATH, system, L"KRNL386.EXE") ||
        !join(path, MAX_PATH, patch, L"KRNL386.ORIG") || !patch_krnl(kernel, path)) return 2;
    if (!join(path, MAX_PATH, system, L"WIN386.EXE") ||
        !join(kernel, MAX_PATH, patch, L"WIN386.ORIG") || !patch_win386(path, kernel)) return 3;
    if (!join(path, MAX_PATH, patch, L"MOUSE31.DRV") || !CopyFileW(driver, path, FALSE) ||
        !set_mouse_driver(root, path)) return 4;
    if (!join(config, MAX_PATH, patch, L"CONFIG.NT") ||
        !join(autoexec, MAX_PATH, patch, L"AUTOEXEC.NT") ||
        swprintf_s(command, MAX_PATH, L"dos=high, umb\r\ndevice=%%SystemRoot%%\\system32\\himem.sys\r\nfiles=128\r\ndosonly\r\n") < 0 ||
        !text(config, command) ||
        swprintf_s(command, MAX_PATH, L"@echo off\r\nSET TEMP=%%SystemRoot%%\\Temp\r\nSET TMP=%%SystemRoot%%\\Temp\r\nSET PATH=%ls;%ls\\SYSTEM;%%SystemRoot%%\\system32\r\n", root, root) < 0 ||
        !text(autoexec, command) || !join(kernel, MAX_PATH, root, L"WIN.COM")) return 5;
    for (mode = 0; mode < 2; ++mode) {
        const wchar_t *name = mode ? L"WIN386" : L"WINSTD";
        const wchar_t *arguments = mode ? L"/3" : L"/S";
        if (!join(path, MAX_PATH, patch, mode ? L"WIN386.PIF" : L"WINSTD.PIF") ||
            !win31_write_pif(path, mode ? L"Windows 3.1 386 Enhanced" : L"Windows 3.1 Standard",
                             kernel, root, arguments, config, autoexec, TRUE) ||
            !join(path, MAX_PATH, patch, mode ? L"WIN386.CMD" : L"WINSTD.CMD") ||
            swprintf_s(command, MAX_PATH,
                        L"@echo off\r\nsetlocal\r\npushd \"%%~dp0..\" || exit /b 3\r\ncall run16 \"%%~dp0%ls.PIF\"\r\nset result=%%errorlevel%%\r\npopd\r\nexit /b %%result%%\r\n",
                        name) < 0 || !text(path, command)) return 6;
    }
    wprintf(L"WIN31LAUNCH: prepared %ls\\PATCH\n", root);
    return 0;
}
