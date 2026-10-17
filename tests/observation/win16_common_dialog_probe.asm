; Independently authored NE witness; never replacement guest media.
; Contracts: wow16/inc/commdlg.h, user.def and commdlg/commdlg.def;
; Win16 template encoding: original wres16.c ConvertDialog16.
bits 16
org 0
%ifndef COMMON_DIALOG_ROUNDS
%define COMMON_DIALOG_ROUNDS 1
%endif
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
dw 2,3,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources:
dw 4,0x8005,1                 ; alignment16, RT_DIALOG, one resource
dd 0
dw (template-$$)/16,(template_end-template+15)/16,0x0030,0x8001,0,0
dw 0
resident: db 8,'CDLPROBE'
dw 0
db 0
modules: dw kernel-imports,user-imports,commdlg-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
commdlg: db 7,'COMMDLG'
entries: db 0
entries_end:
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
align 16,db 0
code:
api 1                         ; InitTask
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2                         ; InitApp
test ax,ax
jz fatal
api 3                         ; GetCurrentTask
mov [task-data],ax
push ds
push word filename-data
push word 0
api 4                         ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
call checkpoint
push cs
push word hook_proc-code
push word [instance-data]
api 5                         ; MakeProcInstance
mov [fr_hook-data],ax
mov [fr_hook-data+2],dx
or ax,dx
jz fatal
push ds
push word classname-data
push ds
push word title-data
push word 0x8000               ; hidden WS_POPUP owner, standard BUTTON
push word 0
push word 0
push word 0
push word 80
push word 24
push word 0
push word 0
push word [instance-data]
push word 0
push word 0
api 6                         ; CreateWindow
test ax,ax
jz fatal
mov [fr_owner-data],ax
mov ax,[instance-data]
mov [fr_instance-data],ax
mov [fr_find-data+2],ds
mov [fr_replace-data+2],ds
mov word [phase-data],1
call checkpoint               ; Distinguish owner success from FindText failure.
destroy_round:
call create_find
push word [dialog-data]
api 7                         ; DestroyWindow: ordinary resource cleanup
mov [destroy_ax-data],ax       ; Record guest-observed result before testing it.
test ax,ax
jz destroy_failed
push word [dialog-data]
api 8                         ; IsWindow
mov [iswindow_ax-data],ax
test ax,ax
jnz stale_window
inc word [closed-data]
call checkpoint
dec word [rounds_left-data]
jnz destroy_round
mov word [phase-data],2
call create_find
cmp word [hooks-data],COMMON_DIALOG_ROUNDS+1
jne failed
mov word [leaving-data],1
call checkpoint
push word [file-data]
api 9                         ; _lclose
; Deliberately leave the second modeless dialog alive. Original task exit
; must destroy it while original COMMDLG resource records are still valid.
mov ax,0x4c00
int 0x21
; Preserve the failing boundary in the existing report. A host thunk return
; trace alone does not prove guest AX; IsWindow may run locally in USER16.
destroy_failed:
mov word [errors-data],0x101
jmp report_failure
stale_window:
mov word [errors-data],0x102
jmp report_failure
failed:
inc word [errors-data]
report_failure:
call checkpoint
fatal:
mov ax,0x4c01
int 0x21
create_find:
push ds
push word findreplace-data
api 10                        ; COMMDLG.FindText
test ax,ax
jz fatal
mov [dialog-data],ax
inc word [created-data]
ret
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 11                        ; _lwrite
cmp ax,report_end-report
jne fatal
ret
hook_proc:
push bp
mov bp,sp
push ds
mov ds,ax                     ; MakeProcInstance supplies instance DS
cmp word [bp+12],0x110         ; WM_INITDIALOG
jne .done
inc word [hooks-data]
cmp word [bp+14],0
jne .done
inc word [errors-data]
.done:
pop ds
pop bp
xor ax,ax
xor dx,dx
retf 10
code_end:
dw 11
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,36
fixup 4,1,83
fixup 5,1,51
fixup 6,2,41
fixup 7,2,53
fixup 8,2,47
fixup 9,1,81
fixup 10,3,11
fixup 11,1,86
align 16,db 0
data:
times 32 db 0
report: db 'CDL2'
phase: dw 0
errors: dw 0
created: dw 0
closed: dw 0
hooks: dw 0
leaving: dw 0
task: dw 0
dialog: dw 0
destroy_ax: dw 0
iswindow_ax: dw 0
report_end:
instance: dw 0
file: dw 0
rounds_left: dw COMMON_DIALOG_ROUNDS
findreplace:
dd findreplace_end-findreplace
fr_owner: dw 0
fr_instance: dw 0
dd 0x00000301                 ; FR_DOWN | FR_ENABLEHOOK | FR_ENABLETEMPLATE
fr_find: dw findbuffer-data,0
fr_replace: dw replacebuffer-data,0
dw 32,32
dd 0
fr_hook: dd 0
dd 1                          ; MAKEINTRESOURCE(1)
findreplace_end:
findbuffer: db 'probe',0
times 26 db 0
replacebuffer: times 32 db 0
classname: db 'BUTTON',0
title: db 'S2 resource owner',0
filename: db 'CDLPROBE.BIN',0
data_end:
align 16,db 0
template:
dd 0x80c80080                 ; popup/caption/sysmenu/modal frame
db 3
dw 0,0,160,60
db 0,0,'S2 resource dialog',0
; Independently authored minimal find dialog, using standard control IDs.
dw 5,5,145,14,0x480           ; edt1
dd 0x50810080
db 0x81,0,0                  ; EDIT, empty text, no create data
dw 5,30,65,14,1
dd 0x50010001
db 0x80,'Find',0,0
dw 80,30,65,14,2
dd 0x50010000
db 0x80,'Cancel',0,0
template_end:
align 16,db 0
image_end:
