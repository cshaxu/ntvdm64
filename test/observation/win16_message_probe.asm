; Independent NE witness, never a replacement for original guest media.
; Imports/ABI: selected wow16 USER/KERNEL .def and inc/windows.h.
; Two actual USER16 tasks, synchronous/nested/reply/posted messages and
; string copyback. Native peer is a separately authored test DLL.
bits 16
org 0
%ifdef THREAD_CLIENT_VIEW
%ifdef RECEIVER
%define EXPECTED_VERSION 0x0300
%else
%define EXPECTED_VERSION 0x030a
%endif
%else
%define EXPECTED_VERSION 0x030a
%endif
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
dw 0,0,0,EXPECTED_VERSION
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
client_view
%endmacro
%macro post 5
push word [%1-data]
push word %2
push word %3
push word %4
push word %5
api 2,110
client_view
%endmacro
%macro peek 3
push ds
push word message-data
push word 0
push word %1
push word %2
push word %3
api 2,109
client_view
%endmacro
%macro push32 1
push word [%1+2-data]
push word [%1-data]
%endmacro
%macro client_view 0
%ifdef THREAD_CLIENT_VIEW
; Independently authored read-only witness of USER16's own FLATFS contract.
; Save the complete EAX and caller FS. No host call may refresh the view first.
push eax
push fs
mov ax,0x3b
mov fs,ax
mov eax,[fs:0x50]
pop fs
mov [client_version-data],ax
cmp ax,EXPECTED_VERSION
je %%valid
or word [errors-data],0x1000
%%valid:
inc word [client_checks-data]
pop eax
%endif
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
%ifdef CLASSIC_DDE_TASK_LOSS
; The sender owns peer startup. The surviving task only observes its cleanup.
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
require 24
push32 module
push ds
push word export-data
api 1,515
mov [native-data],ax
mov [native+2-data],dx
or ax,dx
require 25
%endif
.pump:
%ifdef CLASSIC_DDE_TASK_LOSS
cmp word [dde_task_loss_requested-data],1
jne .pump_message
push word [sender-data]
api 2,47
test ax,ax
jz .task_loss_released
api 1,29
jmp .pump
.task_loss_released:
mov word [dde_task_loss_released-data],1
mov word [dde_task_loss_requested-data],2
mov ax,2
call native_operation
result 3
push word 0
api 2,6
%endif
.pump_message:
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
%ifdef EXTENSION_WITNESS
cmp word [extension_receives-data],2
jne failed
%endif
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
%ifdef CROSS_WORKER
; A Hook-enabled native coordinator launches both separate WOW workers.
; Keep this sender's HWND/task alive until its real PID/TID has been pinned.
api 2,13
mov [started-data],ax
.wait_launch:
cmp word [launch_ready-data],1
je .launched
peek 0,0,0
call deadline
api 1,29
jmp .wait_launch
.launched:
%else
push ds
push word childname-data
push word 0
api 1,166
cmp ax,32
jbe failed
%endif
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
%ifdef CROSS_WORKER
; Original GetWindowTask(foreign) returns this worker's shell, not remote HTASK.
api 2,286
push ax
api 2,224
mov [foreign_expected-data],ax
cmp ax,[peer_task-data]
jne failed
require 22
send peer,0x47a,0,0,0
cmp dx,0x447a
jne failed
require 23
mov [remote_task-data],ax
%else
cmp ax,[task-data]
je failed
%endif
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
%ifdef M4_COPYDATA_PROC32
; USER16 GetWindowLong returns the original native-procedure thunk. Its
; CallWindowProc route must execute the native procedure on THIS task thread.
push ax
push word -4
api 2,135
mov [native_procedure-data],ax
mov [native_procedure+2-data],dx
or ax,dx
jz failed
%endif
send native_window,0x470,0x1234,0x5678,0x9abc
result 0x10
send native_window,0x471,0,0,0
result 0x11
send native_window,0x472,0,0,0
result 0x12
send native_window,0x473,0,0,0
result 0x13
mov word [native_count-data],4
%ifdef M4_STRUCTURES
; All three structure families, first across real Win16 tasks, then to the
; native peer. Native operation3 exercises the reverse native->guest path.
mov ax,[peer-data]
call structure_inputs
send peer,0x24,0x4474,ds,minmax-data
result 0x24
call check_minmax
send peer,0x46,0x4474,ds,position-data
result 0x46
call check_position
send peer,0x4a,[window-data],ds,copydata-data
result 0x4a
mov ax,[native_window-data]
call structure_inputs
send native_window,0x24,0x4474,ds,minmax-data
result 0x24
call check_minmax
send native_window,0x46,0x4474,ds,position-data
result 0x46
call check_position
send native_window,0x4a,[window-data],ds,copydata-data
result 0x4a
%ifdef M4_COPYDATA_PROC32
; Fresh guest input has no received native/far provenance node yet.
push32 native_procedure
push word [native_window-data]
push word 0x4a
push word [window-data]
push ds
push word copydata-data
api 2,122
result 0x4a
%endif
add word [native_count-data],3
%ifdef CLASSIC_DDE
mov ax,13
call native_operation
result 0x8d
.wait_dde:
peek 0x3e8,0x3e8,1
test ax,ax
jnz .dispatch_dde
call deadline
api 1,29
jmp .wait_dde
.dispatch_dde:
push ds
push word message-data
api 2,114
cmp word [dde_received-data],1
jne failed
cmp word [dde_release_attempt-data],1
jne failed
mov ax,14
call native_operation
result 0x8e
inc word [native_count-data]
cmp word [dde_retired-data],1
jne failed
; INITIATE is a direct service/topic atom pair, not a packed DDE lParam.
; Exercise the original initiator registration independently of EXECUTE.
mov ax,22
call native_operation
result 0x96
.wait_initiate:
peek 0x3e0,0x3e0,1
test ax,ax
jnz .dispatch_initiate
call deadline
api 1,29
jmp .wait_initiate
.dispatch_initiate:
push ds
push word message-data
api 2,114
cmp word [dde_initiate_ack-data],1
jne failed
mov ax,23
call native_operation
result 0x97
; Independent reverse-direction witness: a Win16-owned command is posted
; to the native peer, which ACKs the native alias back to this task.
mov ax,15
call native_operation
result 0x8f
push word 0x2002                 ; GMEM_MOVEABLE | GMEM_DDESHARE
push word 0
push word 32
api 1,15                        ; KERNEL GlobalAlloc
test ax,ax
jz failed
mov [dde_reverse_handle-data],ax
push ax
api 1,18                        ; GlobalLock
mov bx,ax
or bx,dx
jz failed
mov di,ax
mov es,dx
mov si,dde_reverse_payload-data
mov cx,17
cld
rep movsb
push word [dde_reverse_handle-data]
api 1,19                        ; GlobalUnlock
post native_window,0x3e8,[window-data],[dde_reverse_handle-data],0
test ax,ax
jz failed
mov ax,16
call native_operation
result 0x90
.wait_reverse_ack:
peek 0x3e4,0x3e4,1
test ax,ax
jnz .dispatch_reverse_ack
call deadline
api 1,29
jmp .wait_reverse_ack
.dispatch_reverse_ack:
push ds
push word message-data
api 2,114
cmp word [dde_reverse_ack-data],1
jne failed
mov ax,0x3e5                   ; WM_DDE_DATA
call dde_packet_roundtrip
mov ax,0x3e7                   ; WM_DDE_POKE
call dde_packet_roundtrip
mov ax,0x3e2                   ; WM_DDE_ADVISE
call dde_packet_roundtrip
call dde_failed_post
%ifdef SYNC_DDE_DIAGNOSTIC
; Same native peer/thread, but generated entirely by the host as a control.
mov ax,15
call native_operation
result 0x8f
mov ax,17
call native_operation
result 0x91
mov ax,16
call native_operation
result 0x90
; Original WOW also accepts applications that send EXECUTE synchronously.
mov ax,15
call native_operation
result 0x8f
push word 0x2002
push word 0
push word 32
api 1,15
test ax,ax
jz failed
mov [dde_reverse_handle-data],ax
push ax
api 1,18
mov bx,ax
or bx,dx
jz failed
mov di,ax
mov es,dx
mov si,dde_reverse_payload-data
mov cx,17
cld
rep movsb
push word [dde_reverse_handle-data]
api 1,19
send native_window,0x3e8,[window-data],[dde_reverse_handle-data],0
mov ax,16
call native_operation
result 0x90
.wait_sync_dde_ack:
peek 0x3e4,0x3e4,1
test ax,ax
jnz .dispatch_sync_dde_ack
call deadline
api 1,29
jmp .wait_sync_dde_ack
.dispatch_sync_dde_ack:
push ds
push word message-data
api 2,114
cmp word [dde_reverse_ack-data],2
jne failed
%endif
%endif
mov ax,3
call native_operation
result 0x20
cmp word [structures-data],4
jne failed
%ifdef M4_REPLY_PAYLOAD
; The native peer replies from another actual thread, protects only its own
; input after the reply, then acknowledges. The receiver never rereads input.
mov ax,5
call native_operation
result 0x55
%ifdef M4_TIMEOUT_PAYLOAD
mov ax,8
call native_operation
result 0x58
%endif
%endif
%endif
%ifdef PENDING_SEND
mov ax,9
call native_operation
result 0x89
cmp word [wait_blocked-data],1
jne failed
mov ax,10
call native_operation
result 0x8a
%endif
%ifdef CONSTRUCTION
call construction_check
%endif
%ifdef MESSAGE_TRANSACTION
; Real native entry/callback fault is caught only by the independent native
; peer; the original guest then continues through the full existing group.
mov ax,12
call native_operation
result 0x8c
%endif
%ifdef EXTENSION_WITNESS
; Real original GetProcAddress32 failure followed by valid resolution.
push32 module
push ds
push word absent_export-data
api 1,515
or ax,dx
jnz failed
push32 module
push ds
push word extension_export-data
api 1,515
mov [extension_native-data],ax
mov [extension_native+2-data],dx
or ax,dx
jz failed
push cs
push word extension_callback-code
push word [instance-data]
api 1,51
mov [extension_procedure-data],ax
mov [extension_procedure+2-data],dx
or ax,dx
jz failed
push bp
mov bp,sp
push word 0
push word 0x79
push32 extension_procedure
push word 0
push word [peer_task-data]
push32 extension_native
push word 0
push word 0
push word 0
push word 3
api 1,517
cmp sp,bp
jne fatal
pop bp
result 0x79
cmp word [extension_calls-data],2
jne failed
; DirectedYield is a scheduling opportunity, not a posted-message barrier.
; GetMessage pumps the receiver's synchronous acknowledgement and waits for
; its explicit posted completion before inspecting the guest counter.
push ds
push word message-data
push word 0
push word 0x478
push word 0x478
api 2,108
test ax,ax
jz failed
cmp word [message+2-data],0x478
jne failed
cmp word [extension_yielded-data],1
jne failed
push32 extension_procedure
api 1,52
inc word [native_count-data]
%endif
cmp word [nested-data],2
jne failed
cmp word [same-data],2
jne failed
cmp word [early-data],1
jne failed
%ifdef CLASSIC_DDE_TASK_LOSS
call dde_task_loss
mov word [phase-data],3
call checkpoint
mov ax,0x4c00
int 0x21
%endif
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
%ifdef CLASSIC_DDE
dde_failed_post:
push word 0x2002
push word 0
push word 32
api 1,15
test ax,ax
jz failed
mov [dde_reverse_handle-data],ax
push ax
api 1,18
mov bx,ax
or bx,dx
jz failed
mov di,ax
mov es,dx
mov word [es:di],0
mov word [es:di+2],1
add di,4
mov si,dde_packet_payload-data
mov cx,14
cld
rep movsb
push word [dde_reverse_handle-data]
api 1,19
mov ax,20
call native_operation
result 0x94
push word [native_window-data]
push word 0x3e5
push word [window-data]
push word 0
push word [dde_reverse_handle-data]
api 2,110
client_view
mov [dde_failed_result-data],ax
mov ax,21
call native_operation
result 0x95
cmp word [dde_failed_result-data],0
jne failed
; Failed delivery must not consume the sender's 16-bit memory.
push word [dde_reverse_handle-data]
api 1,18
mov bx,ax
or bx,dx
jz failed
push word [dde_reverse_handle-data]
api 1,19
push word [dde_reverse_handle-data]
api 1,17
test ax,ax
jnz failed
ret

dde_packet_roundtrip:
mov [dde_packet_message-data],ax
mov ax,15
call native_operation
result 0x8f
push word 0x2002
push word 0
push word 32
api 1,15
test ax,ax
jz failed
mov [dde_reverse_handle-data],ax
push ax
api 1,18
mov bx,ax
or bx,dx
jz failed
mov di,ax
mov es,dx
mov word [es:di],0             ; sender retains data ownership
mov word [es:di+2],1           ; CF_TEXT
add di,4
mov si,dde_packet_payload-data
mov cx,14
cld
rep movsb
push word [dde_reverse_handle-data]
api 1,19
push word [native_window-data]
push word [dde_packet_message-data]
push word [window-data]
push word 0
push word [dde_reverse_handle-data]
api 2,110
client_view
test ax,ax
jz failed
mov ax,16
call native_operation
result 0x90
mov ax,18
call native_operation
result 0x92
push word [dde_reverse_handle-data]
api 1,17                       ; original app-owned guest global handle
test ax,ax
jnz failed
mov ax,19
call native_operation
result 0x93
inc word [dde_packet_count-data]
ret

%ifdef CLASSIC_DDE_TASK_LOSS
; Leave a sender-owned DATA alias live and end this task.  The surviving
; task records the original rule: task exit itself does not call GlobalFree.
dde_task_loss:
mov ax,15
call native_operation
result 0x8f
push word 0x2002
push word 0
push word 32
api 1,15
test ax,ax
jz failed
mov [dde_reverse_handle-data],ax
push ax
api 1,18
mov bx,ax
or bx,dx
jz failed
mov di,ax
mov es,dx
mov word [es:di],0
mov word [es:di+2],1
add di,4
mov si,dde_packet_payload-data
mov cx,14
cld
rep movsb
push word [dde_reverse_handle-data]
api 1,19
push word [native_window-data]
push word 0x3e5
push word [window-data]
push word 0
push word [dde_reverse_handle-data]
api 2,110
client_view
test ax,ax
jz failed
mov ax,16
call native_operation
result 0x90
mov ax,18
call native_operation
result 0x92
post peer,0x47f,[window-data],0,0
test ax,ax
jz failed
mov word [dde_task_loss_armed-data],1
ret
%endif
%endif
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
client_view
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
client_view
%ifdef CONSTRUCTION
cmp word [construction_help_message-data],0
je .construction_window
mov ax,[construction_help_message-data]
cmp ax,[bp+12]
je .construction_help
.construction_window:
cmp word [construction_active-data],1
jne .construction_dispatch
cmp word [bp+12],1
je .construction_create
.construction_dispatch:
cmp word [bp+12],2
jne .construction_regular
mov ax,[bp+14]
cmp ax,[construction_child-data]
jne .construction_regular
inc word [mdi_destroyed-data]
.construction_regular:
%endif
%ifdef M4_STRUCTURES
cmp word [bp+12],0x4a
je .copydata
cmp word [bp+10],0x4474
jne .ordinary
cmp word [bp+12],0x24
je .minmax
cmp word [bp+12],0x46
je .position
.ordinary:
%endif
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
%ifdef PENDING_SEND
cmp word [bp+12],0x47b
je .wait_blocked
cmp word [bp+12],0x47c
je .death_destroy
cmp word [bp+12],0x47d
je .death_create
cmp word [bp+12],0x47e
je .death_stale
%endif
%ifdef CLASSIC_DDE
cmp word [bp+12],0x47f
je .dde_task_loss_notice
cmp word [bp+12],0x3e0
je .dde_initiate
cmp word [bp+12],0x3e8
je .dde_execute
cmp word [bp+12],0x3e4
je .dde_reverse_ack
cmp word [bp+12],0x486
je .dde_retired
%endif
%ifdef CROSS_WORKER
cmp word [bp+12],0x479
je .launch_ready
cmp word [bp+12],0x47a
je .query_task
%endif
%ifdef EXTENSION_WITNESS
cmp word [bp+12],0x474
je .extension_receive
cmp word [bp+12],0x475
je .extension_yield
cmp word [bp+12],0x476
je .extension_yield_ack
%endif
.default:
%ifdef CONSTRUCTION
cmp word [construction_client-data],0
je .plain_default
mov ax,[bp+14]
cmp ax,[window-data]
jne .mdi_default
push word [bp+14]
push word [construction_client-data]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,445                       ; original USER16 DefFrameProc
jmp .return
.mdi_default:
push word [bp+14]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,447                       ; original USER16 DefMDIChildProc
jmp .return
.plain_default:
%endif
push word [bp+14]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,107
jmp .return
%ifdef M4_STRUCTURES
.minmax:
les di,[bp+6]
cmp word [es:di],0x1234
je .minmax_outer
cmp word [es:di],0x2345
jne .bad
call check_minmax_input
jmp .minmax_result
.minmax_outer:
call check_minmax_input
mov word [es:di],0x2345
; Nest a converted structure send while the outer original callback owns
; its stack allocation. Reload the far pointer after reentry, not a flat alias.
%ifdef RECEIVER
push word [sender-data]
%else
push word [peer-data]
%endif
push word 0x24
push word 0x4474
push es
push di
api 2,111
cmp ax,0x24
jne .bad
cmp dx,0x447a
jne .bad
les di,[bp+6]
cmp word [es:di],0x7abc
jne .bad
cmp word [es:di+18],-11
jne .bad
.minmax_result:
mov word [es:di],0x7abc
mov word [es:di+18],-11
inc word [structures-data]
mov ax,0x24
jmp .value
.position:
les di,[bp+6]
mov ax,[window-data]
cmp word [es:di],ax
jne .bad
cmp word [es:di+2],0
jne .bad
cmp word [es:di+4],-12
jne .bad
cmp word [es:di+6],34
jne .bad
cmp word [es:di+8],56
jne .bad
cmp word [es:di+10],78
jne .bad
cmp word [es:di+12],4
jne .bad
mov word [es:di+4],-23
mov word [es:di+6],45
mov word [es:di+8],90
mov word [es:di+12],0x14
inc word [structures-data]
mov ax,0x46
jmp .value
.copydata:
les di,[bp+6]
%ifdef M4_TIMEOUT_PAYLOAD
cmp dword [es:di],0x456789ad
je .copydata_input
%endif
%ifdef M4_REPLY_PAYLOAD
cmp dword [es:di],0x456789ac
je .copydata_input
%endif
cmp dword [es:di],0x456789ab
jne .default
.copydata_input:
cmp dword [es:di+4],8
jne .bad
les di,[es:di+8]
cmp dword [es:di],0x6f63344d ; M4co
jne .bad
cmp dword [es:di+4],0x21217970 ; py!!
jne .bad
cmp word [copy_depth-data],0
jne .copydata_inner
mov word [copy_depth-data],1
mov ax,[bp+6]
mov [copy_far-data],ax
mov ax,[bp+8]
mov [copy_far+2-data],ax
; Original CallWindowProc with a real16-bit procedure uses its existing
; CallBack16 path. It must not replace/free this outer converted allocation.
push32 procedure
push word [window-data]
push word 0x4a
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,122
cmp ax,0x4a
jne .bad
cmp dx,0x447a
jne .bad
mov word [copy_depth-data],0
%ifdef M4_COPYDATA_PROC32
cmp dword [native_procedure-data],0
je .no_native_procedure
; Here target/from/far-header match this still-active received pair.
; The native procedure receives the borrowed original conversion; the outer
; guest far header and data must remain usable after native return.
push32 native_procedure
push word [window-data]
push word 0x4a
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,122
cmp ax,0x4a
jne .bad
cmp dx,0x447a
jne .bad
.no_native_procedure:
%endif
; Reacquire the far header/payload before inspecting them after reentry.
les di,[bp+6]
cmp dword [es:di+4],8
jne .bad
les di,[es:di+8]
cmp dword [es:di],0x6f63344d
jne .bad
cmp dword [es:di+4],0x21217970
jne .bad
%ifdef M4_REPLY_PAYLOAD
; BX is not our state across CallWindowProc. Classify from the read-only
; header again before replying; no input is consulted after ReplyMessage.
les di,[bp+6]
%ifdef M4_TIMEOUT_PAYLOAD
cmp dword [es:di],0x456789ad
je .copydata_timeout
%endif
cmp dword [es:di],0x456789ac
je .copydata_reply
%endif
inc word [structures-data]
mov ax,0x4a
jmp .value
.copydata_inner:
mov ax,[bp+6]
cmp ax,[copy_far-data]
jne .bad
mov ax,[bp+8]
cmp ax,[copy_far+2-data]
jne .bad
inc word [copy_nested-data]
mov ax,0x4a
jmp .value
%ifdef M4_REPLY_PAYLOAD
.copydata_reply:
mov ax,7
call native_operation
cmp ax,0x57
jne .bad
cmp dx,0x447a
jne .bad
push word 0x447a
push word 0x51
api 2,115
; No input pointer read after ReplyMessage. This original generic thunk
; waits for the sender's event, not a guessed delay or guest polling loop.
mov ax,6
call native_operation
cmp ax,0x56
jne .bad
cmp dx,0x447a
jne .bad
mov ax,0x53
jmp .value
%ifdef M4_TIMEOUT_PAYLOAD
.copydata_timeout:
mov ax,7
call native_operation
cmp ax,0x57
jne .bad
cmp dx,0x447a
jne .bad
; No ReplyMessage: the real native sender must reach its deadline while
; this receiver has entered and waits for the sender's acknowledgement.
mov ax,6
call native_operation
cmp ax,0x56
jne .bad
cmp dx,0x447a
jne .bad
mov ax,0x54
jmp .value
%endif
%endif
%endif
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
%ifdef CROSS_WORKER
push ax
api 2,224
mov [peer_task-data],ax
api 2,286
push ax
api 2,224
mov [foreign_expected-data],ax
cmp ax,[peer_task-data]
jne .bad
test ax,ax
jz .bad
send sender,0x47a,0,0,0
cmp dx,0x447a
jne .bad
test ax,ax
jz .bad
mov [remote_task-data],ax
%endif
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
%ifdef CROSS_WORKER
cmp ax,[remote_task-data]
%else
cmp ax,[peer_task-data]
%endif
jne .bad
inc word [posted-data]
xor ax,ax
jmp .value
.quit:
push word 0
api 2,6
mov ax,0xf
jmp .value
%ifdef EXTENSION_WITNESS
.extension_receive:
api 2,192
test ax,ax
jz .bad
inc word [extension_receives-data]
mov ax,0x74
jmp .value
.extension_yield:
send sender,0x476,0,0,0
cmp ax,0x76
jne .bad
cmp dx,0x447a
jne .bad
push word [sender-data]
push word 0x478
push word 0
push word 0
push word 0
api 2,110
test ax,ax
jz .bad
api 1,29
xor ax,ax
jmp .value
.extension_yield_ack:
api 2,192
test ax,ax
jz .bad
inc word [extension_yielded-data]
mov ax,0x76
jmp .value
%endif
%ifdef CONSTRUCTION
.construction_help:
mov ax,[bp+6]
test ax,ax
jz .bad
mov [construction_help_handle-data],ax
push ax
api 1,18                       ; original KERNEL GlobalLock
mov di,ax
mov es,dx
or ax,dx
jz .bad
cmp word [es:di],16
jne .bad
cmp dword [es:di+4],0x11223344
jne .bad
push word [construction_help_handle-data]
api 1,19                       ; original KERNEL GlobalUnlock
inc word [construction_help_calls-data]
mov ax,0x7f
jmp .value
.construction_create:
mov ax,[bp+14]
mov [construction_child-data],ax
inc word [mdi_created-data]
cmp word [construction_mode-data],1
je .construction_title
cmp word [construction_mode-data],2
jne .construction_accept
les di,[bp+6]
mov dword [es:di],0              ; replace nested CREATE params, not its owner
inc word [mdi_params-data]
jmp .construction_accept
.construction_title:
les di,[bp+6]
les di,[es:di]
mov word [es:di+4],construction_foreign-data
mov word [es:di+6],ds
inc word [mdi_title-data]
.construction_accept:
xor ax,ax
xor dx,dx
jmp .return
%endif
%ifdef PENDING_SEND
.wait_blocked:
api 2,192
test ax,ax
jz .bad
send native_window,0x484,0,0,0
cmp dx,0x447a
jne .bad
cmp ax,0x84
jne .bad
inc word [wait_blocked-data]
mov ax,0x7b
jmp .value
.death_create:
cmp word [death_window-data],0
jne .bad
push ds
push word classname-data
push ds
push word death_caption-data
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
test ax,ax
jz .bad
mov [death_window-data],ax
jmp .value
.death_destroy:
api 2,192
test ax,ax
jz .bad
mov ax,[bp+14]
cmp ax,[death_window-data]
jne .bad
push ax
api 2,53
test ax,ax
jz .bad
inc word [wait_death-data]
mov ax,0x7c
jmp .value
.death_stale:
cmp word [wait_death-data],1
jne .bad
push word [death_window-data]
api 2,47
test ax,ax
jnz .bad
send death_window,0x460,0x1234,0x5678,0x9abc
or ax,dx
jnz .bad
mov ax,0x7e
jmp .value
%endif
%ifdef CROSS_WORKER
.launch_ready:
cmp word [bp+10],0x5341
jne .bad
cmp dword [bp+6],0x574f5732
jne .bad
cmp word [launch_ready-data],0
jne .bad
mov word [launch_ready-data],1
mov ax,0x79
jmp .value
.query_task:
api 1,36
cmp ax,[task-data]
jne .bad
jmp .value
%endif
%ifdef CLASSIC_DDE
.dde_task_loss_notice:
%ifdef CLASSIC_DDE_TASK_LOSS
mov ax,[bp+10]
test ax,ax
jz .bad
mov [sender-data],ax
mov word [dde_task_loss_requested-data],1
xor ax,ax
jmp .value
%else
jmp .bad
%endif
.dde_initiate:
; Preserve the atom pair directly in the INITIATE ACK.
mov ax,[bp+6]
mov dx,[bp+8]
or ax,ax
jz .bad
or dx,dx
jz .bad
push word [native_window-data]
push word 0x3e4
push word [window-data]
push dx
push ax
api 2,110
test ax,ax
jz .bad
inc word [dde_initiate_ack-data]
xor ax,ax
jmp .value
.dde_execute:
mov word [dde_stage-data],1
mov ax,[bp+8]
test ax,ax
jz .bad
mov [dde_handle-data],ax
mov word [dde_stage-data],2
push ax
api 1,18
mov di,ax
mov es,dx
or ax,dx
jz .bad
mov word [dde_stage-data],3
cmp word [es:di],0x3454
jne .bad
mov word [dde_stage-data],4
cmp word [es:di+2],0x3734
jne .bad
mov word [dde_stage-data],5
cmp word [es:di+4],0x442d
jne .bad
mov word [dde_stage-data],6
cmp word [es:di+6],0x4544
jne .bad
mov word [dde_stage-data],7
push word [dde_handle-data]
api 1,19
push word [native_window-data]
push word 0x3e4
push word [window-data]
push word [dde_handle-data]
push word 0x8000
api 2,110
test ax,ax
jz .bad
mov word [dde_stage-data],8
push word [dde_handle-data]
api 1,17
or ax,ax
jnz .bad
mov word [dde_stage-data],9
inc word [dde_received-data]
inc word [dde_release_attempt-data]
xor ax,ax
jmp .value
.dde_retired:
mov word [dde_stage-data],10
push word [dde_handle-data]
api 1,18
or ax,dx
jnz .bad
inc word [dde_retired-data]
mov word [dde_stage-data],11
mov ax,0x86
jmp .value
.dde_reverse_ack:
cmp word [bp+6],0x8000
jne .bad
mov ax,[dde_reverse_handle-data]
cmp [bp+8],ax
jne .bad
push ax
api 1,17                        ; original guest handle remains app-owned
test ax,ax
jnz .bad
inc word [dde_reverse_ack-data]
xor ax,ax
jmp .value
%endif
.bad:
or word [errors-data],0x4000
xor ax,ax
.value:
mov dx,0x447a
.return:
client_view
pop bx
pop di
pop si
pop es
pop ds
pop bp
retf 10
%ifdef CONSTRUCTION
construction_check:
mov [construction_mdi+2-data],ds
mov [construction_mdi+6-data],ds
mov ax,[instance-data]
mov [construction_mdi+8-data],ax
push ds
push word construction_client_class-data
push word 0
push word 0
push word 0x4200                 ; WS_CHILD | WS_CLIPCHILDREN
push word 0
push word 0
push word 0
push word 100
push word 60
push word [window-data]
push word 0
push word [instance-data]
push ds
push word construction_client_params-data
api 2,41
require 24
mov [construction_client-data],ax
.next:
mov word [construction_active-data],1
send construction_client,0x220,0,ds,construction_mdi-data
mov word [construction_active-data],0
require 25
cmp ax,[construction_child-data]
jne failed
push ax
api 2,224
cmp ax,[task-data]
jne failed
send construction_child,0x000c,0,ds,construction_text-data
; Native DefMDIChildProc returns zero even when the title is changed.
; Require that return plus the exact subsequent text, not DefWindowProc's BOOL.
or ax,dx
jnz failed
send construction_child,0x000d,32,ds,buffer-data
cmp ax,15
jne failed
mov si,buffer-data
mov di,construction_text-data
mov cx,16
push ds
pop es
cld
repe cmpsb
jne failed
inc word [mdi_text-data]
send construction_client,0x221,[construction_child-data],0,0
push word [construction_child-data]
api 2,47
test ax,ax
jnz failed
inc word [construction_mode-data]
cmp word [construction_mode-data],3
jne .next
push word [construction_client-data]
api 2,53
require 27
mov word [construction_client-data],0
cmp word [mdi_created-data],3
jne failed
cmp word [mdi_destroyed-data],3
jne failed
push ds
push word construction_help_name-data
api 2,118                      ; original USER16 RegisterWindowMessage
cmp ax,0xc000
jb failed
mov [construction_help_message-data],ax
mov ax,11
call native_operation
result 0x8b
push word [construction_help_handle-data]
api 1,18
or ax,dx
jnz failed
inc word [construction_help_freed-data]
; Reverse private-help conversion must preserve a native-heap-sized payload.
send native_window,0x38,0,ds,construction_reverse_help-data
result 0x7e
ret
%endif
%ifdef M4_STRUCTURES
; Caller-owned inputs; 16-bit ABI layouts are from original wowusr.h.
structure_inputs:
mov [position-data],ax
mov word [minmax-data],0x1234
mov word [minmax+18-data],10
mov word [position+4-data],-12
mov word [position+6-data],34
mov word [position+8-data],56
mov word [position+12-data],4
mov [copydata+10-data],ds
ret
check_minmax:
cmp word [minmax-data],0x7abc
jne failed
cmp word [minmax+18-data],-11
jne failed
ret
check_position:
cmp word [position+4-data],-23
jne failed
cmp word [position+6-data],45
jne failed
cmp word [position+8-data],90
jne failed
cmp word [position+10-data],78
jne failed
cmp word [position+12-data],0x14
jne failed
ret
check_minmax_input:
push si
push di
mov si,minmax_expected-data+2
add di,2
mov cx,9
cld
repe cmpsw
pop di
pop si
je .valid
or word [errors-data],0x2000
.valid:
ret
%endif
%ifdef EXTENSION_WITNESS
extension_callback:
push bp
mov bp,sp
push ds
push es
push si
push di
push bx
mov ds,ax
client_view
cmp word [bp+6],0x5678
jne .bad
cmp word [bp+8],0x1234
jne .bad
api 1,36
cmp ax,[task-data]
jne .bad
api 2,192
test ax,ax
jnz .bad
send peer,0x474,0,0,0
cmp ax,0x74
jne .bad
cmp dx,0x447a
jne .bad
inc word [extension_calls-data]
mov ax,0x79
jmp .done
.bad:
or word [errors-data],0x2000
xor ax,ax
.done:
mov dx,0x447a
client_view
pop bx
pop di
pop si
pop es
pop ds
pop bp
retf 4
%endif
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
report:
%ifdef CONSTRUCTION
db 'MSG9'
%else
%ifdef PENDING_SEND
db 'MSG8'
%else
%ifdef CROSS_WORKER
db 'MSG7'
%else
%ifdef M4_STRUCTURES
%ifdef THREAD_CLIENT_VIEW
db 'MSG6'
%else
db 'MSG5'
%endif
%else
%ifdef THREAD_CLIENT_VIEW
db 'MSG2'
%else
db 'MSG1'
%endif
%endif
%endif
%endif
%endif
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
%ifdef THREAD_CLIENT_VIEW
client_version: dw 0
client_expected: dw EXPECTED_VERSION
client_checks: dw 0
%endif
%ifdef M4_STRUCTURES
structures: dw 0
copy_nested: dw 0
%endif
%ifdef CLASSIC_DDE
dde_received: dw 0
dde_release_attempt: dw 0
dde_retired: dw 0
dde_stage: dw 0
dde_reverse_ack: dw 0
dde_packet_count: dw 0
dde_initiate_ack: dw 0
%ifdef CLASSIC_DDE_TASK_LOSS
dde_task_loss_armed: dw 0
dde_task_loss_requested: dw 0
dde_task_loss_released: dw 0
%endif
%endif
%ifdef CROSS_WORKER
remote_task: dw 0
foreign_expected: dw 0
launch_ready: dw 0
%endif
%ifdef PENDING_SEND
wait_blocked: dw 0
wait_death: dw 0
%endif
%ifdef CONSTRUCTION
mdi_created: dw 0
mdi_destroyed: dw 0
mdi_title: dw 0
mdi_params: dw 0
mdi_text: dw 0
construction_help_calls: dw 0
construction_help_freed: dw 0
%endif
report_end:
%ifdef CONSTRUCTION
construction_client: dw 0
construction_child: dw 0
construction_active: dw 0
construction_mode: dw 0
construction_help_message: dw 0
construction_help_handle: dw 0
construction_help_name: db 'WM_WINHELP',0
construction_reverse_help:
dw 512,0
dd 0x11223344
times 500 db 0
dd 0x55667788
construction_client_class: db 'MDICLIENT',0
construction_client_params: dw 0,1000
construction_mdi: dw classname-data,0,construction_title-data,0,0
dw 0,0,80,40
dd 0,0
construction_title: db 'MDI title',0
construction_foreign: db 'not owned by converter',0
construction_text: db 'M4 native title',0
%endif
%ifdef PENDING_SEND
death_window: dw 0
death_caption: db 'T447 death target',0
%endif
instance: dw 0
window: dw 0
peer: dw 0
sender: dw 0
native_window: dw 0
%ifdef M4_COPYDATA_PROC32
native_procedure: dd 0
%endif
started: dw 0
file: dw -1
procedure: dd 0
module: dd 0
native: dd 0
%ifdef CLASSIC_DDE
dde_handle: dw 0
dde_reverse_handle: dw 0
dde_reverse_payload: db 'T449-DDE-REVERSE',0
dde_packet_message: dw 0
dde_failed_result: dw 0
dde_packet_payload: db 'T449-DDE-DATA',0
%endif
%ifdef EXTENSION_WITNESS
extension_native: dd 0
extension_procedure: dd 0
extension_calls: dw 0
extension_receives: dw 0
extension_yielded: dw 0
extension_export: db 'MessageExtension',0
absent_export: db 'T447AbsentExtension',0
%endif
message: times 18 db 0
buffer: times 32 db 0
%ifdef M4_STRUCTURES
minmax: dw 0x1234,2,3,4,5,6,-7,8,9,10
copy_far: dd 0
copy_depth: dw 0
minmax_expected: dw 0x1234,2,3,4,5,6,-7,8,9,10
position: dw 0,0,-12,34,56,78,4
copydata: dd 0x456789ab,8
dw payload-data,0
payload: db 'M4copy!!'
%endif
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
