; Independently authored DOS witness; API results are written after an interval
; so the host can prove two actual VMs were simultaneously executing.
bits 16
org 0x100
push cs
pop ds
xor ax, ax
mov es, ax
xor di, di
mov ax, 0x1684
mov bx, 0x7f80
int 0x2f
mov [api], di
mov [api+2], es
mov ax, es
or ax, di
jz .write
xor ax, ax
call far [api]
pushf
pop word [status]
mov [signature], eax
mov [vm], ebx
mov [generation], edx
mov [generation+4], esi
mov ax, 0xffff
call far [api]
pushf
pop word [negative_status]
mov [negative_result], eax
mov ax, 1
call far [api]
jc .write
mov [callback], edx
test edx, edx
jz .write
push ebp
push edi
mov [before_sp], sp
.callback:
mov ebp, 0x12345678
mov edi, 0x76543210
xor ax, ax
std
call far [callback]
pushf
pop word [callback_flags]
cld
cmp sp, [before_sp]
jne .callback_error
cmp ebp, 0x12345678
jne .callback_error
cmp edi, 0x76543210
jne .callback_error
cmp eax, [signature]
jne .callback_error
cmp ebx, [vm]
jne .callback_error
cmp edx, [generation]
jne .callback_error
cmp esi, [generation+4]
jne .callback_error
test word [callback_flags], 1
jnz .callback_error
test word [callback_flags], 0x400
jz .callback_error
jmp .callback_next
.callback_error:
inc word [callback_errors]
.callback_next:
inc word [callback_count]
cmp word [callback_count], 160
jb .callback
pop edi
pop ebp
; Counted completion must survive arriving before the wait. Two signals
; followed by two waits would hang if completion were only a boolean pulse.
mov ebx, [vm]
mov edx, [generation]
mov esi, [generation+4]
mov ax, 3
call far [api]
jc .semaphore_error
mov ax, 3
call far [api]
jc .semaphore_error
mov ax, 2
call far [api]
jc .semaphore_error
inc word [wait_count]
mov ax, 2
call far [api]
jc .semaphore_error
inc word [wait_count]
; A stale generation must not deposit a completion on a live reused handle.
mov edx, [generation]
xor edx, 0x80000000
mov ax, 3
call far [api]
jnc .semaphore_error
inc word [wait_count]
jmp .pair
.semaphore_error:
inc word [wait_errors]
.pair:
; Test-only file rendezvous. It is not the bridge's notification transport.
; A publishes its identity, then blocks. B must report A was waiting when it
; signals. The outer owned observer bounds failures; no product timeout exists.
cmp byte [role], 'A'
jne .peer
mov dx, pair_ready
xor cx, cx
mov ah, 0x3c
int 0x21
jc .pair_error
mov bx, ax
mov dx, vm
mov cx, 12
mov ah, 0x40
int 0x21
jc .pair_error
cmp ax, 12
jne .pair_error
mov ah, 0x3e
int 0x21
jc .pair_error
mov ax, 2
call far [api]
jc .pair_error
inc word [pair_count]
jmp .notification
.peer:
mov dx, pair_ready
mov ax, 0x3d00
int 0x21
jnc .peer_open
mov ax, 0x1680
int 0x2f
jmp .peer
.peer_open:
mov bx, ax
mov dx, peer_identity
mov cx, 12
mov ah, 0x3f
int 0x21
jc .pair_error
cmp ax, 12
jne .pair_error
mov ah, 0x3e
int 0x21
jc .pair_error
; Give A execution time to enter its wait; the returned waiting flag, not
; this delay, is the evidence that the signal woke a blocked VM.
xor ax, ax
int 0x1a
mov [peer_tick], dx
.peer_yield:
mov ax, 0x1680
int 0x2f
xor ax, ax
int 0x1a
sub dx, [peer_tick]
cmp dx, 9
jb .peer_yield
mov ebx, [peer_identity]
mov edx, [peer_identity+4]
mov esi, [peer_identity+8]
mov ax, 8                          ; host queue + physical IRQ wakes VM A
call far [api]
jc .pair_error
cmp ecx, 0x401                     ; VMM blocked + driver wait entered
jne .pair_error
inc word [pair_count]
jmp .notification
.pair_error:
inc word [pair_errors]
.notification:
mov ax,6
call far [api]
jc .notification_error
mov [notification_published],edx
mov [notification_published+4],esi
.notification_wait:
mov ax,0x1680
int 0x2f
mov ax,5
call far [api]
jc .notification_error
cmp esi,[notification_published+4]
jb .notification_wait
ja .notification_arrived
cmp edx,[notification_published]
jb .notification_wait
.notification_arrived:
test ecx,ecx
jz .notification_error
cmp ebx,15
ja .notification_error
mov [notification_irq],bx
mov [notification_seen],edx
mov [notification_seen+4],esi
jmp .timing
.notification_error:
inc word [notification_errors]
.timing:
xor ax, ax
int 0x1a
mov [start], dx
mov [start+2], cx
.yield:
mov ax, 0x1680
int 0x2f
xor ax, ax
int 0x1a
mov [finish], dx
mov [finish+2], cx
sub dx, [start]
cmp dx, 54
jb .yield
.write:
mov dx, output
xor cx, cx
mov ah, 0x3c
int 0x21
jc .fail
mov bx, ax
mov dx, result
mov cx, result_end-result
mov ah, 0x40
int 0x21
jc .fail
cmp ax, result_end-result
jne .fail
mov ah, 0x3e
int 0x21
jc .fail
mov ax, 0x4c00
int 0x21
.fail:
mov ax, 0x4c01
int 0x21
api: dd 0
result:
db 'VMID'
signature: dd 0
vm: dd 0
generation: dq 0
status: dw 1
negative_status: dw 0
negative_result: dd 0
start: dd 0
finish: dd 0
callback: dd 0
callback_count: dw 0
callback_errors: dw 0
wait_count: dw 0
wait_errors: dw 0
pair_count: dw 0
pair_errors: dw 0
notification_published: dq 0
notification_seen: dq 0
notification_irq: dw 0xffff
notification_errors: dw 0
result_end:
peer_identity: times 12 db 0
peer_tick: dw 0
role: db '@VM_ROLE@',0
before_sp: dw 0
callback_flags: dw 0
output: db '@VM_IDENTITY_RESULT@',0
times 128-($-output) db 0
pair_ready: db '@VM_PAIR_READY@',0
times 128-($-pair_ready) db 0
