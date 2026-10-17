#include "wow_private_user_compat.h"
#include "wow_menu_parser_bindings.h"
#include "wow_default_message_bindings.h"

#include <string.h>


/* Original client.c owns the bitmap merge. The boundary retains the existing
 * invalid-span contract and supplies the zeroed buffer required by USER16. */
WORD WINAPI wow_private_user_get_def_window_proc_bits(PBYTE bits,
    WORD byte_count)
{
    UINT maximum;
    if (!bits) return 0;
    if (!wow_default_messages_ready()) return 0;
    maximum = max(wow_default_message_state.DefWindowMsgs.maxMsgs,
        wow_default_message_state.DefWindowSpecMsgs.maxMsgs);
    if (maximum > MAXWORD || byte_count <= maximum / 8u) return 0;
    memset(bits, 0, maximum / 8u + 1u);
    return WowGetDefWindowProcBits(bits, byte_count);
}

int WINAPI wow_private_user_get_id_from_directory(PBYTE resource_bits,
    UINT resource_type)
{
    /* DIVERGENCE(ADAPTER-WOW-015): retain the exact original rtlres.c
     * resource-directory selector; the private table slot alone remains the
     * standalone boundary. */
    return LookupIconIdFromDirectoryEx(resource_bits,
        resource_type == (UINT)RT_ICON, 0, 0, LR_DEFAULTSIZE);
}

HCURSOR WINAPI wow_private_user_load_create_cursor_icon(HANDLE module,
    LPTSTR module_name, DWORD expected_windows_version, LPCTSTR name,
    DWORD resource_size, PVOID resource_bits, LPTSTR resource_type,
    BOOL client_side)
{
    BOOL icon = resource_type == RT_ICON;
    UINT flags = LR_SHARED | LR_DEFAULTSIZE;

    /* DIVERGENCE(ADAPTER-WOW-016): OpenNT client/clres.c's private
     * WowServerLoadCreateCursorIcon and public CreateIconFromResourceEx
     * converge on ConvertDIBIcon. Preserve default sizing, cursor monochrome
     * conversion, hotspot scaling and shared-resource flags through that
     * public entry, not a second DIB parser. dwExpWinVer is unused upstream;
     * it is an application version, NOT the resource-format version.
     * WU32LoadCursor has already converted resource names to Unicode.
     * Original wcuricon.c retains guest resource aliasing and replacement. */
    UNREFERENCED_PARAMETER(module_name);
    UNREFERENCED_PARAMETER(expected_windows_version);
    UNREFERENCED_PARAMETER(client_side);
    if (!icon && resource_type != RT_CURSOR) return NULL;
    if (!icon) flags |= LR_MONOCHROME;
    if (resource_bits != NULL) {
        if (resource_size == 0) return NULL;
        return (HCURSOR)CreateIconFromResourceEx((PBYTE)resource_bits,
            resource_size, icon, 0x00030000, 0, 0, flags);
    }
    return (HCURSOR)LoadImageW((HINSTANCE)module, (LPCWSTR)name,
        icon ? IMAGE_ICON : IMAGE_CURSOR, 0, 0, flags);
}

/* DIVERGENCE(ADAPTER-WOW-021): OpenNT kernel/ntstubs.c::NtUserGetMenuIndex
 * (Copyright (c) 1985-95 Microsoft Corporation) validates both menus before
 * searching their immediate items. USER server pointers/critical sections
 * cannot enter this worker; public USER supplies validation and item access.
 * Original HMENU32 is a sign-extended WORD, not an independent ID namespace.
 * Full stale handles must still fail validation before any narrow comparison. */
DWORD WINAPI wow_private_user_get_menu_index(HMENU menu, HMENU submenu)
{
    int index, count;
    if (!IsMenu(menu) || !IsMenu(submenu)) {
        SetLastError(ERROR_INVALID_MENU_HANDLE);
        return 0;
    }
    count = GetMenuItemCount(menu);
    for (index = 0; index < count; ++index) {
        HMENU item = GetSubMenu(menu, index);
        HMENU narrow = (HMENU)(INT_PTR)(SHORT)(WORD)(UINT_PTR)item;
        if (item && (item == submenu || narrow == submenu))
            return (DWORD)index;
    }
    return (DWORD)-1;
}

HMENU WINAPI wow_user_load_menu_indirect(const MENUTEMPLATEW *menu_template)
{
    /* ADAPTER-WOW-037: called only by the original Win16 menu consumers,
     * after ConvertMenu16. Native templates may instead use the original
     * parser with wow16=FALSE; no process-wide task flag is fabricated. */
    if (!menu_template) return NULL;
    return CreateMenuFromResource((LPBYTE)menu_template, TRUE);
}

HMENU WINAPI wow_private_user_load_create_menu(HANDLE module, LPTSTR name,
    CONST LPMENUTEMPLATE menu_template, DWORD resource_size,
    BOOL call_client)
{
    /* DIVERGENCE(ADAPTER-WOW-020): USER16 has already found and locked the
     * original Win16 RT_MENU resource before WU32LoadMenu enters this NT4
     * private output slot.  WU32LoadMenu retains the original ConvertMenu16
     * call and passes its converted menu template here. NT4
     * routed that template through WowServerLoadCreateMenu to
     * CreateMenuFromResource. Use that original parser with the explicit
     * Win16 invocation contract: ConvertMenuItems16 stores a packed guest
     * pointer for owner-draw items, not native Unicode text.
     *
     * The historical module/name/client fields identify server bookkeeping,
     * not the template payload.  They remain deliberately unobserved.  The
     * source route has already rejected an absent resource; retain that
     * failure direction for a missing/empty template. */
    UNREFERENCED_PARAMETER(module);
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(call_client);
    if (menu_template == NULL || resource_size == 0u) {
        return NULL;
    }
    return wow_user_load_menu_indirect(menu_template);
}
