/* Exercise the selected original nt_pif.obj with owned, valid PIF records.
 * No malformed-file tolerance or replacement parser is supplied. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <pif.h>
#include "nt_pif.h"
#include "mvdm_softpc_firmware.h"
/* Explicit file names must not enter root/default discovery. */
static unsigned configConversions;
int mvdm_softpc_system_find_file(const char *name,char *path,uint32_t bytes)
{(void)name;(void)path;(void)bytes;ExitProcess(99);return 0;}
uint32_t NtvdmGetWindowsDirectoryA(char *path,uint32_t bytes)
{(void)path;(void)bytes;ExitProcess(99);return 0;}
int mvdm_softpc_config_path_to_ansi(char *path,uint32_t bytes)
{
    /* ASCII identity substitute: verifies the caller boundary, not conversion. */
    if (!path || bytes != MAX_PATH+12) ExitProcess(99);
    ++configConversions; return 1;
}
extern VOID GetPIFConfigFiles(BOOL config, char *path);
extern DWORD dwWNTPifFlags;
ULONG DosSessionId;
BOOL bPifFastPaste;
PIF_DATA pfdata;
extern UCHAR WNTPifFgPr, WNTPifBgPr;
int RcMessageBox(UINT id, CHAR *a, CHAR *b, DWORD flags)
{ (void)id;(void)a;(void)b;(void)flags;ExitProcess(99);return 0; }
#define CHECK(x) do { if(!(x)){printf("PIF_PARSER_FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;} } while(0)
static unsigned char image[8192];
static DWORD used;
static WORD previous;
static void add(const char *signature, const void *body, WORD bytes)
{
    PIFEXTHDR header;
    WORD offset=(WORD)used;
    ((PIFEXTHDR *)(image+previous))->extnxthdrfloff=offset;
    memset(&header,0,sizeof(header));strcpy(header.extsig,signature);
    header.extnxthdrfloff=LASTHEADER;
    header.extfileoffset=offset+sizeof(header);header.extsizebytes=bytes;
    memcpy(image+used,&header,sizeof(header));used+=sizeof(header);
    memcpy(image+used,body,bytes);used+=bytes;previous=offset;
}
int main(void)
{
    char temp[MAX_PATH],path[MAX_PATH],configPath[MAX_PATH+12],autoPath[MAX_PATH+12];DWORD written;HANDLE file;
    W286PIF30 a;W386PIF30 b;WENHPIF40 w;WNTPIF31 nt;
    PIFEXTHDR header;PIF_DATA data;unsigned row;
    CHECK(GetTempPathA(MAX_PATH,temp));CHECK(GetTempFileNameA(temp,"pif",0,path));
    memset(&a,0,sizeof(a));memset(&b,0,sizeof(b));memset(&w,0,sizeof(w));memset(&nt,0,sizeof(nt));
    a.PfMaxXmsK=286;a.PfMinXmsK=28;
    b.PfMaxXmsK=386;b.PfMinXmsK=38;b.PfMaxEMMK=512;b.PfMinEMMK=64;
    b.PfFPriority=70;b.PfBPriority=20;b.PfW386Flags=1u<<12;
    w.tskProp.wIdleSensitivity=75;w.tskProp.flTsk=TSK_BACKGROUND;
    nt.nt31Prop.dwWNTFlags=SUBSYS_DOS;
    for(row=0;row<8;++row){
        memset(image,0,sizeof(image));memset(&header,0,sizeof(header));
        strcpy(header.extsig,STDHDRSIG);header.extnxthdrfloff=LASTHEADER;
        previous=sizeof(STDPIF);memcpy(image+previous,&header,sizeof(header));used=previous+sizeof(header);
        if(row==1)add(W286HDRSIG,&a,sizeof(a));
        if(row==2){add(W286HDRSIG,&a,sizeof(a));add(W386HDRSIG30,&b,sizeof(b));}
        if(row==3){add(W386HDRSIG30,&b,sizeof(b));add(W286HDRSIG,&a,sizeof(a));}
        if(row==4){add(W386HDRSIG30,&b,sizeof(b));add(WENHHDRSIG40,&w,sizeof(w));}
        if(row==5){add(WENHHDRSIG40,&w,sizeof(w));add(W386HDRSIG30,&b,sizeof(b));}
        if(row==6)add(WNTEXTSIG,&nt,sizeof(nt));
        /* Row7 proves defaults do not reset the process priority globals. */
        WNTPifFgPr=73;WNTPifBgPr=27;
        memset(&data,0,sizeof(data));data.IgnoreConfigAutoexec=1;
        file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
        CHECK(file!=INVALID_HANDLE_VALUE);CHECK(WriteFile(file,image,used,&written,NULL)&&written==used);CHECK(CloseHandle(file));
        CHECK(GetPIFData(&data,path));
        CHECK(!data.WinTitle&&!data.CmdLine&&!data.StartFile&&!data.StartDir);
        if(row==0||row==6||row==7){CHECK(data.xmsdes==DEFAULTXMSLMT&&data.emsdes==DEFAULTEMSLMT);CHECK(WNTPifFgPr==73&&WNTPifBgPr==27);}
        if(row==1)CHECK(data.xmsdes==286&&data.xmsreq==28);
        if(row>=2&&row<=5)CHECK(data.xmsdes==386&&data.xmsreq==38&&data.emsdes==512&&data.emsreq==64);
        if(row==2||row==3)CHECK(WNTPifFgPr==70&&WNTPifBgPr==40);
        if(row==4||row==5)CHECK(WNTPifFgPr==55&&WNTPifBgPr==55);
        if(row==6)CHECK(data.SubSysId==SUBSYS_DOS);
    }
    puts("PIF_ORIGINAL_PARSER_8_ROWS_OK");
    /* The first call sets fast paste. Later WNT and failed-open calls keep it. */
    memset(image,0,sizeof(image));
    previous=sizeof(STDPIF);memcpy(image+previous,&header,sizeof(header));used=previous+sizeof(header);
    b.PfW386Flags |= fINT16Paste;add(W386HDRSIG30,&b,sizeof(b));
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);CHECK(WriteFile(file,image,used,&written,NULL)&&written==used);CHECK(CloseHandle(file));
    CHECK(GetPIFData(&data,path)&&bPifFastPaste);
    /* Loading a WNT path retains the prior fast-paste and priority globals.
     * The second selection is ignored, the third replaces retained paths. */
    for(row=0;row<3;++row){
        strcpy(nt.nt31Prop.achConfigFile,row ? "SECOND.NT" : "FIRST.NT");
        strcpy(nt.nt31Prop.achAutoexecFile,row ? "SECOND.AUT" : "FIRST.AUT");
        memset(image,0,sizeof(image));previous=sizeof(STDPIF);
        memcpy(image+previous,&header,sizeof(header));used=previous+sizeof(header);
        add(WNTEXTSIG,&nt,sizeof(nt));
        file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
        CHECK(file!=INVALID_HANDLE_VALUE);CHECK(WriteFile(file,image,used,&written,NULL)&&written==used);CHECK(CloseHandle(file));
        memset(&data,0,sizeof(data));data.IgnoreConfigAutoexec=(row==1);
        CHECK(GetPIFData(&data,path));
        CHECK(bPifFastPaste&&WNTPifFgPr==70&&WNTPifBgPr==40);
        GetPIFConfigFiles(TRUE,configPath);GetPIFConfigFiles(FALSE,autoPath);
        CHECK(!strcmp(configPath,row==2 ? "SECOND.NT" : "FIRST.NT"));
        CHECK(!strcmp(autoPath,row==2 ? "SECOND.AUT" : "FIRST.AUT"));
        GetPIFConfigFiles(TRUE,configPath);
        CHECK(!strcmp(configPath,row==2 ? "SECOND.NT" : "FIRST.NT"));
    }
    CHECK(configConversions==9);
    /* Denied and missing opens reset PIF_DATA defaults/flags but not history. */
    file=CreateFileA(path,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);
    data.xmsdes=123;dwWNTPifFlags=0xffffffff;
    CHECK(!GetPIFData(&data,path));CHECK(CloseHandle(file));
    CHECK(data.xmsdes==DEFAULTXMSLMT&&dwWNTPifFlags==0&&bPifFastPaste);
    CHECK(DeleteFileA(path));data.xmsdes=123;dwWNTPifFlags=0xffffffff;
    CHECK(!GetPIFData(&data,path));
    CHECK(data.xmsdes==DEFAULTXMSLMT&&dwWNTPifFlags==0);
    CHECK(bPifFastPaste&&WNTPifFgPr==70&&WNTPifBgPr==40);
    GetPIFConfigFiles(TRUE,configPath);CHECK(!strcmp(configPath,"SECOND.NT"));
    puts("PIF_ORIGINAL_REPEAT_IGNORE_REPLACE_OPEN_FAILURE_OK");return 0;
}
