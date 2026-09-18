; Test-only DOS COM guest for the selected original video package.
; It enters the real INT 10h BIOS services supplied by the worker:
; AH=09h writes four attributed characters, then AH=06h scrolls their region.
; The DOS marker merely makes successful guest completion unambiguous in the
; captured Console transcript; it is not a substitute video provider.

bits 16
org 100h

start:
    ; Put a visible, attributed cell run in a known text-mode rectangle.
    mov ah, 02h
    xor bh, bh
    mov dh, 10h
    mov dl, 3ch
    int 10h

    mov ah, 09h
    mov al, 'V'
    mov bl, 1fh
    mov cx, 0004h
    xor bh, bh
    int 10h

    call visible_delay

    ; Scroll that exact rectangle up by one row, preserving the original
    ; BIOS service's source-selected text/video memory path.
    mov ax, 0601h
    mov bh, 1fh
    mov cx, 0f3ch
    mov dx, 103fh
    int 10h

    call visible_delay

    ; A post-scroll marker emitted through the original video teletype path
    ; proves that the observed Console cells are not merely DOS stdout.
    mov ah, 02h
    xor bh, bh
    mov dh, 12h
    mov dl, 3ch
    int 10h
    mov si, video_marker
    mov cx, video_marker_end-video_marker
.write_marker:
    lodsb
    mov ah, 0eh
    xor bh, bh
    mov bl, 1fh
    int 10h
    loop .write_marker

    call visible_delay

    mov dx, completion
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

completion db 'S23_INT10_WRITER_OK$'
video_marker db 'S23I'
video_marker_end:

; Give the independently sampled Console a chance to capture each state
; without calling a host sleep or changing the production video path.
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
