/* Authored standard PIF, using the existing original declaration only. */
#define WINNT 1
#include <windows.h>
#include <pif.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv)
{
    STDPIF standard={0};PIFEXTHDR header={0};HANDLE file;DWORD wrote;
    if(argc!=3 || strlen(argv[2])>=sizeof(standard.startfile))return 64;
    strcpy_s(standard.startfile,sizeof(standard.startfile),argv[2]);
    strcpy_s(standard.appname,sizeof(standard.appname),"Authored S7 batch probe");
    standard.MSflags=fDestroy;
    strcpy_s(header.extsig,sizeof(header.extsig),STDHDRSIG);
    header.extnxthdrfloff=0xffffu;
    file=CreateFileA(argv[1],GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 65;
    if(!WriteFile(file,&standard,sizeof(standard),&wrote,NULL) || wrote!=sizeof(standard) ||
       !WriteFile(file,&header,sizeof(header),&wrote,NULL) || wrote!=sizeof(header)){
        CloseHandle(file);return 66;
    }
    CloseHandle(file);return 0;
}
