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
; Real original EDIT -> W32EditNextWord -> Win16 callback, with reentry.
push cs
push word word_break-code
push word [instance-data]
api 1,51                       ; MakeProcInstance
mov [word_proc-data],ax
mov [word_proc-data+2],dx
or ax,dx
require 0x21
send single,0x420,0,[word_proc-data+2],[word_proc-data]
send single,0x421,0,0,0
cmp dx,[word_proc-data+2]
jne failed
cmp ax,[word_proc-data]
jne failed
send single,0x203,0,1,1         ; WM_LBUTTONDBLCLK selects callback's word
send single,0x202,0,1,1         ; release original mouse capture
send single,0x400,0,0,0         ; EM_GETSEL => [0,7]
expect 0,0x22
cmp dx,7
jne failed
cmp word [word_calls-data],0
je failed
mov ax,[word_calls-data]
cmp ax,[word_nested-data]
jne failed
mov ax,[word_actions-data]
and ax,3
expect 3,0x23                  ; WB_LEFT and WB_RIGHT
send single,0x420,0,0,0
send single,0x421,0,0,0
expect 0,0x24
test dx,dx
jnz failed
push word [word_proc-data+2]
push word [word_proc-data]
api 1,52                       ; FreeProcInstance after removal
inc word [single_ok-data]
mov word [phase-data],2
call checkpoint

create edit_name,0x40a10044,multi,owner
; Exercise reverse native array construction, with heap-sized and reset forms.
send multi,0x41b,64,ds,tabstops-data ; EM_SETTABSTOPS
mov [tabs_result-data],ax
mov [tabs_result+2-data],dx
require 0x35
send multi,0x41b,0,0,0
mov [reset_result-data],ax
mov [reset_result+2-data],dx
require 0x36
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
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21

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
; Original lboxctl2.c xxxLBoxCtlScroll, not a cosmetic scrollbar check.
; More rows than fit the 140px list, independent of host font metrics.
%rep 24
send list,0x401,0,ds,alpha-data
require 0x47
%endrep
send list,0x418,10,0,0          ; LB_SETTOPINDEX
expect 0,0x48
send list,0x40f,0,0,0          ; LB_GETTOPINDEX
expect 10,0x49
send list,0x115,1,0,0          ; WM_VSCROLL / SB_LINEDOWN
send list,0x40f,0,0,0
expect 11,0x4a
send list,0x115,0,0,0          ; SB_LINEUP
send list,0x40f,0,0,0
expect 10,0x4b
send list,0x115,4,0,5          ; SB_THUMBPOSITION, low word position
send list,0x40f,0,0,0
expect 5,0x4c
send list,0x115,6,0,0          ; SB_TOP
send list,0x40f,0,0,0
expect 0,0x4d
inc word [scroll_ok-data]
; Keep the original single-selection/scrolling coverage above. A separate
; multi-selection, tabbed list checks the actual array input/output paths.
destroy list
create list_name,0x40a10089,list,owner
send list,0x413,64,ds,tabstops-data ; LB_SETTABSTOPS
require 0x4e
send list,0x413,0,0,0
require 0x4f
%rep 4
send list,0x401,0,ds,alpha-data
%endrep
send list,0x406,1,0,1             ; LB_SETSEL index1
expect 0,0x50
send list,0x406,1,0,3             ; LB_SETSEL index3
expect 0,0x51
send list,0x412,64,ds,selection-data ; LB_GETSELITEMS heap-sized capacity
expect 2,0x52
cmp word [selection-data],1
jne failed
cmp word [selection+2-data],3
jne failed
inc word [list_ok-data]
mov word [phase-data],4
call checkpoint

; All three collaboration forms, including their original internal children.
%assign form 1
%rep 3
create combo_name,0x40a10000+form,combo_%[form],owner
; Both CB_DIR pointer forms use the real original combo/list directory path.
; Invalid drive syntax has no filesystem side effects or wildcard matches.
send combo_%[form],0x405,0,ds,invalid_dir-data
expect 0xffff,0x180+form
send combo_%[form],0x405,0,0,0
expect 0xffff,0x184+form
send combo_%[form],0x406,0,0,0
expect 0,0x188+form
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
%if form > 1
; Source combo.c owns fLBoxVisible and capture, queried through real USER16.
send combo_%[form],0x417,0,0,0 ; CB_GETDROPPEDSTATE
expect 0,0xa0+form
send combo_%[form],0x40f,1,0,0 ; CB_SHOWDROPDOWN
send combo_%[form],0x417,0,0,0
expect 1,0xa4+form
send combo_%[form],0x40f,0,0,0
send combo_%[form],0x417,0,0,0
expect 0,0xa8+form
; Mouse-path focus/capture requires a visible hierarchy. SW_SHOWNOACTIVATE
; affects only this owned private-desktop probe, not the owner's desktop.
push word [owner-data]
push word 4
api 2,42
push word [combo_%[form]-data]
push ds
push word client_rect-data
api 2,33                      ; GetClientRect
; comboini.c excludes the outer edge from buttonrc (PtInRect's right edge
; is exclusive). Aim inside its metric-sized button, not width-2 on edge.
push word 2                   ; SM_CXVSCROLL
api 2,179                     ; GetSystemMetrics
shr ax,1
mov bx,[client_rect-data+4]
sub bx,ax
mov [click_x-data],bx
mov ax,[client_rect-data+6]
shr ax,1
mov [click_y-data],ax
send combo_%[form],0x201,1,[click_y-data],[click_x-data]
api 2,236                     ; GetCapture
cmp ax,[combo_%[form]-data]
je ..@captured_%[form]
mov word [errors-data],0xac+form
jmp failed
..@captured_%[form]:
send combo_%[form],0x417,0,0,0
expect 1,0xb0+form
send combo_%[form],0x202,0,[click_y-data],[click_x-data]
api 2,236
expect 0,0xb4+form
send combo_%[form],0x417,0,0,0
expect 1,0xb8+form              ; releasing the button does not dismiss list
send combo_%[form],0x201,1,[click_y-data],[click_x-data]
send combo_%[form],0x202,0,[click_y-data],[click_x-data]
send combo_%[form],0x417,0,0,0
expect 0,0xbc+form
api 2,236
expect 0,0xc0+form
inc word [drop_ok-data]
%endif
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
; Win16 EDITWORDBREAKPROC: far text, int start, int count, int action.
word_break:
push bp
mov bp,sp
push ds
push es
push si
push di
push bx
mov ds,ax
inc word [word_calls-data]
cmp word [bp+8],7
jne .bad
cmp word [bp+10],7
ja .bad
les di,[bp+12]
mov si,new_beta-data
mov cx,7
.text:
mov al,[es:di]
cmp al,[si]
jne .bad
inc si
inc di
loop .text
send button,0x400,0,0,0         ; nested USER16 query inside original EDIT
cmp ax,1
jne .bad
inc word [word_nested-data]
mov ax,[bp+6]
cmp ax,0
je .left
cmp ax,1
je .right
cmp ax,2
jne .bad
or word [word_actions-data],4
xor ax,ax
jmp .return
.left:
or word [word_actions-data],1
xor ax,ax
jmp .return
.right:
or word [word_actions-data],2
mov ax,7
jmp .return
.bad:
or word [errors-data],0x2000
xor ax,ax
.return:
xor dx,dx
pop bx
pop di
pop si
pop es
pop ds
pop bp
retf 10
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
word_calls: dw 0
word_nested: dw 0
word_actions: dw 0
scroll_ok: dw 0
drop_ok: dw 0
tabs_result: dd 0
reset_result: dd 0
report_end:
client_rect: times 4 dw 0
click_x: dw 0
click_y: dw 0
instance: dw 0
saved_proc: dd 0
word_proc: dd 0
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
invalid_dir: db '?:\T447MAP.INVALID',0
filename: db 'TABTEST.BIN',0
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
tabstops:
%assign tab 1
%rep 64
dw tab*4
%assign tab tab+1
%endrep
selection: times 64 dw 0
data_end:
align 16,db 0
image_end:
