/*
 * OpenNT source: base/mvdm/softpc.new/host/src/nt_error.c,
 * RcErrorDialogBox().  Preserve the reached ordering: OEM parameters become
 * ANSI text, each nonempty parameter gets ". ", then the resource message is
 * appended.  NT4's ntvdm resource table is not part of this CLI composition;
 * a missing resource is therefore an explicit public-Win32 fallback carrying
 * the original resource id, not a silent no-op.
 */
#include "opennt_error_dialog_facade.h"

#include <stdio.h>
#include <string.h>

static __declspec(thread) BOOL g_fixture_suppress;
static __declspec(thread) UINT g_last_error;
static __declspec(thread) UINT g_count;
static __declspec(thread) int g_direct_access_fixture_reply;
static __declspec(thread) UINT g_direct_access_prompt_count;
static __declspec(thread) DWORD g_direct_access_category_bits;
static __declspec(thread) enum bx_ntvdm_opennt_direct_access_choice_v1
    g_direct_access_last_choice = BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_IGNORE;

#define BX_NTVDM_RMB_ABORT 1u
#define BX_NTVDM_RMB_RETRY 2u
#define BX_NTVDM_RMB_IGNORE 4u
#define BX_NTVDM_RMB_ICON_INFO 8u
#define BX_NTVDM_RMB_ICON_BANG 16u
#define BX_NTVDM_RMB_ICON_STOP 32u
#define BX_NTVDM_RMB_ICON_WHAT 64u
#define BX_NTVDM_RMB_EDIT 128u

static size_t append_oem_message(CHAR *destination, size_t capacity,
    const CHAR *source)
{
    CHAR bounded[MAX_PATH];
    CHAR converted[MAX_PATH];
    size_t used;
    if (destination == NULL || capacity == 0u || source == NULL || *source == '\0')
        return 0u;
    (void)strncpy_s(bounded, sizeof(bounded), source, _TRUNCATE);
    if (!OemToCharA(bounded, converted)) return 0u;
    used = strlen(destination);
    if (used >= capacity - 1u) return 0u;
    (void)strncat_s(destination, capacity, converted, _TRUNCATE);
    (void)strncat_s(destination, capacity, ". ", _TRUNCATE);
    return strlen(destination) - used;
}

static void compose_message(UINT error, CHAR *first, CHAR *second,
    CHAR *message, size_t capacity)
{
    size_t used;
    message[0] = '\0';
    (void)append_oem_message(message, capacity, first);
    (void)append_oem_message(message, capacity, second);
    used = strlen(message);
    if (LoadStringA(GetModuleHandleA(NULL), error, message + used,
            (int)(capacity - used)) == 0) {
        /* DIVERGENCE(WIN32-DIV-001): the original ntvdm resource DLL/string table is not a
         * composable CLI input.  Keep the same terminal host notification
         * contract with the resource id observable to the user. */
        (void)sprintf_s(message, capacity, "NTVDM error %u", error);
    }
}

void bx_ntvdm_opennt_rc_error_dialog(UINT error, CHAR *first, CHAR *second)
{
    CHAR message[MAX_PATH * 4u];
    compose_message(error, first, second, message, sizeof(message));
    g_last_error = error;
    ++g_count;
    if (!g_fixture_suppress)
        (void)MessageBoxA(NULL, message, "NTDOS64", MB_OK | MB_ICONSTOP);
}

int bx_ntvdm_opennt_rc_message_box(UINT error, CHAR *first, CHAR *second,
    ULONG flags)
{
    CHAR message[MAX_PATH * 4u];
    UINT style = MB_OK;
    int reply;
    compose_message(error, first, second, message, sizeof(message));
    g_last_error = error;
    ++g_count;
    /* DIVERGENCE(WIN32-DIV-002): OpenNT's resource dialog supports an editable text control.
     * A public MessageBox has no equivalent control.  Keep its call shape and
     * deterministic terminal reply rather than silently pretending it edited
     * msg2; the PIF edit route stays explicitly deferred to its owner. */
    if (flags & BX_NTVDM_RMB_EDIT) return (int)BX_NTVDM_RMB_ABORT;
    if ((flags & (BX_NTVDM_RMB_ABORT | BX_NTVDM_RMB_RETRY |
            BX_NTVDM_RMB_IGNORE)) == (BX_NTVDM_RMB_ABORT |
            BX_NTVDM_RMB_RETRY | BX_NTVDM_RMB_IGNORE)) style |= MB_ABORTRETRYIGNORE;
    else if ((flags & (BX_NTVDM_RMB_ABORT | BX_NTVDM_RMB_IGNORE)) ==
            (BX_NTVDM_RMB_ABORT | BX_NTVDM_RMB_IGNORE)) style |= MB_OKCANCEL;
    else if (flags & BX_NTVDM_RMB_RETRY) style |= MB_RETRYCANCEL;
    if (flags & BX_NTVDM_RMB_ICON_BANG) style |= MB_ICONWARNING;
    else if (flags & BX_NTVDM_RMB_ICON_INFO) style |= MB_ICONINFORMATION;
    else if (flags & BX_NTVDM_RMB_ICON_WHAT) style |= MB_ICONQUESTION;
    else style |= MB_ICONSTOP;
    if (g_fixture_suppress) return (int)BX_NTVDM_RMB_ABORT;
    reply = MessageBoxA(NULL, message, "NTDOS64", style);
    if (reply == IDIGNORE ||
        ((flags & (BX_NTVDM_RMB_ABORT | BX_NTVDM_RMB_IGNORE)) ==
            (BX_NTVDM_RMB_ABORT | BX_NTVDM_RMB_IGNORE) && reply == IDCANCEL))
        return (int)BX_NTVDM_RMB_IGNORE;
    if (reply == IDRETRY) return (int)BX_NTVDM_RMB_RETRY;
    return (int)BX_NTVDM_RMB_ABORT;
}

int bx_ntvdm_opennt_direct_access_dialog(const CHAR *message)
{
    int reply;
    ++g_direct_access_prompt_count;
    if (g_direct_access_fixture_reply != 0) {
        reply = g_direct_access_fixture_reply;
    } else {
        reply = MessageBoxA(NULL, message == NULL ? "NTVDM direct access error" : message,
            "NTDOS64 unsupported DOS operation", MB_ABORTRETRYIGNORE | MB_ICONSTOP);
    }
    g_direct_access_last_choice = reply == IDIGNORE ?
        BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_IGNORE :
        BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_TERMINATE;
    return g_direct_access_last_choice == BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_IGNORE ?
        IDIGNORE : IDABORT;
}

enum bx_ntvdm_opennt_direct_access_choice_v1
bx_ntvdm_opennt_direct_access_last_choice(void)
{ return g_direct_access_last_choice; }
int bx_ntvdm_opennt_direct_access_category_should_prompt(ULONG category)
{
    DWORD bit;
    if (category >= 32u) return 1;
    bit = (DWORD)1u << category;
    if ((g_direct_access_category_bits & bit) != 0u) return 0;
    g_direct_access_category_bits |= bit;
    return 1;
}
DWORD bx_ntvdm_opennt_direct_access_category_bits_get(void)
{ return g_direct_access_category_bits; }
void bx_ntvdm_opennt_direct_access_category_bits_set(DWORD bits)
{ g_direct_access_category_bits = bits; }
int bx_ntvdm_opennt_direct_access_load_string(UINT resource_id, CHAR *buffer,
    UINT capacity)
{
    const CHAR *text;
    if (buffer == NULL || capacity == 0u) return 0;
    /* OpenNT softpc.new/obj.vdm/resource.rc, D_A_MESS through D_A_MOUSEDRVR.
     * D_A_OLDPIF (304) was intentionally commented out there, so preserve
     * the original LoadString failure for that historical resource id. */
    switch (resource_id) {
    case 300u: text = "An application has attempted to %s, which cannot be supported. This may cause the application to function incorrectly."; break;
    case 301u: text = "directly access an incompatible diskette format"; break;
    case 302u: text = "directly access the hard disk"; break;
    case 303u: text = "load a 16-bit Dos device driver"; break;
    case 305u: text = "perform an unknown internal 'BOP' opcode"; break;
    case 306u: text = "allocate Expanded Memory"; break;
    case 307u: text = "use a third party mouse driver"; break;
    default: return 0;
    }
    (void)strncpy_s(buffer, capacity, text, _TRUNCATE);
    return (int)strlen(buffer);
}
const CHAR *bx_ntvdm_opennt_direct_access_fallback_message(void)
{ return "NTVDM direct-access error"; }
void bx_ntvdm_opennt_direct_access_reset_thread(void)
{
    g_direct_access_fixture_reply = 0;
    g_direct_access_prompt_count = 0u;
    g_direct_access_category_bits = 0u;
    g_direct_access_last_choice = BX_NTVDM_OPENNT_DIRECT_ACCESS_CHOICE_V1_IGNORE;
}
UINT bx_ntvdm_opennt_direct_access_prompt_count(void)
{ return g_direct_access_prompt_count; }

void bx_ntvdm_opennt_error_dialog_fixture_suppress(BOOL suppress)
{ g_fixture_suppress = suppress; }
UINT bx_ntvdm_opennt_error_dialog_fixture_last_error(void)
{ return g_last_error; }
UINT bx_ntvdm_opennt_error_dialog_fixture_count(void)
{ return g_count; }
void bx_ntvdm_opennt_direct_access_fixture_reply_set(int reply)
{ g_direct_access_fixture_reply = reply; }
