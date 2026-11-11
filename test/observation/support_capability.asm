; Test-only DOS COM witness for the selected original SoftPC support package.
; It uses ordinary ROM interrupt and port instructions; it installs no hook,
; changes no guest media, and restores the BIOS tick counter after observing
; the original time_of_day BOP path.

bits 16
org 100h

start:
    ; INT 1Ah AH=00 reaches the original time_of_day() service.  Preserve the
    ; ordinary BDA tick count, install a controlled value through AH=01, then
    ; immediately read it back and restore the original count.
    mov ah, 00h
    int 1ah
    mov [old_low], dx
    mov [old_high], cx

    mov cx, 1234h
    mov dx, 5678h
    mov ah, 01h
    int 1ah
    mov ah, 00h
    int 1ah
    cmp cx, 1234h
    jne fail_ticks
    cmp dx, 5678h
    jb fail_ticks

    mov cx, [old_high]
    mov dx, [old_low]
    mov ah, 01h
    int 1ah

    ; The same service's RTC read paths must report an ordinary completed
    ; transaction.  The exact RTC values are host time and are deliberately
    ; not frozen by this witness.
    mov ah, 02h
    int 1ah
    jc fail_rtc_time
    mov ah, 04h
    int 1ah
    jc fail_rtc_date

    ; These reads take the I/O adapter tables initialized by io_init() and
    ; subsequently connected by the original PPI/CMOS/video providers.  They
    ; are read-only and make no assertion about the host-specific values.
    in al, 61h
    mov al, 0ah
    out 70h, al
    in al, 71h
    mov dx, 03dah
    in al, dx

    mov dx, success_text
success:
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

fail_ticks:     mov dx, ticks_failure
                jmp short failure
fail_rtc_time:  mov dx, rtc_time_failure
                jmp short failure
fail_rtc_date:  mov dx, rtc_date_failure
failure:
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

old_low dw 0
old_high dw 0
success_text db 'S27_TIME_TICKS_OK S27_RTC_TIME_OK S27_RTC_DATE_OK S27_IOS_PORTS_OK S27_SUPPORT_OK$'
ticks_failure db 'S27_TIME_TICKS_FAIL$'
rtc_time_failure db 'S27_RTC_TIME_FAIL$'
rtc_date_failure db 'S27_RTC_DATE_FAIL$'
