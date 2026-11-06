; Independently authored test-only static VxD. Never part of release assets.
; Microsoft Windows DDK Nuke_VM takes EBX target, returns for another ordinary
; VM, and never returns for the current/system VM. Reject those two targets.
; Ordinal 2Eh is checked against the MS-DOS 6 smartdrv/sdvxd/vmm.inc table.
bits 32
%macro vmm_call 1
    int 0x20
    dd 0x10000 + %1
%endmacro
segment _LTEXT public align=16 class=CODE use32
control:
    clc
    ret
api:
    pushad
    mov edi, [ebp+16]               ; client EBX: requested target VM
    cmp edi, ebx                   ; never destroy the calling VM
    je .reject
    vmm_call 3                     ; Get_Sys_VM_Handle
    cmp edi, ebx
    je .reject
    mov esi, ebx
.next:
    vmm_call 0x3b                  ; Get_Next_VM_Handle
    cmp ebx, esi
    je .reject
    cmp ebx, edi
    jne .next
    test dword [ebx], 0x400         ; original VMM CB_VM_Status blocked bit
    jz .not_waiting
    vmm_call 0x2e                  ; Nuke_VM, actual VMM destruction
    mov ebp, [esp+8]
    mov dword [ebp+28], 0
    mov dword [ebp+24], 0x400
    and dword [ebp+44], ~1
    popad
    ret
.reject:
    mov eax, 1
    jmp .error
.not_waiting:
    mov eax, 2
.error:
    mov ebp, [esp+8]
    mov [ebp+28], eax
    or dword [ebp+44], 1
    popad
    ret
segment _LDATA public align=16 class=DATA use32
global retire_ddb
retire_ddb:
    dd 0
    dw 0x300, 0x7f81
    db 0, 1
    dw 0
    db 'VMRETIRE'
    dd 0x80000000, control, api, api, 0, 0, 0, 0, 0
