; Independent disposable NE acceptance probe, never replacement guest media.
; Original KERNEL/USER ordinals and MakeProcInstance ABI are audited in
; m0-t447-s2-message-boundary-audit.md. No host callback substitutes.
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
resident: db 7,'CBTPROB'
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
entries: db 0
entries_end:

; Each imported far call is its own NE relocation-chain terminator.
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
align 16,db 0
code:
api 1                        ; InitTask
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2                        ; InitApp
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 3                        ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
call checkpoint
push cs
push word hook_proc-code
push word [instance-data]
api 4                        ; MakeProcInstance
mov [proc-data],ax
mov [proc-data+2],dx
or ax,dx
jz failed
api 5                        ; GetCurrentTask, never a global hook
push word 5                  ; WH_CBT
push word [proc-data+2]
push word [proc-data]
push word [instance-data]
push ax
api 6                        ; SetWindowsHookEx, HHOOK16 is DWORD
mov [hook-data],ax
mov [hook-data+2],dx
or ax,dx
jnz installed
inc word [errors-data]
jmp free_proc
installed:
mov word [phase-data],1
call create_button
test ax,ax
jz bad_first
call destroy_button
jmp first_done
bad_first: inc word [errors-data]
first_done:
call checkpoint
mov word [phase-data],2
call create_button
test ax,ax
jz veto_done
inc word [errors-data]
call destroy_button
veto_done:
call checkpoint
mov word [phase-data],3
call create_button
test ax,ax
jz bad_last
call destroy_button
jmp last_done
bad_last: inc word [errors-data]
last_done:
push word [hook-data+2]
push word [hook-data]
api 7                        ; UnhookWindowsHookEx
mov [unhooked-data],ax
test ax,ax
jnz free_proc
inc word [errors-data]
free_proc:
push word [proc-data+2]
push word [proc-data]
api 8                        ; FreeProcInstance
mov word [freed-data],1
mov word [phase-data],4
jmp finish
failed:
inc word [errors-data]
finish:
call checkpoint
push word [file-data]
push ds
push word history-data
push word 512
api 15                       ; retain all observed class names for attribution
push word [file-data]
api 9                        ; _lclose
cmp word [errors-data],0
jne fatal
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
api 10                       ; _lwrite, append a complete phase record
cmp ax,report_end-report
je .done
inc word [errors-data]
.done: ret

create_button:
push ds
push word classname-data
push ds
push word title-data
push word 0x8000             ; WS_POPUP, hidden
push word 0
push word 0
push word 0
push word 80
push word 24
push word 0                  ; parent
push word 0                  ; menu
push word [instance-data]
push word 0
push word 0                  ; creation parameter
api 11                       ; CreateWindow
ret
destroy_button:
push ax
api 12                       ; DestroyWindow
test ax,ax
jz .bad
inc word [destroyed-data]
ret
.bad: inc word [errors-data]
ret

; FAR PASCAL HookProc(code, wParam, lParam). MakeProcInstance supplies DS in AX.
hook_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
push es
cmp word [bp+12],3           ; HCBT_CREATEWND
jne .chain
push word [bp+10]            ; new HWND, query before any WNDPROC
push ds
push word query-data
push word 32
api 13                       ; GetClassName
push ax
mov bx,[seen-data]
cmp bx,16
jae .recorded
shl bx,5
mov di,history-data
add di,bx
mov si,query-data
push ds
pop es
mov cx,16
cld
rep movsw
inc word [seen-data]
.recorded:
pop ax
cmp ax,6
jne .chain
mov si,query-data
mov di,expected-data
mov cx,6
.compare:
lodsb
or al,0x20
cmp al,[di]
jne .chain
inc di
loop .compare
inc word [created-data]
inc word [queried-data]
.queried:
cmp word [phase-data],2
jne .nested
inc word [vetoed-data]
mov ax,1
xor dx,dx
jmp .return
.nested:
cmp word [phase-data],1
jne .chain
cmp word [depth-data],0
jne .chain
inc word [depth-data]
call create_button
test ax,ax
jz .bad_nested
inc word [nested-data]
call destroy_button
jmp .nested_done
.bad_nested: inc word [errors-data]
.nested_done: dec word [depth-data]
.chain:
push word [hook-data+2]
push word [hook-data]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 14                       ; CallNextHookEx
.return:
pop es
pop di
pop si
pop bx
pop ds
pop bp
retf 8
code_end:
dw 15
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,51
fixup 5,1,36
fixup 6,2,291
fixup 7,2,292
fixup 8,1,52
fixup 9,1,81
fixup 10,1,86
fixup 11,2,41
fixup 12,2,53
fixup 13,2,58
fixup 14,2,293
fixup 15,1,86
align 16,db 0
data:
times 32 db 0
report: db 'CBT1'
phase: dw 0
errors: dw 0
created: dw 0
queried: dw 0
nested: dw 0
vetoed: dw 0
destroyed: dw 0
unhooked: dw 0
freed: dw 0
report_end:
instance: dw 0
file: dw 0
proc: dd 0
hook: dd 0
depth: dw 0
seen: dw 0
query: times 32 db 0
history: times 512 db 0
classname: db 'BUTTON',0
expected: db 'button',0
title: db 'S2 guest CBT',0
filename: db 'CBTPROB.BIN',0
data_end:
image_end:
