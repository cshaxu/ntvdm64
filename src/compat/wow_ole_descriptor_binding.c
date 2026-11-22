/* OPENNT-HOST-297: bind original OLE conversions to existing WOW owners.
 * No OLE activation, guest component or second memory owner is introduced. */
#include "include/wow_ole_descriptor_private.h"
#include "include/wow_ole_descriptor_provider.h"
BYTE g_abLeadTable[256];
static wow_ole_descriptor_provider descriptor_provider;
BOOL WINAPI wow_ole_descriptor_bind(wow_ole_descriptor_provider provider)
{
    wow_ole_descriptor_provider previous;
    if (!provider) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    previous = (wow_ole_descriptor_provider)InterlockedCompareExchangePointer(
        (PVOID volatile *)&descriptor_provider, (PVOID)provider, NULL);
    if (previous && previous != provider) { SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE; }
    return TRUE;
}
static INIT_ONCE lead_once = INIT_ONCE_STATIC_INIT;
static BOOL CALLBACK initialize_lead_table(PINIT_ONCE once,PVOID parameter,PVOID *context)
{
    UINT i;
    (void)once; (void)parameter; (void)context;
    /* Original olethk32.cxx process initialization, same ACP classification. */
    for (i=0;i<256;i++) g_abLeadTable[i]=(BYTE)IsDBCSLeadByte((BYTE)i);
    return TRUE;
}
HGLOBAL WINAPI wow_ole_convert_descriptor(HANDLE memory,UINT direction)
{
    InitOnceExecuteOnce(&lead_once,initialize_lead_table,NULL,NULL);
    if (!descriptor_provider) RaiseFailFastException(NULL, NULL, 0);
    return descriptor_provider(memory,direction);
}
