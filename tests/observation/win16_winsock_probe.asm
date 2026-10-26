; Independent NE callback witness. Loopback-only UDP wait, no external peer.
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
resident: db 8,'WSKPROBE'
dw 0
db 0
modules: dw kernel-imports,user-imports,winsock-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
winsock: db 7,'WINSOCK'
entries: db 0
entries_end:
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
align 16,db 0
code:
api 1
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 3
cmp ax,-1
je fatal
mov [file-data],ax
api 4
mov [task-data],ax
call checkpoint
push word 0x0101
push ds
push word wsadata-data
api 5
mov [startup-data],ax
test ax,ax
jnz finish
push cs
push word blocking_hook-code
push word [instance-data]
api 6
mov [proc-data],ax
mov [proc-data+2],dx
or ax,dx
jz fatal
push word [proc-data+2]
push word [proc-data]
api 7
mov [previous-data],ax
mov [previous-data+2],dx
cmp ax,0xffff
jne bad
cmp dx,0xffff
jne bad
push word 2
push word 2
push word 17
api 8
cmp ax,0xffff
je fatal
mov [socket-data],ax
mov [fds-data+2],ax
push ax
push ds
push word address-data
push word 16
api 9
test ax,ax
jnz fatal
push word 0
push ds
push word fds-data
push word 0
push word 0
push word 0
push word 0
push ds
push word timeout-data
api 10
mov [selected-data],ax
test ax,ax
jnz bad
cmp word [calls-data],0
je bad
mov word [phase-data],1
call checkpoint
push word [socket-data]
push ds
push word buffer-data
push word 1
push word 0
api 11
cmp ax,0xffff
jne bad
api 12
mov [cancel_error-data],ax
cmp ax,10004                  ; WSAEINTR
jne bad
api 13
test ax,ax
jnz bad
api 14
test ax,ax
jne bad
mov word [restored-data],1
push word [socket-data]
api 15
test ax,ax
jnz bad
mov word [closed-data],1
push word [proc-data+2]
push word [proc-data]
api 16
mov word [freed-data],1
api 17
test ax,ax
jnz bad
mov word [cleaned-data],1
mov word [phase-data],2
jmp finish
bad:
inc word [errors-data]
finish:
call checkpoint
push word [file-data]
api 18
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
api 19
cmp ax,report_end-report
jne fatal
ret
; FAR PASCAL BOOL callback(void), original MakeProcInstance supplies AX=DS.
blocking_hook:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
push es
inc word [calls-data]
api 20
cmp ax,[task-data]
jne .bad
api 21
test ax,ax
jnz .phase
.bad:
inc word [errors-data]
.phase:
cmp word [phase-data],1
jne .return
api 22
test ax,ax
jnz .bad_cancel
inc word [cancelled-data]
jmp .return
.bad_cancel:
inc word [errors-data]
.return:
pop es
pop di
pop si
pop bx
pop ds
pop bp
xor ax,ax
retf
code_end:
dw 22
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,36
fixup 5,3,115
fixup 6,1,51
fixup 7,3,109
fixup 8,3,23
fixup 9,3,2
fixup 10,3,18
fixup 11,3,16
fixup 12,3,111
fixup 13,3,114
fixup 14,3,110
fixup 15,3,3
fixup 16,1,52
fixup 17,3,116
fixup 18,1,81
fixup 19,1,86
fixup 20,1,36
fixup 21,3,114
fixup 22,3,113
align 16,db 0
data:
times 32 db 0
report: db 'WSK1'
phase: dw 0
errors: dw 0
startup: dw 0xffff
calls: dw 0
cancelled: dw 0
cancel_error: dw 0
selected: dw 0xffff
restored: dw 0
closed: dw 0
freed: dw 0
cleaned: dw 0
task: dw 0
report_end:
instance: dw 0
file: dw 0
proc: dd 0
previous: dd 0
socket: dw 0xffff
fds: dw 1
times 64 dw 0
address: dw 2,0
db 127,0,0,1
times 8 db 0
timeout: dd 0,500000
buffer: db 0
wsadata: times 512 db 0
filename: db 'WSKPROBE.BIN',0
data_end:
image_end:
