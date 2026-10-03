#include "window_keyboard.h"

#define KEY_LOCK_BITS (CAPSLOCK_ON | NUMLOCK_ON | SCROLLLOCK_ON)

DWORD frontend_keyboard_dispatch(frontend_keyboard_delivery *delivery,
    const frontend_window_input *input,frontend_keyboard_sink sink,void *context)
{
    frontend_window_keyboard_state next;
    INPUT_RECORD physical={0},records[FRONTEND_NATIVE_KEY_RECORDS];DWORD count=1,error;
    const kvm_input_event *event;
    if(!delivery || !input || !sink)return ERROR_INVALID_PARAMETER;
    event=&input->event;next=delivery->physical;
    if(event->type==KVM_EVENT_SOURCE_RETIRED || event->type==KVM_EVENT_INPUT_RESET) {
        if(next.source_identity && next.source_identity!=event->source_identity)return ERROR_SUCCESS;
        frontend_window_keyboard_release_begin(&next,event->type==KVM_EVENT_SOURCE_RETIRED);
        while(frontend_window_keyboard_release_next(&next,&physical)) {
            error=sink(context,&physical,1);if(error)return error;
            delivery->physical=next;
        }
        if(GetLastError()!=ERROR_NO_DATA)return GetLastError();
        delivery->physical=next;
        return frontend_native_keyboard_reset(&delivery->native);
    }
    if(event->type!=KVM_EVENT_KEY && event->type!=KVM_EVENT_TEXT)return ERROR_SUCCESS;
    if(event->type==KVM_EVENT_TEXT) {
        if(!event->source_identity || next.releasing ||
            (next.source_identity && next.source_identity!=event->source_identity))
            return ERROR_INVALID_STATE;
        next.source_identity=event->source_identity;
    } else if(!frontend_window_keyboard_accept(&next,event,input->control_state,input->keyboard_layout,&physical))
        return GetLastError();
    {
        /* Original ntcon/HandleKeyEvent supplies character-bearing records
         * even to VDM. ReturnUnusedKeyEvents may give those records back to
         * a native reader, which cannot reconstruct text from scans alone. */
        error=frontend_native_keyboard_records(&delivery->native,event,&physical,input->keyboard_layout,records,&count);
        if(error)return error;
    }
    error=sink(context,records,count);
    if(!error)delivery->physical=next;
    return error;
}

DWORD frontend_native_keyboard_reset(frontend_native_keyboard *state)
{
    BYTE keys[256]={0};WCHAR text[FRONTEND_NATIVE_KEY_RECORDS];
    if(!state)return ERROR_INVALID_PARAMETER;
    if(state->thread && state->thread!=GetCurrentThreadId())return ERROR_INVALID_THREAD_ID;
    if(state->layout) {
        /* Consume a pending dead key in this owner's translation state only.
         * No synthesized space is sent to either the guest or native Console. */
        if(ToUnicodeEx(VK_SPACE,MapVirtualKeyExW(VK_SPACE,MAPVK_VK_TO_VSC,state->layout),
            keys,text,ARRAYSIZE(text),0,state->layout)<0)return ERROR_INVALID_STATE;
    }
    ZeroMemory(state,sizeof(*state));
    return ERROR_SUCCESS;
}

DWORD frontend_native_keyboard_records(frontend_native_keyboard *state,
    const kvm_input_event *event,const INPUT_RECORD *physical,HKL layout,
    INPUT_RECORD records[FRONTEND_NATIVE_KEY_RECORDS],DWORD *count)
{
    BYTE keys[256]={0};WCHAR text[FRONTEND_NATIVE_KEY_RECORDS];
    DWORD control,error,i;int length;
    if(!state || !event || !physical || !layout || !records || !count)return ERROR_INVALID_PARAMETER;
    *count=0;
    if(state->thread && state->thread!=GetCurrentThreadId())return ERROR_INVALID_THREAD_ID;
    if(state->layout && state->layout!=layout) {
        error=frontend_native_keyboard_reset(state);if(error)return error;
    }
    if(event->type==KVM_EVENT_TEXT) {
        DWORD scalar=event->data.text.scalar;
        if(!scalar || scalar>0x10ffff || (scalar>=0xd800 && scalar<=0xdfff))return ERROR_NO_UNICODE_TRANSLATION;
        ZeroMemory(records,2*sizeof(*records));
        records[0].EventType=KEY_EVENT;records[0].Event.KeyEvent.bKeyDown=TRUE;
        records[0].Event.KeyEvent.wRepeatCount=1;records[0].Event.KeyEvent.wVirtualKeyCode=VK_PACKET;
        if(scalar<=0xffff) { records[0].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)scalar;*count=1; }
        else {
            scalar-=0x10000;records[1]=records[0];
            records[0].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0xd800+(scalar>>10));
            records[1].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0xdc00+(scalar&1023));*count=2;
        }
        return ERROR_SUCCESS;
    }
    if(event->type!=KVM_EVENT_KEY || physical->EventType!=KEY_EVENT ||
        physical->Event.KeyEvent.wVirtualKeyCode>255)return ERROR_INVALID_PARAMETER;
    state->layout=layout;state->thread=GetCurrentThreadId();
    records[0]=*physical;records[0].Event.KeyEvent.uChar.UnicodeChar=0;
    *count=1;
    if(!physical->Event.KeyEvent.bKeyDown)return ERROR_SUCCESS;
    control=physical->Event.KeyEvent.dwControlKeyState;
    keys[physical->Event.KeyEvent.wVirtualKeyCode]=0x80;
    if(control&SHIFT_PRESSED)keys[VK_SHIFT]=0x80;
    if(control&LEFT_CTRL_PRESSED)keys[VK_LCONTROL]=keys[VK_CONTROL]=0x80;
    if(control&RIGHT_CTRL_PRESSED)keys[VK_RCONTROL]=keys[VK_CONTROL]=0x80;
    if(control&LEFT_ALT_PRESSED)keys[VK_LMENU]=keys[VK_MENU]=0x80;
    if(control&RIGHT_ALT_PRESSED)keys[VK_RMENU]=keys[VK_MENU]=0x80;
    if(control&CAPSLOCK_ON)keys[VK_CAPITAL]=1;
    length=ToUnicodeEx(physical->Event.KeyEvent.wVirtualKeyCode,
        physical->Event.KeyEvent.wVirtualScanCode,keys,text,ARRAYSIZE(text),0,layout);
    if(length>FRONTEND_NATIVE_KEY_RECORDS)return ERROR_INSUFFICIENT_BUFFER;
    if(length>0) {
        for(i=0;i<(DWORD)length;++i) {
            records[i]=*physical;records[i].Event.KeyEvent.uChar.UnicodeChar=text[i];
            /* OpenNT keyconv.c marks all but the last character of one
             * translation FAKE_KEYSTROKE; ntcon clears their scan. VK_PACKET
             * retains that character-only identity at our copied boundary. */
            if(i+1u<(DWORD)length) {
                records[i].Event.KeyEvent.wVirtualScanCode=0;
                records[i].Event.KeyEvent.wVirtualKeyCode=VK_PACKET;
            }
        }
        *count=(DWORD)length;
    }
    return ERROR_SUCCESS;
}

static BOOL prospective_down(const frontend_window_keyboard_state *state,
    unsigned scan, unsigned changed, BOOL down)
{
    return scan == changed ? down : state->held[scan].bKeyDown;
}

static DWORD delivered_modifiers(const frontend_window_keyboard_state *state,
    const kvm_input_event *event, DWORD native)
{
    unsigned changed = event->data.key.scan_code |
        ((event->data.key.flags & KVM_KEY_FLAG_EXTENDED) ? 256u : 0u);
    BOOL down = event->data.key.pressed != 0;
    DWORD result = native & KEY_LOCK_BITS, sides;
    /* kvm-base can emit a synthetic physical chord for WM_CHAR. Its modifier
     * facts, not the physical host state, govern that chord. Preserve side
     * identity from delivered scans, using the UI snapshot only when no scan
     * established it (for example a modifier already held on focus entry). */
    if (event->data.key.modifiers & KVM_KEY_MODIFIER_CONTROL) {
        sides = (prospective_down(state, 0x1d, changed, down) ? LEFT_CTRL_PRESSED : 0) |
            (prospective_down(state, 0x11d, changed, down) ? RIGHT_CTRL_PRESSED : 0);
        if (!sides) sides = native & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
        result |= sides ? sides : LEFT_CTRL_PRESSED;
    }
    if (event->data.key.modifiers & KVM_KEY_MODIFIER_ALT) {
        sides = (prospective_down(state, 0x38, changed, down) ? LEFT_ALT_PRESSED : 0) |
            (prospective_down(state, 0x138, changed, down) ? RIGHT_ALT_PRESSED : 0);
        if (!sides) sides = native & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED);
        result |= sides ? sides : LEFT_ALT_PRESSED;
    }
    if (event->data.key.modifiers & KVM_KEY_MODIFIER_SHIFT) result |= SHIFT_PRESSED;
    return result;
}

BOOL frontend_window_keyboard_record(const kvm_input_event *event,
    DWORD control_state, HKL layout, INPUT_RECORD *record)
{
    KEY_EVENT_RECORD *key;
    UINT scan, vk;
    if (!event || !record) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    ZeroMemory(record, sizeof(*record));
    key = &record->Event.KeyEvent;
    key->wRepeatCount = 1;
    key->dwControlKeyState = control_state & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED |
        RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED | SHIFT_PRESSED | NUMLOCK_ON |
        SCROLLLOCK_ON | CAPSLOCK_ON);
    if (event->type == KVM_EVENT_TEXT) {
        /* Original nt_rdp_normalize_key consumes one representable WCHAR.
         * Do not truncate supplementary scalars into unrelated DOS keys. */
        if (!event->data.text.scalar || event->data.text.scalar > 0xffffu ||
            (event->data.text.scalar >= 0xd800u && event->data.text.scalar <= 0xdfffu)) {
            SetLastError(ERROR_NO_UNICODE_TRANSLATION); return FALSE;
        }
        key->bKeyDown = TRUE;
        key->wVirtualKeyCode = VK_PACKET;
        key->uChar.UnicodeChar = (WCHAR)event->data.text.scalar;
    } else if (event->type == KVM_EVENT_KEY) {
        scan = event->data.key.scan_code;
        if (!scan || scan > 0xffu) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
        if (event->data.key.flags & KVM_KEY_FLAG_EXTENDED) {
            scan |= 0xe000u;
            key->dwControlKeyState |= ENHANCED_KEY;
        }
        vk = MapVirtualKeyExW(scan, MAPVK_VSC_TO_VK_EX, layout);
        /* The original nt_event special cases use generic modifier VKs.
         * Pause and PrintScreen share historical scan identities with other
         * keys, so preserve the normalized physical key's explicit identity. */
        switch (event->data.key.key) {
        case KVM_KEY_CONTROL: vk = VK_CONTROL; break;
        case KVM_KEY_ALT: vk = VK_MENU; break;
        case KVM_KEY_SHIFT: vk = VK_SHIFT; break;
        case KVM_KEY_PAUSE: vk = VK_PAUSE; break;
        case KVM_KEY_PRINT_SCREEN: vk = VK_SNAPSHOT; break;
        default: break;
        }
        if (!vk) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
        key->wVirtualKeyCode = (WORD)vk;
        key->wVirtualScanCode = event->data.key.scan_code;
        key->bKeyDown = event->data.key.pressed != 0;
        /* No synthetic Unicode echo for an already accepted physical key. */
    } else { SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }
    record->EventType = KEY_EVENT;
    return TRUE;
}

BOOL frontend_window_keyboard_accept(frontend_window_keyboard_state *state,
    const kvm_input_event *event, DWORD control_state, HKL layout, INPUT_RECORD *record)
{
    unsigned identity;
    if (!state || !event || !record || !event->source_identity) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (state->releasing || (state->source_identity &&
        state->source_identity != event->source_identity)) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (!frontend_window_keyboard_record(event, event->type == KVM_EVENT_KEY ?
        delivered_modifiers(state, event, control_state) : control_state, layout, record)) return FALSE;
    state->source_identity = event->source_identity;
    state->locks = control_state & KEY_LOCK_BITS;
    if (event->type == KVM_EVENT_KEY) {
        identity = event->data.key.scan_code |
            ((event->data.key.flags & KVM_KEY_FLAG_EXTENDED) ? 256u : 0u);
        if (record->Event.KeyEvent.bKeyDown) {
            if (!state->held[identity].bKeyDown) ++state->held_count;
            state->held[identity] = record->Event.KeyEvent;
        } else {
            if (state->held[identity].bKeyDown) --state->held_count;
            ZeroMemory(&state->held[identity], sizeof(state->held[identity]));
        }
    }
    return TRUE;
}

void frontend_window_keyboard_release_begin(frontend_window_keyboard_state *state, BOOL retiring)
{
    state->releasing = TRUE;
    state->retiring = state->retiring || retiring;
}

BOOL frontend_window_keyboard_release_next(frontend_window_keyboard_state *state, INPUT_RECORD *record)
{
    unsigned i;
    DWORD modifiers;
    if (!state || !record) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    ZeroMemory(record, sizeof(*record));
    if (state->releasing) {
        for (i = 0; i < 512; ++i) if (state->held[i].bKeyDown) {
            record->EventType = KEY_EVENT;
            record->Event.KeyEvent = state->held[i];
            ZeroMemory(&state->held[i], sizeof(state->held[i]));
            --state->held_count;
            modifiers = state->locks;
            if (state->held[0x1d].bKeyDown) modifiers |= LEFT_CTRL_PRESSED;
            if (state->held[0x11d].bKeyDown) modifiers |= RIGHT_CTRL_PRESSED;
            if (state->held[0x38].bKeyDown) modifiers |= LEFT_ALT_PRESSED;
            if (state->held[0x138].bKeyDown) modifiers |= RIGHT_ALT_PRESSED;
            if (state->held[0x2a].bKeyDown || state->held[0x36].bKeyDown) modifiers |= SHIFT_PRESSED;
            if (i & 256u) modifiers |= ENHANCED_KEY;
            record->Event.KeyEvent.bKeyDown = FALSE;
            record->Event.KeyEvent.wRepeatCount = 1;
            record->Event.KeyEvent.dwControlKeyState = modifiers;
            return TRUE;
        }
        state->releasing = FALSE;
        if (state->retiring) state->source_identity = 0;
        state->retiring = FALSE;
    }
    SetLastError(ERROR_NO_DATA);
    return FALSE;
}
