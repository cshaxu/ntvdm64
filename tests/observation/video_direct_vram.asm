; Test-only DOS COM guest for the selected CCPU40/C-VID text-memory path.
; It directly writes and reads B800: text memory: no INT 10h write service is
; used.  The markers are deliberately the same stable Console markers as the
; INT 10 witness so the existing independent ConPTY observer can capture them.

bits 16
org 100h

start:
    ; Enter the original 80x25 colour text-mode setup first.  This is not a
    ; write surrogate: its purpose is to establish the selected mode/update
    ; state before the direct B800: reads and writes below.
    mov ax, 0003h
    int 10h

    mov ax, 0b800h
    mov es, ax

    ; Directly write four attributed cells in the selected text plane.
    mov di, 06b8h             ; row 10, column 60: (10 * 80 + 60) * 2
    mov ax, 1f56h             ; white-on-blue 'V'
    mov cx, 4
    rep stosw

    ; A direct read must see the same text-plane cells before DOS emits any
    ; transcript marker.  This is the guest-visible read/write contract.
    mov di, 06b8h
    mov cx, 4
.verify_v:
    cmp word [es:di], 1f56h
    jne failed
    add di, 2
    loop .verify_v

    call visible_delay

    ; A second direct range proves that the observation is not a one-cell
    ; store artefact.  The final I cell is deliberately a different value.
    mov di, 0b78h             ; row 18, column 60
    mov ax, 1f53h             ; 'S'
    stosw
    mov ax, 1f32h             ; '2'
    stosw
    mov ax, 1f33h             ; '3'
    stosw
    mov ax, 1f49h             ; 'I'
    stosw

    mov di, 0b78h
    mov ax, [es:di]
    cmp ax, 1f53h
    jne failed
    mov ax, [es:di+6]
    cmp ax, 1f49h
    jne failed

    call visible_delay
    mov dx, completion
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

completion db 'S23_INT10_WRITER_OK$'
failure    db 'S23_DIRECT_VRAM_FAIL$'

; Give the independently sampled Console a chance to capture each direct
; memory state without touching the worker's video path.
visible_delay:
    push cx
    mov cx, 20h
.outer:
    push cx
    mov cx, 0ffffh
.inner:
    loop .inner
    pop cx
    loop .outer
    pop cx
    ret
