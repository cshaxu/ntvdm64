/*
 * OpenNT source: base/mvdm/softpc.new/host/src/nt_error.c,
 * host_direct_access_error().  That implementation uses TlsDirectError to
 * suppress repeated dialogs for a direct-access category and presents an
 * Abort/Ignore dialog.  Its resource DLL, WOW hard-error path, console
 * positioning and CSRSS worker shell cannot compose into this one-session
 * modern product.  This smallest seam retains its caller-visible contract:
 * Ignore returns; Terminate ends this VDM session through a typed result.
 */

#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <commctrl.h>

#include "top_level_nosupport_shim.h"

static __declspec(thread) DWORD g_seen_reasons;
static __declspec(thread) uint32_t g_prompt_count;
static __declspec(thread) enum bx_ntvdm_top_level_nosupport_choice_v1
    g_last_choice = BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE;
static __declspec(thread) enum bx_ntvdm_top_level_nosupport_choice_v1
    g_fixture_choice;

static LPCWSTR reason_text(uint32_t reason)
{
    static const LPCWSTR texts[] = {
        L"floppy direct access", L"hard-disk direct access",
        L"an unsupported device driver", L"an old PIF request",
        L"an illegal BOP request", L"a missing resource limit",
        L"a mouse-driver request"
    };
    return reason < sizeof(texts) / sizeof(texts[0]) ? texts[reason] :
        L"an unsupported direct-access request";
}

static enum bx_ntvdm_top_level_nosupport_choice_v1 prompt_user(uint32_t reason)
{
    static const TASKDIALOG_BUTTON buttons[] = {
        { IDABORT, L"Terminate" }, { IDIGNORE, L"Ignore" }
    };
    TASKDIALOGCONFIG config;
    int button = 0;
    HRESULT status;
    if (g_fixture_choice != 0)
        return g_fixture_choice;
    ZeroMemory(&config, sizeof(config));
    config.cbSize = sizeof(config);
    config.pszWindowTitle = L"NTDOS64 unsupported DOS operation";
    config.pszMainInstruction = L"The DOS program requested an unsupported operation.";
    config.pszContent = reason_text(reason);
    config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT;
    config.pButtons = buttons;
    config.cButtons = sizeof(buttons) / sizeof(buttons[0]);
    config.nDefaultButton = IDIGNORE;
    status = TaskDialogIndirect(&config, &button, NULL, NULL);
    /* nt_error.c terminates the VDM when its non-WOW dialog path cannot
     * continue.  Do the same for a failed/closed modern dialog, but express
     * it as a bx-vdm controlled stop rather than terminating this process. */
    return SUCCEEDED(status) && button == IDIGNORE ?
        BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE :
        BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_TERMINATE;
}

void bx_ntvdm_top_level_nosupport_v2_direct_access_error(uint32_t reason)
{
    DWORD bit = reason < 32u ? (DWORD)1u << reason : 0u;
    g_last_choice = BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE;
    /* Keep nt_error.c's per-thread same-category suppression.  The retained
     * source only names 0..6; an out-of-range AX stays observable but cannot
     * invoke C's undefined 1 << type operation. */
    if (bit != 0u && (g_seen_reasons & bit) != 0u) return;
    if (bit != 0u) g_seen_reasons |= bit;
    ++g_prompt_count;
    g_last_choice = prompt_user(reason);
}

enum bx_ntvdm_top_level_nosupport_choice_v1
bx_ntvdm_top_level_nosupport_v2_last_choice(void)
{
    return g_last_choice;
}

void bx_ntvdm_top_level_nosupport_v2_reset_thread(void)
{
    g_seen_reasons = 0u;
    g_prompt_count = 0u;
    g_last_choice = BX_NTVDM_TOP_LEVEL_NOSUPPORT_CHOICE_V1_IGNORE;
    g_fixture_choice = 0;
}

void bx_ntvdm_top_level_nosupport_v2_fixture_choice_set(
    enum bx_ntvdm_top_level_nosupport_choice_v1 choice)
{
    g_fixture_choice = choice;
}

uint32_t bx_ntvdm_top_level_nosupport_v2_fixture_prompt_count(void)
{
    return g_prompt_count;
}
