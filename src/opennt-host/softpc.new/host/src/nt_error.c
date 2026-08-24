/*
 * Direct minimal import of
 * refs/opennt/base/mvdm/softpc.new/host/src/nt_error.c.
 *
 * The original translation unit also implements the full NT4 resource,
 * console-positioning, WOW hard-error and CSRSS product shell.  Only the
 * reached host_direct_access_error() owner is independently composable here.
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"

/* DIVERGENCE(HOST-DIV-012): OpenNT used a process TLS slot supplied by its
 * NTVDM product initialization.  This one-session host has no such product
 * shell; a compiler TLS bitset preserves the original per-thread repeated
 * category suppression without installing host state. */
static const char *direct_access_type(ULONG type)
{
    static const char *const types[] = {
        "floppy direct access", "hard-disk direct access",
        "an unsupported device driver", "an old PIF request",
        "an illegal BOP request", "a missing resource limit",
        "a mouse-driver request"
    };
    return type < sizeof(types) / sizeof(types[0]) ? types[type] :
        "an unsupported direct-access request";
}

/*
 * host_direct_access_error is retained in the original source's order:
 * obtain this-thread state, suppress a repeated category, record it, form the
 * prompt, then call the host dialog.  It remains void: Ignore returns to the
 * caller and Terminate is observed by the typed BOP boundary.
 */
VOID host_direct_access_error(ULONG type)
{
    CHAR message[260];
    /* DIVERGENCE(HOST-DIV-012,HOST-DIV-013): OpenNT obtains its per-thread
     * TlsDirectError bitset from NTVDM initialization. The standalone
     * adapter-win32 facade owns the equivalent session-resettable state;
     * its 0..6 result and repeated-category suppression are unchanged, while
     * arbitrary AX values cannot issue an undefined C shift. */
    if (!bx_ntvdm_opennt_direct_access_category_should_prompt(type)) return;

    /* DIVERGENCE(HOST-DIV-014): OpenNT loaded D_A_MESS resources from
     * ntvdm.exe.  That private resource table is not part of this unpack-and-
     * run composition.  Retain the source's one message construction point
     * and supply the same direct-access category through the Win32 facade. */
    (void)sprintf_s(message, sizeof(message),
        "The DOS program requested %s.", direct_access_type(type));

    (void)bx_ntvdm_opennt_direct_access_dialog(message);
}
