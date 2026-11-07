; Test-only observer of VMM-reflected IRQ0/IRQ1; those observations always
; chain. INT15/4F additionally tests explicit receiver acceptance by clearing
; client CF; otherwise it chains unchanged. It never reads port 60h or sends
; EOI. A separate probe API requests diagnostic host controller stimuli.
; Hook_V86_Int_Chain ABI: original Microsoft VxD DDK chapter 21;
; ordinal 41h checked against MS-DOS 6 smartdrv/sdvxd/vmm.inc.
bits 32
segment _LTEXT public align=16 class=CODE use32
control:
    cmp eax, 0                     ; Sys_Critical_Init, static lifetime
    jne .ok
    pushad
    mov eax, 8
    mov esi, observe
    int 0x20
    dd 0x10041                     ; Hook_V86_Int_Chain
    jc .failed
    mov eax, 9
    mov esi, observe
    int 0x20
    dd 0x10041
    jc .failed
    mov eax, 0x15
    mov esi, observe_keyboard_intercept
    int 0x20
    dd 0x10041
    jc .failed
    mov eax, 0x21
    mov esi, observe_dos_read
    int 0x20
    dd 0x10041
    jc .failed
    mov eax, 0x2f
    mov esi, observe_mouse_broadcast
    int 0x20
    dd 0x10041
    jc .failed
    mov eax, 0x16
    mov esi, observe_bios_read
    int 0x20
    dd 0x10041
    jc .failed
    popad
.ok:
    clc
    ret
.failed:
    popad
    stc
    ret
observe:
    pushfd
    pushad
    mov edx, eax                   ; reflected vector
    mov esi, [ebx]                 ; actual CB_VM_Status, observed only
    mov eax, 0x4e544d56
    mov ecx, 250                    ; diagnostic host only; no product service
    db 0xc4,0xc4,0xb1
    cmp dword [esp+28], 8         ; sample display during actual timer delivery
    jne .done
    mov ebx, [esp+16]             ; BOP returns its value in EBX; restore VM identity
    mov edx, [ebx+4]
    xor esi, esi
    mov eax, 0x4e544d56
    mov ecx, 254
    db 0xc4,0xc4,0xb1
.done:
    popad
    popfd
    stc                            ; pass the original interrupt onward
    ret
observe_mouse_broadcast:
    cmp word [ebp+28], 0x1607
    jne .chain
    cmp word [ebp+16], 0x000c
    jne .chain
    pushfd
    pushad
    mov edx, 0x607                 ; observation tag, not a product operation
    movzx esi, word [ebp+24]       ; VMD broadcast subfunction
    mov eax, 0x4e544d56
    mov ecx, 250
    db 0xc4,0xc4,0xb1
    popad
    popfd
.chain:
    stc                           ; leave every original client register unchanged
    ret
observe_dos_read:
    cmp byte [ebp+29], 0x0a        ; actual DOS buffered line read, observation only
    jne .chain
    pushfd
    pushad
    mov edx, [ebx+4]              ; actual CB_High_Linear, observation only
    xor esi, esi
    mov eax, 0x4e544d56
    mov ecx, 251
    db 0xc4,0xc4,0xb1
    popad
    popfd
.chain:
    stc
    ret
observe_bios_read:
    pushfd
    pushad
    mov edx, [ebx+4]
    movzx esi, word [ebp+28]
    mov eax, 0x4e544d56
    mov ecx, 254
    db 0xc4,0xc4,0xb1
    popad
    popfd
    stc
    ret
observe_keyboard_intercept:
    cmp byte [ebp+29], 0x4f         ; Client_AH, BIOS scan interception only
    jne .chain
    pushad
    movzx edx, byte [ebp+28]        ; Client_AL: actual translated scan byte
    mov esi, [ebp+44]              ; Client_EFlags (not host handler flags)
    mov eax, 0x4e544d56
    mov ecx, 20
    db 0xc4,0xc4,0xb1
    cmp eax, 1                     ; explicit diagnostic receiver acceptance
    jne .observed
    and dword [ebp+44], ~1         ; BIOS contract: scan was consumed
    popad
    clc                            ; handled INT15, do not reflect onward
    ret
.observed:
    popad
.chain:
    stc                            ; observation never consumes the key
    ret
segment _LDATA public align=16 class=DATA use32
global observe_ddb
observe_ddb:
    dd 0
    dw 0x300,0x7f82
    db 0,1
    dw 0
    db 'VMINTOBS'
    dd 0x80000000,control,stimulus,stimulus,0,0,0,0,0

segment _LTEXT
stimulus:
    ; Independent probe API: client AX=virtual key plus bit8 make-only / bit9 break-only, BX=scan code.
    ; The diagnostic host routes it through the real emulated controller.
    pushad
    push dword [ebp+28]
    mov dword [ebp+28], 0         ; failure/not yet blocked
    test word [esp], 0x400       ; diagnostic physical mouse stimulus
    jnz .mouse
    mov edi, [ebp+20]             ; optional target VM from witness RDY file
    test edi,edi
    jz .send
    mov esi,ebx                  ; enumerate live VMM handles; no stale dereference
.find:
    int 0x20
    dd 0x1003b                   ; Get_Next_VM_Handle
    cmp ebx,edi
    je .target
    cmp ebx,esi
    jne .find
    jmp .done
.target:
    test dword [ebx],0x400        ; actual VMStat_Blocked, not pre-wait intent
    jz .done
    mov edx, [ebx]
    mov esi,edx
    mov eax,0x4e544d56
    mov ecx,252
    db 0xc4,0xc4,0xb1
.send:
    movzx edx, word [esp]         ; saved original client AX
    movzx esi, word [ebp+16]       ; Client_EBX
    mov eax, 0x4e544d56
    mov ecx, 253
    db 0xc4,0xc4,0xb1
    mov dword [ebp+28],1
    jmp .done
.mouse:
    sub esp,16
    movsx eax,word [ebp+16]
    mov [esp],eax
    movsx eax,word [ebp+24]
    mov [esp+4],eax
    mov ax,[ebp+4]               ; Client_ESI width
    mov [esp+8],ax
    mov ax,[ebp]                 ; Client_EDI height
    mov [esp+10],ax
    mov eax,[esp+16]
    and eax,3
    mov [esp+12],ax
    mov word [esp+14],2          ; existing Window MOVE
    test word [esp+16],0x800
    jz .mouse_send
    mov word [esp+14],1          ; existing Window ENTER
.mouse_send:
    mov edx,esp
    mov esi,16
    mov eax,0x4e544d56
    mov ecx,255
    db 0xc4,0xc4,0xb1
    add esp,16
    test eax,eax
    jz .mouse_ok
    cmp eax,183                  ; explicit reuse of NTCON's already-entered route
    jne .done
    test word [esp],0x800
    jz .done
.mouse_ok:
    mov dword [ebp+28],1
.done:
    add esp,4
    popad
    ret
