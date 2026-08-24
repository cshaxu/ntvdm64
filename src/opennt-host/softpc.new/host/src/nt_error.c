/*
 * Direct minimal import of
 * refs/opennt/base/mvdm/softpc.new/host/src/nt_error.c.
 *
 * The original translation unit also implements the full NT4 resource,
 * console-positioning, WOW hard-error and CSRSS product shell.  Only the
 * reached host_direct_access_error() owner is independently composable here.
 */

#include "nt_error_compat.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

/* DIVERGENCE(HOST-DIV-012): OpenNT used a process TLS slot supplied by its
 * NTVDM product initialization.  This one-session host has no such product
 * shell; a compiler TLS bitset preserves the original per-thread repeated
 * category suppression without installing host state. */
static __declspec(thread) DWORD g_direct_error_bits;
static __declspec(thread) uint32_t g_prompt_count;
static __declspec(thread) enum bx_ntvdm_top_level_nosupport_choice_v1
    g_last_choice = BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE;

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
    DWORD dwDirectError;

    dwDirectError = g_direct_error_bits;

    /* DIVERGENCE(HOST-DIV-013): the historical `1 << type` is undefined for
     * an arbitrary guest AX.  Its defined 0..6 behavior is unchanged; other
     * categories remain observable but are not used as an invalid bit index. */
    if (type < 32u && (dwDirectError & ((DWORD)1u << type)) != 0u)
        return;
    if (type < 32u) g_direct_error_bits = dwDirectError | ((DWORD)1u << type);

    /* DIVERGENCE(HOST-DIV-014): OpenNT loaded D_A_MESS resources from
     * ntvdm.exe.  That private resource table is not part of this unpack-and-
     * run composition.  Retain the source's one message construction point
     * and supply the same direct-access category through the Win32 facade. */
    (void)sprintf_s(message, sizeof(message),
        "The DOS program requested %s.", direct_access_type(type));

    ++g_prompt_count;
    g_last_choice = bx_ntvdm_opennt_direct_access_dialog(message) == IDIGNORE ?
        BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE :
        BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_TERMINATE;
}

enum bx_ntvdm_top_level_nosupport_choice_v1
bx_ntvdm_top_level_nosupport_v2_last_choice(void)
{
    return g_last_choice;
}

void bx_ntvdm_top_level_nosupport_v2_reset_thread(void)
{
    g_direct_error_bits = 0u;
    g_prompt_count = 0u;
    g_last_choice = BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE;
    bx_ntvdm_opennt_direct_access_fixture_reply_set(0);
}

void bx_ntvdm_top_level_nosupport_v2_fixture_choice_set(
    enum bx_ntvdm_top_level_nosupport_choice_v1 choice)
{
    bx_ntvdm_opennt_direct_access_fixture_reply_set(
        choice == BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE ? IDIGNORE : IDABORT);
}

uint32_t bx_ntvdm_top_level_nosupport_v2_fixture_prompt_count(void)
{
    return g_prompt_count;
}
