/* run16 is the public CreateProcess CLI composition, never a VDM worker.
 * BaseInit suffix declarations/values: original basedll.h and baseinit.c.
 * Original image classification is linked from opennt-host, not reimplemented.
 * S3 WIP: the VDM broker branch is not integrated and fails explicitly. */
#include <nt.h>
#include <base_classifier.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command, int show)
{
    LPWSTR *arguments;
    PWSTR childCommand;
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    DWORD type, result=ERROR_INVALID_PARAMETER;
    int count;
    size_t bytes;
    (void)instance; (void)previous; (void)show;
    if (!command || !*command) {
        fputs("Usage: run16.exe <binary> [arguments]\n",stderr);
        return ERROR_INVALID_PARAMETER;
    }
    /* CRT supplies the untouched tail after run16's own executable name.
     * Decode only to identify the target; never rebuild the guest/native tail. */
    arguments=CommandLineToArgvW(command,&count);
    if (!arguments) return (int)GetLastError();
    RtlInitUnicodeString(&BaseDotComSuffixName,L".com");
    RtlInitUnicodeString(&BaseDotPifSuffixName,L".pif");
    RtlInitUnicodeString(&BaseDotExeSuffixName,L".exe");
    if (!count || !*arguments[0]) goto done;
    if (!OpenNtBaseGetBinaryTypeW(arguments[0],&type)) {
        result=GetLastError();
        goto done;
    }
    if (type!=SCS_32BIT_BINARY) {
        result=ERROR_NOT_SUPPORTED;
        fputs("run16: VDM broker dispatch is not integrated in this build.\n",stderr);
        goto done;
    }
    bytes=(wcslen(command)+1)*sizeof(WCHAR);
    childCommand=HeapAlloc(GetProcessHeap(),0,bytes);
    if (!childCommand) { result=ERROR_NOT_ENOUGH_MEMORY; goto done; }
    memcpy(childCommand,command,bytes);
    startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    if (!CreateProcessW(arguments[0],childCommand,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child))
        result=GetLastError();
    else {
        CloseHandle(child.hThread);
        if (WaitForSingleObject(child.hProcess,INFINITE)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(child.hProcess,&result)) result=GetLastError();
        CloseHandle(child.hProcess);
    }
    HeapFree(GetProcessHeap(),0,childCommand);
done:
    LocalFree(arguments);
    return (int)result;
}
