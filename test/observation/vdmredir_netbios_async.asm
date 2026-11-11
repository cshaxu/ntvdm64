; Real DOS INT 5Ch asynchronous NetBIOS NCBADDNAME. A preceding synchronous
; NCBENUM selects an actual LANA exposed by the current host. The non-zero NCB
; post callback requires the original completion queue, simulated network IRQ
; and INT 5Ch callback route to return to this guest procedure.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es

    ; Discovery is immediate: find a real LANA rather than inventing LANA 0.
    mov byte [ncb+0], 37h        ; NCBENUM
    mov byte [ncb+1], 0ffh
    mov byte [ncb+49], 0ffh
    mov word [ncb+4], enum_buffer
    mov word [ncb+6], ds
    mov word [ncb+8], 512
    mov bx, ncb
    int 5ch
    mov [enum_value], al
    or al, al
    jnz no_lana
    mov al, [enum_buffer]
    mov [enum_count], al
    or al, al
    jz no_lana
    mov al, [enum_buffer+1]
    mov [lana_value], al

    ; VDMREDIR itself first performs the original synchronous ResetLana, then
    ; this real asynchronous NCBADDNAME must complete through the guest post.
    mov di, ncb
    mov cx, 32
    xor ax, ax
    rep stosw
    mov byte [ncb+0], 0b0h       ; NCBADDNAME | ASYNCH
    mov byte [ncb+1], 0ffh
    mov byte [ncb+49], 0ffh
    mov al, [lana_value]
    mov [ncb+48], al
    mov si, ncb_name
    mov di, ncb+26
    mov cx, 16
    rep movsb
    mov word [ncb+44], post_routine
    mov word [ncb+46], ds
    mov bx, ncb
    int 5ch
    mov [submit_value], al

    ; Yield through the original DOS idle boundary while waiting for the host
    ; callback thread. BIOS ticks provide the bounded timeout.
    mov ah, 00h
    int 1ah
    mov [start_tick], dx
async_wait:
    cmp byte [post_seen], 1
    je async_passed
    int 28h
    mov ah, 00h
    int 1ah
    sub dx, [start_tick]
    cmp dx, 36                  ; about two seconds, including tick rollover
    jb async_wait

    ; A pending asynchronous NetBIOS operation must be cancellable. This is a
    ; bounded way to make a host that has no peer complete its callback.
    cmp byte [cancel_issued], 0
    jne short async_timeout
    call issue_cancel
    mov byte [cancel_issued], 1
    mov ah, 00h
    int 1ah
    mov [start_tick], dx
    jmp short async_wait

async_timeout:
    call format_status
    mov dx, failed
    mov cx, failed_end-failed
    jmp short async_write
no_lana:
    call format_enum
    mov dx, unavailable
    mov cx, unavailable_end-unavailable
    jmp short async_write
async_passed:
    pushf
    pop ax
    mov byte [if_bit], '0'
    test ax, 0200h
    jz async_emit
    mov byte [if_bit], '1'
async_emit:
    call format_status
    mov dx, passed
    mov cx, passed_end-passed
    call write_stdout
    mov dx, status
    mov cx, status_end-status
    call write_stdout
    jmp short async_exit
async_write:
    call write_stdout
async_exit:
    mov ax, 4c00h
    int 21h

write_stdout:
    mov bx, 1
    mov ah, 40h
    int 21h
    ret

; The original INT 5Ch handler invokes this using PUSHF + FAR CALL; IRET is
; therefore required to restore the flags frame as the original ABI specifies.
post_routine:
    mov byte [cs:post_seen], 1
    iret

format_status:
    mov al, [submit_value]
    mov di, submit_hex
    call hex_byte_at
    mov al, [ncb+1]
    mov di, ret_hex
    call hex_byte_at
    mov al, [ncb+49]
    mov di, cplt_hex
    call hex_byte_at
    mov al, [cancel_value]
    mov di, cancel_hex
    call hex_byte_at
    ret

issue_cancel:
    mov di, cancel_ncb
    mov cx, 32
    xor ax, ax
    rep stosw
    mov byte [cancel_ncb+0], 35h ; NCBCANCEL
    mov word [cancel_ncb+4], ncb
    mov word [cancel_ncb+6], ds
    mov al, [lana_value]
    mov [cancel_ncb+48], al
    mov bx, cancel_ncb
    int 5ch
    mov [cancel_value], al
    ret

format_enum:
    mov al, [enum_value]
    mov di, enum_hex
    call hex_byte_at
    mov al, [enum_count]
    mov di, count_hex
    call hex_byte_at
    ret

hex_byte_at:
    push ax
    mov ah, al
    shr al, 4
    call hex_nibble
    mov [di], al
    mov al, ah
    call hex_nibble
    mov [di+1], al
    pop ax
    ret

hex_nibble:
    and al, 0fh
    add al, '0'
    cmp al, '9'
    jbe short hex_nibble_done
    add al, 7
hex_nibble_done:
    ret

ncb times 64 db 0
cancel_ncb times 64 db 0
enum_buffer times 512 db 0
ncb_name db 'NTVDM420ASYNC   '
post_seen db 0
start_tick dw 0
submit_value db 0
enum_value db 0ffh
enum_count db 0
lana_value db 0ffh
cancel_issued db 0
cancel_value db 0ffh
passed db 'NETBIOS-ASYNC-POST-OK IF='
if_bit db '0',13,10
passed_end:
status db 'NETBIOS-ASYNC-STATUS AL='
submit_hex db '00 RET='
ret_hex db '00 CPLT='
cplt_hex db '00 CANCEL='
cancel_hex db '00',13,10
status_end:
failed db 'NETBIOS-ASYNC-POST-TIMEOUT',13,10
failed_end:
unavailable db 'NETBIOS-ASYNC-NO-LANA ENUM='
enum_hex db '00 COUNT='
count_hex db '00',13,10
unavailable_end:
