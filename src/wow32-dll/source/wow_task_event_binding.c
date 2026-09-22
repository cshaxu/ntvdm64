#include "wow_task_order_bindings.h"
#include <string.h>

/* ADAPTER-WOW-040: retain an event reference without publishing a kernel
 * object pointer. SDK public object-query records expose granted access and
 * type; no NT4 private object layout is copied. Original EVENT_ALL_ACCESS
 * must already be granted, never upgraded while duplicating the handle. */
BOOL WINAPI wow_task_reference_event(HANDLE source, HANDLE *retained)
{
    typedef NTSTATUS (NTAPI *query_object_fn)(HANDLE, OBJECT_INFORMATION_CLASS,
        PVOID, ULONG, PULONG);
    query_object_fn query;
    PUBLIC_OBJECT_BASIC_INFORMATION basic;
    PUBLIC_OBJECT_TYPE_INFORMATION *type = NULL;
    HANDLE copy = NULL;
    ULONG bytes = 0;
    BOOL result = FALSE;
    *retained = NULL;
    query = (query_object_fn)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryObject");
    if (!query || !DuplicateHandle(GetCurrentProcess(), source, GetCurrentProcess(),
            &copy, 0, FALSE, DUPLICATE_SAME_ACCESS)) return FALSE;
    if (query(copy, ObjectBasicInformation, &basic, sizeof(basic), NULL) < 0 ||
        (basic.GrantedAccess & EVENT_ALL_ACCESS) != EVENT_ALL_ACCESS) goto done;
    (void)query(copy, ObjectTypeInformation, NULL, 0, &bytes);
    if (bytes < sizeof(*type)) goto done;
    type = HeapAlloc(GetProcessHeap(), 0, bytes);
    if (!type || query(copy, ObjectTypeInformation, type, bytes, NULL) < 0) goto done;
    /* KeSetEvent cannot consume another object kind. The original caller
     * supplies an event; malformed host inputs must not enter that path. */
    if (type->TypeName.Length != 5 * sizeof(WCHAR) ||
        memcmp(type->TypeName.Buffer, L"Event", 5 * sizeof(WCHAR)) != 0) goto done;
    *retained = copy;
    copy = NULL;
    result = TRUE;
done:
    if (type) HeapFree(GetProcessHeap(), 0, type);
    if (copy) CloseHandle(copy);
    return result;
}
