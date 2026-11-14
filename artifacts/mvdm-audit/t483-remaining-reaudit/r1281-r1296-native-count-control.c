#include <windows.h>
#include <stdio.h>
#include <string.h>
static int run(unsigned mode, unsigned kind)
{
    char name[96], output[8] = {0}, request[8];
    HANDLE server, client, event;
    OVERLAPPED ov = {0};
    DWORD bytes = 0xdeadbeef, done = 0, n, error, wait;
    DWORD pipe_mode = PIPE_READMODE_MESSAGE;
    BOOL ok, completed;
    sprintf(name, "\\\\.\\pipe\\ntvdm-p127-%lu-%u-%u", GetCurrentProcessId(), mode, kind);
    server = CreateNamedPipeA(name, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 64, 64, 0, NULL);
    if (server == INVALID_HANDLE_VALUE) return 0;
    client = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    event = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (client == INVALID_HANDLE_VALUE || !event) return 0;
    if (!ConnectNamedPipe(server, NULL) && GetLastError() != ERROR_PIPE_CONNECTED) return 0;
    if (!SetNamedPipeHandleState(client, &pipe_mode, NULL, NULL)) return 0;
    ov.hEvent = event;
    if (kind == 0 && !WriteFile(server, "data", 4, &n, NULL)) return 0;
    if (kind < 2) ok = ReadFile(client, output, sizeof(output), mode ? NULL : &bytes, &ov);
    else ok = TransactNamedPipe(client, "go", 2, output, kind == 4 ? 2 : sizeof(output), mode ? NULL : &bytes, &ov);
    error = ok ? 0 : GetLastError();
    if (!ok && error != ERROR_IO_PENDING && error != ERROR_MORE_DATA) return 0;
    if (kind == 1 && !WriteFile(server, "data", 4, &n, NULL)) return 0;
    if (kind >= 2) {
        if (!ReadFile(server, request, sizeof(request), &n, NULL) || n != 2) return 0;
        if (kind == 3) { CloseHandle(server); server = INVALID_HANDLE_VALUE; }
        else if (!WriteFile(server, "data", 4, &n, NULL)) return 0;
    }
    wait = WaitForSingleObject(event, 5000);
    if (wait != WAIT_OBJECT_0) return 0;
    completed = GetOverlappedResult(client, &ov, &done, FALSE);
    error = completed ? 0 : GetLastError();
    printf("mode=%s case=%u issue=%u count=%lu wait=%lu complete=%u error=%lu done=%lu\n",
        mode ? "null" : "original", kind, ok, bytes, wait, completed, error, done);
    ok = kind == 3 ? (!completed && error == ERROR_BROKEN_PIPE) :
        kind == 4 ? (!completed && error == ERROR_MORE_DATA && done == 2 && !memcmp(output,"da",2)) :
        (completed && done == 4 && !memcmp(output,"data",4));
    CloseHandle(event); CloseHandle(client);
    if (server != INVALID_HANDLE_VALUE) CloseHandle(server);
    return ok;
}
int main(void)
{
    unsigned mode, kind, count = 0;
    for (mode = 0; mode < 2; ++mode)
        for (kind = 0; kind < 5; ++kind) {
            if (!run(mode, kind)) return 1;
            ++count;
        }
    printf("NATIVE_PIPE_COUNT_CONTROL cases=%u failures=0\n", count);
    return 0;
}
