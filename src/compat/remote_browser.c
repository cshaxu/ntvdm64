#include "compat/include/remote_browser.h"

/* The NULL-transport public branch belongs to original apisrv.c. Only the
 * private Browser transport import remains unavailable here. */
WORD I_BrowserServerEnumForXactsrv(LPCWSTR transport,LPCWSTR client,ULONG native_level,
    USHORT client_level,PVOID buffer,WORD bytes,DWORD preferred,LPDWORD read,
    LPDWORD total,DWORD type,LPCWSTR domain,LPCWSTR first,PWORD converter)
{
    UNREFERENCED_PARAMETER(transport);UNREFERENCED_PARAMETER(client);
    UNREFERENCED_PARAMETER(native_level);UNREFERENCED_PARAMETER(client_level);
    UNREFERENCED_PARAMETER(buffer);UNREFERENCED_PARAMETER(bytes);
    UNREFERENCED_PARAMETER(preferred);UNREFERENCED_PARAMETER(read);
    UNREFERENCED_PARAMETER(total);UNREFERENCED_PARAMETER(type);
    UNREFERENCED_PARAMETER(domain);UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(converter);
    return ERROR_CALL_NOT_IMPLEMENTED;
}
