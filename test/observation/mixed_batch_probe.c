/* Authored native batch witness, never a product image or guest replacement. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <shellapi.h>

static int witness(int argc,wchar_t **argv)
{
    HANDLE file;char line[512],step[128],argument[128];DWORD wrote;
    unsigned bits=(unsigned)(sizeof(void *)*8);
    HANDLE ready=NULL,release=NULL,done=NULL;BOOL held=FALSE,releasing=FALSE;
    if(argc<4 || argc>5)return 64;
    if(!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,argv[2],-1,step,sizeof(step),NULL,NULL))return 65;
    argument[0]=0;
    if(argc==5 && !WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,argv[4],-1,argument,sizeof(argument),NULL,NULL))return 65;
    held=!wcscmp(argv[3],L"hold");releasing=!wcscmp(argv[3],L"release");
    if(held || releasing){
        WCHAR name[160];
        if(argc!=5)return 64;
        swprintf_s(name,160,L"%ls-R",argv[4]);ready=OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,name);
        swprintf_s(name,160,L"%ls-G",argv[4]);release=OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,name);
        swprintf_s(name,160,L"%ls-D",argv[4]);done=OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,name);
        if(!ready || !release || !done)return 69;
        if(releasing && WaitForSingleObject(ready,10000)!=WAIT_OBJECT_0)return 70;
    }
    sprintf_s(line,sizeof(line),"%s|bits=%u|arg=%s|hook=%u\r\n",step,bits,argument,
        GetModuleHandleW(bits==64?L"nthook64.dll":L"nthook32.dll")!=NULL);
    file=CreateFileW(argv[1],FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 66;
    if(!WriteFile(file,line,(DWORD)strlen(line),&wrote,NULL) || wrote!=strlen(line)){
        CloseHandle(file);return 67;
    }
    CloseHandle(file);
    if(held || releasing){
        DWORD bytes=0;
        if(held){
            if(!SetEvent(ready) || WaitForSingleObject(release,10000)!=WAIT_OBJECT_0)return 71;
            file=CreateFileW(argv[1],FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
            sprintf_s(line,sizeof(line),"GEND:%s\r\n",step);
            if(file==INVALID_HANDLE_VALUE || !WriteFile(file,line,(DWORD)strlen(line),&bytes,NULL) || bytes!=strlen(line))return 72;
            CloseHandle(file);if(!SetEvent(done))return 73;
        }else if(!SetEvent(release) || WaitForSingleObject(done,10000)!=WAIT_OBJECT_0)return 74;
        CloseHandle(ready);CloseHandle(release);CloseHandle(done);return held ? 37 : 0;
    }
    printf("S7-NATIVE-%u:%s:%s\n",bits,step,argument);
    fprintf(stderr,"S7-STDERR-%u:%s\n",bits,step);
    if(!wcscmp(argv[3],L"stdin")){
        char input[128];if(!fgets(input,sizeof(input),stdin) || strcmp(input,"S7-INPUT\n"))return 68;
        return 0;
    }
    return _wtoi(argv[3]);
}
#ifdef BATCH_GUI
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    int argc,result;LPWSTR *argv=CommandLineToArgvW(GetCommandLineW(),&argc);
    (void)instance;(void)previous;(void)command;(void)show;
    if(!argv)return 64;
    result=witness(argc,argv);LocalFree(argv);return result;
}
#else
int wmain(int argc,wchar_t **argv){return witness(argc,argv);}
#endif
