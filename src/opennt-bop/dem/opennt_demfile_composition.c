/* Project-owned BOP composition for directly imported OpenNT demfile.c and
 * demlabel.c.  Historical OEM calls are supplied by adapter-win32 and the
 * Redirector protocol stays in its lower host-capability owner. */

#include "opennt_demfile_composition.h"
#include "opennt-host/vdmredir/vrnmpipe_compat.h"

void demChMod(void);
void demCreate(void);
void demCreateNew(void);
void demDelete(void);
void demOpen(void);
void demRename(void);
void demCheckPath(void);

/* The original separate VDMREDIR DLL supplied this imported function pointer.
 * Static composition retains one shared pointer, populated by LoadVdmRedir. */
BOOL (*VrInitialized)(VOID);

BOOL LoadVdmRedir(void)
{
    /* DIVERGENCE(BOP-DIV-051): original NTVDM populated this DLL import
     * pointer after LoadLibrary.  The static provider keeps the exact source
     * indirection while binding it to the re-rooted VDMREDIR owner. */
    VrInitialized = (BOOL (*)(VOID))bx_ntvdm_vr_initialized_provider;
    return bx_ntvdm_vr_initialize_provider();
}

void *Sim32GetVDMPointer(ULONG address, ULONG bytes, int protect)
{
    (void)address; (void)bytes; (void)protect;
    /* VDD JFT/SFT helpers remain imported source but require the original
     * process/VDM table composition; no S3 BOP invokes this fallback. */
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
}

BOOL bx_ntvdm_demfile_publish_handle(HANDLE file)
{ return bx_ntvdm_demhndl_publish_handle(file); }

int bx_ntvdm_demfile_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;
    if (call == NULL) return 0;
    switch (call->service) {
    case BX_NTVDM_DEMFILE_CHMOD: body = demChMod; break;
    case BX_NTVDM_DEMFILE_CREATE: body = demCreate; break;
    case BX_NTVDM_DEMFILE_DELETE: body = demDelete; break;
    case BX_NTVDM_DEMFILE_OPEN: body = demOpen; break;
    case BX_NTVDM_DEMFILE_RENAME: body = demRename; break;
    case BX_NTVDM_DEMFILE_CREATE_NEW: body = demCreateNew; break;
    case BX_NTVDM_DEMFILE_CHECK_PATH: body = demCheckPath; break;
    default: return 0;
    }
    return bx_ntvdm_demhndl_invoke_body(call, body);
}
