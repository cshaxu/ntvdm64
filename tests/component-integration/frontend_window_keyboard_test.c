#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "window_keyboard.h"

typedef struct delivery_check { DWORD error,count;BOOL down; } delivery_check;
typedef struct returned_input { INPUT_RECORD records[16];DWORD count; } returned_input;
static DWORD returned_sink(void *context,const INPUT_RECORD *records,DWORD count)
{
    returned_input *input=context;
    if(count>ARRAYSIZE(input->records)-input->count)return ERROR_INSUFFICIENT_BUFFER;
    memcpy(input->records+input->count,records,count*sizeof(*records));input->count+=count;
    return ERROR_SUCCESS;
}
static void returned_dos_tests(BOOL cooked)
{
    frontend_keyboard_delivery delivery={0};frontend_window_input input={0};
    returned_input returned={0};DWORD index;
    const WORD scans[]={0x1e,0x30,0x1c};
    const WCHAR characters[]={L'a',L'b',L'\r'};
    input.keyboard_layout=LoadKeyboardLayoutW(L"00000409",0);assert(input.keyboard_layout);
    input.event.type=KVM_EVENT_KEY;input.event.source_identity=51;
    for(index=0;index<ARRAYSIZE(scans);++index) {
        input.event.data.key.scan_code=scans[index];input.event.data.key.pressed=TRUE;
        assert(!frontend_keyboard_dispatch(&delivery,&input,returned_sink,&returned));
        input.event.data.key.pressed=FALSE;
        assert(!frontend_keyboard_dispatch(&delivery,&input,returned_sink,&returned));
        assert(returned.count==2*(index+1));
        assert(returned.records[2*index].Event.KeyEvent.wVirtualScanCode==scans[index]);
        assert(returned.records[2*index].Event.KeyEvent.uChar.UnicodeChar==characters[index]);
    }
    if(cooked) {
        HANDLE console=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        DWORD mode=0,written,read;WCHAR text[16];
        assert(console!=INVALID_HANDLE_VALUE && GetConsoleMode(console,&mode));
        assert(SetConsoleMode(console,ENABLE_LINE_INPUT|ENABLE_PROCESSED_INPUT));
        assert(FlushConsoleInputBuffer(console));
        assert(WriteConsoleInputW(console,returned.records,returned.count,&written) && written==returned.count);
        assert(ReadConsoleW(console,text,ARRAYSIZE(text),&read,NULL));
        assert(read==4 && !memcmp(text,L"ab\r\n",4*sizeof(WCHAR)));
        assert(SetConsoleMode(console,mode));CloseHandle(console);
    }
    assert(!frontend_native_keyboard_reset(&delivery.native));
    puts("PASS DOS-dispatched records retain one physical make/break and native cooked typeahead characters");
}
static DWORD checked_sink(void *context,const INPUT_RECORD *records,DWORD count)
{
    delivery_check *check=context;
    if(check->error)return check->error;
    check->count+=count;check->down=records[count-1].Event.KeyEvent.bKeyDown;
    return ERROR_SUCCESS;
}
static void delivery_tests(void)
{
    frontend_keyboard_delivery delivery={0};frontend_window_input input={0};
    delivery_check check={ERROR_BROKEN_PIPE,0,FALSE};
    input.keyboard_layout=GetKeyboardLayout(0);
    input.event.source_identity=37;input.event.type=KVM_EVENT_KEY;
    input.event.data.key.scan_code=0x1e;input.event.data.key.key='A';input.event.data.key.pressed=TRUE;
    assert(frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check)==ERROR_BROKEN_PIPE);
    assert(delivery.physical.held_count==0 && !delivery.physical.source_identity);
    check.error=0;
    assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
    assert(delivery.physical.held_count==1 && check.count==1 && check.down);
    input.event.type=KVM_EVENT_SOURCE_RETIRED;check.error=ERROR_BROKEN_PIPE;
    assert(frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check)==ERROR_BROKEN_PIPE);
    assert(delivery.physical.held_count==1 && check.count==1);
    check.error=0;
    assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
    assert(delivery.physical.held_count==0 && !delivery.physical.source_identity && check.count==2 && !check.down);
    puts("PASS failed sink does not commit press/release; successful retirement sends one matching break");
}

static void input_reset_tests(void)
{
    frontend_keyboard_delivery delivery={0};frontend_window_input input={0};
    delivery_check check={0};unsigned native;
    for(native=0;native<2;++native) {
        ZeroMemory(&delivery,sizeof(delivery));ZeroMemory(&check,sizeof(check));
        input.keyboard_layout=GetKeyboardLayout(0);
        input.event.source_identity=37;input.event.type=KVM_EVENT_KEY;
        input.event.data.key.scan_code=0x1e;input.event.data.key.key='A';input.event.data.key.pressed=TRUE;
        assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
        input.event.type=KVM_EVENT_INPUT_RESET;input.event.source_identity=38;
        assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
        assert(delivery.physical.held_count==1 && check.count==1);
        input.event.source_identity=37;check.error=ERROR_BROKEN_PIPE;
        assert(frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check)==ERROR_BROKEN_PIPE);
        assert(delivery.physical.held_count==1 && check.count==1);
        check.error=0;
        assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
        assert(delivery.physical.held_count==0 && delivery.physical.source_identity==37 && check.count==2 && !check.down);
        assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
        assert(check.count==2);
        input.event.type=KVM_EVENT_KEY;
        assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
        assert(check.count==3 && check.down && delivery.physical.held_count==1);
        input.event.type=KVM_EVENT_SOURCE_RETIRED;
        assert(!frontend_keyboard_dispatch(&delivery,&input,checked_sink,&check));
        assert(check.count==4 && !delivery.physical.source_identity);
    }
    puts("PASS DOS/native input reset: source isolation, rejected sink, idempotence and live-source reuse");
}

static void native_tests(BOOL cooked)
{
    frontend_native_keyboard state={0};
    kvm_input_event event={0};INPUT_RECORD physical={0},records[FRONTEND_NATIVE_KEY_RECORDS],line[4];
    DWORD count;HKL layout=LoadKeyboardLayoutW(L"00000409",0);
    assert(layout);
    event.type=KVM_EVENT_KEY;event.source_identity=1;event.data.key.key='A';
    event.data.key.scan_code=0x1e;event.data.key.pressed=TRUE;
    assert(frontend_window_keyboard_record(&event,0,layout,&physical));
    assert(!frontend_native_keyboard_records(&state,&event,&physical,layout,records,&count));
    assert(count==1 && records[0].Event.KeyEvent.uChar.UnicodeChar==L'a');line[0]=records[0];
    assert(physical.Event.KeyEvent.uChar.UnicodeChar==0); /* DOS record unchanged. */
    event.data.key.key='B';event.data.key.scan_code=0x30;
    assert(frontend_window_keyboard_record(&event,SHIFT_PRESSED,layout,&physical));
    assert(!frontend_native_keyboard_records(&state,&event,&physical,layout,records,&count));
    assert(count==1 && records[0].Event.KeyEvent.uChar.UnicodeChar==L'B');line[1]=records[0];
    event.data.key.key=KVM_KEY_ENTER;event.data.key.scan_code=0x1c;
    assert(frontend_window_keyboard_record(&event,0,layout,&physical));
    assert(!frontend_native_keyboard_records(&state,&event,&physical,layout,records,&count));
    assert(count==1 && records[0].Event.KeyEvent.uChar.UnicodeChar==L'\r');line[2]=records[0];
    event.data.key.pressed=FALSE;
    assert(frontend_window_keyboard_record(&event,0,layout,&physical));
    assert(!frontend_native_keyboard_records(&state,&event,&physical,layout,records,&count));
    assert(count==1 && !records[0].Event.KeyEvent.bKeyDown && !records[0].Event.KeyEvent.uChar.UnicodeChar);
    event.type=KVM_EVENT_TEXT;event.data.text.scalar=0x1f600;
    assert(!frontend_native_keyboard_records(&state,&event,&physical,layout,records,&count));
    assert(count==2 && records[0].Event.KeyEvent.uChar.UnicodeChar==0xd83d && records[1].Event.KeyEvent.uChar.UnicodeChar==0xde00);
    assert(!frontend_native_keyboard_reset(&state));
    {
        HKL international=LoadKeyboardLayoutW(L"00020409",0);
        assert(international);
        event.type=KVM_EVENT_KEY;event.data.key.pressed=TRUE;
        event.data.key.key='\'';event.data.key.scan_code=0x28;
        assert(frontend_window_keyboard_record(&event,0,international,&physical));
        assert(!frontend_native_keyboard_records(&state,&event,&physical,international,records,&count));
        assert(count==1 && !records[0].Event.KeyEvent.uChar.UnicodeChar);
        event.data.key.key='E';event.data.key.scan_code=0x12;
        assert(frontend_window_keyboard_record(&event,0,international,&physical));
        assert(!frontend_native_keyboard_records(&state,&event,&physical,international,records,&count));
        assert(count==1 && records[0].Event.KeyEvent.uChar.UnicodeChar==0xe9);
        event.data.key.key='\'';event.data.key.scan_code=0x28;
        assert(frontend_window_keyboard_record(&event,0,international,&physical));
        assert(!frontend_native_keyboard_records(&state,&event,&physical,international,records,&count));
        assert(!frontend_native_keyboard_reset(&state));
        event.data.key.key='E';event.data.key.scan_code=0x12;
        assert(frontend_window_keyboard_record(&event,0,international,&physical));
        assert(!frontend_native_keyboard_records(&state,&event,&physical,international,records,&count));
        assert(count==1 && records[0].Event.KeyEvent.uChar.UnicodeChar==L'e');
        assert(!frontend_native_keyboard_reset(&state));
        event.data.key.key='\'';event.data.key.scan_code=0x28;
        assert(frontend_window_keyboard_record(&event,0,international,&physical));
        assert(!frontend_native_keyboard_records(&state,&event,&physical,international,records,&count));
        event.data.key.key='B';event.data.key.scan_code=0x30;
        assert(frontend_window_keyboard_record(&event,0,international,&physical));
        assert(!frontend_native_keyboard_records(&state,&event,&physical,international,records,&count));
        assert(count==2 && records[0].Event.KeyEvent.uChar.UnicodeChar==L'\'' &&
            records[0].Event.KeyEvent.wVirtualScanCode==0 &&
            records[0].Event.KeyEvent.wVirtualKeyCode==VK_PACKET);
        assert(records[1].Event.KeyEvent.uChar.UnicodeChar==L'b' &&
            records[1].Event.KeyEvent.wVirtualScanCode==0x30);
        assert(!frontend_native_keyboard_reset(&state));
        puts("PASS noncomposing dead key retains character-only prefix and one physical trailing key");
        puts("PASS Windows dead-key composition and retirement discard without injecting a space");
    }
    if(cooked) {
        HANDLE input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        DWORD mode=0,read;WCHAR text[16];
        assert(input!=INVALID_HANDLE_VALUE && GetConsoleMode(input,&mode));
        assert(SetConsoleMode(input,ENABLE_LINE_INPUT|ENABLE_PROCESSED_INPUT));
        assert(FlushConsoleInputBuffer(input));
        assert(WriteConsoleInputW(input,line,3,&count) && count==3);
        assert(ReadConsoleW(input,text,16,&read,NULL));
        assert(read==4 && !memcmp(text,L"aB\r\n",4*sizeof(WCHAR)));
        {
            INPUT_RECORD returned[6];DWORD index;
            /* Original ReturnUnusedKeyEvents can hand the very same records
             * to a native Console. Scans are sufficient for the DOS keyboard,
             * but WriteConsoleInput does not translate them into characters.
             * A converted sentinel bounds this negative test without a hang. */
            memcpy(returned,line,3*sizeof(*line));
            for(index=0;index<3;++index)returned[index].Event.KeyEvent.uChar.UnicodeChar=0;
            memcpy(returned+3,line,3*sizeof(*line));
            assert(WriteConsoleInputW(input,returned,6,&count) && count==6);
            assert(ReadConsoleW(input,text,16,&read,NULL));
            assert(read==4 && !memcmp(text,L"aB\r\n",4*sizeof(WCHAR)));
            puts("PASS negative native contract: returned scan-only aB/Enter produces no cooked text; converted sentinel completes");
        }
        assert(SetConsoleMode(input,mode));CloseHandle(input);
        puts("PASS real Windows cooked ReadConsole consumes converted aB and Enter exactly once");
    }
    puts("PASS native layout characters, key-up, supplementary Unicode; DOS records unchanged");
}

int main(int argc,char **argv)
{
    input_reset_tests();
    frontend_window_keyboard_state state = {0};
    kvm_input_event event = {0};
    INPUT_RECORD record;
    HKL layout = GetKeyboardLayout(0);
    event.type = KVM_EVENT_KEY;
    event.data.key.key = 'A';
    event.data.key.scan_code = 0x1e;
    event.data.key.pressed = 1;
    assert(frontend_window_keyboard_record(&event, CAPSLOCK_ON | SHIFT_PRESSED, layout, &record));
    assert(record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown);
    assert(record.Event.KeyEvent.wVirtualScanCode == 0x1e);
    assert(record.Event.KeyEvent.uChar.UnicodeChar == 0);
    assert(record.Event.KeyEvent.dwControlKeyState == (CAPSLOCK_ON | SHIFT_PRESSED));
    event.data.key.pressed = 0;
    assert(frontend_window_keyboard_record(&event, 0, layout, &record));
    assert(!record.Event.KeyEvent.bKeyDown && record.Event.KeyEvent.wRepeatCount == 1);
    event.data.key.key = KVM_KEY_CONTROL;
    event.data.key.scan_code = 0x1d;
    event.data.key.flags = KVM_KEY_FLAG_EXTENDED;
    event.data.key.pressed = 1;
    assert(frontend_window_keyboard_record(&event, RIGHT_CTRL_PRESSED, layout, &record));
    assert(record.Event.KeyEvent.wVirtualKeyCode == VK_CONTROL);
    assert(record.Event.KeyEvent.dwControlKeyState == (RIGHT_CTRL_PRESSED | ENHANCED_KEY));
    event.data.key.key = KVM_KEY_ALT;
    event.data.key.scan_code = 0x38;
    assert(frontend_window_keyboard_record(&event, RIGHT_ALT_PRESSED | LEFT_CTRL_PRESSED,
        layout, &record));
    assert(record.Event.KeyEvent.wVirtualKeyCode == VK_MENU);
    assert(record.Event.KeyEvent.dwControlKeyState ==
        (RIGHT_ALT_PRESSED | LEFT_CTRL_PRESSED | ENHANCED_KEY));
    event.type = KVM_EVENT_TEXT;
    event.data.text.scalar = 0x00e9;
    assert(frontend_window_keyboard_record(&event, 0, layout, &record));
    assert(record.Event.KeyEvent.wVirtualKeyCode == VK_PACKET &&
        record.Event.KeyEvent.wVirtualScanCode == 0 && record.Event.KeyEvent.uChar.UnicodeChar == 0x00e9);
    event.data.text.scalar = 0x100e9;
    assert(!frontend_window_keyboard_record(&event, 0, layout, &record));
    assert(GetLastError() == ERROR_NO_UNICODE_TRANSLATION && record.EventType == 0);
    event.type = KVM_EVENT_MOUSE;
    assert(!frontend_window_keyboard_record(&event, 0, layout, &record));
    assert(GetLastError() == ERROR_NOT_SUPPORTED);
    ZeroMemory(&event, sizeof(event));
    event.type = KVM_EVENT_KEY;
    event.source_identity = 17;
    event.data.key.modifiers = KVM_KEY_MODIFIER_CONTROL;
    event.data.key.key = KVM_KEY_CONTROL;
    event.data.key.scan_code = 0x1d;
    event.data.key.flags = KVM_KEY_FLAG_EXTENDED;
    event.data.key.pressed = 1;
    assert(frontend_window_keyboard_accept(&state, &event, RIGHT_CTRL_PRESSED | NUMLOCK_ON, layout, &record));
    assert(frontend_window_keyboard_accept(&state, &event, RIGHT_CTRL_PRESSED | NUMLOCK_ON, layout, &record));
    assert(state.held_count == 1); /* Repeats do not add phantom held keys. */
    event.data.key.key = 'A';
    event.data.key.scan_code = 0x1e;
    event.data.key.flags = 0;
    assert(frontend_window_keyboard_accept(&state, &event, RIGHT_CTRL_PRESSED | NUMLOCK_ON, layout, &record));
    assert(state.held_count == 2);
    event.source_identity = 18;
    assert(!frontend_window_keyboard_accept(&state, &event, 0, layout, &record));
    assert(GetLastError() == ERROR_INVALID_STATE);
    frontend_window_keyboard_release_begin(&state, FALSE);
    assert(frontend_window_keyboard_release_next(&state, &record));
    assert(!record.Event.KeyEvent.bKeyDown && record.Event.KeyEvent.wVirtualScanCode == 0x1e);
    assert(record.Event.KeyEvent.dwControlKeyState == (RIGHT_CTRL_PRESSED | NUMLOCK_ON));
    assert(frontend_window_keyboard_release_next(&state, &record));
    assert(record.Event.KeyEvent.dwControlKeyState == (NUMLOCK_ON | ENHANCED_KEY));
    assert(!frontend_window_keyboard_release_next(&state, &record));
    assert(state.source_identity == 17 && state.held_count == 0);
    frontend_window_keyboard_release_begin(&state, TRUE);
    assert(!frontend_window_keyboard_release_next(&state, &record));
    assert(state.source_identity == 0);
    assert(frontend_window_keyboard_accept(&state, &event, 0, layout, &record));
    event.data.key.pressed = 0;
    assert(frontend_window_keyboard_accept(&state, &event, 0, layout, &record));
    frontend_window_keyboard_release_begin(&state, TRUE);
    assert(!frontend_window_keyboard_release_next(&state, &record)); /* No duplicate break. */
    event.source_identity = 19;
    event.data.key.key = KVM_KEY_SHIFT;
    event.data.key.scan_code = 0x2a;
    event.data.key.flags = 0;
    event.data.key.pressed = 1;
    event.data.key.modifiers = KVM_KEY_MODIFIER_SHIFT;
    assert(frontend_window_keyboard_accept(&state, &event, 0, layout, &record));
    assert(record.Event.KeyEvent.dwControlKeyState == SHIFT_PRESSED); /* WM_CHAR-owned chord. */
    event.data.key.pressed = 0;
    event.data.key.modifiers = 0;
    assert(frontend_window_keyboard_accept(&state, &event, SHIFT_PRESSED, layout, &record));
    assert(record.Event.KeyEvent.dwControlKeyState == 0); /* Do not retain unrelated native state. */

    puts("PASS recovered Window physical keys, modifier sides, text rejection, source retirement and ordered release");
    native_tests(argc==2 && !strcmp(argv[1],"--cooked"));
    delivery_tests();
    returned_dos_tests(argc==2 && !strcmp(argv[1],"--cooked"));
    {
        frontend_keyboard_delivery delivery={0};frontend_window_input input={0};
        returned_input output={0};
        input.keyboard_layout=layout;input.event.source_identity=73;
        input.event.type=KVM_EVENT_TEXT;input.event.data.text.scalar=0x1f600;
        assert(!frontend_keyboard_dispatch(&delivery,&input,returned_sink,&output));
        assert(output.count==2 && output.records[0].Event.KeyEvent.wVirtualKeyCode==VK_PACKET &&
            output.records[0].Event.KeyEvent.uChar.UnicodeChar==0xd83d &&
            output.records[1].Event.KeyEvent.uChar.UnicodeChar==0xde00);
        input.event.source_identity=74;
        assert(frontend_keyboard_dispatch(&delivery,&input,returned_sink,&output)==ERROR_INVALID_STATE);
        input.event.source_identity=73;input.event.data.text.scalar=0xd800;
        assert(frontend_keyboard_dispatch(&delivery,&input,returned_sink,&output)==ERROR_NO_UNICODE_TRANSLATION);
        assert(output.count==2);
        assert(!frontend_native_keyboard_reset(&delivery.native));
        puts("PASS unified text dispatch: surrogate pair, stale source and invalid scalar rejection; worker policy remains local");
    }
    return 0;
}
