; Independently authored NE acceptance witness; not replacement guest media.
; ABI/messages/ordinals: original wow16/inc/windows.h and user/user.def.
; Tests the selected original Button, EDIT, ListBox and ComboBox consumers
; through real USER16 thunks, plus native MENU resources and ordinary exit.
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
resources: dw 4,0
resident: db 8,'CTLPROBE'
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
%macro expect 2
cmp ax,%1
je %%ok
mov word [errors-data],%2
jmp failed
%%ok:
%endmacro
%macro require 1
test ax,ax
jnz %%ok
mov word [errors-data],%1
jmp failed
%%ok:
%endmacro
%macro create 4
push ds
push word %1-data
push ds
push word empty-data
push word ((%2)>>16)&0xffff
push word (%2)&0xffff
push word 0
push word 0
push word 160
push word 140
push word [%4-data]
push word 0
push word [instance-data]
push word 0
push word 0
api 2,41
require 0x100+%3-data
mov [%3-data],ax
inc word [created-data]
%endmacro
%macro send 5
push word [%1-data]
push word %2
push word %3
push word %4
push word %5
api 2,111
%endmacro
%macro settext 2
push word [%1-data]
push ds
push word %2-data
api 2,37
; Win16 SetWindowText returns VOID. Read back rather than asserting AX.
%endmacro
%macro gettext 2
push word [%1-data]
push ds
push word buffer-data
push word 32
api 2,36
mov si,buffer-data
mov di,%2-data
call equal_text
%endmacro
%macro destroy 1
push word [%1-data]
api 2,53
require 0x200+%1-data
push word [%1-data]
api 2,47
expect 0,0x300+%1-data
mov word [%1-data],0
inc word [destroyed-data]
%endmacro
align 16,db 0
code:
api 1,91                       ; InitTask, genuine USER16 task/DS
require 1
mov [instance-data],di
push di
api 2,5                        ; InitApp
require 2
api 1,36                       ; GetCurrentTask
mov [task-data],ax
push ds
push word filename-data
push word 0
api 1,83                       ; _lcreat
cmp ax,-1
jne .file_ready
mov ax,0x4c01
int 0x21
.file_ready:
mov [file-data],ax
call checkpoint
create button_name,0x80000000,owner,null_window
create button_name,0x40010002,button,owner
send button,0x401,1,0,0         ; BM_SETCHECK
send button,0x400,0,0,0         ; BM_GETCHECK
expect 1,0x11
; GetWindowLong exposes the saved original control target as a Win16 thunk.
; CallWindowProc must handle its original HWND32 short carrier, too.
push word [button-data]
push word -4
api 2,135
mov [saved_proc-data],ax
mov [saved_proc-data+2],dx
or ax,dx
require 0x12
push word [saved_proc-data+2]
push word [saved_proc-data]
push word [button-data]
push word 0x400
push word 0
push word 0
push word 0
api 2,122
expect 1,0x13
inc word [button_ok-data]
mov word [phase-data],1
call checkpoint

create edit_name,0x40810080,single,owner
settext single,alpha_beta
send single,0x401,0,5,0         ; EM_SETSEL, MAKELONG(start,end)
send single,0x412,0,ds,new_text-data ; EM_REPLACESEL
gettext single,new_beta
inc word [single_ok-data]
mov word [phase-data],2
call checkpoint

create edit_name,0x40a10044,multi,owner
settext multi,two_lines
send multi,0x40a,0,0,0          ; EM_GETLINECOUNT
expect 2,0x31
send multi,0x40b,1,0,0          ; EM_LINEINDEX, second line after CRLF
expect 5,0x32
send multi,0x401,0,3,0
send multi,0x412,0,ds,three-data
send multi,0x40a,0,0,0
expect 2,0x33
send multi,0x40b,1,0,0
expect 7,0x34
gettext multi,three_lines
inc word [multi_ok-data]
mov word [phase-data],3
call checkpoint

create list_name,0x40a10001,list,owner
send list,0x401,0,ds,alpha-data ; LB_ADDSTRING
expect 0,0x41
send list,0x401,0,ds,beta-data
expect 1,0x42
send list,0x40c,0,0,0          ; LB_GETCOUNT
expect 2,0x43
send list,0x407,1,0,0          ; LB_SETCURSEL
expect 1,0x44
send list,0x409,0,0,0          ; LB_GETCURSEL
expect 1,0x45
send list,0x40a,1,ds,buffer-data ; LB_GETTEXT
expect 4,0x46
mov si,buffer-data
mov di,beta-data
call equal_text
inc word [list_ok-data]
mov word [phase-data],4
call checkpoint

; All three collaboration forms, including their original internal children.
%assign form 1
%rep 3
create combo_name,0x40a10000+form,combo_%[form],owner
send combo_%[form],0x403,0,ds,alpha-data
expect 0,0x50+form
send combo_%[form],0x403,0,ds,beta-data
expect 1,0x54+form
send combo_%[form],0x406,0,0,0
expect 2,0x58+form
send combo_%[form],0x40e,1,0,0
expect 1,0x5c+form
send combo_%[form],0x407,0,0,0
expect 1,0x60+form
send combo_%[form],0x408,1,ds,buffer-data
expect 4,0x64+form
mov si,buffer-data
mov di,beta-data
call equal_text
inc word [combos_ok-data]
%assign form form+1
%endrep
mov word [phase-data],5
call checkpoint

api 2,151                      ; CreateMenu
require 0x71
mov [menu-data],ax
api 2,415                      ; CreatePopupMenu
require 0x72
mov [popup-data],ax
push ax
push word 0
push word 71
push ds
push word alpha-data
api 2,411                      ; AppendMenu, string
require 0x73
push word [menu-data]
push word 0x10                 ; MF_POPUP
push word [popup-data]
push ds
push word beta-data
api 2,411
require 0x74
push word [owner-data]
push word [menu-data]
api 2,158                      ; SetMenu
require 0x75
push word [menu-data]
api 2,263                      ; GetMenuItemCount
expect 1,0x76
push word [popup-data]
push word 71
push word 8                    ; MF_CHECKED, by command
api 2,154
expect 0,0x77
push word [popup-data]
push word 71
push word 0
api 2,250                      ; GetMenuState
and ax,8
expect 8,0x78
push word [owner-data]
push word 0
api 2,158                      ; Detach menu before independent retirement.
require 0x79
push word [menu-data]
push word 0
push word 0x400                ; RemoveMenu(MF_BYPOSITION), preserves child
api 2,412
require 0x7a
push word [menu-data]
api 2,263
expect 0,0x7b
push word [popup-data]
api 2,263
expect 1,0x7c
push word [menu-data]
api 2,152
require 0x7d
push word [popup-data]
api 2,152
require 0x7e
inc word [menus_ok-data]
mov word [phase-data],6
call checkpoint

destroy combo_3
destroy combo_2
destroy combo_1
destroy list
destroy multi
destroy single
destroy button
destroy owner
mov word [phase-data],7
call checkpoint
; Leave a fresh composite and its internal list/edit alive for original
; task-exit finalization; observer cleanup is NOT the success criterion.
create button_name,0x80000000,owner,null_window
create combo_name,0x40a10002,combo_1,owner
send combo_1,0x403,0,ds,alpha-data
expect 0,0x81
mov word [leaving-data],2
mov word [phase-data],8
call checkpoint
push word [file-data]
api 1,81                       ; _lclose
mov ax,0x4c00
int 0x21
failed:
call checkpoint
push word [file-data]
api 1,81
mov ax,0x4c01
int 0x21
equal_text:
lodsb
cmp al,[di]
jne .bad
inc di
test al,al
jnz equal_text
ret
.bad:
mov word [errors-data],0x90
jmp failed
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86                       ; _lwrite
cmp ax,report_end-report
je .ok
mov ax,0x4c02
int 0x21
.ok:
ret
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
report: db 'CTL1'
phase: dw 0
errors: dw 0
created: dw 0
button_ok: dw 0
single_ok: dw 0
multi_ok: dw 0
list_ok: dw 0
combos_ok: dw 0
menus_ok: dw 0
destroyed: dw 0
leaving: dw 0
task: dw 0
report_end:
instance: dw 0
saved_proc: dd 0
file: dw -1
null_window: dw 0
owner: dw 0
button: dw 0
single: dw 0
multi: dw 0
list: dw 0
combo_1: dw 0
combo_2: dw 0
combo_3: dw 0
menu: dw 0
popup: dw 0
button_name: db 'BUTTON',0
edit_name: db 'EDIT',0
list_name: db 'LISTBOX',0
combo_name: db 'COMBOBOX',0
filename: db 'CTLPROBE.BIN',0
empty: db 0
alpha: db 'Alpha',0
beta: db 'Beta',0
alpha_beta: db 'AlphaBeta',0
new_text: db 'New',0
new_beta: db 'NewBeta',0
two_lines: db 'one',13,10,'two',0
three: db 'three',0
three_lines: db 'three',13,10,'two',0
buffer: times 32 db 0
data_end:
align 16,db 0
image_end:
