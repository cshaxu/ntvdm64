#include "wow_user_capture_bindings.h"
#include <stdio.h>
#include <wchar.h>

/* Link the production rtlinit object AND the worker's actual TLS provider.
 * No replacement capture, heap, conversion or TEB implementation. */
static unsigned errors;
static HANDLE ready, resume_thread;
static PWSTR other_buffer;
static DWORD thread_errors;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)

static DWORD WINAPI capture_thread(LPVOID unused)
{
    IN_STRING value;
    (void)unused;
    if (!RtlCaptureAnsiString(&value, "other thread", FALSE)) {
        thread_errors = 1;
        SetEvent(ready);
        return 1;
    }
    other_buffer = value.pstr->Buffer;
    SetEvent(ready);
    if (WaitForSingleObject(resume_thread, 5000) != WAIT_OBJECT_0 ||
        value.fAllocated || wcscmp(value.pstr->Buffer, L"other thread"))
        thread_errors = 1;
    return thread_errors;
}

static void capture_case(const char *text, BOOL force, BOOL allocated)
{
    IN_STRING value;
    WCHAR expected[256];
    BOOL ok = RtlCaptureAnsiString(&value, text, force);
    CHECK(ok);
    if (!ok) return;
    CHECK(value.fAllocated == allocated);
    if (text == NULL) {
        CHECK(value.pstr == &value.strCapture);
        CHECK(value.pstr->Buffer == NULL && value.pstr->Length == 0 &&
            value.pstr->MaximumLength == 0);
    } else {
        int count = MultiByteToWideChar(CP_ACP, 0, text, -1, expected, 256);
        CHECK(count > 0);
        CHECK(value.pstr->Length == (count - 1) * sizeof(WCHAR));
        CHECK(!wcscmp(value.pstr->Buffer, expected));
        CHECK(value.pstr == (allocated ? &value.strCapture :
            &NtCurrentTeb()->StaticUnicodeString));
    }
    if (value.fAllocated) CHECK(HeapFree(GetProcessHeap(), 0, value.strCapture.Buffer));
}

int __cdecl main(void)
{
    char boundary[131];
    char *too_long;
    IN_STRING main_value, rejected;
    HANDLE thread;
    DWORD result = 1;
    capture_case(NULL, FALSE, FALSE);
    capture_case(NULL, TRUE, FALSE);
    capture_case("", FALSE, FALSE);
    capture_case("class-name", FALSE, FALSE);
    capture_case("class-name", TRUE, TRUE);
    memset(boundary, 'x', sizeof(boundary));
    boundary[129] = 0;
    capture_case(boundary, FALSE, FALSE);
    boundary[129] = 'x'; boundary[130] = 0;
    capture_case(boundary, FALSE, TRUE);
    too_long = (char *)HeapAlloc(GetProcessHeap(), 0, 65536);
    CHECK(too_long != NULL);
    if (too_long) {
        memset(too_long, 'x', 65535); too_long[65535] = 0;
        CHECK(!RtlCaptureAnsiString(&rejected, too_long, FALSE));
        CHECK(!rejected.fAllocated);
        HeapFree(GetProcessHeap(), 0, too_long);
    }
    ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    resume_thread = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(ready && resume_thread);
    if (!ready || !resume_thread) return 1;
    CHECK(RtlCaptureAnsiString(&main_value, "main thread", FALSE));
    thread = CreateThread(NULL, 0, capture_thread, NULL, 0, NULL);
    CHECK(thread != NULL);
    if (!thread) return 1;
    CHECK(WaitForSingleObject(ready, 5000) == WAIT_OBJECT_0);
    CHECK(!thread_errors && other_buffer != main_value.pstr->Buffer);
    CHECK(!wcscmp(main_value.pstr->Buffer, L"main thread"));
    SetEvent(resume_thread);
    CHECK(WaitForSingleObject(thread, 5000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &result) && result == 0 && !thread_errors);
    CloseHandle(thread); CloseHandle(ready); CloseHandle(resume_thread);
    printf("WOW_USER_CAPTURE errors=%u real_tls=1 real_conversion=1\n", errors);
    return errors != 0;
}
