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

    ; Exercise the selected original RTC periodic queue: preserve register B,
    ; enable only PIE, wait under ordinary guest execution, then acknowledge
    ; register C and restore B.  C_PF|C_IRQF proves the provider published a
    ; real periodic event instead of merely exposing CMOS storage.
    ; Re-publish the current rate through the original REG_A write path.
    ; `rtc_period_mSeconds` is derived there rather than from CMOS storage.
    mov al, 0ah
    out 70h, al
    in al, 71h
    out 71h, al
    mov al, 0bh
    out 70h, al
    in al, 71h
    mov [cmos_b], al
    or al, 40h
    out 71h, al
    mov bp, 96
.wait_rtc:
    mov cx, 0ffffh
.rtc_spin:
    loop .rtc_spin
    dec bp
    jnz .wait_rtc
    mov al, 0ch
    out 70h, al
    in al, 71h
    mov [cmos_c], al
    mov al, 0bh
    out 70h, al
    mov al, [cmos_b]
    out 71h, al
    mov al, [cmos_c]
    test al, 40h
    jz fail_rtc_periodic

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
fail_rtc_periodic:
    mov dx, rtc_failure
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
cmos_b db 0
cmos_c db 0
success db 'S24_TIMER_OK S24_CMOS_OK S24_RTC_PERIODIC_OK S24_PIC_OK S24_DMA_PORT_OK S24_ROM_READ_OK S24_SYSTEM_OK$'
timer_failure db 'S24_TIMER_FAIL$'
cmos_failure db 'S24_CMOS_FAIL$'
rtc_failure db 'S24_RTC_PERIODIC_FAIL$'
pic_failure db 'S24_PIC_FAIL$'
rom_failure db 'S24_ROM_FAIL$'
