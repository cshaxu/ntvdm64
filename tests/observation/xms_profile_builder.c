/* Test-only PIF: leave UMBs with XMS instead of DOS, retain HIMEM, and
 * reserve INT 15h extended memory through the original HIMEM option. */
#define WINNT 1
#include <windows.h>
#include <pif.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    STDPIF standard = {0};
    PIFEXTHDR first = {0}, header = {0}, nt_header = {0};
    W386PIF30 profile = {0};
    WNTPIF31 nt = {0};
    char path[MAX_PATH];
    FILE *file;
    const char *config = "EMM=RAM\r\ndevice=%SystemRoot%\\system32\\himem.sys /INT15=128\r\ndevice=%SystemRoot%\\tests\\X35UMB.SYS\r\nfiles=20\r\n";
    if (argc != 3 && argc != 4) return 64; /* optional default INT15 profile */
    if (argc == 4) {
        if (strcmp(argv[3], "--default-int15")) return 64;
        config = "EMM=RAM\r\ndos=high\r\ndevice=%SystemRoot%\\system32\\himem.sys\r\ndevice=%SystemRoot%\\tests\\X35UMB.SYS\r\nfiles=20\r\n";
    }
    strcpy_s(standard.appname, sizeof(standard.appname), "S35 XMS profile");
    if (sprintf_s(standard.startfile, sizeof(standard.startfile),
        "%s\\U35.COM", argv[2]) < 0) return 65;
    strcpy_s(first.extsig, sizeof(first.extsig), STDHDRSIG);
    first.extnxthdrfloff = sizeof(standard) + sizeof(first);
    strcpy_s(header.extsig, sizeof(header.extsig), W386HDRSIG30);
    header.extnxthdrfloff = first.extnxthdrfloff + sizeof(header);
    header.extfileoffset = header.extnxthdrfloff + sizeof(nt_header);
    header.extsizebytes = sizeof(profile);
    profile.PfMaxEMMK = profile.PfMinEMMK = 1024;
    profile.PfMaxXmsK = 4096;
    strcpy_s(nt_header.extsig, sizeof(nt_header.extsig), WNTHDRSIG31);
    nt_header.extnxthdrfloff = 0xffff;
    nt_header.extfileoffset = header.extfileoffset + sizeof(profile);
    nt_header.extsizebytes = sizeof(nt);
    if (sprintf_s(nt.nt31Prop.achConfigFile, sizeof(nt.nt31Prop.achConfigFile),
        "%s\\U35.NT", argv[2]) < 0 ||
        sprintf_s(nt.nt31Prop.achAutoexecFile, sizeof(nt.nt31Prop.achAutoexecFile),
        "%s\\U35AUTO.NT", argv[2]) < 0) return 65;
    if (sprintf_s(path, sizeof(path), "%s\\U35.NT", argv[1]) < 0 ||
        fopen_s(&file, path, "wb")) return 1;
    if (fwrite(config, 1, strlen(config), file) != strlen(config) || fclose(file)) return 1;
    if (sprintf_s(path, sizeof(path), "%s\\U35AUTO.NT", argv[1]) < 0 ||
        fopen_s(&file, path, "wb")) return 1;
    if (fputs("@echo off\r\necho S35_XMS_PROFILE_LOADED\r\n", file) < 0 || fclose(file)) return 1;
    if (sprintf_s(path, sizeof(path), "%s\\U35.PIF", argv[1]) < 0 ||
        fopen_s(&file, path, "wb")) return 1;
#define WRITE(record) if (fwrite(&(record), sizeof(record), 1, file) != 1) return 1
    WRITE(standard); WRITE(first); WRITE(header); WRITE(nt_header);
    WRITE(profile); WRITE(nt);
    return fclose(file) ? 1 : 0;
}
