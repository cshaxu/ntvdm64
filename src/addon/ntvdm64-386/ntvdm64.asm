; Project-authored guest VM bridge, S4 candidate. Not released.
; ABI provenance and incomplete contracts are recorded in README.md.
bits 32

%macro vmm_call 1
    int 0x20
    dd 0x00010000 + %1
%endmacro

%macro host_notify 1
    mov eax, 0x4e544d56
    mov ecx, %1
    db 0xc4, 0xc4, 0xb1             ; existing CCPU BOP B1, not a CPU fault
%endmacro

%macro vpicd_call 1
    int 0x20
    dd 0x00030000 + %1
%endmacro

; VMM client register frame, not the VxD's own register values.
client_ebx equ 16
client_edx equ 20
client_ecx equ 24
client_eax equ 28
client_esi equ 4
client_flags equ 44
block_svc_ints equ 1
block_enable_ints equ 4

; Each VMM control block owns this data until Destroy_VM.
vm_generation_low equ 0
vm_generation_high equ 4
vm_executable equ 8
vm_semaphore equ 12
vm_waiting equ 16
vm_io_buffer equ 20
vm_codepage equ 24
vm_token_low equ 28
vm_token_high equ 32
vm_bytes equ 36
screen_header equ 36

segment _LTEXT public align=16 class=CODE use32
global control, api
control:
    pushad
    cmp eax, 0                      ; Sys_Critical_Init
    je .allocate
    cmp eax, 1                      ; Device_Init: the initial system VM
    je .create
    cmp eax, 7                      ; Create_VM
    je .create
    cmp eax, 0x0b                   ; VM_Not_Executeable precedes destruction
    je .retire
    cmp eax, 0x0c                   ; Destroy_VM: no client can resume now
    je .destroy
    cmp eax, 5                      ; System_Exit, including abnormal exit
    je .shutdown
.ok:
    popad
    clc
    ret
.fail:
    popad
    stc
    ret
.allocate:
    push dword 0                    ; reserved flags
    push dword vm_bytes
    vmm_call 0xa7                   ; _Allocate_Device_CB_Area(bytes, flags)
    add esp, 8
    test eax, eax
    jz .fail
    mov [vm_offset], eax
    mov esi, v86_call
    xor edx, edx
    vmm_call 0x0b                   ; Allocate_V86_Call_Back
    test eax, eax
    jz .fail
    mov [v86_callback], eax
    mov esi, command_return
    xor edx, edx
    vmm_call 0x0b
    test eax, eax
    jz .fail
    mov [command_callback], eax
    jmp .ok
.create:
    mov edi, [vm_offset]
    test edi, edi
    jz .fail
    add edi, ebx
    cmp dword [edi+vm_executable], 0
    jne .ok
    xor ecx, ecx
    vmm_call 0x25                   ; Create_Semaphore: ECX initial count
    jc .fail
    mov [edi+vm_semaphore], eax
    ; Never wrap into a previously issued generation.
    mov eax, [generation_low]
    mov edx, [generation_high]
    add eax, 1
    adc edx, 0
    jc .generation_exhausted
    mov [generation_low], eax
    mov [generation_high], edx
    mov [edi+vm_generation_low], eax
    mov [edi+vm_generation_high], edx
    mov dword [edi+vm_executable], 1
    mov dword [edi+vm_token_low], 0
    mov dword [edi+vm_token_high], 0
    cmp dword [esp+28], 1           ; saved Device_Init, only the system VM
    je .notification_init
    jmp .register_vm
.notification_init:
    push dword 0x304               ; paragraph aligned, instanced, zeroed
    push dword 8192                ; instanced DOS directory / BIOS font transfer
    vmm_call 0xa8                   ; _Allocate_Global_V86_Data_Area
    add esp, 8
    test eax, eax
    jz .initialization_failed
    mov [directory_buffer], eax
    host_notify 0                  ; original host IRQ reservation
    test eax, eax
    jnz .initialization_failed
    mov [irq_descriptor], bx
    mov edi, irq_descriptor
    vpicd_call 1                    ; VPICD_Virtualize_IRQ
    jc .release_notification
    mov [irq_handle], eax
    vpicd_call 9                    ; VPICD_Physically_Unmask
    host_notify 1                  ; arm only after guest ownership exists
    test eax, eax
    jnz .shutdown_failed
    mov edx, [command_callback]
    xor esi, esi
    host_notify 15                 ; admit only our allocated V86 FAR callback
    test eax, eax
    jnz .shutdown_failed
.register_vm:
    mov ebx, [esp+16]
    mov edi, [vm_offset]
    add edi, ebx
    mov edx, [edi+vm_generation_low]
    mov esi, [edi+vm_generation_high]
    host_notify 7
    test eax, eax
    jz .ok
    cmp dword [esp+28], 1
    je .shutdown_failed
    jmp .initialization_failed
.release_notification:
    host_notify 5
    jmp .initialization_failed
.shutdown_failed:
    call notification_close
.initialization_failed:
    ; Device_Init has already created the system VM semaphore. A failed IRQ
    ; acquisition must release it before VMM discards this driver's entry.
    mov edi, [vm_offset]
    add edi, [esp+16]               ; saved original EBX, not returned IRQ
    mov dword [edi+vm_executable], 0
    xor eax, eax
    xchg eax, [edi+vm_semaphore]
    test eax, eax
    jz .fail
    vmm_call 0x26
    jmp .fail
.shutdown:
    call notification_close
    jmp .ok
.generation_exhausted:
    mov eax, [edi+vm_semaphore]
    mov dword [edi+vm_semaphore], 0
    vmm_call 0x26                   ; Destroy_Semaphore
    jmp .fail
.retire:
    mov edi, [vm_offset]
    test edi, edi
    jz .ok
    mov dword [ebx+edi+vm_executable], 0
    mov edx, [ebx+edi+vm_generation_low]
    mov esi, [ebx+edi+vm_generation_high]
    host_notify 8                  ; invalidate queued old-generation wakes
    jmp .ok
.destroy:
    call command_buffer_free       ; a destroyed VM cannot resume its callback
    mov edi, [vm_offset]
    test edi, edi
    jz .ok
    add edi, ebx
    xor eax, eax
    xchg eax, [edi+vm_semaphore]
    test eax, eax
    jz .ok
    vmm_call 0x26
    jmp .ok

api:
    ; No READY/command/wait capability is advertised yet.
    pushad
    mov edi, [vm_offset]
    test edi, edi
    jz .unsupported
    add edi, ebx
    cmp dword [edi+vm_executable], 1
    jne .unsupported
    cmp word [ebp+client_eax], 1
    je .callback_address
    cmp word [ebp+client_eax], 2
    je .wait
    cmp word [ebp+client_eax], 3
    je .signal
    cmp word [ebp+client_eax], 8
    je .signal
    cmp word [ebp+client_eax], 4
    je .inventory_count
    cmp word [ebp+client_eax], 5
    je .notification_status
    cmp word [ebp+client_eax], 6
    je .notification_publish
    cmp word [ebp+client_eax], 9
    je .command_address
    cmp word [ebp+client_eax], 0
    jne .unsupported
    mov dword [ebp+client_eax], 0x4e544d56
    mov [ebp+client_ebx], ebx         ; actual VMM-supplied VM handle
    mov eax, [edi+vm_generation_low]
    mov [ebp+client_edx], eax
    mov eax, [edi+vm_generation_high]
    mov [ebp+client_esi], eax
    mov dword [ebp+client_ecx], 1
    and dword [ebp+client_flags], ~1
    popad
    ret
.command_address:
    mov eax, [command_callback]
    mov [ebp+client_edx], eax
    jmp .success
.wait:
    ; One outstanding wait per VM. No guest pointer survives scheduling.
    cmp dword [edi+vm_waiting], 0
    jne .unsupported
    mov dword [edi+vm_waiting], 1
    mov eax, [edi+vm_semaphore]
    xor ecx, ecx                   ; no virtual interrupt servicing / polling
    vmm_call 0x27                   ; Wait_Semaphore consumes one count
    mov edi, [vm_offset]
    add edi, ebx
    mov dword [edi+vm_waiting], 0
    cmp dword [edi+vm_executable], 1
    jne .unsupported
    jmp .success
.inventory_count:
    mov esi, ebx
    xor ecx, ecx
.count_vm:
    mov edi, [vm_offset]
    add edi, ebx
    cmp dword [edi+vm_executable], 1
    jne .count_next
    inc ecx
.count_next:
    vmm_call 0x3b
    cmp ebx, esi
    jne .count_vm
    mov [ebp+client_eax], ecx
    and dword [ebp+client_flags], ~1
    popad
    ret
.signal:
    ; Validate a supplied handle by walking VMM's live inventory before
    ; dereferencing it. Generation prevents reuse from receiving old work.
    mov esi, ebx
.find_vm:
    cmp ebx, [ebp+client_ebx]
    je .found_vm
    vmm_call 0x3b                   ; Get_Next_VM_Handle
    cmp ebx, esi
    jne .find_vm
    jmp .unsupported
.found_vm:
    mov edi, [vm_offset]
    add edi, ebx
    cmp dword [edi+vm_executable], 1
    jne .unsupported
    mov eax, [ebp+client_edx]
    cmp eax, [edi+vm_generation_low]
    jne .unsupported
    mov eax, [ebp+client_esi]
    cmp eax, [edi+vm_generation_high]
    jne .unsupported
    mov eax, [ebx]                  ; original VMM CB_VM_Status, offset zero
    and eax, 0x400                  ; VMStat_Blocked: actually on a semaphore
    or eax, [edi+vm_waiting]
    mov [ebp+client_ecx], eax        ; VMM + driver state before this signal
    cmp word [ebp+client_eax], 8
    je .queue_signal
    mov eax, [edi+vm_semaphore]
    vmm_call 0x28                   ; Signal_Semaphore, including precompletion
    jmp .success
.queue_signal:
    mov edx, [ebp+client_edx]
    mov esi, [ebp+client_esi]
    host_notify 9                  ; actual host queue -> PIC -> VMM wake
    test eax, eax
    jnz .unsupported
    jmp .success
.callback_address:
    mov eax, [v86_callback]
    test eax, eax
    jz .unsupported
    mov [ebp+client_edx], eax
    jmp .success
.notification_status:
    cmp dword [irq_error], 0
    jne .unsupported
    movzx eax, word [irq_descriptor]
    mov [ebp+client_ebx], eax
    mov eax, [irq_serial_low]
    mov [ebp+client_edx], eax
    mov eax, [irq_serial_high]
    mov [ebp+client_esi], eax
    mov eax, [irq_count]
    mov [ebp+client_ecx], eax
    jmp .success
.notification_publish:
    ; Isolated S4 notification witness; no task completion is asserted.
    host_notify 2
    test eax, eax
    jnz .unsupported
    mov [ebp+client_edx], edx
    mov [ebp+client_esi], esi
.success:
    mov dword [ebp+client_eax], 0
    and dword [ebp+client_flags], ~1
    popad
    ret
.unsupported:
    mov dword [ebp+client_eax], 1
    or dword [ebp+client_flags], 1
    popad
    ret

v86_call:
    ; This entry is invoked by an actual guest FAR CALL, which supplies its
    ; return frame. It must NOT be used for a post-BOP redirected continuation.
    call api
    vmm_call 0x4a                   ; Simulate_Far_Ret consumes that frame
    ret

; Entered through the original CCPU CALLF implementation, with a real return
; frame and untouched argument registers. Request creation happens only here,
; after VMM supplies the actual VM. No unbound host token enters V86 code.
command_return:
    pushad
    vmm_call 0x4a                   ; restore original continuation and SS:SP
    mov ebx, [esp+16]
    mov edi, [vm_offset]
    add edi, ebx
    cmp dword [edi+vm_executable], 1
    jne .unavailable
    cmp dword [edi+vm_waiting], 0
    jne .unavailable
    mov dword [edi+vm_waiting], 1
    sub esp, 64                    ; directory snapshot owned by this callback
    mov edx, esp
    call command_directory
    test eax, eax
    jnz .snapshot_failed
    call command_screen
    test eax, eax
    jnz .snapshot_failed
    push esi                       ; copied text snapshot extent
    push edx                       ; temporary VMM heap, released after BOP
    lea eax, [esp+8]
    push eax                       ; copied directory, not an instanced alias
    push dword [ebx+4]              ; documented CB_High_Linear
    push ebp                       ; restored original VMM client frame
    mov edx, esp                    ; synchronous descriptor, never retained
    mov esi, 20
    host_notify 11
    pushad
    mov ebx, [esp+32+20+64+16]
    call command_buffer_free
    popad
    add esp, 20+64
    test eax, eax
    jnz .begin_failed
    ; The per-VM control block is the sole live token owner. IRQ/input
    ; callbacks cannot borrow this suspended command's stack.
    mov edi, [vm_offset]
    add edi, [esp+16]
    mov [edi+vm_token_low], ebx
    mov [edi+vm_token_high], edx
.wait_work:
    mov ebx, [esp+16]              ; original VMM handle in saved PUSHAD
    mov edi, [vm_offset]
    add edi, ebx
    mov eax, [edi+vm_semaphore]
    ; Keep the VM's virtual IRQ handlers runnable while its command callback
    ; waits. The callback itself resumes only on the counted semaphore wake.
    mov ecx, block_svc_ints | block_enable_ints
    vmm_call 0x27
    ; The same counted wake carries either guest I/O or terminal completion.
    ; Apply I/O here, in this VM's resumed callback, never in the IRQ handler.
    mov ebx, [esp+16]
    mov edi, [vm_offset]
    add edi, ebx
    mov edx, [edi+vm_token_low]
    mov esi, [edi+vm_token_high]
    call command_apply_work
    test eax, eax
    jnz .error
    test ecx, ecx                  ; consume the separate terminal wake too
    jnz .wait_work
    mov ebx, [esp+16]
    mov edi, [vm_offset]
    add edi, ebx
    mov edx, [edi+vm_token_low]
    mov esi, [edi+vm_token_high]
    host_notify 12
    cmp eax, 996                    ; ERROR_IO_INCOMPLETE: wait for next event
    je .wait_work
    test eax, eax
    jnz .error
    test edx, edx                  ; original command DOS-reentry CF result
    jnz .reentry
    mov [ebp+client_eax], bl        ; only AL is the terminal exit code
    and dword [ebp+client_flags], ~1
    jmp .done
.reentry:
    or dword [ebp+client_flags], 1
    jmp .done
.error:
    mov [ebp+client_eax], al
    and dword [ebp+client_flags], ~1
.done:
    mov edi, [vm_offset]
    add edi, [esp+16]
    mov dword [edi+vm_token_low], 0
    mov dword [edi+vm_token_high], 0
    mov dword [edi+vm_waiting], 0
    popad
    ret                           ; FAR frame was consumed before admission
.snapshot_failed:
    add esp, 64
.begin_failed:
    mov edi, [vm_offset]
    add edi, [esp+16]
    mov dword [edi+vm_waiting], 0
.return_error:
    mov [ebp+client_eax], al
    and dword [ebp+client_flags], ~1
    popad
    ret
.unavailable:
    mov eax, 5023                  ; ERROR_INVALID_STATE; trusted return frame
    jmp .return_error

; EDX:ESI is the continuation token, EBX is the actual VM. Query and copy one
; owned host job into VMM heap; acknowledge only after the BIOS application.
command_buffer_free:
    pushad
    mov edi, [vm_offset]
    test edi, edi
    jz .done
    add edi, ebx
    xor eax, eax
    xchg eax, [edi+vm_io_buffer]
    test eax, eax
    jz .done
    push dword 0
    push eax
    vmm_call 0x51
    add esp, 8
.done:
    popad
    ret

command_apply_work:
    pushad
    sub esp, 28
    mov dword [esp+28+24], 0
    mov [esp], edx
    mov [esp+4], esi
    xor eax, eax
    mov [esp+8], eax
    mov [esp+12], eax
    mov [esp+16], eax
    mov [esp+20], eax
    mov [esp+24], eax
    mov edx, esp
    mov esi, 28
    host_notify 13
    cmp eax, 1168                   ; no pending I/O, check terminal result
    je .success
    cmp eax, 122                    ; query returns required extent
    jne .finish
    mov eax, [esp+20]
    cmp eax, screen_header
    jb .invalid
    cmp eax, 1048576
    ja .invalid
    push dword 0
    push eax
    vmm_call 0x4f
    add esp, 8
    test eax, eax
    jz .no_memory
    mov [esp+16], eax
    mov ebx, [esp+28+16]
    mov edi, [vm_offset]
    mov [ebx+edi+vm_io_buffer], eax
    mov edx, esp
    mov esi, 28
    host_notify 13
    test eax, eax
    jnz .acknowledge
    mov ebx, [esp+28+16]
    mov edx, [esp+16]
    mov esi, [esp+20]
    call command_screen_apply
    jmp .acknowledge
.no_memory:
    mov eax, 8
    jmp .acknowledge
.invalid:
    mov eax, 13
.acknowledge:
    mov [esp+24], eax
    mov ebx, [esp+28+16]
    call command_buffer_free
    mov edx, esp
    mov esi, 28
    host_notify 14                  ; producer receives actual application error
    mov dword [esp+28+24], 1
    jmp .finish
.success:
    xor eax, eax
.finish:
    mov [esp+28+28], eax
    add esp, 28
    popad
    ret

; Apply an owned text snapshot through the target VM's virtual BIOS. Existing
; geometry/mode must match; mode changes and graphics belong to later provider
; operations, never an implicit mode switch during a text publication.
command_screen_apply:
    pushad
    sub esp, 120                    ; full client state, buffer and cell index
    mov [esp+108], edx
    mov edi, edx
    mov eax, [edi]
    test eax, eax
    jz .invalid
    cmp eax, 255
    ja .invalid
    mov ecx, [edi+4]
    test ecx, ecx
    jz .invalid
    cmp ecx, 256
    ja .invalid
    cmp [edi+8], eax
    jae .invalid
    cmp [edi+12], ecx
    jae .invalid
    imul eax, ecx
    lea eax, [eax*2+screen_header]
    mov dword [esp+116], 0          ; optional VIDEO trailer
    cmp eax, esi
    je .extent_valid
    ja .invalid
    sub esi, eax
    cmp esi, 1056+16420
    jb .invalid
    lea edx, [edi+eax]
    cmp dword [edx], 0x31564d56
    jne .invalid
    cmp dword [edx+4], 1
    jne .invalid
    cmp dword [edx+1052], 1         ; TEXT_FRAME, never an implicit DIB mode
    jne .unsupported_before
    cmp dword [edx+20], 0
    jne .invalid
    mov eax, [edi]
    cmp [edx+8], eax
    jne .unsupported_before
    mov ecx, [edi+4]
    cmp [edx+12], ecx
    jne .unsupported_before
    lea eax, [eax*2]
    cmp [edx+16], eax
    je .stride_valid
    add eax, [edi]
    cmp [edx+16], eax
    jne .invalid
.stride_valid:
    imul eax, ecx
    add eax, 16420
    cmp [edx+24], eax
    jne .invalid
    add eax, 1056
    cmp eax, esi
    jne .invalid
    mov eax, [edx+1056]
    test eax, eax
    jz .invalid
    cmp eax, 32
    ja .invalid
    cmp eax, [edi+32]
    jne .unsupported_before
    cmp dword [edx+1060], 1
    ja .invalid
    mov eax, [edx+1064]
    cmp eax, [edi]
    jae .unsupported_before
    mov eax, [edx+1068]
    cmp eax, [edi+4]
    jae .unsupported_before
    cmp dword [edx+1084], 0         ; second cursor segment needs explicit mapping
    jne .unsupported_before
    cmp dword [edx+1088], 1
    ja .invalid
    mov eax, [edx+1072]
    cmp eax, 31
    ja .unsupported_before
    mov ecx, [edx+1076]
    cmp ecx, 32
    ja .invalid
    add eax, ecx
    cmp eax, 32
    ja .unsupported_before
    xor ecx, ecx
.palette_validate:
    test dword [edx+28+ecx*4], 0xff000000
    jnz .invalid
    inc ecx
    cmp ecx, 16
    jb .palette_validate
    mov eax, [edi]
    lea eax, [eax+eax*2]
    cmp [edx+16], eax
    jne .video_valid
    mov ecx, [edi]
    imul ecx, [edi+4]
    lea esi, [edx+1056+16420+2]
.style_validate:
    cmp byte [esi], 0              ; do not discard per-cell underline
    jne .unsupported_before
    add esi, 3
    loop .style_validate
.video_valid:
    mov [esp+116], edx
.extent_valid:
    cmp dword [edi+24], 7
    ja .invalid
    mov edi, esp
    vmm_call 0x8d
    vmm_call 0x82
    mov word [ebp+client_eax], 0x0f00
    call .int10
    mov edi, [esp+108]
    movzx eax, byte [ebp+client_eax]
    and eax, 0x7f
    cmp eax, [edi+20]
    jne .unsupported
    cmp eax, 3
    jbe .mode_ok
    cmp eax, 7
    jne .unsupported
.mode_ok:
    movzx eax, byte [ebp+client_eax+1]
    cmp eax, [edi]
    jne .unsupported
    mov word [ebp+client_eax], 0x1130
    mov word [ebp+client_ebx], 0
    call .int10
    mov edi, [esp+108]
    movzx eax, byte [ebp+client_edx]
    inc eax
    cmp eax, [edi+4]
    jne .unsupported
    movzx eax, word [ebp+client_ecx]
    cmp eax, [edi+32]
    jne .unsupported
    mov edx, [esp+116]
    test edx, edx
    jz .video_applied
    call command_video_apply
    test eax, eax
    jnz .restore
.video_applied:
    mov dword [esp+112], 0
.cell:
    mov edi, [esp+108]
    mov eax, [esp+112]
    xor edx, edx
    div dword [edi]
    shl eax, 8
    or eax, edx
    mov [ebp+client_edx], ax
    mov eax, [edi+24]
    mov [ebp+client_ebx+1], al
    mov word [ebp+client_eax], 0x0200
    call .int10
    mov edi, [esp+108]
    mov ecx, [esp+112]
    mov ax, [edi+ecx*2+screen_header]
    mov [ebp+client_ebx], ah
    mov ah, 9
    mov [ebp+client_eax], ax
    mov eax, [edi+24]
    mov [ebp+client_ebx+1], al
    mov word [ebp+client_ecx], 1
    call .int10
    inc dword [esp+112]
    mov edi, [esp+108]
    mov eax, [edi]
    imul eax, [edi+4]
    cmp [esp+112], eax
    jb .cell
    mov eax, [edi+12]
    shl eax, 8
    or eax, [edi+8]
    mov [ebp+client_edx], ax
    mov eax, [edi+24]
    mov [ebp+client_ebx+1], al
    mov word [ebp+client_eax], 0x0200
    call .int10
    mov edi, [esp+108]
    mov eax, [edi+16]
    mov [ebp+client_ecx], ax
    mov word [ebp+client_eax], 0x0100
    call .int10
    xor eax, eax
    jmp .restore
.unsupported:
    mov eax, 50
.restore:
    mov [esp+120+28], eax
    vmm_call 0x86
    mov esi, esp
    vmm_call 0x8e
    jmp .return
.unsupported_before:
    mov dword [esp+120+28], 50
    jmp .return
.invalid:
    mov dword [esp+120+28], 13
.return:
    add esp, 120
    popad
    ret
.int10:
    mov eax, 0x10
    vmm_call 0x84
    ret

; Called only after complete packet and current VM geometry validation, inside
; the saved nested-V86 context. EDX=owned trailer, EDI=owned BIOS snapshot.
; ES:BP uses instanced GVDA memory, never a protected pointer passed to BIOS.
command_video_apply:
    pushad
    sub esp, 16
    mov [esp], edx
    mov [esp+4], edi
    mov dword [esp+8], 0
.font:
    mov eax, [directory_buffer]
    shr eax, 4
    mov [ebp+56], ax                ; Client_ES
    mov word [ebp+8], 0             ; Client_BP
    mov eax, 0x3808                 ; Map_Flat Client_ES:Client_EBP
    vmm_call 0x1c
    cmp eax, -1
    je .mapping_error
    mov edi, eax
    mov esi, [esp]
    mov edx, [esi+1056]             ; packed BIOS bytes per character
    add esi, 1092                   ; VIDEO style.fonts, 32 bytes per glyph
    mov eax, [esp+8]
    shl eax, 13
    add esi, eax
    mov dword [esp+12], 256
.glyph:
    mov ecx, edx
    cld
    rep movsb
    mov eax, 32
    sub eax, edx
    add esi, eax
    dec dword [esp+12]
    jnz .glyph
    mov esi, [esp]
    mov eax, [esi+1056]
    shl eax, 8
    or eax, [esp+8]
    mov [ebp+client_ebx], ax         ; BH=height, BL=font bank
    mov word [ebp+client_ecx], 256
    mov word [ebp+client_edx], 0
    mov word [ebp+client_eax], 0x1100 ; load only, preserve text geometry
    call .int10
    inc dword [esp+8]
    cmp dword [esp+8], 2
    jb .font
    mov esi, [esp]
    mov eax, [esi+1060]
    shl eax, 2                      ; font B=0, font A=1 if attribute selects
    mov [ebp+client_ebx], ax
    mov word [ebp+client_eax], 0x1103
    call .int10
    mov word [ebp+client_ebx], 0     ; native high background bit is intensity
    mov word [ebp+client_eax], 0x1003
    call .int10
    mov dword [esp+8], 0
.palette:
    mov eax, [esp+8]
    mov ah, al
    mov [ebp+client_ebx], ax         ; attribute slot -> same DAC slot
    mov word [ebp+client_eax], 0x1000
    call .int10
    mov esi, [esp]
    mov ecx, [esp+8]
    mov eax, [esi+28+ecx*4]          ; 00RRGGBB -> VGA six-bit components
    mov [ebp+client_ebx], cx
    shr al, 2
    mov [ebp+client_ecx], al
    shr eax, 8
    shr al, 2
    mov [ebp+client_ecx+1], al
    shr eax, 8
    shr al, 2
    mov [ebp+client_edx+1], al
    mov word [ebp+client_eax], 0x1010
    call .int10
    inc dword [esp+8]
    cmp dword [esp+8], 16
    jb .palette
    mov esi, [esp]
    mov edi, [esp+4]
    mov eax, [esi+1064]
    mov [edi+8], eax
    mov eax, [esi+1068]
    mov [edi+12], eax
    mov eax, [esi+1072]
    mov ecx, eax
    shl eax, 8
    add ecx, [esi+1076]
    dec ecx
    or eax, ecx
    cmp dword [esi+1088], 0
    je .hidden
    cmp dword [esi+1076], 0
    jne .cursor
.hidden:
    mov eax, 0x2000
.cursor:
    mov [edi+16], eax
    mov ecx, [edi]
    imul ecx, [edi+4]
    mov eax, [esi+16]
    xor edx, edx
    div dword [edi]
    mov edx, eax                    ; two or three bytes per VIDEO cell
    add edi, screen_header
    add esi, 1056+16420
.cells:
    mov ax, [esi]
    mov [edi], ax
    add esi, edx
    add edi, 2
    loop .cells
    xor eax, eax
    jmp .done
.mapping_error:
    mov eax, 487
.done:
    mov [esp+16+28], eax
    add esp, 16
    popad
    ret
.int10:
    mov eax, 0x10
    vmm_call 0x84
    ret

; Snapshot through this VM's virtual BIOS, never a host/global B800 alias.
; Header: nine dwords (columns, rows, cursor x/y, shape, mode, page, code page, font height), then
; row-major OEM character/attribute words. No guest pointer survives the BOP.
; Returns EAX=status, EDX=owned VMM heap, ESI=extent. All client state and the
; active page's cursor are restored. Graphics admission is not implemented.
command_screen:
    pushad
    sub esp, 144                   ; 108-byte client state + nine locals
    mov edi, esp
    vmm_call 0x8d
    vmm_call 0x82
    mov word [ebp+client_eax], 0x0f00
    call .int10
    movzx eax, byte [ebp+client_eax]
    and eax, 0x7f
    cmp eax, 3
    jbe .text
    cmp eax, 7
    jne .unsupported
.text:
    mov [esp+132], eax
    movzx eax, byte [ebp+client_eax+1]
    test eax, eax
    jz .unsupported
    mov [esp+112], eax
    movzx eax, byte [ebp+client_ebx+1]
    mov [esp+128], eax
    mov word [ebp+client_eax], 0x0300
    call .int10
    movzx eax, word [ebp+client_edx]
    mov [esp+120], eax
    movzx eax, word [ebp+client_ecx]
    mov [esp+124], eax
    mov word [ebp+client_eax], 0x1130
    mov word [ebp+client_ebx], 0
    call .int10
    movzx eax, word [ebp+client_ecx]
    mov [esp+140], eax
    movzx eax, byte [ebp+client_edx]
    inc eax
    mov [esp+116], eax
    imul eax, [esp+112]
    lea eax, [eax*2+screen_header]
    mov [esp+144+4], eax           ; returned ESI in saved PUSHAD
    push dword 0
    push eax
    vmm_call 0x4f                   ; _HeapAllocate(bytes, flags)
    add esp, 8
    test eax, eax
    jz .no_memory
    mov [esp+108], eax
    mov edi, [vm_offset]
    add edi, [esp+144+16]
    mov [edi+vm_io_buffer], eax
    mov [esp+144+20], eax          ; returned EDX
    mov edi, eax
    mov eax, [esp+112]
    mov [edi], eax
    mov eax, [esp+116]
    mov [edi+4], eax
    movzx eax, byte [esp+120]
    mov [edi+8], eax
    movzx eax, byte [esp+121]
    mov [edi+12], eax
    mov eax, [esp+124]
    mov [edi+16], eax
    mov eax, [esp+132]
    mov [edi+20], eax
    mov eax, [esp+128]
    mov [edi+24], eax
    mov eax, [esp+144+16]
    mov ecx, [vm_offset]
    mov eax, [eax+ecx+vm_codepage]
    mov [edi+28], eax
    mov eax, [esp+140]
    mov [edi+32], eax
    mov dword [esp+136], 0
.cell:
    mov eax, [esp+136]
    xor edx, edx
    div dword [esp+112]
    shl eax, 8
    or eax, edx
    mov [ebp+client_edx], ax
    mov eax, [esp+128]
    mov [ebp+client_ebx+1], al
    mov word [ebp+client_eax], 0x0200
    call .int10
    mov eax, [esp+128]
    mov [ebp+client_ebx+1], al
    mov word [ebp+client_eax], 0x0800
    call .int10
    mov edi, [esp+108]
    mov ecx, [esp+136]
    mov ax, [ebp+client_eax]
    mov [edi+ecx*2+screen_header], ax
    inc dword [esp+136]
    mov eax, [esp+112]
    imul eax, [esp+116]
    cmp [esp+136], eax
    jb .cell
    mov eax, [esp+120]
    mov [ebp+client_edx], ax
    mov eax, [esp+128]
    mov [ebp+client_ebx+1], al
    mov word [ebp+client_eax], 0x0200
    call .int10
    xor eax, eax
    jmp .finish
.unsupported:
    mov eax, 50                    ; ERROR_NOT_SUPPORTED, not a success stub
    jmp .finish
.no_memory:
    mov eax, 8
.finish:
    mov [esp+144+28], eax          ; status survives VMM restore services
    vmm_call 0x86
    mov esi, esp
    vmm_call 0x8e
    add esp, 144
    popad
    ret
.int10:
    mov eax, 0x10
    vmm_call 0x84
    ret

; Only the admitted COMMAND callback calls DOS, never the IRQ/control path.
; Preserve all 108 bytes, including VMM's alternate nested-execution frame.
; EDX is the caller's 64-byte snapshot. Copy while nested V86 execution still
; owns the DOS buffer mapping, before End_Nest_Exec restores the outer client.
command_directory:
    pushad
    sub esp, 108
    mov edi, esp
    vmm_call 0x8d                   ; Save_Client_State
    vmm_call 0x82                   ; Begin_Nest_V86_Exec
    mov eax, [directory_buffer]
    shr eax, 4                     ; allocation is paragraph aligned
    mov [ebp+60], ax                ; Client_DS
    mov word [ebp+client_esi], 0
    mov al, [esp+client_eax]         ; original default drive
    inc al                         ; AH=47: one based, zero means current
    mov [ebp+client_edx], al
    mov word [ebp+client_eax], 0x4700
    mov eax, 0x21
    vmm_call 0x84                   ; Exec_Int
    xor edi, edi
    test dword [ebp+client_flags], 1
    jz .directory_done
    movzx edi, word [ebp+client_eax]
.directory_done:
    test edi, edi
    jnz .restore
    mov eax, 0x3c04                ; Client_DS:Client_ESI
    vmm_call 0x1c                  ; Map_Flat in the current nested VM
    cmp eax, -1
    je .mapping_error
    mov esi, eax
    mov edi, [esp+108+20]          ; caller's saved EDX
    mov ecx, 64
    cld
    rep movsb
    mov word [ebp+client_eax], 0x6601 ; original DOS active code-page authority
    mov eax, 0x21
    vmm_call 0x84
    test dword [ebp+client_flags], 1
    jnz .codepage_error
    movzx eax, word [ebp+client_ebx]
    mov edx, [esp+108+16]
    mov ecx, [vm_offset]
    mov [edx+ecx+vm_codepage], eax
    xor edi, edi
    jmp .restore
.mapping_error:
    mov edi, 487                   ; ERROR_INVALID_ADDRESS
    jmp .restore
.codepage_error:
    movzx edi, word [ebp+client_eax]
.restore:
    mov [esp+108+28], edi           ; result survives both VMM services
    vmm_call 0x86                   ; End_Nest_Exec
    mov esi, esp
    vmm_call 0x8e                   ; Restore_Client_State
    add esp, 108
    popad
    ret

notification_interrupt:
    pushad
    host_notify 3                  ; snapshot the host publication serial
    test eax, eax
    jnz .failed
    mov [irq_serial_low], edx
    mov [irq_serial_high], esi
.drain:
    host_notify 10
    cmp eax, 259                    ; ERROR_NO_MORE_ITEMS
    je .acknowledge
    test eax, eax
    jnz .failed
    mov edi, ebx                    ; requested VM handle
    vmm_call 3                     ; Get_Sys_VM_Handle
    mov ebp, ebx
.find_target:
    cmp ebx, edi
    je .target
    vmm_call 0x3b
    cmp ebx, ebp
    jne .find_target
    jmp .drain                     ; a retired VM cannot receive a wake
.target:
    mov ecx, [vm_offset]
    add ecx, ebx
    cmp dword [ecx+vm_executable], 1
    jne .drain
    cmp edx, [ecx+vm_generation_low]
    jne .drain
    cmp esi, [ecx+vm_generation_high]
    jne .drain
    mov eax, [ecx+vm_semaphore]
    vmm_call 0x28                   ; legal VMM wake, no target VM event
    jmp .drain
.acknowledge:
    mov edx, [irq_serial_low]
    mov esi, [irq_serial_high]
    host_notify 4                  ; acknowledge observed doorbell, not work
    test eax, eax
    jnz .failed
    inc dword [irq_count]
    jmp .eoi
.failed:
    mov [irq_error], eax
    mov eax, [irq_handle]
    vpicd_call 8                    ; stop repeated delivery on broken ABI
.eoi:
    mov eax, [irq_handle]
    vpicd_call 4                    ; VPICD_Phys_EOI
    popad
    clc                            ; handled this reserved interrupt
    ret

notification_close:
    mov eax, [irq_handle]
    test eax, eax
    jz .done
    vpicd_call 8
    host_notify 5                  ; no rearm while VPICD is relinquished
    mov eax, [irq_handle]
    vpicd_call 0x11                 ; VPICD_Force_Default_Behavior
    mov dword [irq_handle], 0
.done:
    ret

segment _LDATA public align=16 class=DATA use32
global ntvdm64_ddb
ntvdm64_ddb:
    dd 0
    dw 0x300, 0x7f80                ; isolated S4 API ID, not a system device
    db 0, 1
    dw 0
    db 'NTVDM64 '
    dd 0x80000000
    dd control, api, api, 0, 0, 0, 0, 0
vm_offset: dd 0
generation_low: dd 0
generation_high: dd 0
v86_callback: dd 0
command_callback: dd 0
directory_buffer: dd 0
irq_handle: dd 0
irq_serial_low: dd 0
irq_serial_high: dd 0
irq_count: dd 0
irq_error: dd 0
irq_descriptor:
    dw 0xffff, 0
    dd notification_interrupt, 0, 0, 0, 0, 500
