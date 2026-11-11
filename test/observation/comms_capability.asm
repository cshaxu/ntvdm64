; Test-only DOS COM/LPT witness for the selected standalone profile.
; It deliberately uses only original INT 14h/17h dispatch.  It neither
; installs a serial/printer provider nor writes to an attached device.

bits 16
org 100h

start:
    mov dx, step_com
    mov ah, 09h
    int 21h

    ; INT 14h function 00: initialize COM1.  This reaches the original
    ; rs232_io -> UART -> lazy host_com_open path, which either binds a real
    ; configured COM1 device or preserves its original unavailable status.
    mov ax, 00E3h
    xor dx, dx
    int 14h
    mov [com_ah], ah
    mov [com_al], al

    ; Function 03 reads the same original line/modem status path.
    mov ah, 03h
    xor dx, dx
    int 14h
    mov [status_ah], ah
    mov [status_al], al

    mov dx, step_lpt
    mov ah, 09h
    int 21h

    ; INT 17h function 01 initializes LPT1; function 02 reads its original
    ; status-table path.  No printable byte is sent.
    mov ah, 01h
    xor dx, dx
    int 17h
    mov [lpt_init], ah
    mov ah, 02h
    xor dx, dx
    int 17h
    mov [lpt_status], ah

    mov dx, success_text
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h

step_com     db 'S29_STEP_COM $'
step_lpt     db 'S29_STEP_LPT $'
success_text db 'S29_INT14_OK S29_INT17_OK S29_COMMS_OK$'
com_ah       db 0
com_al       db 0
status_ah    db 0
status_al    db 0
lpt_init     db 0
lpt_status   db 0
