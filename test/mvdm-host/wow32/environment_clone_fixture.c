#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STATUS_SUCCESS ((NTSTATUS)0)
#define UNICODE_NULL ((WCHAR)0)
typedef NTSTATUS (NTAPI *CREATE)(BOOLEAN,PVOID*);
typedef NTSTATUS (NTAPI *DESTROY)(PVOID);
typedef NTSTATUS (NTAPI *CONVERT)(PWSTR,ULONG,PULONG,PCSTR,ULONG);
static CREATE create_env;static DESTROY destroy_env;static CONVERT convert_env;
static int fail_alloc,fail_convert,allocated,destroyed,checks,errors;
static void check(int ok, const char *name)
{
    ++checks;
    if (!ok) {
        ++errors;
        printf("FAIL %s\n", name);
    }
}
static void *allocate(HANDLE heap, DWORD flags, SIZE_T size)
{
    void *result;
    if (fail_alloc) return NULL;
    result = HeapAlloc(heap, flags, size);
    if (result) ++allocated;
    return result;
}
static NTSTATUS destroy(PVOID environment)
{
    ++destroyed;
    return destroy_env(environment);
}
static NTSTATUS convert(PWSTR buffer, ULONG size, PULONG written, PCSTR text, ULONG bytes)
{
    return fail_convert ? STATUS_INVALID_PARAMETER :
        convert_env(buffer, size, written, text, bytes);
}
#define HeapAlloc allocate
#define RtlCreateEnvironment create_env
#define RtlDestroyEnvironment destroy
#define RtlOemToUnicodeN convert
#include "environment_clone_exact.h"
#undef HeapAlloc
static void sample(char *block, const char *label)
{
    void *environment = NULL;
    DWORD size = WOWGetEnvironmentSize(block, NULL), i;
    NTSTATUS status = WOWCloneEnvironment(&environment, block);
    check(status == 0 && environment != NULL, label);
    if (!environment) return;
    for (i = 0; i < size; ++i) {
        if (((WCHAR *)environment)[i] != (unsigned char)block[i]) break;
    }
    check(i == size, "complete order/duplicate/raw-entry preservation");
    check(((WCHAR *)environment)[size] == 0, "double termination including empty block");
    check(destroy_env(environment) == 0, "native destroy accepts allocation");
}

int main(void)
{
    HMODULE native = GetModuleHandleA("ntdll.dll");
    void *environment;
    NTSTATUS status;
    char ordinary[] = "=C:=C:\\work\0A=one\0A=two\0Z=last\0RAW\0";
    char empty[] = {0, 0};
    char *large;
    int allocated_before, destroyed_before;
    create_env = (CREATE)GetProcAddress(native, "RtlCreateEnvironment");
    destroy_env = (DESTROY)GetProcAddress(native, "RtlDestroyEnvironment");
    convert_env = (CONVERT)GetProcAddress(native, "RtlOemToUnicodeN");
    if (!create_env || !destroy_env || !convert_env) return 2;
    sample(ordinary, "ordinary");
    sample(empty, "empty");
    large = malloc(40002);
    if (!large) return 2;
    memset(large, 'X', 40000);
    large[0] = 'A'; large[1] = '=';
    large[40000] = large[40001] = 0;
    sample(large, "long block exceeds USHORT unicode length");
    free(large);
    environment = NULL;
    status = WOWCloneEnvironment(&environment, NULL);
    check(status == 0 && environment != NULL, "inherit native environment");
    if (environment) check(destroy_env(environment) == 0, "destroy inherited");
    fail_alloc = 1;
    environment = (void *)1;
    destroyed_before = destroyed;
    status = WOWCloneEnvironment(&environment, ordinary);
    check(status == STATUS_NO_MEMORY && environment == (void *)1 &&
          destroyed == destroyed_before, "allocation failure leaves output untouched");
    fail_alloc = 0;
    fail_convert = 1;
    environment = (void *)1;
    allocated_before = allocated;
    destroyed_before = destroyed;
    status = WOWCloneEnvironment(&environment, ordinary);
    check(status == STATUS_INVALID_PARAMETER && environment == (void *)1 &&
          allocated == allocated_before + 1 && destroyed == destroyed_before + 1,
          "conversion failure destroys exactly once");
    printf("WOW_ENVIRONMENT checks=%d failures=%d\n", checks, errors);
    return errors ? 1 : 0;
}
