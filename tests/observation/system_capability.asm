; Test-only DOS COM guest for selected original system devices.  It never
; installs a device replacement: every operation is a normal guest BIOS,
; port-I/O or ROM-memory access.

bits 16
org 100h

start:
    ; BIOS tick before and after a normal CPL3 instruction workload.  HLT is
    ; deliberately privileged in the selected CCPU40 profile, so this proves
    ; ordinary PIC delivery without treating an invalid HLT resume as timing.
    mov ah, 00h
    int 1ah
    mov [tick_low], dx
    mov bp, 96
.wait_tick:
    mov cx, 0ffffh
.spin:
    loop .spin
    dec bp
    jnz .wait_tick
    mov ah, 00h
    int 1ah
    cmp dx, [tick_low]
    je fail_timer

    ; RTC register D always reports the original CMOS valid-RAM bit.
    mov al, 0dh
    out 70h, al
    in al, 71h
    test al, 80h
    jz fail_cmos

    ; PIC mask is a normal port round-trip; restore exactly its old state.
    in al, 21h
    mov [pic_mask], al
    out 21h, al
    in al, 21h
    cmp al, [pic_mask]
    jne fail_pic

    ; DMA status port is selected and readable.  The later S24 transfer probe
    ; owns channel programming/SAS-copy semantics separately.
    in al, 08h
    mov [dma_status], al

    ; The generic original ROM initializer must publish a readable reset ROM.
    mov ax, 0f000h
    mov es, ax
    mov al, [es:0fff0h]
    cmp al, 0ffh
    je fail_rom
    cmp al, 00h
    je fail_rom

    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

fail_timer:
    mov dx, timer_failure
    jmp short failure
fail_cmos:
    mov dx, cmos_failure
    jmp short failure
fail_pic:
    mov dx, pic_failure
    jmp short failure
fail_rom:
    mov dx, rom_failure
failure:
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

tick_low dw 0
pic_mask db 0
dma_status db 0
success db 'S24_TIMER_OK S24_CMOS_OK S24_PIC_OK S24_DMA_PORT_OK S24_ROM_READ_OK S24_SYSTEM_OK$'
timer_failure db 'S24_TIMER_FAIL$'
cmos_failure db 'S24_CMOS_FAIL$'
pic_failure db 'S24_PIC_FAIL$'
rom_failure db 'S24_ROM_FAIL$'
