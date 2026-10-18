; Independently authored NE; never a rebuilt/replacement original guest.
; Contracts: USER user.def/windows.h, original WU32DialogBoxParam,
; dlgbegin.c WOWDlgInit/GetEditDS and dlgmgr.c ReleaseEditDS.
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
resources:
dw 4,0x8005,3
dd 0
dw (shared_template-$$)/16,(shared_end-shared_template+15)/16,0x0030,0x8001,0,0
dw (local_template-$$)/16,(local_end-local_template+15)/16,0x0030,0x8002,0,0
dw (bad_template-$$)/16,(bad_end-bad_template+15)/16,0x0030,0x8003,0,0
dw 0
resident: db 8,'DLGPROBE'
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
%macro require 1
test ax,ax
jnz %%ok
mov word [errors-data],%1
jmp failed
%%ok:
%endmacro
%macro dialog_args 1
push word [instance-data]
push word 0
push word %1
push word 0
push word [procedure-data+2]
push word [procedure-data]
push word 0x1234
push word 0x4473
%endmacro
align 16,db 0
code:
api 1,91
require 1
mov [instance-data],di
push di
api 2,5
require 2
api 1,36
mov [task-data],ax
push ds
push word filename-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push cs
push word dialog_proc-code
push word [instance-data]
api 1,51
mov [procedure-data],ax
mov [procedure-data+2],dx
or ax,dx
require 3
call checkpoint

; Two modeless resource dialogs, exercising shared/private EDIT DS.
mov word [mode-data],1
modeless:
mov ax,[mode-data]
cmp ax,1
jne .local
dialog_args 1
jmp .create
.local:
dialog_args 2
.create:
api 2,241
require 0x11
mov [dialog-data],ax
cmp word [errors-data],0
jne failed
call validate_keyboard
mov bx,[terminated-data]
mov [before_term-data],bx
push word [dialog-data]         ; validation APIs may overwrite AX
api 2,53
require 0x12
push word [dialog-data]
api 2,47
test ax,ax
jnz failed
mov ax,[terminated-data]
sub ax,[before_term-data]
cmp ax,1
jne failed
mov ax,[mode-data]
mov [phase-data],ax
call checkpoint
inc word [mode-data]
cmp word [mode-data],3
jne modeless

; Modal resource dialogs post their completion command during initialization;
; original modal queue/dispatch must deliver it before EndDialog can run.
modal:
cmp word [mode-data],3
jne .local
dialog_args 1
jmp .create
.local:
dialog_args 2
.create:
api 2,239
cmp ax,0x4473
jne failed
cmp word [errors-data],0
jne failed
inc word [modal_ok-data]
mov ax,[mode-data]
mov [phase-data],ax
call checkpoint
inc word [mode-data]
cmp word [mode-data],5
jne modal

; First EDIT allocates shared DS, then nonexistent second class rolls back.
mov ax,[initialized-data]
mov [before_init-data],ax
dialog_args 3
api 2,241
test ax,ax
jnz failed
mov ax,[initialized-data]
cmp ax,[before_init-data]
jne failed
inc word [failed_ok-data]
mov word [phase-data],5
call checkpoint

; Original SCDLG_NOREVALIDATE deliberately returns the stale nonzero HWND
; when WM_INITDIALOG destroys it (Gupta SQLWin compatibility in dlgbegin.c).
mov word [mode-data],6
dialog_args 1
api 2,241
require 0x61
mov [dialog-data],ax
push ax
api 2,47
test ax,ax
jnz failed
cmp word [killed-data],1
jne failed
cmp word [errors-data],0
jne failed
mov word [phase-data],6
call checkpoint

; Leave one shared EDIT dialog alive for normal task resource cleanup.
mov word [mode-data],7
dialog_args 1
api 2,241
require 0x71
cmp word [errors-data],0
jne failed
mov word [leaving-data],1
mov word [phase-data],7
call checkpoint
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
failed:
or word [errors-data],0x8000
call checkpoint
fatal:
mov ax,0x4c01
int 0x21
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86
cmp ax,report_end-report
jne fatal
ret

dialog_proc:
push bp
mov bp,sp
push ds
push si
push di
push bx
mov ds,ax
cmp word [bp+12],0x82
jne .init
inc word [terminated-data]
jmp .done
.init:
cmp word [bp+12],0x111
jne .initialize
cmp word [mode-data],2
ja .modal_command
cmp word [bp+10],1
je .default_command
cmp word [bp+10],2
jne .done
inc word [cancel_commands-data]
jmp .handled
.default_command:
inc word [default_commands-data]
jmp .handled
.modal_command:
cmp word [bp+10],1
jne .done
cmp word [mode-data],3
je .command
cmp word [mode-data],4
jne .done
.command:
inc word [deferred-data]
jmp .end_modal
.initialize:
cmp word [bp+12],0x110
jne .done
inc word [initialized-data]
cmp word [bp+6],0x4473
jne .bad
cmp word [bp+8],0x1234
jne .bad
mov ax,[bp+14]
mov [dialog-data],ax
call validate_dialog
cmp word [mode-data],3
je .post_modal
cmp word [mode-data],4
je .post_modal
cmp word [mode-data],6
jne .done
push word [dialog-data]
api 2,53
test ax,ax
jz .bad
inc word [killed-data]
jmp .done
.post_modal:
push word [dialog-data]
push word 0x111
push word 1
push word 0
push word 0
api 2,110
test ax,ax
jz .bad
jmp .done
.end_modal:
push word [dialog-data]
push word 0x4473
api 2,88
jmp .done
.bad:
or word [errors-data],0x4000
.done:
xor ax,ax
jmp .return
.handled:
mov ax,1
.return:
xor dx,dx
pop bx
pop di
pop si
pop ds
pop bp
retf 10

; MSG16 -> original IsDialogMessage/default-button, no physical input.
validate_keyboard:
push word [dialog-data]
push word 8                    ; SW_SHOWNA on private observer desktop
api 2,42
push word [dialog-data]
push word 1
api 2,91
test ax,ax
jz .bad
mov [default_button-data],ax
push word [dialog-data]
push word 0x400                ; DM_GETDEFID
push word 0
push word 0
push word 0
api 2,111
cmp ax,1
jne .bad
cmp dx,0x534b
jne .bad
push word [edit1-data]
api 2,22
api 2,23
cmp ax,[edit1-data]
jne .bad
mov [message-data],ax
mov word [message-data+4],9
push word [dialog-data]
push ds
push word message-data
api 2,90
test ax,ax
jz .bad
api 2,23
cmp ax,[edit2-data]
jne .bad
inc word [tab_focus-data]
push word [edit2-data]
push word 0x87                 ; original ML EDIT: DLGC_WANTALLKEYS
push word 0
push word 0
push word 0
api 2,111
and ax,4
cmp ax,4
jne .bad
mov ax,[edit2-data]
mov [message-data],ax
mov word [message-data+4],13
push word [dialog-data]
push ds
push word message-data
api 2,90
; Original manager translates/dispatches this key itself and returns TRUE,
; without invoking the default button (dlgmgr2.c's final return path).
test ax,ax
jz .bad
mov ax,[default_commands-data]
inc ax
cmp ax,[mode-data]
jne .bad
inc word [edit_keys-data]
push word [edit1-data]
api 2,22
api 2,23
cmp ax,[edit1-data]
jne .bad
mov [message-data],ax
push word [dialog-data]
push ds
push word message-data
api 2,90
test ax,ax
jz .bad
mov ax,[default_commands-data]
cmp ax,[mode-data]
jne .bad
mov word [message-data+4],27
push word [dialog-data]
push ds
push word message-data
api 2,90
test ax,ax
jz .bad
mov ax,[cancel_commands-data]
cmp ax,[mode-data]
jne .bad
ret
.bad:
or word [errors-data],0x1000
jmp failed

validate_dialog:
push word [dialog-data]
push word 100
api 2,91
test ax,ax
jz .bad
mov [edit1-data],ax
push ax
push word -6
api 2,133
test ax,ax
jz .bad
mov [ds1-data],ax
push word [dialog-data]
push word 101
api 2,91
test ax,ax
jz .bad
mov [edit2-data],ax
push ax
push word -6
api 2,133
mov [ds2-data],ax
cmp ax,[ds1-data]
jne .bad
cmp word [mode-data],2
je .local
cmp word [mode-data],4
je .local
cmp ax,[instance-data]
je .bad
inc word [shared_ok-data]
jmp .text
.local:
cmp ax,[instance-data]
jne .bad
inc word [local_ok-data]
.text:
push word [edit1-data]
push ds
push word replacement-data
api 2,37
push word [edit1-data]
push ds
push word buffer-data
push word 32
api 2,36
cmp ax,6
jne .bad
mov si,buffer-data
mov di,replacement-data
.compare:
lodsb
cmp al,[di]
jne .bad
inc di
test al,al
jnz .compare
; Exercise multiline original EDIT in the same DS before any destruction.
push word [edit2-data]
push word 0x40a
push word 0
push word 0
push word 0
api 2,111
cmp ax,2
jne .bad
inc word [text_ok-data]
ret
.bad:
or word [errors-data],0x2000
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
report: db 'DLG1'
phase: dw 0
errors: dw 0
initialized: dw 0
terminated: dw 0
text_ok: dw 0
shared_ok: dw 0
local_ok: dw 0
modal_ok: dw 0
failed_ok: dw 0
killed: dw 0
leaving: dw 0
task: dw 0
ds1: dw 0
ds2: dw 0
deferred: dw 0
tab_focus: dw 0
default_commands: dw 0
cancel_commands: dw 0
edit_keys: dw 0
report_end:
instance: dw 0
procedure: dd 0
dialog: dw 0
edit1: dw 0
edit2: dw 0
default_button: dw 0
before_init: dw 0
before_term: dw 0
mode: dw 0
file: dw -1
filename: db 'DLGPROBE.BIN',0
replacement: db 'Edited',0
buffer: times 32 db 0
message: dw 0,0x100,9
dd 1,0
dw 0,0
data_end:
%macro template_body 2
dd 0x80c80080+%1
db 3
dw 0,0,160,60
db 0,0,'Original dialog DS',0
dw 5,5,140,14,100
dd 0x50810080
db 0x81,'Single',0,0
dw 5,25,140,25,101
dd 0x50a10044
%if %2
db 'NoSuchDialogControl447',0
%else
db 0x81
%endif
db 'one',13,10,'two',0,0
dw 5,50,50,10,1
dd 0x50010001                 ; child visible tabstop default pushbutton
db 0x80,'OK',0,0
%endmacro
align 16,db 0
shared_template:
template_body 0,0
shared_end:
align 16,db 0
local_template:
template_body 0x20,0
local_end:
align 16,db 0
bad_template:
template_body 0,1
bad_end:
align 16,db 0
image_end:
