#include "include/mvdm_vm_guest_io.h"
#include "include/console_io.h"
#include "include/guest_memory_access.h"
#include "include/mvdm_vm_continuation_store.h"
#include "include/mvdm_vm_wake.h"

#include <string.h>

/* Original SoftPC keyboard mapping and recursive ICA lock. */
extern BOOL BiosKeyToInputRecord(PKEY_EVENT_RECORD);
extern void host_ica_lock(void);
extern void host_ica_unlock(void);

/* Existing BOP transport; copy a bounded descriptor, never retain VMM memory.
 * token[2], ticket[2], buffer address, capacity/extent, guest status. */
DWORD mvdm_vm_application_bop(uint32_t operation, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t descriptor[7], size;
    uint64_t token, ticket;
    DWORD error;
    void *bytes = NULL;
    if (extent != sizeof(descriptor) || !mvdm_softpc_guest_memory_copy_from(
        address, (uint8_t *)descriptor, sizeof(descriptor))) return ERROR_INVALID_ADDRESS;
    memcpy(&token, descriptor, 8); memcpy(&ticket, descriptor + 2, 8);
    if (operation == 14)
        return mvdm_vm_continuation_ack_io(token, vm, ticket, descriptor[6]);
    if (descriptor[5] > CONSOLE_IO_MAX_DATA_BYTES) return ERROR_INVALID_DATA;
    if (descriptor[4] && descriptor[5]) {
        bytes = HeapAlloc(GetProcessHeap(), 0, descriptor[5]);
        if (!bytes) return ERROR_NOT_ENOUGH_MEMORY;
    }
    error = mvdm_vm_continuation_read_io(token, vm, &ticket, bytes,
        bytes ? descriptor[5] : 0, &size);
    if (!error && !mvdm_softpc_guest_memory_copy_to(descriptor[4], bytes, size))
        error = ERROR_INVALID_ADDRESS;
    if (bytes) HeapFree(GetProcessHeap(), 0, bytes);
    memcpy(descriptor + 2, &ticket, 8); descriptor[5] = size;
    if (!mvdm_softpc_guest_memory_copy_to(address, (uint8_t *)descriptor,
        sizeof(descriptor))) error = ERROR_INVALID_ADDRESS;
    return error;
}

static DWORD bios_key_modifiers(DWORD flags)
{
    DWORD result = flags & SHIFT_PRESSED;
    if (flags & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) result |= LEFT_CTRL_PRESSED;
    if (flags & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) result |= LEFT_ALT_PRESSED;
    return result;
}
static DWORD returned_bios_word(const console_io_input *input, UINT codepage, WORD *word)
{
    DWORD scan;
    if (!input->key_down) return ERROR_NO_MORE_ITEMS;
    if (input->character) {
        WCHAR character = (WCHAR)input->character;
        char bytes[2];
        BOOL substituted = FALSE;
        int length = WideCharToMultiByte(codepage, WC_NO_BEST_FIT_CHARS,
            &character, 1, bytes, sizeof(bytes), NULL, &substituted);
        if (!length) return GetLastError();
        if (substituted || length != 1) return ERROR_NO_UNICODE_TRANSLATION;
        scan = input->scan;
        if ((input->control & ENHANCED_KEY) &&
            (input->virtual_key == VK_RETURN || input->virtual_key == VK_DIVIDE)) scan = 0xe0;
        if (scan > 255) return ERROR_INVALID_DATA;
        *word = (WORD)((scan << 8) | (BYTE)bytes[0]);
        return ERROR_SUCCESS;
    }
    switch (input->virtual_key) {
    case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT:
    case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
    case VK_MENU: case VK_LMENU: case VK_RMENU:
    case VK_CAPITAL: case VK_NUMLOCK: case VK_SCROLL:
        return ERROR_NO_MORE_ITEMS; /* No BIOS character, no raw IRQ replay. */
    }
    /* nt_keycd.c's reverse table has no F11/F12 physical scans. Use the
     * original spckbd.asm lowercase/uppercase/ctl_f_table and keybd_io.c
     * Alt rule here: BIOS scans 85/87/89/8b hex, Alt > Ctrl > Shift. */
    if ((input->virtual_key == VK_F11 && input->scan == 0x57) ||
        (input->virtual_key == VK_F12 && input->scan == 0x58)) {
        DWORD modifiers = bios_key_modifiers(input->control);
        scan = 0x85 + input->scan - 0x57;
        if (modifiers & LEFT_ALT_PRESSED) scan += 6;
        else if (modifiers & LEFT_CTRL_PRESSED) scan += 4;
        else if (modifiers & SHIFT_PRESSED) scan += 2;
        *word = (WORD)(scan << 8);
        return ERROR_SUCCESS;
    }
    /* Reuse nt_keycd.c's original function-key mapping rather than maintaining
     * another Shift/Ctrl/Alt scan table. This runs only for returned keys. */
    for (scan = 1; scan <= 0xa6; ++scan) {
        KEY_EVENT_RECORD decoded = {0};
        decoded.wVirtualScanCode = (WORD)scan;
        if (BiosKeyToInputRecord(&decoded) && decoded.wVirtualScanCode == input->scan &&
            bios_key_modifiers(decoded.dwControlKeyState) == bios_key_modifiers(input->control)) {
            *word = (WORD)(scan << 8);
            if ((input->control & ENHANCED_KEY) && !bios_key_modifiers(input->control))
                *word |= 0xe0;
            return ERROR_SUCCESS;
        }
    }
    return ERROR_NOT_SUPPORTED;
}
DWORD mvdm_vm_executor_return_key_bop(uint32_t vm, uint32_t address,
    uint32_t extent, uint32_t *word, uint32_t *repeat)
{
    uint32_t descriptor[3], count;
    uint64_t generation;
    console_io_input input;
    WORD converted = 0;
    DWORD error;
    *word = *repeat = 0;
    if (extent != sizeof(descriptor)) return ERROR_INVALID_PARAMETER;
    if (!mvdm_softpc_guest_memory_copy_from(address, (uint8_t *)descriptor, sizeof(descriptor)))
        return ERROR_INVALID_ADDRESS;
    generation = ((uint64_t)descriptor[1] << 32) | descriptor[0];
    if (!descriptor[2] || !IsValidCodePage(descriptor[2])) return ERROR_INVALID_PARAMETER;
    host_ica_lock();
    for (;;) {
        error = mvdm_vm_wake_read_input(vm, generation, &input, 1, &count, 0);
        if (error) break;
        if (!count) { error = ERROR_NO_MORE_ITEMS; break; }
        error = returned_bios_word(&input, descriptor[2], &converted);
        if (error && error != ERROR_NO_MORE_ITEMS) break;
        {
            DWORD consumed = mvdm_vm_wake_read_input(vm, generation, &input, 1, &count, 1);
            if (consumed) { error = consumed; break; }
        }
        if (error == ERROR_NO_MORE_ITEMS) continue;
        *word = converted;
        *repeat = input.repeat;
        break;
    }
    host_ica_unlock();
    return error;
}
