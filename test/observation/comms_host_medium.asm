; S30 DOS guest witness for a genuine Windows serial endpoint.
; COM3 is deliberately selected because the host inventory records a real
; USB serial device at that original guest-to-host ordinal.  This fixture
; initializes, sends one NUL byte and queries the port.  NUL avoids inventing
; an external device protocol while still reaching the original TX path.

bits 16
org 100h

start:
    ; INT 14h / AH=00 is the original guest serial initialization route.
    ; DX=2 selects COM3, whose original host provider opens "COM3".
    mov ax, 00E3h
    mov dx, 2
    int 14h
    test ah, 80h
    jnz failed

    ; INT 14h / AH=01 is the original serial transmit route.  The host
    ; provider's normal completion is indicated by a clear timeout bit.
    mov ax, 0100h
    mov dx, 2
    int 14h
    test ah, 80h
    jnz failed

    ; Verify that the same original provider also answers line/modem status.
    mov ah, 03h
    mov dx, 2
    int 14h
    test ah, 80h
    jnz failed

    mov dx, success_text
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h

failed:
    mov dx, failure_text
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h

success_text db 'S30_COM3_OPEN_OK S30_COM3_TX_OK$'
failure_text db 'S30_COM3_OPEN_OR_TX_FAIL$'
