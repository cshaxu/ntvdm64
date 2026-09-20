; Independent protected-mode DOS I/O and segment/stack restoration witness.
; Test-owned file only; original guest media remain immutable.
%ifdef TSR_PM_RETENTION
%ifndef TSR_EXIT
%error TSR_PM_RETENTION requires TSR_EXIT
%endif
%ifdef CLIENT32
%error TSR_PM_RETENTION currently verifies the 16-bit resident contract
%endif
%endif
bits 16
org 100h
%ifdef EXEC_LOAD_CALL
%ifndef EXEC_LOAD_CLEANUP
%error EXEC_LOAD_CALL requires EXEC_LOAD_CLEANUP
%endif
%ifndef EXEC_LOAD_ONLY
%error EXEC_LOAD_CALL requires EXEC_LOAD_ONLY
%endif
%endif
%ifdef EXEC_INHERIT
%ifndef EXEC_CHILD
%error EXEC_INHERIT requires EXEC_CHILD
%endif
%ifndef EXEC_FAILURE
%error EXEC_INHERIT requires EXEC_FAILURE
%endif
%ifdef EXEC_LOAD_ONLY
%error EXEC_INHERIT requires executing the child
%endif
%endif
%ifdef IOCTL_DEVICE_MOCK
%ifndef IOCTL_METADATA_MOCK
%error IOCTL_DEVICE_MOCK requires IOCTL_METADATA_MOCK
%endif
%endif
%ifdef IOCTL_CONTROL_MOCK
%ifndef IOCTL_METADATA_MOCK
%error IOCTL_CONTROL_MOCK requires IOCTL_METADATA_MOCK
%endif
%endif
%ifdef EXEC_LOAD_ONLY
%ifndef EXEC_CHILD
%error EXEC_LOAD_ONLY requires EXEC_CHILD and EXEC_FAILURE
%endif
%ifndef EXEC_FAILURE
%error EXEC_LOAD_ONLY requires EXEC_CHILD and EXEC_FAILURE
%endif
%endif
%ifdef EXTENDED_FCB
%ifndef FCB_FAMILY
%error EXTENDED_FCB requires FCB_FAMILY
%endif
%define fcb_name_offset 8
%else
%define fcb_name_offset 1
%endif
%ifdef CODE32
%ifndef CLIENT32
%error CODE32 requires CLIENT32
%endif
%endif
%ifdef CLIENT32
%define result_count eax
%else
%define result_count ax
%endif
%ifndef TRACK_SECTORS
%define TRACK_SECTORS 1
%endif
%ifdef HIGH_OFFSET
%ifndef CLIENT32
%error HIGH_OFFSET requires CLIENT32
%endif
%ifndef HIGH_BUFFER
%error HIGH_OFFSET requires HIGH_BUFFER
%endif
%define io_payload 10000h
%ifdef HIGH_COUNT
%define io_buffer 30000h
%else
%define io_buffer 14000h
%endif
%else
%define io_payload payload
%define io_buffer buffer
%endif
%ifdef HIGH_COUNT
%ifndef HIGH_OFFSET
%error HIGH_COUNT requires HIGH_OFFSET
%endif
%define io_count 10019h
%else
%define io_count (payload_end-payload)
%endif

%macro checked_int21 0
    mov [ss:saved_sp], esp
    mov [ss:saved_ss], ss
    mov [ss:saved_ds], ds
    mov [ss:saved_es], es
    mov [ss:saved_fs], fs
    mov [ss:saved_gs], gs
    int 21h
    pushf
    push eax
    mov eax, esp
%ifdef CODE32
    add eax, 8
%else
    add eax, 6
%endif
    cmp eax, [cs:saved_sp]
    jne failed
    mov ax, ss
    cmp ax, [cs:saved_ss]
    jne failed
    mov ax, ds
    cmp ax, [cs:saved_ds]
    jne failed
    mov ax, es
    cmp ax, [cs:saved_es]
    jne failed
    mov ax, fs
    cmp ax, [cs:saved_fs]
    jne failed
    mov ax, gs
    cmp ax, [cs:saved_gs]
    jne failed
    pop eax
    popf
%endmacro

%macro drive_function 1
%if %1 = 0
    mov ah, 1Bh
%elif %1 = 1
    mov ah, 1Ch
%elif %1 = 2
    mov ah, 1Fh
%else
    mov ah, 32h
%endif
    xor dl, dl             ; Current drive, no physical-media mutation.
%endmacro

%macro memory_failure_registers 0
%ifdef DOS_MEMORY_FAILURE_TRACE
    mov [memory_result], ax
    mov [memory_result+2], bx
    pushfw
    pop ax
    mov [memory_result+4], ax
    mov ax, [memory_result]
    pushfd
    pushad
    mov edx, memory_result_label
    mov ah, 09h
    int 21h
%assign result_word 0
%rep 3
%assign result_shift 12
%rep 4
    movzx ebx, word [memory_result+result_word*2]
    shr ebx, result_shift
    and ebx, 15
    mov dl, [memory_digits+ebx]
    mov ah, 02h
    int 21h
%assign result_shift result_shift-4
%endrep
    mov dl, ' '
    mov ah, 02h
    int 21h
%assign result_word result_word+1
%endrep
    mov dl, 13
    mov ah, 02h
    int 21h
    mov dl, 10
    int 21h
    popad
    popfd
%endif
%endmacro

%ifdef CODE32
%define failed failed16
%endif
    mov sp, stack_top
%ifdef NETWORK_DPL_POINTER
    mov ax, ds
    add ax, 10h
    mov [network_dpl+14], ax
    mov word [network_dpl+10], 0
    movzx eax, word [network_dpl+14]
    shl eax, 4
    mov [network_expected_pointer], eax
%endif
    mov bx, (image_end - $$ + 100h + 15) / 16
    mov ah, 4Ah
    int 21h
    jc failed
%ifdef NETWORK_SPOOL_TRACE
    mov ax, 1100h
    int 2Fh
    memory_failure_registers
    mov ax, 1125h
    int 2Fh
    memory_failure_registers
    mov ax, 5D07h
    int 21h
    memory_failure_registers
%endif
    mov ax, 1687h
%ifdef TSR_EXIT
    ; Publish only this independent probe's real-mode allocation identity.
    mov [tsr_record], ds
    mov dx, tsr_record_name
    xor cx, cx
    mov ah, 3Ch
    int 21h
    jc failed
    mov bx, ax
    mov dx, tsr_record
    mov cx, 6
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, 6
    jne failed
    mov ah, 3Eh
    int 21h
    jc failed
    mov ax, 1687h
%endif
%ifdef COUNTRY_QUERY
    mov dx, country_real
    mov ax, 3800h
    int 21h
    jc failed
    mov [country_id], bx
    push ds
    pop es
    mov di, country_extended_real
    mov bx, 0FFFFh
    mov dx, 0FFFFh
    mov cx, 41
    mov ax, 6501h
    int 21h
    jc failed
    mov [country_extended_length], cx
    mov ax, 1687h
%endif
%ifdef IOCTL_PRINTER
    mov dx, printer_name
    mov ax, 3D00h
    int 21h
    jc failed
    mov bx, ax
    mov dx, printer_count
    mov cx, 0565h         ; Original printer driver: read retry count only.
    mov ax, 440Ch
    int 21h
    pushf
    mov ah, 3Eh
    int 21h
    popf
    jc failed
    mov ax, [printer_count]
    mov [printer_expected], ax
    not ax               ; Ensure an unchanged target cannot pass.
    mov [printer_count], ax
    mov dx, printer_real_ok
    mov ah, 09h
    int 21h
    mov ax, 1687h
%endif
%ifdef IOCTL_MEDIA_QUERY
    mov dx, media_real
    xor bx, bx
    mov cx, 0866h
    mov ax, 440Dh
    int 21h
    jc failed
    mov dx, media_real_ok
    mov ah, 09h
    int 21h
    mov ax, 1687h
%endif
%ifdef POINTER_QUERY
    push es
%assign pointer_index 0
%rep 2
    mov ah, 34h + pointer_index * (52h-34h)
    int 21h
    mov ax, es
    movzx eax, ax
    shl eax, 4
    movzx ebx, bx
    add eax, ebx
    mov [indos_real_linear + pointer_index*4], eax
%assign pointer_index pointer_index+1
%endrep
    pop es
    push ds
    mov byte [ss:pointer_stage], 'R'
    mov ax, 6300h
    int 21h
    jc failed
    mov ax, ds
    movzx eax, ax
    shl eax, 4
    movzx esi, si
    add eax, esi
    pop ds
    mov [dbcs_real_linear], eax
    mov ax, 1687h
%endif
%ifdef DRIVE_QUERY
%assign drive_index 0
%rep 4
    push ds
    drive_function drive_index
    int 21h
    cmp al, 0FFh
    je failed
    mov [ss:drive_result+drive_index], al
    mov ax, ds
    movzx eax, ax
    shl eax, 4
    movzx ebx, bx
    add eax, ebx
    pop ds
    mov [drive_linear+drive_index*4], eax
%assign drive_index drive_index+1
%endrep
    mov ax, 1687h
%endif
%ifdef NETWORK_NAME
    mov dx, network_baseline
    mov ax, 5E00h
    int 21h
    jc failed
    test ch, ch
    jz failed
    mov ax, 1687h
%endif
%ifdef NETWORK_PRINT
%assign print_call 0
%rep 2
    push ds
    pop es
    mov si, print_real
    mov dx, si
    mov di, si
    mov bx, 0FFFFh
    mov cx, 64
    mov ax, 5E02h+print_call
    int 21h
    mov [print_ax+print_call*2], ax
    pushf
    pop ax
    and ax, 1
    mov [print_cf+print_call*2], ax
%assign print_call print_call+1
%endrep
    mov ax, 1687h
%endif
    int 2Fh
    test ax, ax
    jnz failed
    mov [entry], di
    mov [entry+2], es
    mov bx, si
    test bx, bx
    jz .enter
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
.enter:
%ifdef CLIENT32
    mov ax, 1
%else
    xor ax, ax
%endif
    call far [entry]
    jc failed
%ifdef CODE32
    push ds
    pop es
    mov bx, cs
    mov edi, code_descriptor
    mov ax, 000Bh
    int 31h
    jc failed
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [code_entry+4], ax
    mov bx, ax
    or byte [code_descriptor+6], 40h
    mov edi, code_descriptor
    mov ax, 000Ch
    int 31h
    jc failed
    movzx esp, sp
    jmp dword far [code_entry]
failed16:
    mov ax, 4C01h
    int 21h
%undef failed
bits 32
code32_start:
%endif
    mov ax, ss
    mov bx, ds
    cmp ax, bx               ; Saved-state storage uses the original stack base.
    jne failed
%ifdef HIGH_BUFFER
    mov [initial_ds], ds
%ifdef HIGH_OFFSET
%ifdef HIGH_COUNT
    mov bx, 5
%else
    mov bx, 2
%endif
%else
    mov bx, 1
%endif
    xor cx, cx
    mov ax, 0501h
    int 31h
    jc failed
    cmp bx, 10h              ; Require memory above the real-mode address range.
    jbe failed
    mov [memory_handle], di
    mov [memory_handle+2], si
    mov [memory_base], cx
    mov [memory_base+2], bx
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [memory_selector], ax
    mov bx, ax
    mov cx, [memory_base+2]
    mov dx, [memory_base]
    mov ax, 0007h
    int 31h
    jc failed
    mov bx, [memory_selector]
%ifdef HIGH_OFFSET
%ifdef HIGH_COUNT
    mov cx, 4
%else
    mov cx, 1
%endif
%else
    xor cx, cx
%endif
    mov dx, 0FFFFh
    mov ax, 0008h
    int 31h
    jc failed
    mov es, [memory_selector]
    xor esi, esi
    xor edi, edi
    mov ecx, image_end
    cld
    a32 rep movsb
    mov ds, [memory_selector]
%endif
    mov ax, ds
    mov es, ax
    mov gs, ax
%ifdef HIGH_OFFSET
%ifdef HIGH_COUNT
    mov edi, io_payload
    mov ecx, io_count
    xor eax, eax
.fill_payload:
    a32 stosb
    add al, 17
    dec ecx
    jnz .fill_payload
%else
    mov esi, payload
    mov edi, io_payload
    mov ecx, payload_end-payload
    cld
    a32 rep movsb
%endif
    mov edi, io_buffer
    mov ecx, io_count+17
    mov al, 0A5h
    a32 rep stosb
%endif
    xor ax, ax
    mov fs, ax
%ifdef NETWORK_DPL
%ifdef NETWORK_SPOOL
    mov ax, 5D07h
    checked_int21
%ifdef NETWORK_SPOOL_TRACE
    memory_failure_registers
%endif
    jc failed
    mov [network_truncate_saved], dl
%assign spool_flag 0
%rep 2
    mov dx, spool_flag
    mov ax, 5D08h
    checked_int21
%ifdef NETWORK_SPOOL_TRACE
    memory_failure_registers
%endif
    jc failed
    mov ax, 5D07h
    checked_int21
    jc failed
    cmp dx, spool_flag
    jne failed
%assign spool_flag spool_flag+1
%endrep
    mov dx, 2
    mov ax, 5D08h
    checked_int21
    jnc failed
    cmp ax, 87
    jne failed
    mov ax, 5D07h
    checked_int21
    jc failed
    cmp dx, 1
    jne failed
    mov dl, [network_truncate_saved]
    mov ax, 5D08h
    checked_int21
    jc failed
    mov ax, 5D09h
    checked_int21
    jc failed
    or ax, ax
    jnz failed
    mov edx, network_spool_ok
    mov ah, 09h
    checked_int21
%endif
%assign dpl_call 0
%rep 3
    mov edx, network_dpl
%if dpl_call = 0
    mov ax, 5D01h
%elif dpl_call = 1
    mov ax, 5D03h
%else
    mov ax, 5D04h
%endif
    checked_int21
    jc failed
    cmp edx, network_dpl
    jne failed
%assign dpl_call dpl_call+1
%endrep
    mov edx, network_dpl
    mov ax, 5D0Ah
    checked_int21
    xor bx, bx
%ifdef NETWORK_DPL_POINTER
    mov edi, 0A5A50000h
%endif
    mov ax, 5900h
    push es
    int 21h
    cmp ax, 5
    jne failed
    cmp bx, 0803h
    jne failed
    cmp ch, 2
    jne failed
%ifdef NETWORK_DPL_POINTER
%ifdef CLIENT32
    cmp edi, 0FFFFh
    ja failed
%endif
    movzx edi, di
    mov bx, es
    mov ax, 0006h
    int 31h
    jc failed
    movzx eax, cx
    shl eax, 16
    mov ax, dx
    add eax, edi
    cmp eax, [network_expected_pointer]
    jne failed
%endif
    pop es
%ifdef NETWORK_DPL_POINTER
    mov edx, network_pointer_ok
    mov ah, 09h
    checked_int21
%endif
    cmp byte [network_dpl_guard], 0A5h
    jne failed
    mov edx, network_dpl_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef NETWORK_PRINT
%assign print_call 0
%rep 2
    mov esi, print_pm
    mov edx, esi
    mov edi, esi
    mov bx, 0FFFFh
    mov cx, 64
    mov ax, 5E02h+print_call
    checked_int21
    mov bp, ax
    pushf
    pop ax
    and ax, 1
    cmp ax, [print_cf+print_call*2]
    jne failed
    cmp bp, [print_ax+print_call*2]
    jne failed
    cmp esi, print_pm
    jne failed
    cmp edi, print_pm
    jne failed
%assign print_call print_call+1
%endrep
%assign print_byte 0
%rep 65
    mov al, [print_pm+print_byte]
    cmp al, [print_real+print_byte]
    jne failed
%assign print_byte print_byte+1
%endrep
    mov edx, print_ok
    mov ah, 09h
    checked_int21
%assign print_status 0
%rep 4
    mov bp, [print_ax+print_status*2]
%rep 4
    rol bp, 4
    movzx ebx, bp
    and ebx, 0Fh
    mov dl, [print_hex+ebx]
    mov ah, 02h
    int 21h
%endrep
    mov dl, ' '
    mov ah, 02h
    int 21h
%assign print_status print_status+1
%endrep
%endif
%ifdef NETWORK_NAME
    mov edx, network_name
    mov ax, 5E00h
    checked_int21
    jc failed
    test ch, ch
    jz failed
    cmp edx, network_name
    jne failed
    cmp byte [network_guard], 0A5h
    jne failed
%assign name_byte 0
%rep 16
    mov al, [network_name+name_byte]
    cmp al, [network_baseline+name_byte]
    jne failed
%assign name_byte name_byte+1
%endrep
    mov edx, network_name_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef NETWORK_ASSIGN_CREATE
    mov esi, assign_target
    mov edi, assign_share
    mov cx, 0BEEFh
    mov bx, 4
    mov ax, 5F03h
    checked_int21
    jc failed
    mov word [assign_index], 0
.assign_find:
    mov bx, [assign_index]
    mov esi, assign_local
    mov edi, assign_remote
    mov ax, 5F02h
    checked_int21
    jc failed
    cmp byte [assign_local], 'Z'
    je .assign_found
    inc word [assign_index]
    cmp word [assign_index], 26
    jb .assign_find
    jmp failed
.assign_found:
%assign assign_query 0
%rep 2
%if assign_query = 1
    mov bx, [assign_index]
    mov esi, assign_local
    mov edi, assign_remote
    mov ax, 5F05h
    checked_int21
    jc failed
%endif
    cmp bl, 4
    jne failed
    cmp cx, 0BEEFh
    jne failed
%assign share_byte 0
%rep 15
    mov al, [assign_remote+share_byte]
    cmp al, [assign_share+share_byte]
    jne failed
%assign share_byte share_byte+1
%endrep
%assign assign_query assign_query+1
%endrep
    mov esi, assign_target
    mov ax, 5F04h
    checked_int21
    jc failed
    mov edx, assign_create_ok
    mov ah, 09h
    checked_int21
    mov edi, assign_local
    mov ecx, 145
    mov al, 0A5h
    a32 rep stosb
%endif
%ifdef NETWORK_ASSIGN
%assign assign_call 0
%rep 2
    mov esi, assign_local
    mov edi, assign_remote
    mov bx, 0FFFFh           ; Read-only enumeration beyond the device list.
%if assign_call = 0
    mov ax, 5F02h
%else
    mov ax, 5F05h
%endif
    checked_int21
    jnc failed
    cmp ax, 18              ; Original ERROR_NO_MORE_FILES.
    jne failed
    cmp esi, assign_local
    jne failed
    cmp edi, assign_remote
    jne failed
%assign assign_byte 0
%rep 145
    cmp byte [assign_local+assign_byte], 0A5h
    jne failed
%assign assign_byte assign_byte+1
%endrep
%assign assign_call assign_call+1
%endrep
    mov edx, assign_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef EXEC_FAILURE
%ifdef EXEC_INHERIT
%assign vector_index 0
%rep 2
    mov bl, 23h+vector_index
    mov ax, 0200h
    int 31h
    jc failed
    mov [inherit_vectors+vector_index*4], dx
    mov [inherit_vectors+vector_index*4+2], cx
%assign vector_index vector_index+1
%endrep
    mov edx, inherit_name
    xor cx, cx
    mov ah, 3Ch
    checked_int21
    jc failed
    cmp ax, 5             ; Controlled fresh probe: first nonstandard handle.
    jne failed
    mov edx, inherit_name
    mov ax, 3D80h         ; Read-only, explicitly not inherited.
    checked_int21
    jc failed
    cmp ax, 6
    jne failed
%endif
    mov ah, 51h
    checked_int21
    mov [exec_parent], bx
    push es
    mov es, bx
    mov ax, [es:2Ch]
    mov [exec_environment], ax
    push ds
    pop es
    mov ax, ds
    mov [exec_params+4], ax
%assign exec_case 0
%assign exec_count 2
%ifdef EXEC_CHILD
%assign exec_count 3
%endif
%rep exec_count
%if exec_case = 2
    mov edx, exec_child
%else
    mov edx, exec_missing
%endif
    mov ebx, exec_params
%if exec_case = 0
    mov ax, 4B03h          ; Original host explicitly rejects overlays.
%else
    mov ax, 4B00h          ; Valid command-tail packet, absent test pathname.
%endif
    checked_int21
%if exec_case = 2
    jc failed
%else
    jnc failed
%if exec_case = 0
    cmp ax, 1
%else
    cmp ax, 2
%endif
    jne failed
%endif
    cmp bx, exec_params
    jne failed
%if exec_case = 2
    cmp dx, exec_child
%else
    cmp dx, exec_missing
%endif
    jne failed
%if exec_case = 2
    mov ah, 4Dh
    checked_int21
    cmp ax, 7
    jne failed
%endif
    mov ah, 51h
    checked_int21
    cmp bx, [exec_parent]
    jne failed
    mov es, bx
    mov ax, [es:2Ch]
    cmp ax, [exec_environment]
    jne failed
    push ds
    pop es
%assign exec_case exec_case+1
%endrep
%ifdef EXEC_LOAD_ONLY
    ; Negative witness, isolated worker only: CF=0 is insufficient.
    mov edx, exec_load_checkpoint
    xor cx, cx
    mov ah, 3Ch
    checked_int21
    jc failed
    mov bx, ax
    mov ah, 3Eh
    checked_int21
    jc failed
    mov dword [exec_params+14], 0A5A5A5A5h
    mov dword [exec_params+18], 0A5A5A5A5h
    mov edx, exec_missing
    mov ebx, exec_params
    mov ax, 4B01h
    checked_int21
    jnc failed
    cmp ax, 2
    jne failed
    cmp dword [exec_params+14], 0A5A5A5A5h
    jne failed
    cmp dword [exec_params+18], 0A5A5A5A5h
    jne failed
    mov edx, exec_child
    mov ebx, exec_params
    mov ax, 4B01h
    checked_int21
    jc failed
    ; Load-only selects a child PSP without executing it. Restore the
    ; parent's DOS identity through the public API before parent teardown.
    mov bx, [exec_parent]
    mov ah, 50h
    checked_int21
    mov edx, exec_load_returned
    xor cx, cx
    mov ah, 3Ch
    checked_int21
    jc failed
    mov bx, ax
    mov edx, exec_params+14
    mov cx, 8
    mov ah, 40h
    checked_int21
    jc failed
    mov ah, 3Eh
    checked_int21
    jc failed
%ifdef EXEC_LOAD_CLEANUP
    cmp word [exec_params+18], 100h
    jne failed
    mov bx, [exec_params+20]
    cmp bx, [exec_params+16]
    jne failed
    mov [exec_load_image], bx
    mov ax, 0002h
    int 31h
    jc failed
    mov es, ax
    cmp word [es:0], 20CDh
    jne failed
    mov ax, [es:2Ch]
    mov [exec_load_environment], ax
    push ds
    pop es
%ifdef EXEC_LOAD_CALL
    mov edi, exec_load_regs
    xor eax, eax
    mov ecx, 50
    a32 rep stosb
    mov ax, [exec_load_image]
    mov [exec_load_regs+34], ax
    mov [exec_load_regs+36], ax
    mov eax, [exec_params+18]
    mov [exec_load_regs+42], eax
    mov eax, [exec_params+14]
    mov [exec_load_regs+46], eax
    mov dword [exec_load_regs+28], 0BEEFh
    mov edi, exec_load_regs
    xor bx, bx
    xor cx, cx
    mov ax, 0301h
    int 31h
    jc failed
    cmp word [exec_load_regs+28], 0C0DEh
    jne failed
    mov edx, exec_load_call_ok
    mov ah, 09h
    checked_int21
%endif
%assign load_free 0
%rep 2
    mov edi, exec_load_regs
    xor eax, eax
    mov ecx, 50
    a32 rep stosb
%if load_free = 0
    mov ax, [exec_load_environment]
%else
    mov ax, [exec_load_image]
%endif
    mov [exec_load_regs+34], ax
    mov dword [exec_load_regs+28], 4900h
    mov edi, exec_load_regs
    mov bx, 21h
    xor cx, cx
    mov ax, 0300h
    int 31h
    jc failed
    test word [exec_load_regs+32], 1
    jnz failed
%assign load_free load_free+1
%endrep
    mov edx, exec_load_clean_ok
    mov ah, 09h
    checked_int21
%else
    cmp dword [exec_params+14], 0A5A5A5A5h
    jne failed
    cmp dword [exec_params+18], 0A5A5A5A5h
    jne failed
    mov edx, exec_load_missing
    mov ah, 09h
    checked_int21
    ; Changed bytes alone would not prove valid entry points or cleanup.
    jmp failed
%endif
%endif
    pop es
%ifdef EXEC_INHERIT
%assign vector_index 0
%rep 2
    mov bl, 23h+vector_index
    mov ax, 0200h
    int 31h
    jc failed
    cmp dx, [inherit_vectors+vector_index*4]
    jne failed
    cmp cx, [inherit_vectors+vector_index*4+2]
    jne failed
%assign vector_index vector_index+1
%endrep
    mov bx, 5
    xor cx, cx
    xor edx, edx
    mov ax, 4200h
    checked_int21
    jc failed
    mov edx, inherit_read
    mov cx, 4
    mov ah, 3Fh
    checked_int21
    jc failed
    cmp ax, 4
    jne failed
    cmp dword [inherit_read], 21444843h ; CHD!
    jne failed
    mov ah, 3Eh
    checked_int21
    jc failed
    mov bx, 6
    mov edx, inherit_read
    mov cx, 4
    mov ah, 3Fh
    checked_int21
    jc failed
    cmp ax, 4
    jne failed
    cmp dword [inherit_read], 21444843h
    jne failed
    mov ah, 3Eh
    checked_int21
    jc failed
    mov edx, inherit_name
    mov ah, 41h
    checked_int21
    jc failed
    mov edx, inherit_ok
    mov ah, 09h
    checked_int21
%endif
    mov edx, exec_failure_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef CREATE_PSP
    mov ah, 51h
    checked_int21
    mov [created_parent], bx
%ifdef HIGH_BUFFER
    mov eax, [0]
    mov [created_prefix], eax
    mov dx, ds
    mov ah, 26h             ; Original owner deliberately ignores high memory.
    checked_int21
    mov eax, [created_prefix]
    cmp [0], eax
    jne failed
    mov edx, create_psp_high_ok
    mov ah, 09h
    checked_int21
%endif
    mov bx, 17             ; A test-owned low-DOS block, plus a guard paragraph.
    mov ax, 0100h
    int 31h
    jc failed
    mov [created_selector], dx
    mov [created_segment], ax
    push es
    mov es, dx
    mov byte [es:100h], 0A5h
    mov ah, 26h
    checked_int21
    cmp dx, [created_selector]
    jne failed
    cmp word [es:0], 20CDh
    jne failed
    cmp word [es:50h], 21CDh
    jne failed
    cmp byte [es:52h], 0CBh
    jne failed
    cmp word [es:34h], 18h
    jne failed
    mov ax, [created_segment]
    cmp word [es:36h], ax
    jne failed
    cmp byte [es:100h], 0A5h
    jne failed
    pop es
    mov dx, [created_selector]
    mov ax, 0101h
    int 31h
    jc failed
    mov ah, 51h
    checked_int21
    cmp bx, [created_parent]
    jne failed
    mov edx, create_psp_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef DOS_MEMORY
    push es
%ifdef DOS_MEMORY_FAILURE
    mov bx, 3456h
    mov ah, 48h
    checked_int21
    memory_failure_registers
    jnc failed
%ifndef DOS_MEMORY_PRESERVATION
    cmp ax, 8
    jne failed
%endif
%endif
    mov bx, 16             ; 256 bytes, allocated by original DOSX pmallc.
    mov ah, 48h
    checked_int21
    jc failed
    mov es, ax
    mov dword [es:0], 12345678h
    mov byte [es:255], 5Ah
%ifdef DOS_MEMORY_FAILURE
    mov bx, 0456h
    mov ah, 4Ah
    checked_int21
    memory_failure_registers
    jnc failed
%ifndef DOS_MEMORY_PRESERVATION
    cmp ax, 8
    jne failed
%endif
    cmp dword [es:0], 12345678h
    jne failed
    cmp byte [es:255], 5Ah
    jne failed
%endif
    mov bx, 32             ; Grow, preserving the original prefix.
    mov ah, 4Ah
    checked_int21
    jc failed
    cmp dword [es:0], 12345678h
    jne failed
    cmp byte [es:255], 5Ah
    jne failed
    mov byte [es:511], 0A5h
    mov bx, 16             ; Shrink without changing the surviving data.
    mov ah, 4Ah
    checked_int21
    jc failed
    cmp dword [es:0], 12345678h
    jne failed
    cmp byte [es:255], 5Ah
    jne failed
    mov [ss:saved_sp], esp
    mov [ss:saved_ds], ds
    mov ah, 49h
    int 21h                ; Original pmfree deliberately clears ES.
    jc failed
    cmp esp, [cs:saved_sp]
    jne failed
    mov ax, ds
    cmp ax, [cs:saved_ds]
    jne failed
    mov ax, es
    test ax, ax
    jnz failed
    mov ah, 49h            ; Null selector is not an allocated block.
    checked_int21
    jnc failed
    cmp ax, 9
    jne failed
    mov bx, 16
    mov ah, 4Ah
    checked_int21
    jnc failed
    cmp ax, 9
    jne failed
    pop es
%endif
%ifdef POINTER_QUERY
    push es
%assign pointer_index 0
%rep 2
    mov [ss:saved_sp], esp
    mov [ss:saved_ds], ds
    mov ah, 34h + pointer_index * (52h-34h)
    int 21h                ; ES:BX intentionally returns a new selector.
    cmp esp, [cs:saved_sp]
    jne failed
    mov ax, ds
    cmp ax, [cs:saved_ds]
    jne failed
    movzx eax, bx
    mov [indos_offset], eax
    mov bx, es
    mov ax, 0006h
    int 31h
    jc failed
    movzx eax, cx
    shl eax, 16
    mov ax, dx
    add eax, [indos_offset]
    cmp eax, [indos_real_linear + pointer_index*4]
    jne failed
%assign pointer_index pointer_index+1
%endrep
    pop es
    push ds
    mov [ss:saved_sp], esp
    mov [ss:saved_es], es
    mov byte [ss:pointer_stage], 'P'
    mov ax, 6300h
    int 21h                ; ReturnDSSI intentionally replaces DS.
    jc failed
    mov byte [ss:pointer_stage], 'S'
    cmp esp, [cs:saved_sp]
    jne failed
    mov ax, es
    cmp ax, [cs:saved_es]
    jne failed
    movzx eax, si
    mov [ss:dbcs_offset], eax
    mov bx, ds
    movzx ebx, bx
    mov [ss:dbcs_selector], ebx
    pop ds
    mov byte [ss:pointer_stage], 'B'
    mov ax, 0006h
    int 31h
    jc failed
    movzx eax, cx
    shl eax, 16
    mov ax, dx
    add eax, [cs:dbcs_offset]
    mov [ss:dbcs_pm_linear], eax
    mov byte [ss:pointer_stage], 'C'
    cmp eax, [dbcs_real_linear]
    jne failed
    mov byte [ss:pointer_stage], 'F'
    mov ax, 6301h          ; Selected non-DBCS guest leaves these unchanged.
    checked_int21
    jc failed
    mov ax, 6302h
    checked_int21
    jc failed
    mov ax, 63FFh
    checked_int21
    jc failed
%endif
%ifdef DRIVE_QUERY
%assign drive_index 0
%rep 4
    push ds
    mov [ss:saved_sp], esp
    mov [ss:saved_es], es
    drive_function drive_index
    int 21h                ; Original owner returns a new DS:BX.
    cmp al, [cs:drive_result+drive_index]
    jne failed
    cmp esp, [cs:saved_sp]
    jne failed
    mov ax, es
    cmp ax, [cs:saved_es]
    jne failed
    movzx eax, bx
    mov [ss:drive_offset], eax
    mov bx, ds
    pop ds
    mov ax, 0006h
    int 31h
    jc failed
    movzx eax, cx
    shl eax, 16
    mov ax, dx
    add eax, [cs:drive_offset]
    cmp eax, [drive_linear+drive_index*4]
    jne failed
%assign drive_index drive_index+1
%endrep
%assign drive_index 1
%rep 2
    push ds
    mov [ss:saved_sp], esp
    mov [ss:saved_es], es
    drive_function drive_index
    mov dl, 0FFh           ; Outside the original DOS drive-number range.
    int 21h
    cmp al, 0FFh
    jne failed
    cmp esp, [cs:saved_sp]
    jne failed
    mov ax, es
    cmp ax, [cs:saved_es]
    jne failed
    pop ds                 ; Returned pointer is unspecified on failure.
%assign drive_index drive_index+2
%endrep
%endif
%ifdef KEYBOARD_INPUT
    mov edx, keyboard_ready
    mov ah, 09h
    checked_int21
    mov edx, keyboard_buffer
%ifdef KEYBOARD_FLUSH
    mov ax, 0C0Ah
%else
    mov ah, 0Ah
%endif
    checked_int21
    cmp dx, keyboard_buffer
    jne failed
    cmp byte [keyboard_buffer], 8
    jne failed
%ifdef KEYBOARD_EMPTY
    cmp byte [keyboard_buffer+1], 0
    jne failed
    cmp byte [keyboard_buffer+2], 13
    jne failed
%else
%ifdef KEYBOARD_OVERFLOW
    cmp byte [keyboard_buffer+1], 7
%else
    cmp byte [keyboard_buffer+1], 4
%endif
    jne failed
    cmp dword [keyboard_buffer+2], 'ABCD'
    jne failed
%ifdef KEYBOARD_OVERFLOW
    cmp dword [keyboard_buffer+6], 0D474645h ; EFG and CR, within capacity.
%else
    cmp byte [keyboard_buffer+6], 13
%endif
    jne failed
%endif
    cmp byte [keyboard_guard], 0A5h
    jne failed
%ifdef KEYBOARD_DISCARD
    mov cx, 2C5Ah          ; Standard BIOS store-key API: scan Z, ASCII Z.
    mov ax, 0500h
    int 16h
    test al, al
    jnz failed
    mov ah, 0Bh
    checked_int21
    cmp al, 0FFh           ; Prove pending input before asking DOS to flush.
    jne failed
    mov ax, 0C00h          ; Original invalid follow-up means flush only.
    checked_int21
    test al, al
    jnz failed
    mov ah, 0Bh
    checked_int21
    test al, al            ; The queued key must no longer be available.
    jnz failed
%ifdef KEYBOARD_STATUS
    mov ax, 0600h
    mov dl, 0FFh
    mov bx, 1
    or bx, bx              ; Enter with ZF=0; an empty poll must return ZF=1.
    checked_int21
    jnz failed
    test al, al
    jnz failed
    mov cx, 2C5Ah
    mov ax, 0500h
    int 16h
    test al, al
    jnz failed
    mov ax, 0600h
    mov dl, 0FFh
    xor bx, bx             ; Enter with ZF=1; a real key must return ZF=0.
    checked_int21
    jz failed
    cmp al, 'Z'
    jne failed
    mov ax, 0C06h
    mov dl, 0FFh
    mov bx, 1
    or bx, bx
    checked_int21
    jnz failed
    test al, al
    jnz failed
%endif
%endif
%endif
%ifdef IOCTL_METADATA_MOCK
%ifdef IOCTL_CONTROL_MOCK
%assign control_fn 0
%rep 4
%assign control_mode 0
%rep 3
    mov edi, control_packet
    mov ecx, 16
    mov al, 5Ah
    cld
    a32 rep stosb
    mov byte [control_packet], control_mode
    mov edx, control_packet
    mov bx, 0FEh
    mov cx, 16
    mov ax, 4402h+control_fn
    checked_int21
%if control_mode = 2
    jnc failed
    cmp ax, 5
%else
    jc failed
    cmp ax, 16-control_mode*8
%endif
    jne failed
%assign control_byte 0
%rep 16
%if !(control_fn & 1) && control_mode < 2 && control_byte < (16-control_mode*8)
    cmp byte [control_packet+control_byte], 0A6h
%elif control_byte = 0
    cmp byte [control_packet], control_mode
%else
    cmp byte [control_packet+control_byte], 5Ah
%endif
    jne failed
%assign control_byte control_byte+1
%endrep
    cmp byte [control_packet+16], 0A5h
    jne failed
%assign control_mode control_mode+1
%endrep
%assign control_fn control_fn+1
%endrep
    mov edx, control_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef IOCTL_DEVICE_MOCK
%assign device_case 0
%rep 4
%if device_case = 0
%assign device_fn 0840h
%assign device_size 48
%elif device_case = 1
%assign device_fn 0860h
%assign device_size 38
%elif device_case = 2
%assign device_fn 0862h
%assign device_size 5
%else
%assign device_fn 0868h
%assign device_size 4
%endif
    mov edi, device_packet
    mov ecx, 49
    mov al, 5Ah
    cld
    a32 rep stosb
%if device_case = 0
    mov word [device_packet+38], 2
%endif
    mov byte [device_packet+device_size], 0A5h
    mov edx, device_packet
    mov bx, 0FEh
    mov cx, device_fn
    mov ax, 440Dh
    checked_int21
%if device_case = 3
    jnc failed
    cmp ax, 1
    jne failed
%else
    jc failed
%endif
%assign device_byte 0
%rep device_size
%if device_case = 1
    cmp byte [device_packet+device_byte], 0A6h
%elif device_case = 0 && device_byte = 38
    cmp byte [device_packet+device_byte], 2
%elif device_case = 0 && device_byte = 39
    cmp byte [device_packet+device_byte], 0
%else
    cmp byte [device_packet+device_byte], 5Ah
%endif
    jne failed
%assign device_byte device_byte+1
%endrep
    cmp byte [device_packet+device_size], 0A5h
    jne failed
%assign device_case device_case+1
%endrep
    mov edx, device_ok
    mov ah, 09h
    checked_int21
%endif
%assign meta_index 0
%rep 4
%if meta_index < 2
%assign meta_size 25
%assign meta_fn 0846h + meta_index*20h
%else
%assign meta_size 2
%assign meta_fn 0847h + (meta_index-2)*20h
%endif
    mov edi, metadata_packet
    mov ecx, 26
    mov al, 5Ah
    cld
    a32 rep stosb
    mov byte [metadata_packet+meta_size], 0A5h
    mov edx, metadata_packet
    mov bx, 0FEh
    mov cx, meta_fn
    mov ax, 440Dh
    checked_int21
    jc failed
%assign meta_byte 0
%rep meta_size
%if meta_index & 1
    cmp byte [metadata_packet+meta_byte], 0A6h
%else
    cmp byte [metadata_packet+meta_byte], 5Ah
%endif
    jne failed
%assign meta_byte meta_byte+1
%endrep
    cmp byte [metadata_packet+meta_size], 0A5h
    jne failed
%assign meta_index meta_index+1
%endrep
    mov edx, metadata_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef IOCTL_MEDIA_QUERY
    mov edx, media_pm
    xor bx, bx
    mov cx, 0866h
    mov ax, 440Dh
    checked_int21
    jc failed
    mov esi, media_real
    mov edi, media_pm
    mov ecx, 25
    cld
    a32 repe cmpsb
    jne failed
    cmp word [media_guard], 0A55Ah
    jne failed
    mov edx, media_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef IOCTL_FORMAT_MOCK
    mov edx, format_packet
    mov bx, 0FEh
    mov cx, 0842h
    mov ax, 440Dh
    checked_int21
    jc failed
    cmp byte [format_packet], 0
    jne failed
    cmp word [format_packet+1], 1
    jne failed
    cmp word [format_packet+3], 2
    jne failed
    cmp word [format_guard], 0A55Ah
    jne failed
    mov edx, format_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef IOCTL_TRACK_MOCK
    mov ax, ds
%ifdef CLIENT32
    mov [track_packet+13], ax
%else
    mov [track_packet+11], ax
%endif
    mov edx, track_packet
    mov bx, 0FEh          ; Test-only resource token, never a physical drive.
%ifdef TRACK_WRITE
    mov cx, 0841h
%else
    mov cx, 0861h
%endif
    mov ax, 440Dh
    checked_int21
%ifdef TRACK_FAIL
    jnc failed
    cmp ax, 5
    jne failed
%else
    jc failed
%endif
    mov esi, track_data
    mov ecx, TRACK_SECTORS*512
track_check:
%ifdef TRACK_FAIL
%ifndef TRACK_WRITE
    cmp ecx, 512
    jbe track_unchanged
%endif
%endif
    cmp byte [esi], 5Ah
    jne failed
    jmp track_next
%ifdef TRACK_FAIL
%ifndef TRACK_WRITE
track_unchanged:
    cmp byte [esi], 0
    jne failed
%endif
%endif
track_next:
    inc esi
    dec ecx
    jnz track_check
    cmp byte [track_guard], 0A5h
    jne failed
    mov edx, track_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef IOCTL_PRINTER
    mov edx, printer_name
    mov ax, 3D00h
    checked_int21
    jc failed
    mov bx, ax
    mov edx, printer_count
    mov cx, 0565h
    mov ax, 440Ch
    checked_int21
    pushf
    mov ah, 3Eh
    checked_int21
    popf
    jc failed
    mov edx, printer_return_ok
    mov ah, 09h
    checked_int21
    mov ax, [printer_count]
    cmp ax, [printer_expected]
    jne failed
    cmp word [printer_guard], 0A55Ah
    jne failed
    mov edx, printer_ok
    mov ah, 09h
    checked_int21
%endif
%ifdef IOCTL_FAILURE
%assign ioctl_index 0
%rep 3
%if ioctl_index = 2
    mov ax, 440Ch
%else
    mov ax, 4402h+ioctl_index
%endif
    mov bx, 0FFFFh         ; Invalid handle: never reaches a real device.
    mov cx, 2
    mov edx, ioctl_buffer
    checked_int21
    jnc failed
    cmp ax, 6
    jne failed
    cmp dx, ioctl_buffer
    jne failed
    cmp dword [ioctl_buffer], 0A5A55AA5h
    jne failed
%assign ioctl_index ioctl_index+1
%endrep
%endif
%ifdef COUNTRY_QUERY
    mov edx, country_pm
    mov ax, 3800h
    checked_int21
    jc failed
    cmp bx, [country_id]
    jne failed
    cmp dx, country_pm
    jne failed
    mov esi, country_real
    mov edi, country_pm
    mov ecx, 34
    cld
    a32 repe cmpsb
    jne failed
    cmp byte [country_guard], 0A5h
    jne failed
    mov edi, country_extended_pm
    mov bx, 0FFFFh
    mov dx, 0FFFFh
    mov cx, 41
    mov ax, 6501h
    checked_int21
    jc failed
    cmp cx, [country_extended_length]
    jne failed
    cmp di, country_extended_pm
    jne failed
    mov esi, country_extended_real
    mov edi, country_extended_pm
    mov ecx, 41
    cld
    a32 repe cmpsb
    jne failed
    cmp byte [country_extended_guard], 0A5h
    jne failed
%endif
%ifdef TEMP_FILE
    mov edx, temp_path
    xor ecx, ecx
    mov ah, 5Ah
    checked_int21
    jc failed
    cmp dx, temp_path
    jne failed
    cmp byte [temp_path+6], 0
    je failed
    cmp byte [temp_guard], 0A5h
    jne failed
    mov bx, ax
    mov ah, 3Eh
    checked_int21
    jc failed
    mov edx, temp_path
    mov ax, 3D00h
    checked_int21
    jc failed
    mov bx, ax
    mov ah, 3Eh
    checked_int21
    jc failed
    mov edx, temp_path
    mov ah, 41h
    checked_int21
    jc failed
    mov edx, temp_path
    mov ax, 3D00h
    checked_int21
    jnc failed
    cmp ax, 2
    jne failed
%endif
%ifdef PARSE_FAMILY
    mov esi, parse_name
    mov edi, parse_fcb
    mov ax, 2900h
    checked_int21
    test al, al
    jnz failed
    cmp si, parse_name+9
    jne failed
    cmp di, parse_fcb
    jne failed
    cmp byte [parse_fcb], 0
    jne failed
    mov esi, parse_expected
    mov edi, parse_fcb+1
    mov ecx, 11
    cld
    a32 repe cmpsb
    jne failed
    cmp byte [parse_guard], 0A5h
    jne failed
    mov esi, parse_wildcard
    mov edi, parse_fcb
    mov ax, 2900h
    checked_int21
    cmp al, 1              ; Wildcard result differs from an ordinary filename.
    jne failed
    cmp si, parse_wildcard+8
    jne failed
    cmp di, parse_fcb
    jne failed
    mov esi, parse_wild_expected
    mov edi, parse_fcb+1
    mov ecx, 11
    cld
    a32 repe cmpsb
    jne failed
    cmp byte [parse_guard], 0A5h
    jne failed
%endif
%ifdef BPB_REFUSAL
    xor esi, esi
    xor edi, edi
    clc
    mov ah, 53h             ; Original host explicitly refuses BPB translation.
    checked_int21
    jnc failed
%endif
%ifdef PSP_FAMILY
    mov ah, 51h
    checked_int21
    jc failed
    test bx, bx
    jz failed
    mov [original_psp], bx
    mov ah, 62h
    checked_int21
    jc failed
    cmp bx, [original_psp]
    jne failed
    clc
    mov ah, 50h             ; Re-publish only our original PSP, never private data.
    checked_int21
    jc failed
    cmp bx, [original_psp]
    jne failed
%ifdef HIGH_BUFFER
    mov bx, [memory_selector] ; Known valid selector above 1 MiB cannot be a PSP.
    clc
    mov ah, 50h
    checked_int21
    jnc failed
    cmp bx, [memory_selector]
    jne failed
%endif
    mov ah, 62h
    checked_int21
    jc failed
    cmp bx, [original_psp]
    jne failed
%endif
    mov edx, filename
    xor ecx, ecx
    mov ah, 5Bh               ; Create new: refuse an existing test file.
    checked_int21
    jc failed
    mov [handle], ax
%ifdef FILE_FAMILY
    mov bx, ax
    mov ax, 4400h            ; Disk-file device information, not a character device.
    checked_int21
    jc failed
    test dx, 80h
    jnz failed
    mov bx, [handle]
    mov ah, 45h              ; Duplicate and close without invalidating the owner.
    checked_int21
    jc failed
    cmp ax, [handle]
    je failed
    mov bx, ax
    mov ah, 3Eh
    checked_int21
    jc failed
    mov bx, [handle]
    mov ah, 68h              ; Commit the still-live original handle.
    checked_int21
    jc failed
    mov edx, filename
    mov ax, 4300h
    checked_int21
    jc failed
    test cx, 18h             ; Neither directory nor volume label.
    jnz failed
%endif
    mov ax, [handle]
    mov bx, ax
    mov edx, io_payload
    mov ecx, io_count
    mov ah, 40h
    checked_int21
    jc failed
    cmp result_count, io_count
    jne failed
    mov bx, [handle]
    xor ecx, ecx
    xor edx, edx
    mov ax, 4200h
    checked_int21
    jc failed
    or ax, dx
    jnz failed
    mov bx, [handle]
    mov edx, io_buffer
    mov ecx, io_count
%ifdef HIGH_BUFFER
    add ecx, 17              ; Exercise short read after multiple 4K chunks.
%endif
    mov ah, 3Fh
    checked_int21
    jc failed
    cmp result_count, io_count
    jne failed
%ifdef HIGH_COUNT
    mov esi, io_payload
%else
    mov esi, payload
%endif
    mov edi, io_buffer
    mov ecx, io_count
    cld
    a32 repe cmpsb
    jne failed
%ifdef HIGH_BUFFER
    mov edi, io_buffer+io_count
    cmp byte [edi], 0A5h
    jne failed
    mov bx, [handle]
    mov edx, io_buffer+io_count
    mov ecx, 17
    mov ah, 3Fh              ; EOF must not overwrite the destination.
    checked_int21
    jc failed
    test result_count, result_count
    jnz failed
    cmp byte [edi], 0A5h
    jne failed
    mov bx, 0FFFFh
    mov ah, 3Fh              ; Failed buffered read must retain DOS error 6.
    checked_int21
    jnc failed
    cmp ax, 6
    jne failed
    cmp byte [edi], 0A5h
    jne failed
%endif
    mov bx, [handle]
    mov ah, 3Eh
    checked_int21
    jc failed
%ifdef FILE_FAMILY
    mov edx, filename
    mov edi, renamed
    mov ah, 56h              ; Both DS:DX and ES:DI require translation.
    checked_int21
    jc failed
    mov edx, filename
    mov ax, 3D00h
    checked_int21
    jnc failed
    cmp ax, 2
    jne failed
    mov edx, renamed
    mov edi, filename
    mov ah, 56h
    checked_int21
    jc failed
%endif
%ifdef EXTENDED_OPEN
    mov esi, filename
    xor ebx, ebx
    xor ecx, ecx
    mov edx, 1              ; Open existing, fail if missing; never create.
    mov ax, 6C00h
    checked_int21
    jc failed
    cmp cx, 1
    jne failed
    cmp si, filename
    jne failed
    mov bx, ax
    mov ah, 3Eh
    checked_int21
    jc failed
%endif
%ifdef FIND_FAMILY
    push es
    mov ah, 2Fh
    int 21h
    mov [ss:old_dta_selector], es
    mov [ss:old_dta_offset], bx
    pop es
    mov edx, find_dta
    mov ah, 1Ah
    checked_int21
    push es
    mov ah, 2Fh
    int 21h
    mov ax, es
    mov dx, ds
    cmp ax, dx
    jne failed
    cmp bx, find_dta
    jne failed
    pop es
    mov edx, filename
    xor ecx, ecx
    mov ah, 4Eh
    checked_int21
    jc failed
    cmp dword [find_dta+26], io_count
    jne failed
    mov esi, find_dta+30
    mov edi, found_name
    mov ecx, 10
    cld
    a32 repe cmpsb
    jne failed
    mov ah, 4Fh
    checked_int21
    jnc failed
    cmp ax, 18              ; Exact filename admits one result only.
    jne failed
    cmp byte [find_guard], 0A5h
    jne failed
    push ds
    mov ds, [ss:old_dta_selector]
    movzx edx, word [ss:old_dta_offset]
    mov ah, 1Ah
    checked_int21
    pop ds
    push es
    mov ah, 2Fh
    int 21h
    mov ax, es
    cmp ax, [ss:old_dta_selector]
    jne failed
    cmp bx, [ss:old_dta_offset]
    jne failed
    pop es
    mov edx, directory
    mov ah, 39h             ; New test-only directory; refuse collisions.
    checked_int21
    jc failed
    mov edx, directory
    mov ax, 4300h
    checked_int21
    jc failed
    test cx, 10h
    jz failed
%ifdef DIRECTORY_NAV
    xor edx, edx
    mov esi, old_directory
    mov ah, 47h             ; Original GetCurDir: translated 64-byte output.
    checked_int21
    jc failed
    mov edx, directory
    mov ah, 3Bh
    checked_int21
    jc failed
    xor edx, edx
    mov esi, current_directory
    mov ah, 47h
    checked_int21
    jc failed
    mov edi, current_directory
    mov ecx, 64
    xor al, al
    cld
    a32 repne scasb
    jne failed
    sub edi, 7              ; Require the final component, not just any path.
    cmp edi, current_directory
    jb failed
    mov esi, directory_leaf
    mov ecx, 7
    a32 repe cmpsb
    jne failed
    mov edx, parent_directory
    mov ah, 3Bh
    checked_int21
    jc failed
    xor edx, edx
    mov esi, current_directory
    mov ah, 47h
    checked_int21
    jc failed
    mov esi, old_directory
    mov edi, current_directory
.compare_directory:
    a32 lodsb
    a32 scasb
    jne failed
    test al, al
    jnz .compare_directory
    cmp byte [directory_guard], 0A5h
    jne failed
%endif
    mov edx, directory
    mov ah, 3Ah
    checked_int21
    jc failed
%endif
    mov edx, filename
%ifdef FCB_FAMILY
    xor edx, edx
    mov esi, fcb_old_directory
    mov ah, 47h
    checked_int21
    jc failed
    mov edx, fcb_directory
    mov ah, 3Bh
    checked_int21
    jc failed
    push es
    mov ah, 2Fh
    int 21h
    mov [ss:fcb_old_dta_sel], es
    mov [ss:fcb_old_dta_off], bx
    pop es
    mov edx, fcb_dta
    mov ah, 1Ah
    checked_int21
    mov edx, fcb_query
    mov ah, 11h
    checked_int21
    test al, al
    jnz failed
%ifdef EXTENDED_FCB
    cmp byte [fcb_dta], 0FFh
    jne failed
%endif
    mov esi, fcb_query+fcb_name_offset
    mov edi, fcb_dta+fcb_name_offset
    mov ecx, 11
    cld
    a32 repe cmpsb
    jne failed
    mov edx, fcb_query
    mov ah, 12h
    checked_int21
    cmp al, 0FFh
    jne failed
    mov edx, fcb_rename
    mov ah, 17h
    checked_int21
    test al, al
    jnz failed
    mov edx, fcb_leaf
    mov ax, 3D00h
    checked_int21
    jnc failed
    cmp ax, 2
    jne failed
    mov edx, fcb_new_leaf
    mov ax, 3D00h
    checked_int21
    jc failed
    mov bx, ax
    mov ah, 3Eh
    checked_int21
    jc failed
    mov edx, fcb_rename_back
    mov ah, 17h
    checked_int21
    test al, al
    jnz failed
    mov edx, fcb_delete
    mov ah, 13h
    checked_int21
    test al, al
    jnz failed
    mov edx, fcb_delete
    mov ah, 13h
    checked_int21
    cmp al, 0FFh
    jne failed
    cmp byte [fcb_guard], 0A5h
    jne failed
    push ds
    mov ds, [ss:fcb_old_dta_sel]
    movzx edx, word [ss:fcb_old_dta_off]
    mov ah, 1Ah
    checked_int21
    pop ds
    push es
    mov ah, 2Fh
    int 21h
    mov ax, es
    cmp ax, [ss:fcb_old_dta_sel]
    jne failed
    cmp bx, [ss:fcb_old_dta_off]
    jne failed
    pop es
    mov edx, fcb_parent
    mov ah, 3Bh
    checked_int21
    jc failed
    xor edx, edx
    mov esi, fcb_restored_directory
    mov ah, 47h
    checked_int21
    jc failed
    mov esi, fcb_old_directory
    mov edi, fcb_restored_directory
    mov ecx, 64
.fcb_compare_directory:
    a32 lodsb
    a32 scasb
    jne failed
    test al, al
    jz .fcb_directory_restored
    dec ecx
    jnz .fcb_compare_directory
    jmp failed
.fcb_directory_restored:
%else
    mov ah, 41h
    checked_int21
    jc failed
%endif
    mov edx, filename
    mov ax, 3D00h
    checked_int21
    jnc failed
    cmp ax, 2                 ; Missing file must retain CF and DOS error.
    jne failed
%ifdef EXTENDED_OPEN
    mov esi, filename
    xor ebx, ebx
    xor ecx, ecx
    mov edx, 1
    mov ax, 6C00h
    checked_int21
    jnc failed
    cmp ax, 2
    jne failed
%endif
%ifdef HIGH_BUFFER
    mov ax, [cs:initial_ds]
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov bx, [memory_selector]
    mov ax, 0001h
    int 31h
    jc failed
    mov si, [memory_handle+2]
    mov di, [memory_handle]
    mov ax, 0502h
    int 31h
    jc failed
%endif
    mov edx, success
    mov ah, 09h
    checked_int21
%ifdef TSR_EXIT
    ; Independent probe residency only; install no interrupt hooks.
%ifdef TSR_PM_RETENTION
    xor bx, bx
    mov cx, 4096
    mov ax, 0501h
    int 31h
    jc failed
    mov [tsr_pm_base], cx
    mov [tsr_pm_base+2], bx
    mov [tsr_pm_handle], di
    mov [tsr_pm_handle+2], si
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [tsr_pm_selector], ax
    mov bx, ax
    mov cx, [tsr_pm_base+2]
    mov dx, [tsr_pm_base]
    mov ax, 0007h
    int 31h
    jc failed
    xor cx, cx
    mov dx, 4095
    mov ax, 0008h
    int 31h
    jc failed
    mov es, [tsr_pm_selector]
    mov dword [es:0], 38525354h
    push ds
    pop es
    mov dx, tsr_record_name
    mov ax, 3D01h
    int 21h
    jc failed
    mov bx, ax
    mov dx, tsr_record
    mov cx, 16
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, 16
    jne failed
    mov ah, 3Eh
    int 21h
    jc failed
%endif
    mov dx, (image_end-$$+100h+15)/16
    mov ax, 3100h
    int 21h
    jmp failed
%endif
    mov ax, 4C00h
    int 21h
failed:
    push cs
    pop ds
    mov edx, failure
    mov ah, 09h
    int 21h
%ifdef POINTER_QUERY
    mov dl, [cs:pointer_stage]
    mov ah, 02h
    int 21h
%assign pointer_value 0
%rep 4
    mov esi, [cs:dbcs_real_linear + pointer_value*4]
%rep 8
    rol esi, 4
    mov ebx, esi
    and ebx, 0Fh
    mov dl, [cs:pointer_hex+ebx]
    mov ah, 02h
    int 21h
%endrep
%assign pointer_value pointer_value+1
%endrep
%endif
    mov ax, 4C01h
    int 21h
entry dd 0
%ifdef CODE32
code_entry dd code32_start
    dw 0
code_descriptor times 8 db 0
%endif
saved_sp dd 0
saved_ss dw 0
saved_ds dw 0
saved_es dw 0
saved_fs dw 0
saved_gs dw 0
handle dw 0
%ifdef NETWORK_DPL
network_dpl dw 5,0803h,0200h,0,0,0,0,0,0,0,0
network_dpl_guard db 0A5h
network_truncate_saved db 0
network_spool_ok db 'S38_NETWORK_SPOOL_FLAG_REJECT_RESTORE_OK',13,10,'$'
network_dpl_ok db 'S38_NETWORK_DPL_ERROR_ROUNDTRIP_OK',13,10,'$'
%ifdef NETWORK_DPL_POINTER
network_expected_pointer dd 0
network_pointer_ok db 'S38_NETWORK_ERROR_POINTER_LINEAR_OK',13,10,'$'
%endif
%endif
%ifdef NETWORK_PRINT
print_ax times 2 dw 0
print_cf times 2 dw 0
print_real times 65 db 0A5h
print_pm times 65 db 0A5h
print_ok db 'S38_NETWORK_PRINT_REAL_PM_EQUAL_OK',13,10,'$'
print_hex db '0123456789ABCDEF'
%endif
%ifdef NETWORK_ASSIGN_CREATE
assign_target db 'Z:',0
assign_share db '\\LOCALHOST\O$',0
assign_index dw 0
assign_create_ok db 'S38_NETWORK_ASSIGN_CREATE_QUERY_BREAK_OK',13,10,'$'
%endif
%ifdef NETWORK_ASSIGN
assign_local times 16 db 0A5h
assign_remote times 128 db 0A5h
assign_guard db 0A5h
assign_ok db 'S38_NETWORK_ASSIGN_END_PRESERVED_OK',13,10,'$'
%endif
%ifdef NETWORK_NAME
network_baseline times 16 db 0
network_name times 16 db 0A5h
network_guard db 0A5h
network_name_ok db 'S38_NETWORK_NAME_REAL_PM_EQUAL_OK',13,10,'$'
%endif
%ifdef EXEC_FAILURE
exec_parent dw 0
%ifdef EXEC_INHERIT
inherit_name db 'tests\D38INH.TMP',0
inherit_read dd 0
inherit_vectors times 2 dd 0
inherit_ok db 'S38_EXEC_INHERITED_HANDLE_PARENT_SURVIVES_OK',13,10,'$'
%endif
exec_environment dw 0
exec_missing db 'tests\NOEXEC38.EXE',0
%ifdef EXEC_CHILD
exec_child db 'tests\D38CH.COM',0
%endif
%ifdef CLIENT32
exec_params dd exec_tail
    dw 0
%else
exec_params dw 0,exec_tail,0
%endif
%ifdef EXEC_LOAD_ONLY
    times 16 db 0
%else
    times 8 db 0
%endif
exec_tail db 3,' OK',13
    times 123 db 0
exec_failure_ok db 'S38_INT21_EXEC_FAILURE_PARENT_ENV_OK',13,10,'$'
%ifdef EXEC_LOAD_ONLY
exec_load_missing db 'S38_EXEC_LOAD_ONLY_CF0_OUTPUT_UNCHANGED',13,10,'$'
exec_load_checkpoint db 'tests\L38PRE.OUT',0
exec_load_returned db 'tests\L38POST.OUT',0
%ifdef EXEC_LOAD_CLEANUP
exec_load_image dw 0
exec_load_environment dw 0
exec_load_regs times 50 db 0
exec_load_clean_ok db 'S38_EXEC_LOAD_ONLY_RETURN_FREE_OK',13,10,'$'
%endif
%endif
%endif
%ifdef CREATE_PSP
created_parent dw 0
created_selector dw 0
created_segment dw 0
%ifdef HIGH_BUFFER
created_prefix dd 0
create_psp_high_ok db 'S38_INT21_CREATE_PSP_HIGH_UNCHANGED_OK',13,10,'$'
%endif
create_psp_ok db 'S38_INT21_CREATE_PSP_FIELDS_FREE_OK',13,10,'$'
%endif
%ifdef IOCTL_METADATA_MOCK
metadata_packet times 26 db 0
metadata_ok db 'S38_INT21_METADATA_MOCK_PAIRS_OK',13,10,'$'
%endif
%ifdef IOCTL_MEDIA_QUERY
media_real times 25 db 0
media_pm dw 0
    times 23 db 0A5h
media_guard dw 0A55Ah
media_real_ok db 'S38_IOCTL_MEDIA_REAL_OK',13,10,'$'
media_ok db 'S38_INT21_IOCTL_MEDIA_QUERY_OK',13,10,'$'
%endif
%ifdef IOCTL_FORMAT_MOCK
%ifdef FORMAT_EXTENDED
format_packet db 3
    dw 1,2,3
%else
format_packet db 1
    dw 1,2
%endif
format_guard dw 0A55Ah
format_ok db 'S38_INT21_FORMAT_MOCK_PACKET_OK',13,10,'$'
%endif
%ifdef IOCTL_TRACK_MOCK
%ifdef TRACK_FAIL
track_packet db 80h
%else
track_packet db 0
%endif
    dw 0,0,0,TRACK_SECTORS
%ifdef CLIENT32
    dd track_data
%else
    dw track_data
%endif
    dw 0
%ifdef TRACK_WRITE
track_data times TRACK_SECTORS*512 db 5Ah
%else
track_data times TRACK_SECTORS*512 db 0
%endif
track_guard db 0A5h
track_ok db 'S38_INT21_TRACK_MOCK_TRANSFER_OK',13,10,'$'
%endif
%ifdef IOCTL_PRINTER
printer_name db 'PRN',0
printer_expected dw 0
printer_count dw 0
printer_guard dw 0A55Ah
printer_real_ok db 'S38_PRINTER_REAL_READ_OK',13,10,'$'
printer_return_ok db 'S38_PRINTER_PM_RETURN_OK',13,10,'$'
printer_ok db 'S38_INT21_PRINTER_IOCTL_READ_OK',13,10,'$'
%endif
%ifdef IOCTL_FAILURE
ioctl_buffer db 0A5h,5Ah,0A5h,0A5h
%endif
%ifdef KEYBOARD_INPUT
keyboard_ready db 'S38_KEYBOARD_READY',13,10,'$'
keyboard_buffer db 8,0
    times 8 db 0
keyboard_guard db 0A5h
%endif
%ifdef DRIVE_QUERY
drive_linear times 4 dd 0
drive_result times 4 db 0
drive_offset dd 0
%endif
%ifdef POINTER_QUERY
indos_real_linear dd 0
list_real_linear dd 0
indos_offset dd 0
dbcs_real_linear dd 0
dbcs_pm_linear dd 0
dbcs_offset dd 0
dbcs_selector dd 0
pointer_stage db '0'
pointer_hex db '0123456789ABCDEF'
%endif
%ifdef COUNTRY_QUERY
country_id dw 0
country_real times 34 db 0
country_pm times 34 db 0
country_guard db 0A5h
country_extended_length dw 0
country_extended_real times 41 db 0
country_extended_pm times 41 db 0
country_extended_guard db 0A5h
%endif
%ifdef TEMP_FILE
temp_path db 'tests\',0
    times 12 db 0
temp_guard db 0A5h
%endif
%ifdef FCB_FAMILY
fcb_directory db 'tests',0
fcb_parent db '..',0
fcb_leaf db 'D38IO.TMP',0
fcb_new_leaf db 'D38FCB.TMP',0
fcb_old_dta_sel dw 0
fcb_old_dta_off dw 0
fcb_old_directory times 64 db 0
fcb_restored_directory times 64 db 0
fcb_query:
%ifdef EXTENDED_FCB
    db 0FFh,0,0,0,0,0,0
%endif
    db 0,'D38IO   TMP'
    times 25 db 0
fcb_delete:
%ifdef EXTENDED_FCB
    db 0FFh,0,0,0,0,0,0
%endif
    db 0,'D38IO   TMP'
    times 25 db 0
fcb_rename db 0,'D38IO   TMP'
    times 5 db 0
    db 'D38FCB  TMP'
    times 9 db 0
fcb_rename_back db 0,'D38FCB  TMP'
    times 5 db 0
    db 'D38IO   TMP'
    times 9 db 0
fcb_dta times 44 db 0A5h
fcb_guard db 0A5h
%endif
%ifdef PARSE_FAMILY
parse_name db 'D38IO.TMP',0
    times 10 db 0
parse_expected db 'D38IO   TMP'
parse_wildcard db 'D38*.TMP',0
    times 11 db 0
parse_wild_expected db 'D38?????TMP'
parse_fcb times 37 db 0
parse_guard db 0A5h
%endif
%ifdef PSP_FAMILY
original_psp dw 0
%endif
%ifdef HIGH_BUFFER
initial_ds dw 0
memory_handle dd 0
memory_base dd 0
memory_selector dw 0
%endif
filename db 'tests\D38IO.TMP',0
%ifdef FIND_FAMILY
old_dta_selector dw 0
old_dta_offset dw 0
find_dta times 43 db 0A5h
find_guard db 0A5h
found_name db 'D38IO.TMP',0
directory db 'tests\D38DIR',0
%ifdef DIRECTORY_NAV
parent_directory db '..\..',0
directory_leaf db 'D38DIR',0
old_directory times 64 db 0
current_directory times 64 db 0
directory_guard db 0A5h
%endif
%endif
%ifdef FILE_FAMILY
renamed db 'tests\D38RN.TMP',0
%endif
payload db 0,1,2,3,'S38_PROTECTED_FILE',13,10,0FFh
%ifdef HIGH_BUFFER
times 8193 db 5Ah
%endif
payload_end:
buffer times payload_end-payload db 0
%ifdef HIGH_BUFFER
times 17 db 0A5h
%endif
success:
%ifdef IOCTL_FAILURE
    db 'S38_INT21_IOCTL_INVALID_HANDLE_BUFFER_OK',13,10
%endif
%ifdef KEYBOARD_INPUT
    db 'S38_INT21_BUFFERED_KEYBOARD_CONTENT_GUARD_OK',13,10
%ifdef KEYBOARD_DISCARD
    db 'S38_INT21_QUEUED_KEY_DISCARDED_OK',13,10
%ifdef KEYBOARD_STATUS
    db 'S38_INT21_NONBLOCKING_EMPTY_ZF_OK',13,10
%endif
%endif
%ifdef KEYBOARD_EMPTY
    db 'S38_INT21_KEYBOARD_EMPTY_OK',13,10
%endif
%ifdef KEYBOARD_OVERFLOW
    db 'S38_INT21_KEYBOARD_CAPACITY_OK',13,10
%endif
%ifdef KEYBOARD_FLUSH
    db 'S38_INT21_FLUSH_THEN_BUFFERED_INPUT_OK',13,10
%endif
%endif
%ifdef DRIVE_QUERY
    db 'S38_INT21_DRIVE_POINTERS_REAL_PM_EQUAL_OK',13,10
    db 'S38_INT21_INVALID_DRIVE_REJECT_OK',13,10
%endif
%ifdef DOS_MEMORY
    db 'S38_INT21_MEMORY_GROW_SHRINK_FREE_REJECT_OK',13,10
%endif
%ifdef POINTER_QUERY
    db 'S38_INT21_INDOS_SELECTOR_LINEAR_OK',13,10
    db 'S38_INT21_LIST_SELECTOR_LINEAR_OK',13,10
    db 'S38_INT21_DBCS_SELECTOR_LINEAR_OK',13,10
    db 'S38_INT21_NON_DBCS_SUBFUNCTION_STATE_OK',13,10
%endif
%ifdef COUNTRY_QUERY
    db 'S38_INT21_COUNTRY_REAL_PM_EQUAL_OK',13,10
    db 'S38_INT21_EXTENDED_COUNTRY_EQUAL_OK',13,10
%endif
%ifdef TEMP_FILE
    db 'S38_INT21_TEMP_CREATE_REOPEN_DELETE_OK',13,10
%endif
%ifdef EXTENDED_OPEN
    db 'S38_INT21_EXTENDED_OPEN_OK',13,10
%endif
%ifdef FCB_FAMILY
    db 'S38_INT21_FCB_FIND_RENAME_DELETE_OK',13,10
    db 'S38_INT21_FCB_DTA_DIRECTORY_RESTORED_OK',13,10
%ifdef EXTENDED_FCB
    db 'S38_INT21_EXTENDED_FCB_OK',13,10
%endif
%endif
%ifdef PARSE_FAMILY
    db 'S38_INT21_PARSE_FCB_OK',13,10
    db 'S38_INT21_PARSE_WILDCARD_OK',13,10
%endif
%ifdef BPB_REFUSAL
    db 'S38_INT21_BPB_REFUSAL_OK',13,10
%endif
%ifdef PSP_FAMILY
    db 'S38_INT21_PSP_IDENTITY_OK',13,10
%ifdef HIGH_BUFFER
    db 'S38_INT21_PSP_HIGH_REJECT_PRESERVE_OK',13,10
%endif
%endif
%ifdef DIRECTORY_NAV
    db 'S38_INT21_DIRECTORY_RESTORE_OK',13,10
%endif
%ifdef FIND_FAMILY
    db 'S38_INT21_DTA_FIND_DIRECTORY_OK',13,10
%endif
%ifdef FILE_FAMILY
    db 'S38_INT21_FILE_FAMILY_OK',13,10
%endif
%ifdef HIGH_COUNT
    db 'S38_INT21_COUNT_ABOVE_64K_OK',13,10
%endif
%ifdef CODE32
    db 'S38_INT21_CODE32_OK',13,10
%endif
%ifdef HIGH_OFFSET
    db 'S38_INT21_EDX_ABOVE_64K_OK',13,10
%endif
%ifdef CLIENT32
    db 'S38_INT21_CLIENT32_STATE_OK',13,10
%endif
%ifdef HIGH_BUFFER
    db 'S38_INT21_HIGH_BUFFER_CHUNKS_EOF_CLEANUP_OK',13,10
%endif
    db 'S38_INT21_IO_STATE_FAILURE_CLEANUP_OK',13,10,'$'
failure db 'S38_FAIL_INT21_IO_STATE',13,10,'$'
%ifdef EXEC_LOAD_CALL
exec_load_call_ok db 'S38_EXEC_LOADED_ENTRY_STACK_TAIL_CALL_OK',13,10,'$'
%endif
%ifdef DOS_MEMORY_FAILURE_TRACE
memory_result times 3 dw 0
memory_digits db '0123456789ABCDEF'
memory_result_label db 'S38_MEMORY_AX_BX_FLAGS $'
%endif
%ifdef IOCTL_CONTROL_MOCK
control_packet times 16 db 0
    db 0A5h
control_ok db 'S38_IOCTL_CONTROL_FULL_SHORT_FAILURE_OK',13,10,'$'
%endif
%ifdef IOCTL_DEVICE_MOCK
device_packet times 49 db 0
device_ok db 'S38_IOCTL_DEVICE_LENGTH_RETURN_GUARD_OK',13,10,'$'
%endif
%ifdef TSR_EXIT
tsr_record_name db 'tests\D38TSR.DAT',0
tsr_record dw 0, (image_end-$$+100h+15)/16, tsr_sentinel
%ifdef TSR_PM_RETENTION
tsr_pm_selector dw 0
tsr_pm_base dd 0
tsr_pm_handle dd 0
%endif
tsr_sentinel db 'S38-TSR-RESIDENT'
%endif
times 4096 db 0
stack_top:
image_end:
