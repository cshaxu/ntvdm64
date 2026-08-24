/* Project-owned BOP provider composition for imported OpenNT demdir.c.
 * The OEM directory calls retain their historical spelling but are supplied
 * by adapter-win32; this unit owns only source-body selection and scoped
 * copied CPU/guest-memory invocation. */
#include "opennt_demdir_composition.h"

void demCreateDir(void);
void demDeleteDir(void);
void demQueryCurrentDir(void);
void demSetCurrentDir(void);

int bx_ntvdm_demdir_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;
    if (call == NULL) return 0;
    switch (call->service) {
    case BX_NTVDM_DEMDIR_CREATE: body = demCreateDir; break;
    case BX_NTVDM_DEMDIR_DELETE: body = demDeleteDir; break;
    case BX_NTVDM_DEMDIR_QUERY_CURRENT: body = demQueryCurrentDir; break;
    case BX_NTVDM_DEMDIR_SET_CURRENT: body = demSetCurrentDir; break;
    default: return 0;
    }
    return bx_ntvdm_demhndl_invoke_body(call, body);
}
