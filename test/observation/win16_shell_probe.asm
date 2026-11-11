; Independent NE witness. SHELL ordinal20 is the original ShellExecute.
; CHILD builds a disposable sibling, never replacement guest media.
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
resident: db 8,'SHLPROBE'
dw 0
db 0
modules: dw kernel-imports,user-imports,shell-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
shell: db 5,'SHELL'
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
push di
api 2                         ; InitApp
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 3                         ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
%ifndef CHILD
call checkpoint
mov ax,target-data
call execute
mov [success-data],ax
%ifndef IGNORE_EXCEPTION
cmp ax,32
ja .success
inc word [errors-data]
.success:
%endif
mov word [phase-data],1
call checkpoint
mov ax,missing-data
call execute
mov [failure-data],ax
cmp ax,32
jbe .failed_expected
inc word [errors-data]
.failed_expected:
%ifndef CALLBACK_ABORT
; The resolver-abort witness must not assume its interrupted first request
; successfully created a child. The following missing-target API still runs.
mov word [remaining-data],1000
.wait_child:
api 7                         ; Yield, let the actual child finish
push ds
push word child_report-data
push word 0
api 8                         ; _lopen
cmp ax,-1
jne .child_seen
dec word [remaining-data]
jnz .wait_child
inc word [errors-data]
jmp .child_done
.child_seen:
push ax
api 9                         ; _lclose
.child_done:
%endif
mov word [phase-data],2
%else
mov word [phase-data],3
%endif
call checkpoint
push word [file-data]
api 4                         ; _lclose
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 5                         ; _lwrite
cmp ax,report_end-report
jne fatal
ret
%ifndef CHILD
execute:
push bp
mov bp,sp
push word 0                   ; HWND
push ds
push word operation-data
push ds
push ax                       ; target
push ds
push word empty-data          ; parameters
push ds
push word directory-data
push word 0                   ; SW_HIDE
api 6
cmp sp,bp
jne fatal
pop bp
ret
%endif
code_end:
%ifdef CHILD
dw 5
%else
%ifdef CALLBACK_ABORT
dw 6
%else
dw 9
%endif
%endif
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,81
fixup 5,1,86
%ifndef CHILD
fixup 6,3,20
%ifndef CALLBACK_ABORT
fixup 7,1,29
fixup 8,1,85
fixup 9,1,81
%endif
%endif
align 16,db 0
data:
times 32 db 0
report: db 'SHL1'
phase: dw 0
errors: dw 0
success: dw 0
failure: dw 0xffff
report_end:
file: dw 0
remaining: dw 0
child_report: db 'SHLCHILD.BIN',0
%ifdef CHILD
filename: db 'SHLCHILD.BIN',0
%else
filename: db 'SHLPROBE.BIN',0
%endif
target: db 'SHLCHILD.EXE',0
missing: db 'NO447SHL.EXE',0
operation: db 'open',0
directory: db '.',0
empty: db 0
data_end:
image_end:
