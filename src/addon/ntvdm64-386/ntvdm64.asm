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

; Each VMM control block owns this data until Destroy_VM.
vm_generation_low equ 0
vm_generation_high equ 4
vm_executable equ 8
vm_semaphore equ 12
vm_waiting equ 16
vm_bytes equ 20

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
    cmp dword [esp+28], 1           ; saved Device_Init, only the system VM
    je .notification_init
    jmp .register_vm
.notification_init:
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
irq_handle: dd 0
irq_serial_low: dd 0
irq_serial_high: dd 0
irq_count: dd 0
irq_error: dd 0
irq_descriptor:
    dw 0xffff, 0
    dd notification_interrupt, 0, 0, 0, 0, 500
