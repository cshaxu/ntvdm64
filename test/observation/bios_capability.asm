; Test-only DOS COM witness for the selected original BIOS package.  It uses
; the original ROM interrupt vectors and BOP encodings; it installs no handler
; and never changes guest media or the product image.

bits 16
org 100h

start:
    ; reset.c publishes these ROM vectors during ordinary worker startup.
    xor ax, ax
    mov es, ax
    cmp word [es:11h * 4], 0
    je fail_reset
    cmp word [es:12h * 4], 0
    je fail_reset
    cmp word [es:15h * 4], 0
    je fail_reset

    ; The ROM stubs invoke equipment() and memory_size(), which must return
    ; exactly the mutable values currently held in the original BDA.
    int 11h
    mov bx, ax
    mov ax, 40h
    mov es, ax
    cmp bx, [es:10h]
    jne fail_equipment
    int 12h
    mov bx, ax
    cmp bx, [es:13h]
    jne fail_memory

    ; These cases take the original cassette_io() BOP path, not the special
    ; all-assembly INT 15h wait paths.
    mov ah, 80h
    int 15h
    jc fail_cassette_ok
    or ah, ah
    jnz fail_cassette_ok
    mov ah, 85h
    int 15h
    jc fail_cassette_ok
    or ah, ah
    jnz fail_cassette_ok
    mov ah, 0c0h
    int 15h
    jc fail_configuration
    or ah, ah
    jnz fail_configuration
    mov ax, es
    or ax, bx
    jz fail_configuration

    ; Invalid cassette function is the documented original failure contract.
    mov ah, 24h
    int 15h
    jnc fail_cassette_error
    cmp ah, 86h
    jne fail_cassette_error

    ; BOP 21 is the original Get_build_id() ABI: AL is module number; DS:CX
    ; receives Base$, BX receives its nonzero build identifier.
    push cs
    pop ds
    mov cx, build_name
    mov al, 1
    db 0c4h, 0c4h, 21h
    or al, al
    jnz fail_build_id
    or bx, bx
    jz fail_build_id
    cmp byte [build_name], 'B'
    jne fail_build_id
    cmp byte [build_name + 4], '$'
    jne fail_build_id

    ; In this NTVDM profile BOP FF has no generic base function zero.  Its
    ; original cntlbop.c error response is AX=ERR_NO_FUNCTION (1).
    mov ax, 0100h
    db 0c4h, 0c4h, 0ffh
    cmp ax, 1
    jne fail_control

    ; The selected profile starts COMMAND with PIF EMS size zero.  Original
    ; InitSpcEmm therefore deliberately declines installation; do not call
    ; whatever default INT 67h vector remains.  S26's source ledger records
    ; the original config->BOP66->InitSpcEmm conditional as profile-null.
    mov dx, success_text
success:
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

fail_reset:             mov dx, reset_failure
                        jmp short failure
fail_equipment:         mov dx, equipment_failure
                        jmp short failure
fail_memory:            mov dx, memory_failure
                        jmp short failure
fail_cassette_ok:       mov dx, cassette_ok_failure
                        jmp short failure
fail_configuration:     mov dx, configuration_failure
                        jmp short failure
fail_cassette_error:    mov dx, cassette_error_failure
                        jmp short failure
fail_build_id:          mov dx, build_id_failure
                        jmp short failure
fail_control:           mov dx, control_failure
                        jmp short failure
failure:
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

build_name times 5 db 0
success_text db 'S26_RESET_VECTOR_OK S26_EQUIPMENT_OK S26_MEMORY_SIZE_OK S26_CASSETTE_OK S26_CASSETTE_ERROR_OK S26_BUILD_ID_OK S26_CONTROL_BOP_OK S26_EMM_PROFILE_NULL S26_BIOS_OK$'
reset_failure db 'S26_RESET_VECTOR_FAIL$'
equipment_failure db 'S26_EQUIPMENT_FAIL$'
memory_failure db 'S26_MEMORY_SIZE_FAIL$'
cassette_ok_failure db 'S26_CASSETTE_FAIL$'
configuration_failure db 'S26_CONFIGURATION_FAIL$'
cassette_error_failure db 'S26_CASSETTE_ERROR_FAIL$'
build_id_failure db 'S26_BUILD_ID_FAIL$'
control_failure db 'S26_CONTROL_BOP_FAIL$'
