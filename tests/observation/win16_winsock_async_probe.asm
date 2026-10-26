; Independent Win16 asynchronous Winsock completion witness.  It creates a
; task-owned hidden USER window, requests localhost through ordinal 103, and
; proves the original WSOCK32 post route reaches the corresponding Win16
; message and task handle.  This is disposable test media, never guest media.
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
resident: db 8,'WSKASYNC'
dw 0
db 0
modules: dw kernel-imports,user-imports,winsock-imports
imports: db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
winsock: db 7,'WINSOCK'
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
api 1,91                         ; KERNEL InitTask
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2,5                          ; USER InitApp
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 1,83                         ; KERNEL _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
call checkpoint
push cs
push word window_proc-code
push word [instance-data]
api 1,51                         ; KERNEL MakeProcInstance
mov [procedure-data],ax
mov [procedure-data+2],dx
or ax,dx
jz fatal
mov ax,[procedure-data]
mov [wndclass+2-data],ax
mov ax,[procedure-data+2]
mov [wndclass+4-data],ax
mov ax,[instance-data]
mov [wndclass+10-data],ax
mov ax,ds
mov [wndclass+24-data],ax
push ds
push word wndclass-data
api 2,57                         ; USER RegisterClass
test ax,ax
jz fatal
push ds
push word classname-data
push ds
push word title-data
push word 0x8000                 ; hidden WS_POPUP
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
api 2,41                         ; USER CreateWindow
test ax,ax
jz fatal
mov [window-data],ax
push word 0x0101
push ds
push word wsadata-data
api 3,115                        ; WINSOCK WSAStartup
mov [startup-data],ax
test ax,ax
jnz finish
mov word [phase-data],1
push word [window-data]
push word 0x501
push ds
push word hostname-data
push ds
push word buffer-data
push word 512
api 3,103                        ; WSAAsyncGetHostByName
test ax,ax
jz bad
mov [task-data],ax
call checkpoint
%ifdef CANCEL_REQUEST
push word [task-data]
api 3,108                        ; WINSOCK WSACancelAsyncRequest
test ax,ax
jnz bad
mov word [cancel-data],1
mov word [phase-data],2
jmp .teardown
%endif
%ifdef TASK_LOSS
mov word [phase-data],3
call checkpoint
mov ax,0x4c00
int 0x21
%endif
.pump:
push ds
push word message-data
push word 0
push word 0
push word 0
api 2,108                        ; USER GetMessage
test ax,ax
jz .completed
push ds
push word message-data
api 2,114                        ; USER DispatchMessage
jmp .pump
.completed:
cmp word [phase-data],2
jne bad
cmp word [completion-data],1
jne bad
.teardown:
api 3,116                        ; WINSOCK WSACleanup
test ax,ax
jnz bad
push word [window-data]
api 2,53                         ; USER DestroyWindow
test ax,ax
jz bad
push word [procedure-data+2]
push word [procedure-data]
api 1,52                         ; KERNEL FreeProcInstance
mov word [freed-data],1
jmp finish
bad:
inc word [errors-data]
finish:
call checkpoint
push word [file-data]
api 1,81                         ; KERNEL _lclose
mov ax,0x4c00
cmp word [errors-data],0
je .exit
mov ax,0x4c01
.exit:
int 0x21
fatal:
mov ax,0x4c01
int 0x21
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86                         ; KERNEL _lwrite
cmp ax,report_end-report
jne fatal
ret

; FAR PASCAL WNDPROC(HWND, UINT, WPARAM, LPARAM).  MakeProcInstance supplies
; AX as this task's data selector.
window_proc:
push bp
mov bp,sp
push ds
push es
push si
push di
push bx
mov ds,ax
cmp word [bp+12],0x501
jne .default
mov bx,[task-data]
cmp word [bp+10],bx
jne .bad
cmp word [bp+8],0               ; HIWORD(lParam): async error
jne .bad
inc word [completion-data]
mov word [phase-data],2
push word 0
api 2,6                          ; USER PostQuitMessage
xor ax,ax
jmp .return
.default:
push word [bp+14]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,107                        ; USER DefWindowProc
jmp .return
.bad:
inc word [errors-data]
push word 0
api 2,6
xor ax,ax
.return:
pop bx
pop di
pop si
pop es
pop ds
pop bp
retf 10
code_end:
dw calls
%macro fixup 1
db 3,1
dw ..@call_%1-code+1,module_%1,ordinal_%1
%endmacro
%assign call_number 1
%rep calls
fixup call_number
%assign call_number call_number+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'WSA1'
phase: dw 0
errors: dw 0
startup: dw 0xffff
task: dw 0
completion: dw 0
freed: dw 0
cancel: dw 0
report_end:
instance: dw 0
file: dw 0
window: dw 0
procedure: dd 0
wndclass:
dw 0
dd 0
dw 0,0,0,0,0,0
dd 0
dw classname-data,0
message: times 18 db 0
hostname: db 'localhost',0
classname: db 'T467_WINSOCK_ASYNC',0
title: db 'Winsock async witness',0
filename: db 'WSKASYNC.BIN',0
buffer: times 512 db 0
wsadata: times 512 db 0
data_end:
image_end:
