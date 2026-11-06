/* OPENNT-HOST-297: bind original OLE conversions to existing WOW owners.
 * No OLE activation, guest component or second memory owner is introduced. */
#include "wow_ole_descriptor_private.h"
BYTE g_abLeadTable[256];
static INIT_ONCE lead_once = INIT_ONCE_STATIC_INIT;
static BOOL CALLBACK initialize_lead_table(PINIT_ONCE once,PVOID parameter,PVOID *context)
{
    UINT i;
    (void)once; (void)parameter; (void)context;
    /* Original olethk32.cxx process initialization, same ACP classification. */
    for (i=0;i<256;i++) g_abLeadTable[i]=(BYTE)IsDBCSLeadByte((BYTE)i);
    return TRUE;
}
void WINAPI wow_ole_touch_pointer(VPVOID vp)
{
    /* The existing original helper owns RET_FORCESEGMENTFAULT and mapping. */
    (void)GetPModeVDMPointerAssert(vp);
}
HGLOBAL WINAPI wow_ole_convert_descriptor(HANDLE memory,UINT direction)
{
    InitOnceExecuteOnce(&lead_once,initialize_lead_table,NULL,NULL);
    return ConvertObjDescriptor(memory,direction);
}
