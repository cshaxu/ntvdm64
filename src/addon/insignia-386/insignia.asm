; Independently authored NIDDB lifecycle companion, T479 S6 candidate.
; ABI provenance / bounded contract: README.md. Not original INSIGNIA code.
bits 32
%macro vmm_call 1
    int 0x20
    dd 0x00010000 + %1
%endmacro
%macro niddb_call 1
    mov eax,%1
    clc
    db 0xc4,0xc4,0xb1
%endmacro

segment _LTEXT public align=16 class=CODE use32
global control
control:
    pushad
    cmp eax,0                       ; Sys_Critical_Init
    je .allocate
    cmp eax,1                       ; Device_Init, before instance snapshot
    je .instance_byte
    cmp eax,3                       ; Sys_VM_Init
    je .system_vm
    cmp eax,7                       ; Create_VM, before any fallible VM_Init
    je .new_vm
    cmp eax,9                       ; VM_Init (not Create_VM)
    je .child_vm
    cmp eax,0x0b                    ; VM_Not_Executeable
    je .retire
    cmp eax,5                       ; System_Exit
    je .shutdown
    cmp eax,0x17                    ; Device_Reboot_Notify
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
    push dword 0
    push dword 4                   ; created flag in each VMM control block
    vmm_call 0xa7
    add esp,8
    test eax,eax
    jz .fail
    mov [vm_offset],eax
    jmp .ok
.instance_byte:
    push dword 0
    push dword instance_item
    vmm_call 0xa6                   ; _AddInstanceItem, initialization-only
    add esp,8
    jmp .ok
.system_vm:
    niddb_call 3                    ; original snapshot -> system VM instance
    jmp .created
.new_vm:
    mov edi,[vm_offset]
    mov dword [ebx+edi],0           ; do not inherit the system VM's live flag
    jmp .ok
.child_vm:
    niddb_call 9                    ; original snapshot -> fresh child instance
.created:
    jc .fail
    cmp eax,80                     ; original MAX_VMS; never truncate a result
    jae .fail
    mov edx,[ebx+4]
    mov [edx+0x4ac],al
    mov edi,[vm_offset]
    mov dword [ebx+edi],1
    jmp .ok
.retire:
    mov edi,[vm_offset]
    test edi,edi
    jz .ok
    cmp dword [ebx+edi],0
    je .ok                         ; creation failed before NIDDB registration
    mov dword [ebx+edi],0
    niddb_call 0x0b
    jmp .ok
.shutdown:
    ; Reuse the original NIDDB-only reset for this bounded companion.
    ; Original System_Exit additionally calls unavailable VDD instance-page
    ; marking cleanup. This addon registers no such VDD marks. See README.
    niddb_call 0x17
    jmp .ok

segment _LDATA public align=16 class=DATA use32
global insignia_ddb
insignia_ddb:
    dd 0
    dw 0x300,0                      ; no callable VxD API/device ID
    db 0,1
    dw 0
    db 'NIDDB   '
    dd 0x80000000
    dd control,0,0,0,0,0,0,0
vm_offset: dd 0
instance_item:
    dd 0,0,0x4ac,1,0x200            ; V86 address (not CB_High_Linear), ALWAYS_Field
