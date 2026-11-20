/*
 * DIVERGENCE(ADAPTER-WIN32-030): Same-shaped public-Win32 replacement for
 * reached NT4 Console Server client calls.  Modern public Console/User32 APIs
 * provide record and coordinate operations but no private server export or
 * cursor reference count.  Original SoftPC callers retain their names,
 * parameters and ordering; unsupported operations fail explicitly.
 */
#include <windows.h>
#include <stddef.h>
#include "conapi.h"
#include "console_client.h"
#include "console_input.h"
#include "compat/include/mvdm_softpc_mouse_bridge.h"
#include "compat/include/mvdm_keyboard_history.h"
#include "compat/include/console_io.h"
typedef char keyboard_live_capacity_check[(MVDM_KEY_HISTORY_CAPACITY<=CONSOLE_IO_INPUT_LIVE_CAPACITY) ? 1 : -1];
extern mvdm_keyboard_history nt_keyboard_history;


BOOL WINAPI ReadConsoleInputExW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                LPDWORD read, USHORT flags)
{
    DWORD available;
    if ((flags & ~CONSOLE_READ_VALID) != 0u) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (count == 0u)
        return (flags & CONSOLE_READ_NOREMOVE) != 0u ?
            PeekConsoleInputW(input, records, count, read) :
            ReadConsoleInputW(input, records, count, read);
    if (!records || !read) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    *read = 0u;
    if ((flags & (CONSOLE_READ_NOWAIT | CONSOLE_READ_NOREMOVE)) != 0u) {
        if (!PeekConsoleInputW(input, records, 1u, &available)) return FALSE;
        if (available == 0u) return TRUE;
        if ((flags & CONSOLE_READ_NOREMOVE) != 0u) {
            *read = available;
            return TRUE;
        }
    }
    return ReadConsoleInputW(input, records, count, read);
}

/* DIVERGENCE(MVDM-HOST-DIV-211): current Console/RDP may report a virtual
 * key or UTF-16 character without the PC Scan-1 byte that the original
 * SoftPC input worker passes to KeyMsgToKeyCode. Normalize only at that
 * host boundary: the original table still selects the SoftPC key number and
 * keyba.c remains the only guest keyboard-controller owner. */
static WORD nt_rdp_decode_scan(WORD raw_scan)
{
    return (raw_scan & 0xff00u) == 0xe000u ?
        (WORD)(0x0100u | (raw_scan & 0x00ffu)) :
        (WORD)(raw_scan & 0x00ffu);
}

static WORD nt_rdp_resolve_scan(WORD virtual_key)
{
    return nt_rdp_decode_scan((WORD)MapVirtualKeyExW(virtual_key,
        MAPVK_VK_TO_VSC_EX, GetKeyboardLayout(0u)));
}

static DWORD nt_rdp_emit_transition(PINPUT_RECORD records, DWORD capacity,
    DWORD count, WCHAR character, WORD scan, WORD virtual_key,
    DWORD control_state, BOOL down)
{
    KEY_EVENT_RECORD *key;
    if (count >= capacity || scan == 0u || virtual_key == 0u) return 0u;
    records[count].EventType = KEY_EVENT;
    key = &records[count].Event.KeyEvent;
    ZeroMemory(key, sizeof(*key));
    key->bKeyDown = down;
    key->wRepeatCount = 1u;
    key->wVirtualKeyCode = virtual_key;
    key->wVirtualScanCode = (WORD)(scan & 0xffu);
    key->uChar.UnicodeChar = character;
    key->dwControlKeyState = control_state;
    if ((scan & 0x0100u) != 0u) key->dwControlKeyState |= ENHANCED_KEY;
    return count + 1u;
}

static DWORD nt_rdp_normalize_key(const KEY_EVENT_RECORD *input,
    PINPUT_RECORD output, DWORD capacity)
{
    KEY_EVENT_RECORD key;
    WORD scan;
    WORD virtual_key;
    SHORT mapped;
    BYTE key_flags;
    CHAR oem_character;
    CHAR digit_text[4];
    CHAR *digit;
    DWORD control_state;
    DWORD count = 0u;

    if (input == NULL || output == NULL || capacity == 0u) return 0u;
    key = *input;
    if (key.wVirtualScanCode != 0u) {
        output[0].EventType = KEY_EVENT;
        output[0].Event.KeyEvent = key;
        return 1u;
    }
    /* Scanless printable input carries the intended character, even when
     * Console supplies an ordinary VK. Mapping only that VK loses Shift.
     * Physical scans and Ctrl/Alt shortcuts retain their original identity. */
    if (key.wVirtualKeyCode != 0u && key.wVirtualKeyCode != VK_PACKET &&
        (key.uChar.UnicodeChar < L' ' || key.uChar.UnicodeChar == 0x7fu ||
         (key.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED |
                                  LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)))) {
        scan = nt_rdp_resolve_scan(key.wVirtualKeyCode);
        if (scan == 0u) return 0u;
        key.wVirtualScanCode = (WORD)(scan & 0xffu);
        if ((scan & 0x0100u) != 0u) key.dwControlKeyState |= ENHANCED_KEY;
        output[0].EventType = KEY_EVENT;
        output[0].Event.KeyEvent = key;
        return 1u;
    }
    if (!key.bKeyDown || key.uChar.UnicodeChar == 0u) return 0u;
    /* DoStringPaste is UTF-16 too.  A surrogate is not an independently
     * representable PC key, so retain the original worker's one-WCHAR input
     * boundary rather than inventing a Unicode code-point keyboard. */
    if (key.uChar.UnicodeChar >= 0xd800u && key.uChar.UnicodeChar <= 0xdfffu)
        return 0u;
    mapped = VkKeyScanExW(key.uChar.UnicodeChar, GetKeyboardLayout(0u));
    if (mapped == -1) {
        /* Source owner: ntcon/server/clipbrd.c::DoStringPaste.  Its OEM
         * numeric-keypad fallback is the only representable route for a
         * character outside the active keyboard layout. */
        if (WideCharToMultiByte(GetConsoleOutputCP(), 0,
                &key.uChar.UnicodeChar, 1, &oem_character, 1, NULL, NULL) != 1)
            return 0u;
        _itoa((unsigned char)oem_character, digit_text, 10);
        if ((count = nt_rdp_emit_transition(output, capacity, count, 0,
                0x38u, VK_MENU, LEFT_ALT_PRESSED, TRUE)) == 0u) return 0u;
        for (digit = digit_text; *digit != '\0'; ++digit) {
            virtual_key = (WORD)(*digit - '0' + VK_NUMPAD0);
            scan = nt_rdp_resolve_scan(virtual_key);
            if ((count = nt_rdp_emit_transition(output, capacity, count, 0,
                    scan, virtual_key, LEFT_ALT_PRESSED, TRUE)) == 0u ||
                (count = nt_rdp_emit_transition(output, capacity, count, 0,
                    scan, virtual_key, LEFT_ALT_PRESSED, FALSE)) == 0u)
                return 0u;
        }
        return nt_rdp_emit_transition(output, capacity, count,
            key.uChar.UnicodeChar, 0x38u, VK_MENU, 0u, FALSE);
    }
    virtual_key = (WORD)(mapped & 0xffu);
    scan = nt_rdp_resolve_scan(virtual_key);
    if (scan == 0u) return 0u;
    key_flags = (BYTE)((mapped >> 8u) & 0xffu);
    if ((key_flags & 6u) == 6u) {
        if ((count = nt_rdp_emit_transition(output, capacity, count,
            0, 0x38u, VK_MENU,
            ENHANCED_KEY | LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED, TRUE)) == 0u)
            return 0u;
    } else if ((key_flags & 1u) != 0u &&
        (count = nt_rdp_emit_transition(output, capacity, count,
            0, 0x2au, VK_SHIFT, SHIFT_PRESSED, TRUE)) == 0u) return 0u;
    control_state = 0u;
    if ((key_flags & 1u) != 0u) control_state |= SHIFT_PRESSED;
    if ((key_flags & 2u) != 0u) control_state |= LEFT_CTRL_PRESSED;
    if ((key_flags & 4u) != 0u) control_state |= RIGHT_ALT_PRESSED;
    if ((count = nt_rdp_emit_transition(output, capacity, count,
            key.uChar.UnicodeChar, scan, virtual_key, control_state, TRUE)) == 0u ||
        (count = nt_rdp_emit_transition(output, capacity, count,
            key.uChar.UnicodeChar, scan, virtual_key, control_state, FALSE)) == 0u)
        return 0u;
    /* Only the character repeats; modifier transitions remain single. */
    output[count - 2u].Event.KeyEvent.wRepeatCount =
        key.wRepeatCount ? key.wRepeatCount : 1u;
    if ((key_flags & 6u) == 6u)
        return nt_rdp_emit_transition(output, capacity, count,
            0, 0x38u, VK_MENU, ENHANCED_KEY, FALSE);
    if ((key_flags & 1u) != 0u &&
        (count = nt_rdp_emit_transition(output, capacity, count,
            0, 0x2au, VK_SHIFT, 0u, FALSE)) == 0u) return 0u;
    return count;
}

BOOL ntvdm_console_read_pc_input(HANDLE input, PINPUT_RECORD records,
    DWORD capacity, LPDWORD read, USHORT flags)
{
    INPUT_RECORD raw[5];
    uint64_t raw_receipts[5],key_receipts[NTVDM_PC_INPUT_RECORDS];
    uint64_t live[MVDM_KEY_HISTORY_CAPACITY];
    DWORD available, index, used = 0u, limit,key_count=0;BOOL tagged=FALSE;
    /* Validate before consuming: every raw record can expand to eight keys.
     * One bounded batch, no hidden carry queue across DOS/native handoff. */
    if (!records || !read || capacity < NTVDM_PC_INPUT_RECORDS ||
        (flags & CONSOLE_READ_NOREMOVE) != 0u || (flags & ~CONSOLE_READ_VALID) != 0u) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *read = 0u;
    /* Bound removal by downstream capacity, not by a fixed per-event delay.
     * At most one relative sample is produced per raw record. On pressure,
     * leave ALL input in its original queue and return to nt_event_loop so
     * its original suspend/APC handling remains reachable. Never wait while
     * holding ICA or drop/merge motion across a button or direction change. */
    limit=min((DWORD)ARRAYSIZE(raw),mvdm_softpc_mouse_capacity(mvdm_softpc_mouse_current()));
    if (!limit) return TRUE;
    if(flags & CONSOLE_READ_NOWAIT) {
        if(!PeekConsoleInputW(input,raw,1,&available))return FALSE;
        if(!available)return TRUE;
    }
    if(!ntvdm_console_read_input_tagged(input,raw,raw_receipts,limit,&available,FALSE,
        live,mvdm_keyboard_history_live(&nt_keyboard_history,live)))return FALSE;
    for (index = 0u; index < available; ++index) {
        if (raw[index].EventType == KEY_EVENT) {
            DWORD first=used,j,produced;
            produced=nt_rdp_normalize_key(&raw[index].Event.KeyEvent,records+used,capacity-used);
            used+=produced;
            /* A scan-less character may expand into several synthetic PC
             * transitions. It has no single physical held-key receipt. */
            for(j=first;j<used;++j)if(records[j].EventType==KEY_EVENT) {
                uint64_t receipt=produced==1 ? raw_receipts[index] : 0;
                key_receipts[key_count++]=receipt;if(receipt)tagged=TRUE;
            }
        } else records[used++] = raw[index];
    }
    if(tagged && !mvdm_keyboard_history_stage_receipts(&nt_keyboard_history,key_receipts,key_count)) {
        SetLastError(ERROR_INVALID_STATE);return FALSE;
    }
    *read = used;
    return TRUE;
}
