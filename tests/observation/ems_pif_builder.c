#define WINNT 1
#include <windows.h>
#include <pif.h>

#include <stdio.h>
#include <string.h>

static int write_exact(HANDLE file, const void *data, DWORD bytes)
{
    DWORD written = 0;
    return WriteFile(file, data, bytes, &written, NULL) && written == bytes;
}

int main(int argc, char **argv)
{
    char pif_path[MAX_PATH], config_path[MAX_PATH], autoexec_path[MAX_PATH];
    STDPIF standard;
    PIFEXTHDR first, w386_header, nt_header;
    W386PIF30 w386;
    WNTPIF31 nt;
    HANDLE file;
    static const char config[] = "EMM=RAM\r\ndos=high, umb\r\ndevice=%SystemRoot%\\system32\\himem.sys\r\n";
    static const char autoexec[] = "@echo off\r\n";

    if (argc != 2 || snprintf(pif_path, sizeof(pif_path), "%s\\E30.PIF", argv[1]) < 0 ||
        snprintf(config_path, sizeof(config_path), "%s\\E30.NT", argv[1]) < 0 ||
        snprintf(autoexec_path, sizeof(autoexec_path), "%s\\E30AUTO.NT", argv[1]) < 0) return 64;
    memset(&standard, 0, sizeof(standard));
    memset(&first, 0, sizeof(first));
    memset(&w386_header, 0, sizeof(w386_header));
    memset(&nt_header, 0, sizeof(nt_header));
    memset(&w386, 0, sizeof(w386));
    memset(&nt, 0, sizeof(nt));

    strcpy_s(standard.appname, sizeof(standard.appname), "S30 EMS capability");
    strcpy_s(standard.startfile, sizeof(standard.startfile), "E30.COM");
    strcpy_s(first.extsig, sizeof(first.extsig), STDHDRSIG);
    first.extnxthdrfloff = (WORD)(sizeof(standard) + sizeof(first));
    strcpy_s(w386_header.extsig, sizeof(w386_header.extsig), W386HDRSIG30);
    w386_header.extnxthdrfloff = (WORD)(sizeof(standard) + sizeof(first) + sizeof(w386_header));
    w386_header.extfileoffset = (WORD)(sizeof(standard) + sizeof(first) + sizeof(w386_header) + sizeof(nt_header));
    w386_header.extsizebytes = (WORD)sizeof(w386);
    w386.PfMaxEMMK = 1024;
    w386.PfMinEMMK = 1024;
    w386.PfMaxXmsK = 1024;
    strcpy_s(nt_header.extsig, sizeof(nt_header.extsig), WNTHDRSIG31);
    nt_header.extnxthdrfloff = 0xffffu;
    nt_header.extfileoffset = (WORD)(w386_header.extfileoffset + sizeof(w386));
    nt_header.extsizebytes = (WORD)sizeof(nt);
    strcpy_s(nt.nt31Prop.achConfigFile, sizeof(nt.nt31Prop.achConfigFile), config_path);
    strcpy_s(nt.nt31Prop.achAutoexecFile, sizeof(nt.nt31Prop.achAutoexecFile), autoexec_path);

    file = CreateFileA(config_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, config, (DWORD)sizeof(config) - 1u)) return 1;
    CloseHandle(file);
    file = CreateFileA(autoexec_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, autoexec, (DWORD)sizeof(autoexec) - 1u)) return 1;
    CloseHandle(file);
    file = CreateFileA(pif_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, &standard, sizeof(standard)) ||
        !write_exact(file, &first, sizeof(first)) || !write_exact(file, &w386_header, sizeof(w386_header)) ||
        !write_exact(file, &nt_header, sizeof(nt_header)) || !write_exact(file, &w386, sizeof(w386)) ||
        !write_exact(file, &nt, sizeof(nt))) return 2;
    CloseHandle(file);
    return 0;
}
