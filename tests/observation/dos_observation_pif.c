/* Authored test PIF. Original NT31 SUBSYS_DOS=1 selects DOS-only execution. */
#define WINNT 1
#include <windows.h>
#include <pif.h>
#include <stdio.h>
#include <string.h>
static BOOL write_part(HANDLE file,const void *bytes,DWORD count)
{
    DWORD written;return WriteFile(file,bytes,count,&written,NULL) && written==count;
}
int main(int argc,char **argv)
{
    STDPIF standard={0};PIFEXTHDR first={0},extension={0};WNTPIF31 nt={0};HANDLE file;
    if(argc!=5 || (strcmp(argv[4],"0") && strcmp(argv[4],"1")) ||
        strlen(argv[2])>=sizeof(standard.startfile) || strlen(argv[3])>=sizeof(standard.params))return 64;
    strcpy_s(standard.startfile,sizeof(standard.startfile),argv[2]);
    strcpy_s(standard.params,sizeof(standard.params),argv[3]);
    strcpy_s(standard.appname,sizeof(standard.appname),"DOS observation fixture");
    standard.MSflags=fDestroy;
    strcpy_s(first.extsig,sizeof(first.extsig),STDHDRSIG);
    first.extnxthdrfloff=(WORD)(sizeof(standard)+sizeof(first));
    strcpy_s(extension.extsig,sizeof(extension.extsig),WNTHDRSIG31);
    extension.extnxthdrfloff=0xffff;
    extension.extfileoffset=(WORD)(sizeof(standard)+sizeof(first)+sizeof(extension));
    extension.extsizebytes=(WORD)sizeof(nt);
    nt.nt31Prop.dwWNTFlags=argv[4][0]=='1' ? 1u : 0u;
    /* NT extension replaces these paths even when strings are empty.
     * Use existing immutable package startup files, not an empty override. */
    strcpy_s(nt.nt31Prop.achConfigFile,sizeof(nt.nt31Prop.achConfigFile),"Z:\\system32\\CONFIG.NT");
    strcpy_s(nt.nt31Prop.achAutoexecFile,sizeof(nt.nt31Prop.achAutoexecFile),"Z:\\system32\\AUTOEXEC.NT");
    file=CreateFileA(argv[1],GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 65;
    if(!write_part(file,&standard,sizeof(standard)) || !write_part(file,&first,sizeof(first)) ||
        !write_part(file,&extension,sizeof(extension)) || !write_part(file,&nt,sizeof(nt))) {
        CloseHandle(file);return 66;
    }
    CloseHandle(file);return 0;
}
