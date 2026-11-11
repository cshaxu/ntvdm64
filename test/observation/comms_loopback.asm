; S30 original UART loopback witness.  It first opens real host COM3 through
; INT 14h, then uses its original 8250 register interface to set MCR loopback
; and prove a TX byte is returned through RX without an external peer.

bits 16
org 100h

start:
    mov ax, 03E3h
    mov dx, 2
    int 14h

    mov dx, 3ebh                 ; COM3 LCR
    mov al, 03h                  ; 8N1
    out dx, al
    mov dx, 3ech                 ; COM3 MCR
    mov al, 13h                  ; DTR | RTS | loopback
    out dx, al
    mov dx, 3e8h                 ; COM3 TX/RX
    mov al, 5ah
    out dx, al

    mov cx, 0ffffh
wait_rx:
    mov dx, 3edh                 ; COM3 LSR
    in al, dx
    test al, 01h
    jnz read_rx
    loop wait_rx
    jmp failed

read_rx:
    mov dx, 3e8h
    in al, dx
    cmp al, 5ah
    jne failed
    mov dx, success_text
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

failed:
    mov dx, failure_text
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

success_text db 'S30_COM3_LOOPBACK_TX_RX_OK$'
failure_text db 'S30_COM3_LOOPBACK_TX_RX_FAIL$'
