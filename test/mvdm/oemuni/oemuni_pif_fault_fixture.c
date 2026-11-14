/* Execute the tracked original TU; substitute only faultable test boundaries. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mvdm_softpc_firmware.h"
static HFILE WINAPI fault_open(LPCSTR,int);
static UINT WINAPI fault_read(HFILE,LPVOID,UINT);
static LONG WINAPI fault_seek(HFILE,LONG,int);
static HFILE WINAPI fault_close(HFILE);
static void *fault_malloc(size_t);
#define _lopen fault_open
#define _lread fault_read
#define _llseek fault_seek
#define _lclose fault_close
#define malloc fault_malloc
#include "mvdm-host/softpc.new/host/src/nt_pif.c"
#undef _lopen
#undef _lread
#undef _llseek
#undef _lclose
#undef malloc
ULONG DosSessionId;
BOOL bPifFastPaste;
PIF_DATA pfdata;
static unsigned char image[4096];
static UINT length,position,reads,seeks,closes,failRead,failSeek,allocFails,allocCalls,dialogs;
static int reply,active;
static HFILE WINAPI fault_open(LPCSTR path,int flags)
{if(strcmp(path,"owned.pif")||flags!=OF_READ||active)ExitProcess(98);active=1;position=0;return 7;}
static UINT WINAPI fault_read(HFILE file,LPVOID buffer,UINT bytes)
{if(file!=7||!active)ExitProcess(98);if(++reads==failRead)return (UINT)-1;
 if(position+bytes>length)ExitProcess(98);memcpy(buffer,image+position,bytes);position+=bytes;return bytes;}
static LONG WINAPI fault_seek(HFILE file,LONG offset,int origin)
{if(file!=7||!active||origin||offset<0||(UINT)offset>length)ExitProcess(98);
 if(++seeks==failSeek)return -1;position=(UINT)offset;return offset;}
static HFILE WINAPI fault_close(HFILE file)
{if(file!=7||!active)ExitProcess(98);active=0;++closes;return 0;}
static void *fault_malloc(size_t bytes)
{++allocCalls;if(allocFails){--allocFails;return NULL;}return malloc(bytes);}
int RcMessageBox(UINT id,CHAR *a,CHAR *b,ULONG flags)
{if(id!=EG_MALLOC_FAILURE||!a||!b||flags!=(RMB_ABORT|RMB_RETRY|RMB_IGNORE|RMB_ICON_STOP))ExitProcess(98);++dialogs;return reply;}
uint32_t NtvdmGetSystemDirectoryA(char *path,uint32_t bytes)
{(void)path;(void)bytes;ExitProcess(99);return 0;}
uint32_t NtvdmGetWindowsDirectoryA(char *path,uint32_t bytes)
{(void)path;(void)bytes;ExitProcess(99);return 0;}
int mvdm_softpc_config_path_to_ansi(char *path,uint32_t bytes)
{(void)path;(void)bytes;ExitProcess(99);return 0;}
#define CHECK(x) do{if(!(x)){printf("PIF_FAULT_FAIL line=%d\n",__LINE__);return 1;}}while(0)
int main(void)
{
 PIFEXTHDR first,extension,tail;W386PIF30 body;PIF_DATA data;unsigned row;void *p;
 memset(&first,0,sizeof(first));memset(&extension,0,sizeof(extension));memset(&tail,0,sizeof(tail));memset(&body,0,sizeof(body));
 strcpy(first.extsig,STDHDRSIG);first.extnxthdrfloff=sizeof(STDPIF)+sizeof(first);
 strcpy(extension.extsig,W386HDRSIG30);extension.extfileoffset=first.extnxthdrfloff+sizeof(extension);
 extension.extsizebytes=sizeof(body);extension.extnxthdrfloff=extension.extfileoffset+sizeof(body);
 tail.extnxthdrfloff=LASTHEADER;body.PfMaxXmsK=386;body.PfFPriority=70;body.PfBPriority=20;
 memcpy(image+sizeof(STDPIF),&first,sizeof(first));memcpy(image+first.extnxthdrfloff,&extension,sizeof(extension));
 memcpy(image+extension.extfileoffset,&body,sizeof(body));memcpy(image+extension.extnxthdrfloff,&tail,sizeof(tail));length=extension.extnxthdrfloff+sizeof(tail);
 for(row=0;row<9;++row){
  reads=seeks=closes=0;failRead=row<5?row+1:0;failSeek=row>=5?row-4:0;
  memset(&data,0,sizeof(data));data.IgnoreConfigAutoexec=1;
  CHECK(!GetPIFData(&data,"owned.pif"));CHECK(!active&&closes==1);
  /* Read5/seek4 occur after the386 record has already changed state. */
  CHECK(data.xmsdes==((row==4||row==8)?386:DEFAULTXMSLMT));
 }
 failRead=failSeek=reads=seeks=closes=0;memset(&data,0,sizeof(data));
 CHECK(GetPIFData(&data,"owned.pif")&&!active&&closes==1&&data.xmsdes==386);
 allocFails=1;allocCalls=dialogs=0;reply=RMB_RETRY;p=ch_malloc(16);
 CHECK(p&&allocCalls==2&&dialogs==1);free(p);
 allocFails=1;allocCalls=dialogs=0;reply=RMB_IGNORE;p=ch_malloc(16);
 CHECK(!p&&allocCalls==1&&dialogs==1);
 puts("PIF_ORIGINAL_9_IO_FAILURES_CLOSE_PARTIAL_STATE_RETRY_IGNORE_OK");return 0;
}
