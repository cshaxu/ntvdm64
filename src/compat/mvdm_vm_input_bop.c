#include "include/mvdm_vm_input.h"
#include "include/guest_memory_access.h"

DWORD mvdm_vm_input_send_key(mvdm_vm_rundown *owner, uint32_t vm, uint64_t token,
    const KEY_EVENT_RECORD *key)
{
    mvdm_vm_input_job *job;
    DWORD error = ERROR_NOT_FOUND;
    if (!owner || !vm || !key) return ERROR_INVALID_PARAMETER;
    AcquireSRWLockShared(&owner->lock);
    for (job = owner->jobs; job && job->token != token; job = job->next) {}
    if (owner->closing) error = ERROR_OPERATION_ABORTED;
    else if (job && job->vm == vm && job->input)
        error = job->finishing ? ERROR_OPERATION_ABORTED : mvdm_vm_input_key(job->input, key);
    ReleaseSRWLockShared(&owner->lock);
    return error;
}

DWORD mvdm_vm_input_key_bop(mvdm_vm_rundown *owner, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t descriptor[8];
    KEY_EVENT_RECORD key = {0};
    uint64_t token;
    if (!owner) return ERROR_NOT_READY;
    if (extent != sizeof(descriptor)) return ERROR_INVALID_PARAMETER;
    if (!mvdm_softpc_guest_memory_copy_from(address, (uint8_t *)descriptor,
        sizeof(descriptor))) return ERROR_INVALID_ADDRESS;
    if (descriptor[2] > 1 || !descriptor[3] || descriptor[3] > UINT16_MAX ||
        descriptor[4] > UINT16_MAX || descriptor[5] > UINT16_MAX ||
        descriptor[6] > UINT16_MAX) return ERROR_INVALID_DATA;
    token = ((uint64_t)descriptor[1] << 32) | descriptor[0];
    key.bKeyDown = (BOOL)descriptor[2];
    key.wRepeatCount = (WORD)descriptor[3];
    key.wVirtualKeyCode = (WORD)descriptor[4];
    key.wVirtualScanCode = (WORD)descriptor[5];
    key.uChar.UnicodeChar = (WCHAR)descriptor[6];
    key.dwControlKeyState = descriptor[7];
    return mvdm_vm_input_send_key(owner, vm, token, &key);
}

/* Original BIOS character conversion; physical identity and current BIOS
 * modifier state come from the same guest VM, not the host foreground state. */
extern BOOL BiosKeyToInputRecord(PKEY_EVENT_RECORD);
static WORD bios_modifier(uint32_t physical)
{
    switch (physical) {
    case 0x2a: case 0x36: return VK_SHIFT;
    case 0x1d: case 0x9d: return VK_CONTROL;
    case 0x38: case 0xb8: return VK_MENU;
    default: return 0;
    }
}
/* AH4F precedes the original BIOS state update. Project only this edge onto
 * the copied BIOS flags; the guest remains the owner of the stored state. */
static void bios_modifier_edge(uint32_t physical, BOOL down, uint32_t *flags, uint32_t *extended)
{
    uint32_t bit = 0;
    switch (physical) {
    case 0x2a: bit = 2; break;
    case 0x36: bit = 1; break;
    case 0x1d: bit = 0x100; break;
    case 0x38: bit = 0x200; break;
    case 0x9d: *extended = (*extended & ~4u) | (down ? 4u : 0); break;
    case 0xb8: *extended = (*extended & ~8u) | (down ? 8u : 0); break;
    }
    *flags = (*flags & ~bit) | (down ? bit : 0);
    if (physical == 0x1d || physical == 0x9d)
        *flags = (*flags & ~4u) | ((*flags & 0x100) || (*extended & 4) ? 4u : 0);
    if (physical == 0x38 || physical == 0xb8)
        *flags = (*flags & ~8u) | ((*flags & 0x200) || (*extended & 8) ? 8u : 0);
}
static DWORD bios_controls(uint32_t flags, uint32_t extended, uint32_t physical)
{
    DWORD state = physical & 128 ? ENHANCED_KEY : 0;
    if (flags & 3) state |= SHIFT_PRESSED;
    if (flags & 0x40) state |= CAPSLOCK_ON;
    if (flags & 0x20) state |= NUMLOCK_ON;
    if (flags & 0x10) state |= SCROLLLOCK_ON;
    if (flags & 0x100) state |= LEFT_CTRL_PRESSED;
    if (flags & 0x200) state |= LEFT_ALT_PRESSED;
    if (extended & 4) state |= RIGHT_CTRL_PRESSED;
    if (extended & 8) state |= RIGHT_ALT_PRESSED;
    if ((flags & 4) && !(state & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
        state |= LEFT_CTRL_PRESSED;
    if ((flags & 8) && !(state & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)))
        state |= LEFT_ALT_PRESSED;
    return state;
}
DWORD mvdm_vm_input_bios_key_bop(mvdm_vm_rundown *owner, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t descriptor[7], physical;
    KEY_EVENT_RECORD key = {0};
    mvdm_vm_input_job *job;
    uint64_t token;
    DWORD error = ERROR_NOT_FOUND;
    if (!owner) return ERROR_NOT_READY;
    if (extent != sizeof(descriptor)) return ERROR_INVALID_PARAMETER;
    if (!mvdm_softpc_guest_memory_copy_from(address, (uint8_t *)descriptor,
        sizeof(descriptor))) return ERROR_INVALID_ADDRESS;
    physical = descriptor[2];
    if (physical > 255 || descriptor[3] > 2 ||
        (descriptor[3] == 2 ? (physical || !descriptor[4] || descriptor[4] > 255) : !(physical & 127)) ||
        descriptor[4] > UINT16_MAX || descriptor[5] > UINT16_MAX ||
        descriptor[6] > 255 || (!descriptor[3] && descriptor[4])) return ERROR_INVALID_DATA;
    token = ((uint64_t)descriptor[1] << 32) | descriptor[0];
    if (descriptor[3]) {
        key.bKeyDown = TRUE;
        if (!descriptor[4] && bios_modifier(physical)) {
            key.wRepeatCount = 1;
            key.wVirtualKeyCode = bios_modifier(physical);
            key.wVirtualScanCode = (WORD)(physical & 127);
        } else {
            key.wVirtualScanCode = (WORD)(descriptor[4] >> 8);
            key.uChar.AsciiChar = (CHAR)(descriptor[4] & 255);
            if (!BiosKeyToInputRecord(&key)) return ERROR_NO_UNICODE_TRANSLATION;
            /* The original unmappable-character branch reconstructs a full
             * Alt/numpad sequence. Do not label a plain pair equivalent. */
            if (descriptor[3] == 2 && !key.wVirtualScanCode) return ERROR_NOT_SUPPORTED;
        }
    }
    AcquireSRWLockExclusive(&owner->lock);
    for (job = owner->jobs; job && job->token != token; job = job->next) {}
    if (owner->closing) error = ERROR_OPERATION_ABORTED;
    else if (job && job->vm == vm && job->input) {
        if (job->finishing) error = ERROR_OPERATION_ABORTED;
        else if (descriptor[3] == 2) {
            /* Original nt_event.c reconstructs mapped BIOS characters as native
             * down/up records. They have no remaining physical key to pair:
             * Alt was already released when the BIOS committed this word. */
            KEY_EVENT_RECORD pair[2] = {key, key};
            pair[1].bKeyDown = FALSE;
            error = mvdm_vm_input_keys(job->input, pair, 2);
        } else if (descriptor[3] || job->held[physical].bKeyDown) {
            if (!descriptor[3]) {
                key = job->held[physical];
                key.bKeyDown = FALSE;
                key.wRepeatCount = 1;
                key.uChar.UnicodeChar = 0; /* Same physical KEYUP shape as NTCON. */
            }
            if (bios_modifier(physical))
                bios_modifier_edge(physical, descriptor[3] != 0, &descriptor[5], &descriptor[6]);
            key.dwControlKeyState = bios_controls(descriptor[5], descriptor[6], physical);
            error = mvdm_vm_input_key(job->input, &key);
            if (!error) {
                if (descriptor[3]) job->held[physical] = key;
                else ZeroMemory(&job->held[physical], sizeof(key));
            }
        }
    }
    ReleaseSRWLockExclusive(&owner->lock);
    return error;
}

DWORD mvdm_vm_input_mouse_bop(mvdm_vm_rundown *owner, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t d[7];
    uint64_t token;
    mvdm_vm_input_job *job;
    MOUSE_EVENT_RECORD mouse = {0};
    DWORD error = ERROR_NOT_FOUND;
    if (!owner) return ERROR_NOT_READY;
    if (extent != sizeof(d)) return ERROR_INVALID_PARAMETER;
    if (!mvdm_softpc_guest_memory_copy_from(address, (uint8_t *)d, sizeof(d)))
        return ERROR_INVALID_ADDRESS;
    if (!d[2] || d[2] > 127 || d[3] > 7 || d[4] > INT16_MAX ||
        d[5] > INT16_MAX || d[6] > 0xffffffu) return ERROR_INVALID_DATA;
    token = ((uint64_t)d[1] << 32) | d[0];
    mouse.dwMousePosition.X = (SHORT)d[4];
    mouse.dwMousePosition.Y = (SHORT)d[5];
    mouse.dwButtonState = (d[3] & 1 ? FROM_LEFT_1ST_BUTTON_PRESSED : 0) |
        (d[3] & 2 ? RIGHTMOST_BUTTON_PRESSED : 0) |
        (d[3] & 4 ? FROM_LEFT_2ND_BUTTON_PRESSED : 0);
    mouse.dwControlKeyState = bios_controls(d[6] & 0xffffu, d[6] >> 16, 0);
    mouse.dwEventFlags = (d[2] & 0x7e) ? 0 : MOUSE_MOVED;
    AcquireSRWLockExclusive(&owner->lock);
    for (job = owner->jobs; job; job = job->next)
        if (job->vm == vm && job->token == token) break;
    if (job && !job->finishing && !owner->closing)
        error = mvdm_vm_input_mouse(job->input, &mouse);
    ReleaseSRWLockExclusive(&owner->lock);
    return error;
}
