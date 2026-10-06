/* Independently authored controlled native descendants. No product provider,
 * process scan or substituted broker. Named events/journal prove this run. */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
static HANDLE open_event(const wchar_t *name)
{return OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,name);}
static int append(const wchar_t *path,const char *tag,DWORD pid)
{
    char text[160];DWORD bytes=0;
    HANDLE file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return 2;
    int count=sprintf_s(text,"%s %lu width=%u hook=%u\r\n",tag,pid,(unsigned)(sizeof(void*)*8),
        GetModuleHandleW(sizeof(void*)==8 ? L"nthook64.dll" : L"nthook32.dll") ? 1u : 0u);
    BOOL written=count>0 && WriteFile(file,text,(DWORD)count,&bytes,NULL) && bytes==(DWORD)count;
    CloseHandle(file);return written ? 0 : 3;
}
typedef struct spawn_group {wchar_t **arguments;volatile LONG failures;} spawn_group;
static DWORD WINAPI spawn_fast(void *value)
{
    spawn_group *group=(spawn_group *)value;wchar_t **args=group->arguments;
    WCHAR image[MAX_PATH],command[2048];STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};DWORD code=0;
    BOOL ok=GetModuleFileNameW(NULL,image,MAX_PATH)!=0;
    if(ok)swprintf_s(command,L"\"%s\" leaf-fast \"%s\" \"%s\" unused \"%s\"",image,args[2],args[4],args[5]);
    if(ok)ok=CreateProcessW(image,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&child);
    if(ok) {
        CloseHandle(child.hThread);
        ok=WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0 &&
            GetExitCodeProcess(child.hProcess,&code) && code==23;
        CloseHandle(child.hProcess);
    }
    if(!ok)InterlockedIncrement(&group->failures);
    return ok ? 0 : 1;
}
int wmain(int argc,wchar_t **argv)
{
    if(argc!=6)return 2;
    if(!wcsncmp(argv[1],L"host",4)) {
        WCHAR image[MAX_PATH],launcher[MAX_PATH],command[2048],name[160];
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION target={0};DWORD code=0;
        if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 2;
        wcscpy_s(launcher,image);WCHAR *slash=wcsrchr(launcher,L'\\');if(!slash)return 2;*slash=0;
        wcscat_s(launcher,L"\\..\\..\\system32\\run16.exe");
        WCHAR canonical[MAX_PATH];DWORD canonical_chars=GetFullPathNameW(launcher,MAX_PATH,canonical,NULL);
        if(!canonical_chars || canonical_chars>=MAX_PATH)return 2;wcscpy_s(launcher,canonical);
        const WCHAR *mode=!wcscmp(argv[1],L"host-fast") ? L"parent-fast" :
            !wcscmp(argv[1],L"host-suspended") ? L"parent-suspended" :
            !wcscmp(argv[1],L"host-concurrent") ? L"parent-concurrent" : L"parent";
        swprintf_s(command,L"\"%s\" \"%s\" %s \"%s\" \"%s\" \"%s\" \"%s\"",
            launcher,image,mode,argv[2],argv[3],argv[4],argv[5]);
        if(!wcscmp(argv[1],L"host-cmd")) {
            WCHAR cmd[MAX_PATH],batch[MAX_PATH],text[2048];char encoded[4096];DWORD written=0;
            UINT length=sizeof(void*)==4 ? GetSystemWow64DirectoryW(cmd,MAX_PATH) : GetSystemDirectoryW(cmd,MAX_PATH);
            if(!length || length>=MAX_PATH)return 2;wcscat_s(cmd,L"\\cmd.exe");
            wcscpy_s(batch,image);WCHAR *end=wcsrchr(batch,L'\\');if(!end)return 2;
            wcscpy_s(end+1,MAX_PATH-(end+1-batch),L"L.CMD");
            swprintf_s(text,L"@echo off\r\n\"%s\" /d /c %s parent %s %s %s %s\r\nexit /b %%errorlevel%%\r\n",
                cmd,image,argv[2],argv[3],argv[4],argv[5]);
            int bytes=WideCharToMultiByte(CP_ACP,0,text,-1,encoded,sizeof(encoded),NULL,NULL);
            HANDLE file=CreateFileW(batch,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            if(file==INVALID_HANDLE_VALUE || bytes<2)return 2;
            BOOL ok=WriteFile(file,encoded,(DWORD)(bytes-1),&written,NULL) && written==(DWORD)(bytes-1);
            CloseHandle(file);if(!ok)return 3;
            swprintf_s(command,L"\"%s\" \"%s\" /d /c \"%s\"",launcher,cmd,batch);
        }
        if(!CreateProcessW(launcher,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&target))return 3;
        CloseHandle(target.hThread);
        if(WaitForSingleObject(target.hProcess,30000)!=WAIT_OBJECT_0)return 4;
        if(!GetExitCodeProcess(target.hProcess,&code))return 5;CloseHandle(target.hProcess);
        if(append(argv[5],"DIRECTRESULT",code))return 3;
        swprintf_s(name,L"%s-done",argv[3]);HANDLE done=open_event(name);
        swprintf_s(name,L"%s-host",argv[3]);HANDLE host=open_event(name);
        if(!done || !host)return 6;SetEvent(done);
        if(WaitForSingleObject(host,20000)!=WAIT_OBJECT_0)return 7;
        CloseHandle(done);CloseHandle(host);return (int)code;
    }
    HANDLE ready=open_event(argv[2]),stop=open_event(argv[3]);
    if(!ready || !stop)return 2;
    int code=0;
    if(!wcscmp(argv[1],L"leaf") || !wcscmp(argv[1],L"leaf-fast")) {
        if(append(argv[5],"CHILD",GetCurrentProcessId()))return 3;
        if(wcscmp(argv[1],L"leaf-fast"))SetEvent(ready);
        if(wcscmp(argv[1],L"leaf-fast") && WaitForSingleObject(stop,20000)!=WAIT_OBJECT_0)return 4;
        code=23;
    } else if(!wcscmp(argv[1],L"parent-concurrent")) {
        HANDLE threads[16]={0};spawn_group group={argv,0};
        if(append(argv[5],"ROOT",GetCurrentProcessId()))return 3;
        for(unsigned i=0;i<16;++i){threads[i]=CreateThread(NULL,0,spawn_fast,&group,0,NULL);if(!threads[i])return 5;}
        if(WaitForMultipleObjects(16,threads,TRUE,15000)!=WAIT_OBJECT_0)return 6;
        for(unsigned i=0;i<16;++i)CloseHandle(threads[i]);
        if(group.failures)return 7;SetEvent(ready);
        if(WaitForSingleObject(stop,20000)!=WAIT_OBJECT_0)return 4;
        code=37;
    } else {
        WCHAR image[MAX_PATH],command[2048];
        PROCESS_INFORMATION child={0};STARTUPINFOW startup={sizeof(startup)};
        if(!GetModuleFileNameW(NULL,image,MAX_PATH))return 3;
        BOOL fast=!wcscmp(argv[1],L"parent-fast"),suspended=!wcscmp(argv[1],L"parent-suspended");
        swprintf_s(command,L"\"%s\" %s \"%s\" \"%s\" unused \"%s\"",image,fast ? L"leaf-fast" : L"leaf",argv[2],argv[4],argv[5]);
        ULONGLONG before=GetTickCount64();
        if(!CreateProcessW(image,command,NULL,NULL,FALSE,suspended ? CREATE_SUSPENDED : 0,NULL,NULL,&startup,&child))return 5;
        if(append(argv[5],"CREATEMS",(DWORD)(GetTickCount64()-before)))return 3;
        if(append(argv[5],"ROOT",GetCurrentProcessId()))return 3;
        if(suspended) {
            WCHAR name[160];swprintf_s(name,L"%s-resume",argv[3]);HANDLE resume=open_event(name);
            if(!resume || append(argv[5],"CREATED",child.dwProcessId))return 6;
            SetEvent(ready);
            if(WaitForSingleObject(resume,15000)!=WAIT_OBJECT_0 || ResumeThread(child.hThread)==(DWORD)-1)return 7;
            CloseHandle(resume);
        }
        CloseHandle(child.hThread);
        if(fast) {
            DWORD actual=0;
            if(WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(child.hProcess,&actual))return 8;
            if(append(argv[5],"CHILDRESULT",actual))return 3;
            SetEvent(ready);
        }
        /* The test controls root completion independently of child lifetime. */
        if(WaitForSingleObject(stop,20000)!=WAIT_OBJECT_0)return 4;
        CloseHandle(child.hProcess);code=37;
    }
    CloseHandle(ready);CloseHandle(stop);return code;
}
