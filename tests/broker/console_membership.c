/* Formal Console membership mechanism; not a broker identity provider. */
#include <windows.h>
#include "broker/console_membership.h"
#include "broker/console_probe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static HANDLE ready;

static BOOL probe(const char *image,DWORD caller,DWORD *candidates,BYTE *expected,DWORD version,DWORD count)
{
    SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};
    HANDLE input=NULL,writer=NULL,reader=NULL,output=NULL,handles[2];
    STARTUPINFOEXA startup={0};
    PROCESS_INFORMATION process={0};
    BROKER_CONSOLE_PROBE_REQUEST request={version,caller,count,0};
    BROKER_CONSOLE_PROBE_REPLY reply;
    BYTE wire[sizeof(reply)+BROKER_CONSOLE_PROBE_MAX_CANDIDATES];
    SIZE_T size=0;
    DWORD done,code,available,received=0;
    ULONGLONG deadline;
    BOOL ok=FALSE;
    char command[2048];
    if (!CreatePipe(&input,&writer,&sa,0) || !CreatePipe(&reader,&output,&sa,0)) goto cleanup;
    if (!SetHandleInformation(writer,HANDLE_FLAG_INHERIT,0) ||
        !SetHandleInformation(reader,HANDLE_FLAG_INHERIT,0)) goto cleanup;
    InitializeProcThreadAttributeList(NULL,1,0,&size);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,size);
    if (!startup.lpAttributeList) goto cleanup;
    if (!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&size)) {
        HeapFree(GetProcessHeap(),0,startup.lpAttributeList);startup.lpAttributeList=NULL;goto cleanup;
    }
    handles[0]=input; handles[1]=output;
    if (!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        handles,sizeof(handles),NULL,NULL)) goto cleanup;
    startup.StartupInfo.cb=sizeof(startup);
    startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput=input;
    startup.StartupInfo.hStdOutput=startup.StartupInfo.hStdError=output;
    if (sprintf_s(command,sizeof(command),"\"%s\" --internal-console-probe",image)<0) goto cleanup;
    if (!CreateProcessA(image,command,NULL,NULL,TRUE,DETACHED_PROCESS|EXTENDED_STARTUPINFO_PRESENT,
        NULL,NULL,&startup.StartupInfo,&process)) goto cleanup;
    CloseHandle(input);input=NULL; CloseHandle(output);output=NULL;
    if (!WriteFile(writer,&request,sizeof(request),&done,NULL) || done!=sizeof(request)) goto cleanup;
    if (version==BROKER_CONSOLE_PROBE_VERSION &&
        count && (!WriteFile(writer,candidates,count*sizeof(DWORD),&done,NULL) || done!=count*sizeof(DWORD))) goto cleanup;
    CloseHandle(writer);writer=NULL;
    /* Drain while the child is alive: the largest valid response exceeds a
     * default anonymous pipe buffer, so waiting for exit first can deadlock. */
    deadline=GetTickCount64()+5000;
    for (;;) {
        if (!PeekNamedPipe(reader,NULL,0,NULL,&available,NULL)) {
            if (GetLastError()==ERROR_BROKEN_PIPE) break;
            goto cleanup;
        }
        if (available) {
            if (available>sizeof(wire)-received) goto cleanup;
            if (!ReadFile(reader,wire+received,available,&done,NULL) || !done) goto cleanup;
            received+=done;
            continue;
        }
        if (GetTickCount64()>=deadline) goto cleanup;
        WaitForSingleObject(process.hProcess,10);
    }
    if (WaitForSingleObject(process.hProcess,5000)!=WAIT_OBJECT_0) goto cleanup;
    if (!GetExitCodeProcess(process.hProcess,&code) ||
        code!=(version==BROKER_CONSOLE_PROBE_VERSION ? 0u : (DWORD)ERROR_INVALID_DATA)) goto cleanup;
    if (received<sizeof(reply)) goto cleanup;
    memcpy(&reply,wire,sizeof(reply));
    if (reply.version!=BROKER_CONSOLE_PROBE_VERSION || reply.reserved || reply.status!=code) goto cleanup;
    if (code) {ok=reply.count==0 && received==sizeof(reply);goto cleanup;}
    if (reply.count!=count || received!=sizeof(reply)+count ||
        (count && memcmp(wire+sizeof(reply),expected,count))) goto cleanup;
    ok=TRUE;
cleanup:
    if (process.hProcess) {
        if (WaitForSingleObject(process.hProcess,0)!=WAIT_OBJECT_0) {
            TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);ok=FALSE;
        }
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
    }
    if (startup.lpAttributeList) {DeleteProcThreadAttributeList(startup.lpAttributeList);HeapFree(GetProcessHeap(),0,startup.lpAttributeList);}
    if (input) CloseHandle(input);if (output) CloseHandle(output);
    if (reader) CloseHandle(reader);if (writer) CloseHandle(writer);
    return ok;
}

static BOOL child(char *image, char *event, DWORD flags, PROCESS_INFORMATION *p)
{
    STARTUPINFOA startup = {sizeof(startup)};
    char command[2048];
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    if (sprintf_s(command, sizeof(command), "\"%s\" wait %s", image, event) < 0) return FALSE;
    ResetEvent(ready);
    if (!CreateProcessA(image, command, NULL, NULL, FALSE, flags,
        NULL, NULL, &startup, p)) return FALSE;
    return WaitForSingleObject(ready, 5000) == WAIT_OBJECT_0;
}

int main(int argc, char **argv)
{
    HANDLE stop;
    PROCESS_INFORMATION a = {0}, b = {0}, c = {0};
    char image[MAX_PATH], event[128], readyName[160];
    int result = 1;
    DWORD code, i;
    DWORD candidates[3];
    BYTE members[3] = {7,7,7};
    DWORD many[BROKER_CONSOLE_PROBE_MAX_CANDIDATES];
    BYTE expected[BROKER_CONSOLE_PROBE_MAX_CANDIDATES];
    PROCESS_INFORMATION *children[] = {&a, &b, &c};
    if (argc == 3 && !strcmp(argv[1], "wait")) {
        stop = OpenEventA(SYNCHRONIZE, FALSE, argv[2]);
        if (!stop) return 10;
        sprintf_s(readyName, sizeof(readyName), "%s-ready", argv[2]);
        ready = OpenEventA(EVENT_MODIFY_STATE, FALSE, readyName);
        if (!ready || !SetEvent(ready)) return 12;
        CloseHandle(ready);
        code = WaitForSingleObject(stop, 30000);
        CloseHandle(stop);
        return code == WAIT_OBJECT_0 ? 0 : 11;
    }
    if (!GetModuleFileNameA(NULL, image, sizeof(image))) return 2;
    sprintf_s(event, sizeof(event), "Local\\ntvdm-console-fixture-%lu-%lu",
        GetCurrentProcessId(), GetTickCount());
    stop = CreateEventA(NULL, TRUE, FALSE, event);
    if (!stop || GetLastError() == ERROR_ALREADY_EXISTS) return 3;
    sprintf_s(readyName, sizeof(readyName), "%s-ready", event);
    ready = CreateEventA(NULL, TRUE, FALSE, readyName);
    if (!ready) return 3;
    /* Only this test controller detaches. It never changes a user's process. */
    FreeConsole();
    if (!child(image, event, CREATE_NEW_CONSOLE, &a)) goto done;
    if (!AttachConsole(a.dwProcessId)) goto done;
    if (!child(image, event, 0, &b)) goto done;
    candidates[0]=a.dwProcessId; candidates[1]=b.dwProcessId;
    if (broker_console_membership(a.dwProcessId,candidates,2,members)!=ERROR_ACCESS_DENIED ||
        members[0]!=7 || members[1]!=7) goto done;
    FreeConsole();
    if (!child(image, event, CREATE_NEW_CONSOLE, &c)) goto done;
    candidates[2]=c.dwProcessId;
    if (broker_console_membership(b.dwProcessId,candidates,3,members) ||
        members[0]!=1 || members[1]!=1 || members[2]!=0) goto done;
    if (broker_console_membership(c.dwProcessId,candidates,3,members) ||
        members[0]!=0 || members[1]!=0 || members[2]!=1) goto done;
    /* Successful queries leave this controller detached. Invalid requests
     * neither attach nor publish partial output. */
    if (GetConsoleProcessList(&code,1)) goto done;
    if (broker_console_membership(0,candidates,3,members)!=ERROR_INVALID_PARAMETER ||
        members[0]!=0 || members[1]!=0 || members[2]!=1) goto done;
    if (argc!=2) goto done;
    if (!probe(argv[1],c.dwProcessId,candidates,members,BROKER_CONSOLE_PROBE_VERSION,3) ||
        !probe(argv[1],c.dwProcessId,candidates,members,0,3) ||
        !probe(argv[1],c.dwProcessId,NULL,NULL,BROKER_CONSOLE_PROBE_VERSION,0)) goto done;
    for (i=0;i<BROKER_CONSOLE_PROBE_MAX_CANDIDATES;++i) {
        many[i]=candidates[i%3];expected[i]=members[i%3];
    }
    if (!probe(argv[1],c.dwProcessId,many,expected,BROKER_CONSOLE_PROBE_VERSION,
        BROKER_CONSOLE_PROBE_MAX_CANDIDATES)) goto done;
    result = 0;
done:
    FreeConsole();
    SetEvent(stop);
    for (i = 0; i < 3; ++i) if (children[i]->hProcess) {
        if (WaitForSingleObject(children[i]->hProcess, 5000) != WAIT_OBJECT_0) {
            TerminateProcess(children[i]->hProcess, 99); /* Exact owned child only. */
            WaitForSingleObject(children[i]->hProcess, 5000);
            result = 4;
        }
        if (!GetExitCodeProcess(children[i]->hProcess, &code) || code != 0) result = 5;
        CloseHandle(children[i]->hThread); CloseHandle(children[i]->hProcess);
    }
    CloseHandle(stop);
    CloseHandle(ready);
    if (!result) puts("PASS: formal membership, same/different Console, attached-controller refusal, unchanged failure output, detached return, owned-child normal exit");
    else fprintf(stderr, "FAIL console membership result=%d error=%lu\n", result, GetLastError());
    return result;
}
