#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef void (__cdecl *request_fn)(void);
static HANDLE start_worker, worker_done;
static volatile LONG *detach_wait_result;
static request_fn request_self;

__declspec(dllexport) void __cdecl probe_set_detach_wait(HANDLE start,
    HANDLE done, volatile LONG *result)
{ start_worker = start; worker_done = done; detach_wait_result = result; }

__declspec(dllexport) void __cdecl probe_set_self_request(request_fn fn)
{ request_self = fn; }

__declspec(dllexport) void __cdecl probe_callback(void)
{ if (request_self) request_self(); }

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    (void)instance; (void)reserved;
    if (reason == DLL_PROCESS_DETACH && detach_wait_result) {
        SetEvent(start_worker);
        /* Test-only bounded wait demonstrates why waiting here is unsafe. */
        *detach_wait_result = (LONG)WaitForSingleObject(worker_done, 1000);
    }
    return TRUE;
}
