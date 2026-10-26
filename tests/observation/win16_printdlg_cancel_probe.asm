; Independently authored cancellation witness.  The original PrintDlg hook
; receives WM_INITDIALOG and asks original USER.EndDialog to cancel.
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
dw 2,3,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 0,0
resident: db 8,'PDCANCEL'
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
api 1                         ; KERNEL.InitTask
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2                         ; USER.InitApp
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 3                         ; KERNEL._lcreat
cmp ax,-1
je fatal
mov [file-data],ax
push cs
push word hook_proc-code
push word [instance-data]
api 4                         ; KERNEL.MakeProcInstance
mov [hook-data],ax
mov [hook-data+2],dx
or ax,dx
jz fatal
mov [printdlg-data+12],word 0x1000 ; PD_ENABLEPRINTHOOK
mov [printdlg-data+14],word 0
mov ax,[hook-data]
mov [printdlg-data+32],ax
mov ax,[hook-data+2]
mov [printdlg-data+34],ax
push ds
push word printdlg-data
api 5                         ; COMMDLG.PrintDlg @20
mov [result-data],ax
api 6                         ; COMMDLG.CommDlgExtendedError @26
mov [error-data],ax
mov [error-data+2],dx
push word [hook-data+2]
push word [hook-data]
api 7                         ; KERNEL.FreeProcInstance
mov word [freed-data],1
call checkpoint
push word [file-data]
api 9                         ; KERNEL._lclose
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
hook_proc:
push bp
mov bp,sp
push ds
mov ds,ax                     ; MakeProcInstance restores the task DS
cmp word [bp+12],0x0110       ; WM_INITDIALOG in PARMWP callback shape
jne .done
inc word [hooks-data]
push word [bp+14]             ; callback HWND16
push word 0
api 8                          ; USER.EndDialog(hwnd, FALSE)
.done:
pop ds
pop bp
xor ax,ax
xor dx,dx
retf 10
checkpoint:
push word [file-data]
push ds
push word record-data
push word record_end-record
api 10                         ; KERNEL._lwrite
cmp ax,record_end-record
jne fatal
ret
code_end:
dw 10
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,51
fixup 5,3,20
fixup 6,3,26
fixup 7,1,52
fixup 8,2,88
fixup 9,1,81
fixup 10,1,86
align 16,db 0
data:
times 32 db 0
record: db 'PDC1'
result: dw 0
error: dd 0
hooks: dw 0
freed: dw 0
record_end:
instance: dw 0
file: dw 0
hook: dd 0
printdlg:
dd 52
dw 0,0,0,0
dd 0
dw 0,0,0,0,0,0
dd 0,0,0,0,0
dw 0,0
filename: db 'PDCANCEL.BIN',0
data_end:
image_end:
