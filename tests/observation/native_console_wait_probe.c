/* Test-only: can the original NT wait consume the modern Console handle? */
#include <windows.h>
#include <stdio.h>

typedef LONG (NTAPI *native_wait_fn)(ULONG, HANDLE *, ULONG, BOOLEAN, LARGE_INTEGER *);
typedef LONG (NTAPI *native_alert_fn)(HANDLE);

int main(int argc, char **argv)
{
    HANDLE handles[2];
    LARGE_INTEGER zero;
    DWORD mode;
    LONG status;
    FILE *log;
    native_wait_fn wait;
    native_alert_fn alert;
    if (argc != 2 || fopen_s(&log, argv[1], "w")) return 1;
    wait = (native_wait_fn)GetProcAddress(GetModuleHandleA("ntdll.dll"),
        "NtWaitForMultipleObjects");
    alert = (native_alert_fn)GetProcAddress(GetModuleHandleA("ntdll.dll"),
        "NtAlertThread");
    handles[0] = GetStdHandle(STD_INPUT_HANDLE);
    handles[1] = CreateEvent(NULL, TRUE, TRUE, NULL);
    if (!wait || !alert || !handles[1] || !GetConsoleMode(handles[0], &mode)) {
        fprintf(log, "console-unavailable error=%lu\n", GetLastError());
        fclose(log); return 2;
    }
    zero.QuadPart = 0;
    status = wait(2, handles, 1, TRUE, &zero);
    fprintf(log, "native-console-and-signaled-event status=%08lX\n", (ULONG)status);
    if (status != 0 && status != 1) { fclose(log); return 3; }
    ResetEvent(handles[1]);
    status = alert(GetCurrentThread());
    if (status < 0) { fclose(log); return 4; }
    zero.QuadPart = -10000000;
    status = wait(1, &handles[1], 1, TRUE, &zero);
    fprintf(log, "native-pending-alert status=%08lX expected=00000101\n", (ULONG)status);
    CloseHandle(handles[1]);
    fclose(log);
    return status == 0x101 ? 0 : 5;
}
