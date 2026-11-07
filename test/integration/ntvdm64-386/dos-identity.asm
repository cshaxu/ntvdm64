%ifdef ORDERED_COMMAND
%define CUSTOM_COMMAND 1
%endif
%ifdef UNREAD_INPUT
%define CUSTOM_COMMAND 1
%endif
; Independently authored DOS witness; API results are written after an interval
; so the host can prove two actual VMs were simultaneously executing.
bits 16
org 0x100
%macro keyboard_ready 0
mov si, output
%%name:
cmp byte [si], 0
je %%ready
inc si
jmp %%name
%%ready:
mov word [si-3], 'RD'
mov byte [si-1], 'Y'
mov dx, output
xor cx, cx
mov ah, 0x3c
int 0x21
jc .fail
mov bx, ax
mov dx, vm
mov cx, 4
mov ah, 0x40
int 0x21
jc .fail
cmp ax,4
jne .fail
mov ah, 0x3e
int 0x21
jc .fail
mov word [si-3], 'BI'
mov byte [si-1], 'N'
%endmacro
push cs
pop ds
%ifdef CONTINUATION
db 0xc4,0xc4,0x54,254
%endif
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
%ifdef CONTINUATION
db 0xc4,0xc4,0x54,254
mov dx, test_directory
mov ah, 0x3b
int 0x21
db 0xc4,0xc4,0x54,254
jc .write
mov dl, [test_directory]
sub dl, 'A'
mov ah, 0x0e
int 0x21
mov ah, 0x19
int 0x21
mov [default_drive], al
mov ax, 9
call far [api]
jc .write
mov [command_callback], edx
%ifdef BLOCKED_KEYBOARD
cmp byte [role],'A'
jne .ordinary_keyboard
keyboard_ready
.ordinary_keyboard:
%endif
push ebp
push edi
push es
mov al, [role]
%ifdef REAL_COMMAND
%ifndef CUSTOM_COMMAND
sub al, 'A'
add al, '7'
mov [test_command+13], al
%endif
%else
mov [test_command+5], al
%endif
mov al, [role]
mov [test_environment+5], al
mov ax, cs
add ax, (test_environment-$$+0x100)/16
mov es, ax
%ifdef REAL_COMMAND
push dword -1
push dword -1
push dword -1
%else
push dword 0x33333333
push dword 0x22222222
push dword 0x11111111
%endif
mov [standard_offset], sp
%macro continuation_case 1-2 0
%ifdef CUSTOM_COMMAND
mov byte [test_environment+13], '0'+%2
%endif
%ifdef DIRECTORY_YIELD
; Diagnostic scheduling control: ordinary guest yielding, no deferred host
; completion. Retain the same directory assertion after other VMs can run.
xor ax, ax
int 0x1a
mov [directory_tick], dx
%%yield:
mov ax, 0x1680
int 0x2f
xor ax, ax
int 0x1a
sub dx, [directory_tick]
cmp dx, 18
jb %%yield
%endif
mov ah, 0x47
mov dl, [default_drive]
inc dl
mov si, direct_directory
int 0x21
jc %%error
push es
mov ah, 0x52
int 0x21
mov [directory_lol], bx
mov [directory_lol+2], es
mov eax, [es:bx+0x16]           ; original SysInitVars.SYSI_CDS
mov [directory_cds], eax
mov al, [es:bx+0x21]            ; original SysInitVars.SYSI_NCDS
mov [directory_cds_count], al
pop es
; Distinct real BIOS text per VM, with a nonzero cursor to prove restoration.
xor bx, bx
xor dx, dx
mov ah, 2
db 0xc4,0xc4,0x54,254
int 0x10
db 0xc4,0xc4,0x54,254
mov al, [role]
mov ah, 9
mov bx, 0x001e
mov cx, 1
db 0xc4,0xc4,0x54,254
int 0x10
db 0xc4,0xc4,0x54,254
mov dx, 0x0302
mov ah, 2
int 0x10
db 0xc4,0xc4,0x54,254
mov [before_sp], sp
mov eax, 0x54320000 | (%2 << 8) | %1
mov al, [default_drive]
mov ebx, 0x10203040
mov ecx, 0x50607080
mov cl, [role]
mov [expected_ecx], ecx
mov edx, [command_callback]
mov esi, 0x11220000
mov si, test_command
mov [expected_esi], esi
mov edi, 0x55660000
mov di, direct_directory
mov [expected_edi], edi
mov ebp, 0x99aa0000
mov bp, [standard_offset]
mov [expected_ebp], ebp
std
clc
pushf
pop word [before_flags]
db 0xc4,0xc4,0x54,%1
pushf
pop word [callback_flags]
cld
cmp sp, [before_sp]
jne %%error
%ifdef REAL_COMMAND
%if %1 == 10
cmp eax, 0x54320000
jne %%error
%else
cmp byte [role], 'A'
jne %%native_b
cmp eax, 0x54320025 | (%2 << 8)
jne %%error
jmp %%native_result
%%native_b:
cmp eax, 0x54320026 | (%2 << 8)
jne %%error
%%native_result:
%endif
%else
cmp eax, 0x54320025 | (%2 << 8)
jne %%error
%endif
cmp ebx, 0x10203040
jne %%error
cmp ecx, [expected_ecx]
jne %%error
cmp edx, [command_callback]
jne %%error
cmp esi, [expected_esi]
jne %%error
cmp edi, [expected_edi]
jne %%error
cmp ebp, [expected_ebp]
jne %%error
mov ax, [callback_flags]
xor ax, [before_flags]
test ax, 0x0ed5                 ; arithmetic flags, DF, IF and result CF
jnz %%error
%ifndef REAL_COMMAND
mov ah, 3
xor bx, bx
int 0x10
cmp dx, 0x0504
jne %%error
; Read back the independently applied host result in this actual guest VM.
mov ah, 2
xor bx, bx
xor dx, dx
int 0x10
mov ah, 8
int 0x10
cmp ah, 0x2f
jne %%error
mov bl, [role]
add bl, 'a'-'A'
cmp al, bl
jne %%error
%endif
%ifdef VIDEO_APPLICATION
mov word [video_failure], 1
mov ax, 0x1015
mov bx, 7
int 0x10
cmp ch, 0
jne %%error
cmp byte [role], 'A'
jne %%blue
cmp dh, 63
jne %%error
cmp cl, 0
jne %%error
jmp %%palette_done
%%blue:
cmp dh, 0
jne %%error
cmp cl, 63
jne %%error
%%palette_done:
%ifdef VIDEO_FONT_CONTROL
; Independent ordinary DOS BIOS control, not evidence of driver font loading.
call load_control_fonts
%endif
mov word [video_failure], 2
call check_video_fonts
jc %%error
mov word [video_failure], 0
%endif
%ifdef UNREAD_INPUT
%if %1 == 8
; Actual BIOS consumer, after the production native command has returned.
; Repeated peek must preserve each word. Twenty words exceed the BIOS ring.
%ifidn UNREAD_MODE,sequence
mov cx,20
%elifidn UNREAD_MODE,repeat
mov cx,20
%else
mov cx,1
%endif
xor si,si
%%returned:
mov [unread_index],si
mov word [unread_stage],1
%ifidn UNREAD_MODE,function
mov ah,0x01
%else
mov ah,0x11
%endif
int 0x16
mov [unread_actual],ax
jz %%error
mov word [unread_stage],2
mov bx,ax
%ifidn UNREAD_MODE,function
mov ah,0x01
%else
mov ah,0x11
%endif
int 0x16
jz %%error
cmp ax,bx
jne %%error
mov word [unread_stage],3
%ifidn UNREAD_MODE,function
mov ah,0x00
%else
mov ah,0x10
%endif
int 0x16
mov [unread_actual],ax
cmp ax,bx
jne %%error
%ifidn UNREAD_MODE,sequence
mov dx,0x1e61
test si,1
jz %%role
mov dx,0x3062
%%role:
cmp byte [role],'A'
je %%compare
mov dx,0x2e63
test si,1
jz %%compare
mov dx,0x2064
%%compare:
%elifidn UNREAD_MODE,repeat
mov dx,0x2c7a
%elifidn UNREAD_MODE,function
mov dx,0x3b00
%elifidn UNREAD_MODE,extended
mov dx,0x8500
%elifidn UNREAD_MODE,oem
mov dx,0x1282                    ; U+00E9 -> CP437 82h, original physical scan
cmp byte [role],'A'
je %%oem_ready
mov dx,0x1e84                    ; U+00E4 -> CP437 84h in the other VM
%%oem_ready:
%endif
mov word [unread_stage],4
mov [unread_expected],dx
cmp ax,dx
jne %%error
inc si
loop %%returned
mov word [unread_stage],5
%ifidn UNREAD_MODE,function
mov ah,0x01
%else
mov ah,0x11
%endif
int 0x16
jnz %%error
mov word [unread_stage],0
%endif
%endif
inc word [continuation_count]
jmp %%done
%%error:
inc word [continuation_errors]
%%done:
%endmacro
continuation_case 8
continuation_case 8, 1
continuation_case 10
%ifdef NESTED_COMMAND
; Retain actual guest text after the nested command. Merely queueing MEM
; and observing EXIT is not proof that MEM executed or its output returned.
push ds
mov dx, nested_output
xor cx, cx
mov ah, 0x3c
int 0x21
jc .nested_failed
mov bx, ax
mov ax, 0xb800
mov ds, ax
xor dx, dx
mov cx, 4000
mov ah, 0x40
int 0x21
pushf
push ax
mov ah, 0x3e
int 0x21
pop ax
popf
jc .nested_failed
cmp ax, 4000
jne .nested_failed
pop ds
jmp .nested_done
.nested_failed:
pop ds
inc word [continuation_errors]
.nested_done:
%endif
add sp, 12
pop es
pop edi
pop ebp
%endif
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
%ifdef KEYBOARD
keyboard_ready
%ifdef CONSUME_KEYBOARD
cmp byte [role], 'A'
jne .read_keyboard
mov ah, 1
int 0x16
jnz .fail                         ; consumed scan must not also reach BIOS
mov word [keyboard_result], 0
jmp .keyboard_done
.read_keyboard:
%endif
mov ah, 0
int 0x16
mov [keyboard_result], ax
%ifdef CONSUME_KEYBOARD
.keyboard_done:
%endif
%endif
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
%ifdef CONTINUATION
continuation_count: dw 0
continuation_errors: dw 0
%endif
%ifdef VIDEO_APPLICATION
video_failure: dw 0
video_sample0: dw 0
video_sample1: dw 0
video_height: dw 0
video_offset: dw 0
%endif
%ifdef KEYBOARD
keyboard_result: dw 0
%endif
%ifdef UNREAD_INPUT
unread_stage: dw 0
unread_index: dw 0
unread_actual: dw 0
unread_expected: dw 0
%endif
result_end:
%ifdef CONTINUATION
command_callback: dd 0
default_drive: db 0
before_flags: dw 0
standard_offset: dw 0
expected_ecx: dd 0
expected_esi: dd 0
expected_ebp: dd 0
expected_edi: dd 0
direct_directory: times 64 db 0
directory_lol: dd 0
directory_cds: dd 0
directory_cds_count: dd 0
directory_tick: dw 0
%ifdef VIDEO_APPLICATION
%ifdef VIDEO_FONT_CONTROL
load_control_fonts:
push ax
push bx
push cx
push dx
push di
push bp
push es
push ds
pop es
mov di, control_font
mov cx, 8192
mov al, 0x3c
cmp byte [role], 'A'
je .fill
mov al, 0x66
.fill:
cld
rep stosb
mov ax, 0x1130
xor bx, bx
int 0x10
mov [video_height], cx
push ds
pop es
mov bp, control_font
mov bh, cl
xor bl, bl
mov cx, 256
xor dx, dx
mov ax, 0x1100
int 0x10
push ds
pop es
mov bp, control_font
mov bh, [video_height]
mov bl, 1
mov cx, 256
xor dx, dx
mov ax, 0x1100
int 0x10
pop es
pop bp
pop di
pop dx
pop cx
pop bx
pop ax
ret
control_font: times 8192 db 0
%endif
; Read both actual VGA font banks through this VM's virtual plane mapping.
; Preserve every touched sequencer/graphics register and index latch, including
; the failure path. This is a guest device observation, not a host call trace.
check_video_fonts:
push ax
push bx
push cx
push dx
push si
push di
push bp
push es
mov ax, 0x1130
xor bx, bx
int 0x10
%ifndef VIDEO_FONT_CONTROL
; A control load owns its requested row count. The BIOS can change its
; reported character height while restoring mode registers after that load.
mov [video_height], cx
%endif
mov dx, 0x3c4
in al, dx
mov [video_seq_index], al
mov al, 2
call .read_index
mov [video_regs], al
mov al, 4
call .read_index
mov [video_regs+1], al
mov dx, 0x3ce
in al, dx
mov [video_gc_index], al
mov al, 4
call .read_index
mov [video_regs+2], al
mov al, 5
call .read_index
mov [video_regs+3], al
mov al, 6
call .read_index
mov [video_regs+4], al
mov dx, 0x3c4
mov ax, 0x0402
out dx, ax
mov ax, 0x0704
out dx, ax
mov dx, 0x3ce
mov ax, 0x0204
out dx, ax
mov ax, 0x0005
out dx, ax
mov ax, 0x0406
out dx, ax
mov ax, 0xa000
mov es, ax
mov bl, 0x3c
cmp byte [role], 'A'
je .expected
mov bl, 0x66
.expected:
%ifdef VIDEO_FONT_DIRECT_CONTROL
; Diagnostic control: actual guest MOV stores under the same virtual VGA
; plane selection, rather than the host BIOS's SAS write helper. This is not
; acceptance of the driver's earlier font application.
xor si, si
mov cx, 256
.write_glyph:
xor bp, bp
.write_row:
mov [es:si+bp], bl
mov [es:si+bp+0x4000], bl
inc bp
cmp bp, [video_height]
jb .write_row
add si, 32
loop .write_glyph
%endif
xor di, di
xor si, si
mov cx, 256
.glyph:
xor bp, bp
.row:
mov ax, si
add ax, bp
mov [video_offset], ax
mov al, [es:si+bp]
mov [video_sample0], al
mov al, [es:si+bp+0x4000]
mov [video_sample1], al
cmp [es:si+bp], bl
jne .mismatch
cmp [es:si+bp+0x4000], bl
jne .mismatch
inc bp
cmp bp, [video_height]
jb .row
add si, 32
loop .glyph
jmp .restore
.mismatch:
mov di, 1
.restore:
mov dx, 0x3ce
mov al, 6
mov ah, [video_regs+4]
out dx, ax
mov al, 5
mov ah, [video_regs+3]
out dx, ax
mov al, 4
mov ah, [video_regs+2]
out dx, ax
mov al, [video_gc_index]
out dx, al
mov dx, 0x3c4
mov al, 4
mov ah, [video_regs+1]
out dx, ax
mov al, 2
mov ah, [video_regs]
out dx, ax
mov al, [video_seq_index]
out dx, al
test di, di
jz .ok
stc
jmp .done
.ok:
clc
.done:
pop es
pop bp
pop di
pop si
pop dx
pop cx
pop bx
pop ax
ret
.read_index:
out dx, al
inc dx
in al, dx
dec dx
ret
video_regs: times 5 db 0
video_seq_index: db 0
video_gc_index: db 0
%endif
%ifdef REAL_COMMAND
%ifdef CUSTOM_COMMAND
test_command: db '@VM_ORDER_COMMAND@',0
times 256-($-test_command) db 0
%else
test_command: db 'cmd /c exit 37',13
%endif
%else
test_command: db 'echo X',13
%endif
align 16, db 0
%ifdef REAL_COMMAND
test_environment: db '@VM_ENVIRONMENT@',0
times 512-($-test_environment) db 0
%else
test_environment: db 'ROLE=X',0,0
%endif
test_directory: db '@VM_DIRECTORY@',0
times 128-($-test_directory) db 0
%endif
peer_identity: times 12 db 0
peer_tick: dw 0
role: db '@VM_ROLE@',0
before_sp: dw 0
callback_flags: dw 0
output: db '@VM_IDENTITY_RESULT@',0
times 128-($-output) db 0
pair_ready: db '@VM_PAIR_READY@',0
times 128-($-pair_ready) db 0
%ifdef NESTED_COMMAND
nested_output: db '@VM_NESTED_OUTPUT@',0
times 128-($-nested_output) db 0
%endif
