#include "compat/include/base_resource.h"
#include <stdio.h>

static unsigned errors, duplicates, closes;
static int context;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "resource failure line %u\n", __LINE__); } } while (0)

static NTSTATUS duplicate(void *ctx, HANDLE sp, HANDLE source, HANDLE tp,
    PHANDLE output, ACCESS_MASK access, ULONG attributes, ULONG options)
{
    CHECK(ctx == &context && sp == (HANDLE)1 && source == (HANDLE)2 &&
        tp == (HANDLE)3 && access == 4 && attributes == 5 && options == 6);
    ++duplicates;
    *output = (HANDLE)7;
    return (NTSTATUS)0xc0000022L;
}

static NTSTATUS close_resource(void *ctx, HANDLE handle)
{
    CHECK(ctx == &context && handle == (HANDLE)8);
    ++closes;
    return (NTSTATUS)0xc0000008L;
}

static DWORD WINAPI independent_thread(void *unused)
{
    (void)unused;
    return OpenNtBaseBindResources(NULL) != NULL;
}

int main(void)
{
    OPENNT_BASE_RESOURCE_BINDING bound = { &context, duplicate, close_resource };
    OPENNT_BASE_RESOURCE_BINDING missing = { NULL, NULL, NULL };
    HANDLE event, copy = NULL, thread;
    DWORD code = 99;
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(event != NULL);
    CHECK(OpenNtBaseBindResources(NULL) == NULL);
    CHECK(OpenNtBaseDuplicateObject(GetCurrentProcess(), event,
        GetCurrentProcess(), &copy, 0, 0, DUPLICATE_SAME_ACCESS) == 0);
    CHECK(copy && SetEvent(copy) && WaitForSingleObject(event, 0) == WAIT_OBJECT_0);
    CHECK(OpenNtBaseClose(copy) == 0);
    CHECK(OpenNtBaseDuplicateObject(NULL, event, GetCurrentProcess(),
        &copy, 0, 0, DUPLICATE_SAME_ACCESS) < 0);
    CHECK(OpenNtBaseBindResources(&bound) == NULL);
    copy = NULL;
    CHECK(OpenNtBaseDuplicateObject((HANDLE)1, (HANDLE)2, (HANDLE)3,
        &copy, 4, 5, 6) == (NTSTATUS)0xc0000022L && copy == (HANDLE)7);
    CHECK(OpenNtBaseClose((HANDLE)8) == (NTSTATUS)0xc0000008L);
    CHECK(OpenNtBaseBindResources(&missing) == &bound);
    CHECK(OpenNtBaseClose(event) == (NTSTATUS)0xc00000bbL);
    CHECK(OpenNtBaseDuplicateObject(GetCurrentProcess(), event,
        GetCurrentProcess(), &copy, 0, 0, DUPLICATE_SAME_ACCESS) == (NTSTATUS)0xc00000bbL);
    CHECK(WaitForSingleObject(event, 0) == WAIT_OBJECT_0);
    CHECK(OpenNtBaseBindResources(&bound) == &missing);
    thread = CreateThread(NULL, 0, independent_thread, NULL, 0, NULL);
    CHECK(thread && WaitForSingleObject(thread, 5000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &code) && code == 0);
    if (thread) CloseHandle(thread);
    CHECK(OpenNtBaseBindResources(NULL) == &bound);
    CHECK(OpenNtBaseClose(event) == 0);
    CHECK(duplicates == 1 && closes == 1);
    printf("BASE_RESOURCE errors=%u duplicates=%u closes=%u\n", errors, duplicates, closes);
    return errors != 0;
}
