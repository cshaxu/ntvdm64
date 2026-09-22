/* Test-only PIF that clears fINT16Paste.  This makes the original COMMAND
 * keyboard-layout service take its registry-backed path even for US layout;
 * the guest media and product PIF parser are untouched. */
#define WINNT 1
#include <windows.h>
#include <pif.h>
#include <stdio.h>
#include <string.h>

static int write_exact(HANDLE file, const void *data, DWORD bytes)
{
    DWORD written;
    return WriteFile(file,data,bytes,&written,NULL) && written==bytes;
}

int main(int argc, char **argv)
{
    STDPIF standard={0};
    PIFEXTHDR first={0},w386_header={0},nt_header={0};
    W386PIF30 w386={0};
    WNTPIF31 nt={0};
    char path[MAX_PATH],runtime[MAX_PATH],*slash;
    HANDLE file;

    if (argc!=3 || sprintf_s(path,sizeof(path),"%s\\S45KEY.PIF",argv[1])<0)
        return 64;
    strcpy_s(runtime,sizeof(runtime),argv[1]);
    slash=strrchr(runtime,'\\');
    if (!slash) return 64;
    *slash='\0';
    strcpy_s(standard.appname,sizeof(standard.appname),"S45 keyboard profile");
    /* The initial original COMMAND consumes the PIF and calls its keyboard
     * service before it executes this target.  Start MEM directly: nesting a
     * second COMMAND would exercise a separate command-reentry contract and
     * cannot be used as evidence for this one keyboard-reader route. */
    if (strcpy_s(standard.startfile,sizeof(standard.startfile),argv[2])!=0)
        return 64;
    /* A direct PIF starts a separate Console.  Ask the original PIF policy
     * to close that Console after COMMAND's finite /c child exits, rather
     * than retaining a deliberately inactive test worker forever. */
    standard.MSflags|=0x10;
    strcpy_s(first.extsig,sizeof(first.extsig),STDHDRSIG);
    first.extnxthdrfloff=(WORD)(sizeof(standard)+sizeof(first));
    strcpy_s(w386_header.extsig,sizeof(w386_header.extsig),W386HDRSIG30);
    w386_header.extnxthdrfloff=(WORD)(first.extnxthdrfloff+sizeof(w386_header));
    w386_header.extfileoffset=(WORD)(w386_header.extnxthdrfloff+sizeof(nt_header));
    w386_header.extsizebytes=(WORD)sizeof(w386);
    /* Start from the original editor's default resource profile.  The PIF
     * extension is authoritative, so zero-initializing these fields would
     * deliberately disable XMS/EMS rather than inherit product defaults. */
    w386.PfFPriority=100;
    w386.PfBPriority=50;
    w386.PfMaxEMMK=1024;
    w386.PfMinEMMK=0;
    w386.PfMaxXmsK=1024;
    w386.PfMinXmsK=0;
    /* Change only the keyboard-policy bit: unlike the usual profile, no
     * fINT16Paste.  Keep windowed mode for a modern Console test. */
    w386.PfW386Flags=fPollingDetect;
    strcpy_s(nt_header.extsig,sizeof(nt_header.extsig),WNTHDRSIG31);
    nt_header.extnxthdrfloff=0xffffu;
    nt_header.extfileoffset=(WORD)(w386_header.extfileoffset+sizeof(w386));
    nt_header.extsizebytes=(WORD)sizeof(nt);
    /* GetPIFData treats a present NT extension as authoritative for these
     * paths.  Leave neither blank: that would test a malformed startup
     * profile rather than COMMAND's non-fast-paste keyboard branch. */
    if (sprintf_s(nt.nt31Prop.achConfigFile,sizeof(nt.nt31Prop.achConfigFile),
            "%s\\config.nt",runtime)<0 ||
        sprintf_s(nt.nt31Prop.achAutoexecFile,sizeof(nt.nt31Prop.achAutoexecFile),
            "%s\\autoexec.nt",runtime)<0) return 64;
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE || !write_exact(file,&standard,sizeof(standard)) ||
        !write_exact(file,&first,sizeof(first)) || !write_exact(file,&w386_header,sizeof(w386_header)) ||
        !write_exact(file,&nt_header,sizeof(nt_header)) || !write_exact(file,&w386,sizeof(w386)) ||
        !write_exact(file,&nt,sizeof(nt))) return 1;
    CloseHandle(file);
    return 0;
}
