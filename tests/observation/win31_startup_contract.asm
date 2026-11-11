; Independent Win16 integration witness; no host input or desktop automation.
; W31BOOT.BIN proves the Win16 probe started; W31READY.BIN records a live
; Program Manager HWND and GetWinFlags. ExitWindows requests guest shutdown.
bits 16
org 0
db 'MZ'
dw (image_end-$$) % 512,((image_end-$$)+511)/512,0,4,0,0xffff,0,0x100,0,0,0,0x40,0
times 0x3c-($-$$) db 0
dd ne
ne:
db 'NE',5,1
dw entries-ne,entries_end-entries
dd 0
dw 0x0302,2,0x100,0x1000
dw 0,1,0,2
dw 2,2,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 0,0
resident: db 8,'W31CHECK'
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
entries: db 0
entries_end:
%assign calls 0
%macro api 2
%assign calls calls+1
..@call_%[calls]: db 0x9a
dw 0xffff,0
%xdefine module_%[calls] %1
%xdefine ordinal_%[calls] %2
%endmacro
align 16,db 0
code:
api 1,91                         ; InitTask
test ax,ax
jz fatal
push di
api 2,5                          ; InitApp
test ax,ax
jz fatal
api 1,132                        ; GetWinFlags
mov [flags-data],ax
mov [flags+2-data],dx
push ds
push word boot_name-data
push word 0
api 1,83                         ; _lcreat
cmp ax,-1
je fatal
push ax
api 1,81                         ; _lclose
.wait:
api 1,29                         ; Yield: permit shell to create its window
push ds
push word class_name-data
push word 0                      ; NULL window title
push word 0
api 2,50                         ; FindWindow("Progman", NULL)
test ax,ax
jz .wait
mov [hwnd-data],ax
%ifdef W31_PROMPT_CHECK
%ifdef W31_SCHEDULING_CHECK
; A closed append receipt is observable without sharing a live DOS handle.
call .heartbeat
%endif
; Launch the installed DOS Prompt profile with the fixture's /c command.
push ds
push word prompt_name-data
push word 1
api 1,166                        ; WinExec
cmp ax,32
jb fatal
%ifdef W31_SCHEDULING_CHECK
%ifdef W31_SCHEDULING_INTERACTIVE
; Use the interactive COMMAND prompt, not COMMAND /c's SingleCom path.
.schedule_focus:
api 1,29
push word 0
push word 0
push ds
push word schedule_title-data
api 2,50
test ax,ax
jz .schedule_focus
mov [schedule_window-data],ax
push ax
api 2,44
push word [schedule_window-data]
api 2,45
push word [schedule_window-data]
api 2,59
call .schedule_settle
api 2,60
cmp ax,[schedule_window-data]
jne .schedule_focus
mov si,schedule_keys-data
call .schedule_type
jmp .schedule_wait
.schedule_type:
lodsw
test ax,ax
jz .schedule_type_done
pushad
mov bx,ax
shr bx,8
and ax,0xff
push ax
push bx
xor si,si
xor di,di
api 2,289                       ; register-based Win16 USER keybd_event
pop bx
pop ax
or ah,0x80
xor si,si
xor di,di
api 2,289
popad
call .schedule_settle
jmp .schedule_type
.schedule_type_done:
ret
.schedule_settle:
pushad
push word 0
push word 0
push word 100
push word 0
push word 0
api 2,10                        ; SetTimer; pump messages between test keys
test ax,ax
jz fatal
mov [schedule_timer-data],ax
.schedule_message:
push ds
push word schedule_message-data
push word 0
push word 0
push word 0
api 2,108
test ax,ax
jz fatal
cmp word [schedule_message-data+2],0x113
jne .schedule_dispatch
mov ax,[schedule_message-data+4]
cmp ax,[schedule_timer-data]
je .schedule_timer_done
.schedule_dispatch:
push ds
push word schedule_message-data
api 2,114
jmp .schedule_message
.schedule_timer_done:
push word 0
push word [schedule_timer-data]
api 2,12
popad
ret
%endif
.schedule_wait:
api 1,29                         ; Independent Win16 task must get CPU time.
call .heartbeat
push ds
push word schedule_done-data
push word 0
api 1,85
cmp ax,-1
je .schedule_wait
push ax
api 1,81
call .heartbeat                 ; Post-child progress is mandatory too.
%ifdef W31_SCHEDULING_INTERACTIVE
mov si,schedule_exit_keys-data
call .schedule_type
%endif
jmp .schedule_complete
.heartbeat:
push ds
push word heartbeat_name-data
push word 1
api 1,85
cmp ax,-1
jne .heartbeat_open
push ds
push word heartbeat_name-data
push word 0
api 1,83
cmp ax,-1
je fatal
.heartbeat_open:
mov [heartbeat_file-data],ax
push ax
push word 0
push word 0
push word 2
api 1,84                         ; _llseek(handle, 0, SEEK_END)
push word [heartbeat_file-data]
push ds
push word heartbeat_byte-data
push word 1
api 1,86
cmp ax,1
jne fatal
push word [heartbeat_file-data]
api 1,81
ret
.schedule_complete:
%endif
%ifdef W31_PROMPT_PAIR
mov bx,first_running-data
call .wait_file
; The PIF title may change while COMMAND executes its child. Remember the
; actual DOS window while it is the only prompt, before starting prompt B.
.first_active:
api 1,29
api 2,60
test ax,ax
jz .first_active
cmp ax,[hwnd-data]               ; Program Manager
je .first_active
mov [first_window-data],ax
push ds
push word second_prompt-data
push word 1
api 1,166
cmp ax,32
jb fatal
mov bx,second_running-data
call .wait_file
.second_active:
api 1,29
api 2,60
test ax,ax
jz .second_active
cmp ax,[hwnd-data]
je .second_active
cmp ax,[first_window-data]
je .second_active
mov [second_window-data],ax
; The host requests focus only after observing the return to graphics.
; A graphics frame alone does not establish DOS keyboard focus.
mov bx,second_request-data
call .wait_file
.activate_second:
api 1,29
push word [second_window-data]
api 2,44
push word [second_window-data]
api 2,45
push word [second_window-data]
api 2,59
api 1,29
api 2,60
cmp ax,[second_window-data]
jne .activate_second
push ds
push word second_focus-data
push word 0
api 1,83
cmp ax,-1
je fatal
push ax
push ax
push ds
push word second_id-data
push word 1
api 1,86
cmp ax,1
jne fatal
api 1,81
mov bx,second_done-data
call .wait_file
.activate_first:
api 1,29
push word [first_window-data]
api 2,44                         ; OpenIcon
push word [first_window-data]
api 2,45                         ; BringWindowToTop across guest tasks
push word [first_window-data]
api 2,59                         ; SetActiveWindow
api 1,29
api 2,60                         ; GetActiveWindow
cmp ax,[first_window-data]
jne .activate_first
push ds
push word first_focus-data
push word 0
api 1,83
cmp ax,-1
je fatal
push ax
push ax
push ds
push word first_id-data
push word 1
api 1,86                         ; _lwrite
cmp ax,1
jne fatal
api 1,81
mov bx,first_done-data
%else
mov bx,receipt_name-data
%endif
call .wait_file
push ds
push word app_name-data
push word 1
api 1,166
cmp ax,32
jb fatal
.await_app:
api 1,29
push ds
push word app_class-data
push word 0
push word 0
api 2,50                         ; FindWindow
test ax,ax
jz .await_app
mov [app_window-data],ax
; Center the pointer so the relative move cannot be clipped at an edge.
push word 300
push word 220
api 2,70
push ds
push word initial_point-data
api 2,17
mov bx,mouse_ready-data
call .announce
.await_press:
api 1,29
push word 1                      ; VK_LBUTTON
api 2,249
test ax,0x8000
jz .await_press
push ds
push word current_point-data
api 2,17
mov eax,[current_point-data]
cmp eax,[initial_point-data]
je .await_press
mov bx,mouse_down-data
call .announce
.await_release:
api 1,29
push word 1
api 2,249
test ax,0x8000
jnz .await_release
push word [app_window-data]
push word 0x10                   ; WM_CLOSE
push word 0
push word 0
push word 0
api 2,110                        ; PostMessage
test ax,ax
jz fatal
.await_close:
api 1,29
push word [app_window-data]
api 2,47                         ; IsWindow
test ax,ax
jnz .await_close
jmp .finished
.wait_file:
push bx
api 1,29
pop bx
push bx
push ds
push bx
push word 0
api 1,85                         ; _lopen
pop bx
cmp ax,-1
je .wait_file
push ax
api 1,81
ret
.announce:
push ds
push bx
push word 0
api 1,83
cmp ax,-1
je fatal
push ax
api 1,81
ret
.finished:
%endif
push ds
push word ready_name-data
push word 0
api 1,83                         ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word report-data
push word report_end-report
api 1,86                         ; _lwrite
cmp ax,report_end-report
jne fatal
push word [file-data]
api 1,81                         ; _lclose
push word 0                      ; DWORD reserved
push word 0
push word 0                      ; UINT return code
api 2,7                          ; ExitWindows
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
code_end:
dw calls
%assign call_number 1
%rep calls
db 3,1
dw ..@call_%[call_number]-code+1,module_%[call_number],ordinal_%[call_number]
%assign call_number call_number+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'W31C'
flags: dd 0
hwnd: dw 0
report_end:
file: dw 0
class_name: db 'Progman',0
; Runner replaces these fixed-width fields with paths in its owned copy.
boot_name: db '@W31_BOOT_PATH@',0
times 128-($-boot_name) db 0
ready_name: db '@W31_READY_PATH@',0
times 128-($-ready_name) db 0
%ifdef W31_PROMPT_CHECK
prompt_name: db W31_INSTALL,'\DOSCHECK.PIF',0
%ifdef W31_SCHEDULING_CHECK
heartbeat_name: db W31_INSTALL,'\PATCH\SCHED.HB',0
schedule_done: db W31_INSTALL,'\PATCH\SCHED.END',0
heartbeat_file: dw 0
heartbeat_byte: db 'H'
%ifdef W31_SCHEDULING_INTERACTIVE
schedule_title: db 'Scheduling prompt',0
schedule_window: dw 0
schedule_timer: dw 0
schedule_message: times 18 db 0
schedule_keys: dw 0x1f53,0x2e43,0x2348,0x1245,0x2044,0x34be,0x2e43,0x184f,0x324d,0x1c0d,0
schedule_exit_keys: dw 0x1245,0x2d58,0x1749,0x1454,0x1c0d,0
%endif
%endif
%ifdef W31_PROMPT_PAIR
second_prompt: db W31_INSTALL,'\DOSPAIR.PIF',0
first_running: db W31_INSTALL,'\PATCH\A.RUN',0
first_done: db W31_INSTALL,'\PATCH\A.END',0
second_done: db W31_INSTALL,'\PATCH\B.END',0
second_running: db W31_INSTALL,'\PATCH\B.RUN',0
second_request: db W31_INSTALL,'\PATCH\B.REQ',0
second_focus: db W31_INSTALL,'\PATCH\B.FOC',0
second_window: dw 0
second_id: db 'B'
first_focus: db W31_INSTALL,'\PATCH\A.FOC',0
first_window: dw 0
first_id: db 'A'
%else
receipt_name: db W31_INSTALL,'\PATCH\EXEC.TXT',0
%endif
app_name: db W31_INSTALL,'\NOTEPAD.EXE',0
app_class: db 'Notepad',0
app_window: dw 0
initial_point: dd 0
current_point: dd 0
mouse_ready: db W31_INSTALL,'\PATCH\MREADY.BIN',0
mouse_down: db W31_INSTALL,'\PATCH\MDOWN.BIN',0
%endif
data_end:
image_end:
