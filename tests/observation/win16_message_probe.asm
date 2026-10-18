; Independent NE witness, never a replacement for original guest media.
; Imports/ABI: selected wow16 USER/KERNEL .def and inc/windows.h.
; Two actual USER16 tasks, synchronous/nested/reply/posted messages and
; string copyback. Native peer is a separately authored test DLL.
bits 16
org 0
db 'MZ'
dw (image_end-$$) % 512,((image_end-$$)+511)/512
dw 0,4,0,0xffff                 ; relocations, header paras, min/max allocation
dw 0,0x100                      ; initial SS:SP
dw 0,0,0,0x40,0                 ; checksum, IP, CS, reloc-table offset, overlay
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
resident:
%ifdef RECEIVER
db 7,'MSGRECV'
%else
db 7,'MSGSEND'
%endif
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
%macro result 1
cmp ax,%1
jne %%bad
cmp dx,0x447a
je %%ok
%%bad:
mov word [errors-data],%1+0x100
jmp failed
%%ok:
%endmacro
%macro send 5
push word [%1-data]
push word %2
push word %3
push word %4
push word %5
api 2,111
%endmacro
%macro post 5
push word [%1-data]
push word %2
push word %3
push word %4
push word %5
api 2,110
%endmacro
%macro peek 3
push ds
push word message-data
push word 0
push word %1
push word %2
push word %3
api 2,109
%endmacro
%macro push32 1
push word [%1+2-data]
push word [%1-data]
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
push word window_proc-code
push word [instance-data]
api 1,51
mov [procedure-data],ax
mov [procedure+2-data],dx
or ax,dx
require 3
mov ax,[procedure-data]
mov [wndclass+2-data],ax
mov ax,[procedure+2-data]
mov [wndclass+4-data],ax
mov ax,[instance-data]
mov [wndclass+10-data],ax
mov [wndclass+24-data],ds
push ds
push word wndclass-data
api 2,57
require 4
push ds
push word classname-data
push ds
push word caption-data
push word 0x8000
push word 0
push word 0
push word 0
push word 100
push word 60
push word 0
push word 0
push word [instance-data]
push word 0
push word 0
api 2,41
require 5
mov [window-data],ax
push ax
api 2,224
cmp ax,[task-data]
jne failed
call checkpoint
%ifdef RECEIVER
.pump:
push ds
push word message-data
push word 0
push word 0
push word 0
api 2,108
test ax,ax
jz .completed
push ds
push word message-data
api 2,114
jmp .pump
.completed:
cmp word [basic-data],1
jne failed
cmp word [nested-data],1
jne failed
cmp word [early-data],1
jne failed
cmp word [posted-data],1
jne failed
mov word [phase-data],5
call checkpoint
%else
push ds
push word childname-data
push word 0
api 1,166
cmp ax,32
jbe failed
api 2,13
mov [started-data],ax
.find:
push ds
push word peerclass-data
push word 0
push word 0
api 2,50
test ax,ax
jnz .found
call deadline
api 1,29
jmp .find
.found:
mov [peer-data],ax
push ax
api 2,224
mov [peer_task-data],ax
cmp ax,[task-data]
je failed
send peer,0x460,0x1234,0x5678,0x9abc
result 1
send peer,0x461,[window-data],0x2233,0x4455
result 2
send peer,0x464,0,0,0
result 4
post peer,0x465,[window-data],0,0
require 6
api 2,13
mov [started-data],ax
.ack:
peek 0x466,0x466,1
test ax,ax
jnz .dispatch_ack
call deadline
api 1,29
jmp .ack
.dispatch_ack:
push ds
push word message-data
api 2,114
cmp word [posted-data],1
jne failed
mov word [phase-data],1
call checkpoint
; Actual original input conversion and receiver-to-sender output copyback.
send peer,0x000c,0,ds,guest_text-data
require 7
send peer,0x000d,32,ds,buffer-data
cmp ax,10
jne failed
mov si,buffer-data
mov di,guest_text-data
call equal_text
inc word [text-data]
; Original peek filtering, repeatable no-remove and exactly one removal.
post window,0x467,0x1701,0,0
require 8
post window,0x468,0x1801,0,0
require 9
peek 0x468,0x468,0
require 10
cmp word [message+4-data],0x1801
jne failed
peek 0x468,0x468,0
require 11
peek 0x468,0x468,1
require 12
peek 0x468,0x468,0
test ax,ax
jnz failed
peek 0x467,0x467,1
require 13
cmp word [message+4-data],0x1701
jne failed
inc word [filtered-data]
push word [task-data]
push word 0x469
push word 0x1901
push word 0
push word 0
api 2,116
require 14
peek 0x469,0x469,1
require 15
cmp word [message-data],0
jne failed
cmp word [message+4-data],0x1901
jne failed
inc word [threadpost-data]
mov word [phase-data],2
call checkpoint
; Load only the test native peer through original generic thunks.
push ds
push word library-data
push word 0
push word 0
push word 0
push word 0
api 1,513
mov [module-data],ax
mov [module+2-data],dx
or ax,dx
require 16
push32 module
push ds
push word export-data
api 1,515
mov [native-data],ax
mov [native+2-data],dx
or ax,dx
require 17
xor ax,ax
call native_operation
result 1
mov ax,1
call native_operation
result 2
push ds
push word nativeclass-data
push word 0
push word 0
api 2,50
require 18
mov [native_window-data],ax
send native_window,0x470,0x1234,0x5678,0x9abc
result 0x10
send native_window,0x471,0,0,0
result 0x11
send native_window,0x472,0,0,0
result 0x12
send native_window,0x473,0,0,0
result 0x13
mov word [native_count-data],4
cmp word [nested-data],2
jne failed
cmp word [same-data],2
jne failed
cmp word [early-data],1
jne failed
mov ax,2
call native_operation
result 3
push32 module
api 1,514
require 19
mov word [phase-data],3
call checkpoint
send peer,0x46f,0,0,0
result 0xf
api 2,13
mov [started-data],ax
.wait_peer_exit:
push word [peer-data]
api 2,47
test ax,ax
jz .peer_gone
call deadline
api 1,29
jmp .wait_peer_exit
.peer_gone:
send peer,0x460,0,0,0
or ax,dx
jnz failed
inc word [destroyed-data]
; WM_QUIT is not constrained by the selected message filter.
push word 7
api 2,6
push ds
push word message-data
push word 0
push word 0x467
push word 0x467
api 2,108
test ax,ax
jnz failed
cmp word [message+2-data],0x12
jne failed
cmp word [message+4-data],7
jne failed
mov word [phase-data],4
call checkpoint
%endif
push word [window-data]
api 2,53
require 20
push32 procedure
api 1,52
push ds
push word classname-data
push word [instance-data]
api 2,403
require 21
mov word [phase-data],6
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
deadline:
api 2,13
sub ax,[started-data]
cmp ax,10000
ja failed
ret
equal_text:
lodsb
cmp al,[di]
jne failed
inc di
test al,al
jnz equal_text
ret
native_operation:
push bp
mov bp,sp
push word 0
push ax
push32 native
push word 0
push word 0
push word 0
push word 1
api 1,517
cmp sp,bp
jne fatal
pop bp
ret
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86
cmp ax,report_end-report
jne fatal
ret
window_proc:
push bp
mov bp,sp
push ds
push es
push si
push di
push bx
mov ds,ax
cmp word [bp+12],0x460
je .basic
cmp word [bp+12],0x461
je .nested_send
cmp word [bp+12],0x462
je .nested_receive
cmp word [bp+12],0x463
je .same
cmp word [bp+12],0x464
je .early
cmp word [bp+12],0x465
je .posted
cmp word [bp+12],0x466
je .ack
cmp word [bp+12],0x46f
je .quit
push word [bp+14]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,107
jmp .return
.basic:
cmp word [bp+10],0x1234
jne .bad
cmp word [bp+8],0x5678
jne .bad
cmp word [bp+6],0x9abc
jne .bad
inc word [basic-data]
mov ax,1
jmp .value
.nested_send:
api 2,192
test ax,ax
jz .bad
mov ax,[bp+10]
mov [sender-data],ax
send sender,0x462,0,0x2233,0x4455
cmp ax,2
jne .bad
cmp dx,0x447a
jne .bad
inc word [nested-data]
mov ax,2
jmp .value
.nested_receive:
api 2,192
test ax,ax
jz .bad
cmp word [bp+8],0x2233
jne .bad
cmp word [bp+6],0x4455
jne .bad
inc word [nested-data]
send window,0x463,0,0,0
cmp ax,3
jne .bad
mov ax,2
jmp .value
.same:
; A same-thread nested call inherits the actual outer received send state.
api 2,192
test ax,ax
jz .bad
inc word [same-data]
mov ax,3
jmp .value
.early:
api 2,192
test ax,ax
jz .bad
inc word [early-data]
push word 0x447a
push word 4
api 2,115
push word 0x447a
push word 0xff
api 2,115
api 1,29
mov ax,8
jmp .value
.posted:
api 2,192
test ax,ax
jnz .bad
inc word [posted-data]
mov ax,[bp+10]
mov [sender-data],ax
post sender,0x466,0xabcd,0,[task-data]
test ax,ax
jz .bad
xor ax,ax
jmp .value
.ack:
api 2,192
test ax,ax
jnz .bad
cmp word [bp+10],0xabcd
jne .bad
mov ax,[bp+6]
cmp ax,[peer_task-data]
jne .bad
inc word [posted-data]
xor ax,ax
jmp .value
.quit:
push word 0
api 2,6
mov ax,0xf
jmp .value
.bad:
or word [errors-data],0x4000
xor ax,ax
.value:
mov dx,0x447a
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
%assign call_number 1
%rep calls
db 3,1
dw ..@call_%[call_number]-code+1,module_%[call_number],ordinal_%[call_number]
%assign call_number call_number+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'MSG1'
%ifdef RECEIVER
role: dw 1
%else
role: dw 0
%endif
phase: dw 0
errors: dw 0
task: dw 0
peer_task: dw 0
basic: dw 0
nested: dw 0
early: dw 0
posted: dw 0
same: dw 0
filtered: dw 0
threadpost: dw 0
text: dw 0
native_count: dw 0
destroyed: dw 0
report_end:
instance: dw 0
window: dw 0
peer: dw 0
sender: dw 0
native_window: dw 0
started: dw 0
file: dw -1
procedure: dd 0
module: dd 0
native: dd 0
message: times 18 db 0
buffer: times 32 db 0
wndclass:
dw 0
dd 0
dw 0,0,0,0,0,0
dd 0
dw classname-data,0
%ifdef RECEIVER
classname: db 'T447_MSG_RECV',0
filename: db 'MSGRECV.BIN',0
%else
classname: db 'T447_MSG_SEND',0
filename: db 'MSGSEND.BIN',0
%endif
caption: db 'Message witness',0
peerclass: db 'T447_MSG_RECV',0
nativeclass: db 'T447_MSG_NATIVE',0
childname: db 'MSGRECV.EXE',0
guest_text: db 'Guest text',0
library: db 'MSGHOST.DLL',0
export: db 'MessagePeer',0
data_end:
align 16,db 0
image_end:
