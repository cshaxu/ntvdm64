#include "mvdm_vm_executor.h"
#include "mvdm_softpc_guest_memory.h"

typedef struct command_job {
    struct command_job *next;
    mvdm_vm_executor *owner;
    mvdm_vm_continuation *continuation;
    ntvdm_command_launch *launch;
    ntvdm_vm_text *text;
    mvdm_vm_io *io;
    uint64_t token;
    uint64_t generation;
    uint32_t vm;
    BOOL finishing;
    KEY_EVENT_RECORD held[256]; /* Per-command physical make/release pairing. */
} command_job;

struct mvdm_vm_executor {
    SRWLOCK lock;
    CONDITION_VARIABLE changed;
    ntvdm_vm_host *host;
    uint64_t payload;
    command_job *jobs;
    ULONG active;
    BOOL closing;
};

DWORD mvdm_vm_executor_create(ntvdm_vm_host *host, mvdm_vm_executor **output)
{
    mvdm_vm_executor *executor;
    DWORD error;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output = NULL;
    if (!host) return ERROR_INVALID_PARAMETER;
    executor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*executor));
    if (!executor) return ERROR_NOT_ENOUGH_MEMORY;
    InitializeSRWLock(&executor->lock);
    InitializeConditionVariable(&executor->changed);
    executor->host = host;
    error = ntvdm_vm_payload_begin(host, &executor->payload);
    if (error) { HeapFree(GetProcessHeap(), 0, executor); return error; }
    *output = executor;
    return ERROR_SUCCESS;
}

static VOID CALLBACK execute(PTP_CALLBACK_INSTANCE instance, void *context)
{
    command_job *job = context, **p;
    mvdm_vm_executor *owner = job->owner;
    common_frontend_location location = {0};
    mvdm_vm_completion completion = {0};
    DWORD error, status, exit_code = 0;
    (void)instance;
    /* VMM may reuse a numeric VM handle before its old native chain finishes
     * retirement. Serialize only that handle's frontend incarnation off CPU;
     * independent VMs do not wait for one another. Tokens order admissions. */
    AcquireSRWLockExclusive(&owner->lock);
    for (;;) {
        command_job *older;
        for (older = owner->jobs; older; older = older->next)
            if (older->vm == job->vm && older->token < job->token) break;
        if (!older) break;
        if (!SleepConditionVariableSRW(&owner->changed, &owner->lock, INFINITE, 0))
            ExitProcess(GetLastError());
    }
    ReleaseSRWLockExclusive(&owner->lock);
    status = WaitForSingleObject(mvdm_vm_continuation_invalidated(job->continuation), 0);
    completion.error = status == WAIT_TIMEOUT ? ERROR_SUCCESS :
        status == WAIT_FAILED ? GetLastError() : ERROR_OPERATION_ABORTED;
    if (!completion.error)
        completion.error = ntvdm_vm_host_register(owner->host, owner->payload,
            job->vm, mvdm_vm_io_ready(job->io), &mvdm_vm_io_operations, job->io, &location);
    if (!completion.error)
        completion.error = mvdm_vm_command_execute(job->continuation, job->launch,
            owner->host, &location, &exit_code);
    completion.exit_code = exit_code;
    /* The command has ended, but retirement still owns live I/O callbacks.
     * Reject new producer events before that join; lifetime is not admission. */
    AcquireSRWLockExclusive(&owner->lock);
    job->finishing = TRUE;
    ReleaseSRWLockExclusive(&owner->lock);
    if (location.incarnation) {
        error = ntvdm_vm_host_retire(owner->host, &location);
        /* Failed retirement cannot free memory under unjoined I/O callbacks.
         * This is an unrecoverable local lifecycle failure, not success. */
        if (error && error != ERROR_NOT_FOUND) ExitProcess(error);
    }
    error = mvdm_vm_io_return_input(job->io, job->vm, job->generation);
    if (error && !completion.error)
        completion.error = error == ERROR_NOT_FOUND ? ERROR_OPERATION_ABORTED : error;
    /* Remove input access before destroying its provider. Close waits for the
     * job until ALL resources and the completion producer are finished. */
    AcquireSRWLockExclusive(&owner->lock);
    for (p = &owner->jobs; *p != job; p = &(*p)->next) {}
    *p = job->next;
    mvdm_vm_io_destroy(job->io);
    job->io = NULL;
    ReleaseSRWLockExclusive(&owner->lock);
    ntvdm_command_launch_destroy(job->launch);
    ntvdm_vm_text_destroy(job->text);
    error = mvdm_vm_continuation_complete(job->continuation, &completion);
    if (error && error != ERROR_OPERATION_ABORTED) ExitProcess(error);
    mvdm_vm_continuation_release(job->continuation);
    AcquireSRWLockExclusive(&owner->lock);
    --owner->active;
    HeapFree(GetProcessHeap(), 0, job);
    WakeAllConditionVariable(&owner->changed);
    ReleaseSRWLockExclusive(&owner->lock);
}

DWORD mvdm_vm_executor_submit(mvdm_vm_executor *owner, uint32_t vm,
    ntvdm_command_launch *launch, ntvdm_vm_text *text, uint64_t *token)
{
    command_job *job;
    DWORD error;
    if (!token) return ERROR_INVALID_PARAMETER;
    *token = 0;
    if (!owner || !launch || !text) return ERROR_INVALID_PARAMETER;
    job = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*job));
    if (!job) return ERROR_NOT_ENOUGH_MEMORY;
    job->owner = owner; job->vm = vm; job->launch = launch; job->text = text;
    /* Never hold the executor lock across ICA operations. CPU admission may
     * already be inside ICA, so close/input must not invert that lock order. */
    error = mvdm_vm_continuation_begin(vm, &job->token, &job->continuation);
    if (!error) job->generation = mvdm_vm_continuation_generation(job->continuation);
    if (!error)
        error = mvdm_vm_io_create(text, job->continuation, "VM command", &job->io);
    if (!error) {
        AcquireSRWLockExclusive(&owner->lock);
        if (owner->closing) error = ERROR_OPERATION_ABORTED;
        else if (owner->active == MAXDWORD) error = ERROR_ARITHMETIC_OVERFLOW;
        else {
            command_job *other;
            for (other = owner->jobs; other; other = other->next)
                if (other->vm == vm && other->generation == job->generation) break;
            if (other) error = ERROR_BUSY;
            else {
                job->next = owner->jobs;
                owner->jobs = job;
                if (!TrySubmitThreadpoolCallback(execute, job, NULL)) {
                    error = GetLastError();
                    owner->jobs = job->next;
                } else { ++owner->active; *token = job->token; }
            }
        }
        ReleaseSRWLockExclusive(&owner->lock);
    }
    if (error) {
        mvdm_vm_io_destroy(job->io);
        mvdm_vm_continuation_abandon(job->continuation);
        mvdm_vm_continuation_release(job->continuation);
        HeapFree(GetProcessHeap(), 0, job);
    }
    return error;
}

DWORD mvdm_vm_executor_key(mvdm_vm_executor *owner, uint32_t vm, uint64_t token,
    const KEY_EVENT_RECORD *key)
{
    command_job *job;
    DWORD error = ERROR_NOT_FOUND;
    if (!owner || !vm || !key) return ERROR_INVALID_PARAMETER;
    AcquireSRWLockShared(&owner->lock);
    for (job = owner->jobs; job && job->token != token; job = job->next) {}
    if (owner->closing) error = ERROR_OPERATION_ABORTED;
    else if (job && job->vm == vm && job->io)
        error = job->finishing ? ERROR_OPERATION_ABORTED : mvdm_vm_io_key(job->io, key);
    ReleaseSRWLockShared(&owner->lock);
    return error;
}

DWORD mvdm_vm_executor_key_bop(mvdm_vm_executor *owner, uint32_t vm,
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
    return mvdm_vm_executor_key(owner, vm, token, &key);
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
DWORD mvdm_vm_executor_bios_key_bop(mvdm_vm_executor *owner, uint32_t vm,
    uint32_t address, uint32_t extent)
{
    uint32_t descriptor[7], physical;
    KEY_EVENT_RECORD key = {0};
    command_job *job;
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
    else if (job && job->vm == vm && job->io) {
        if (job->finishing) error = ERROR_OPERATION_ABORTED;
        else if (descriptor[3] == 2) {
            /* Original nt_event.c reconstructs mapped BIOS characters as native
             * down/up records. They have no remaining physical key to pair:
             * Alt was already released when the BIOS committed this word. */
            KEY_EVENT_RECORD pair[2] = {key, key};
            pair[1].bKeyDown = FALSE;
            error = mvdm_vm_io_keys(job->io, pair, 2);
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
            error = mvdm_vm_io_key(job->io, &key);
            if (!error) {
                if (descriptor[3]) job->held[physical] = key;
                else ZeroMemory(&job->held[physical], sizeof(key));
            }
        }
    }
    ReleaseSRWLockExclusive(&owner->lock);
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
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
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

void mvdm_vm_executor_close(mvdm_vm_executor *owner)
{
    if (!owner) return;
    AcquireSRWLockExclusive(&owner->lock);
    owner->closing = TRUE;
    ReleaseSRWLockExclusive(&owner->lock);
    mvdm_vm_continuation_reset();
    AcquireSRWLockExclusive(&owner->lock);
    while (owner->active)
        if (!SleepConditionVariableSRW(&owner->changed, &owner->lock, INFINITE, 0))
            ExitProcess(GetLastError());
    ReleaseSRWLockExclusive(&owner->lock);
}

void mvdm_vm_executor_destroy(mvdm_vm_executor *owner)
{
    if (!owner) return;
    mvdm_vm_executor_close(owner);
    /* The worker closes the host next; it owns the payload registry. */
    HeapFree(GetProcessHeap(), 0, owner);
}
