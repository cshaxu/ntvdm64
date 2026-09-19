; S33 test-only DOS file-service witness.  Every operation enters original
; INT 21h/NTDOS DEM dispatch; it creates and removes only D33.TMP in the
; active disposable test directory.

bits 16
org 100h

start:
    mov dx, file_name
    xor cx, cx                     ; normal attributes
    mov ah, 3Ch                    ; create/truncate
    int 21h
    jc fail
    mov [handle], ax

    mov bx, ax
    mov cx, payload_end-payload
    mov dx, payload
    mov ah, 40h                    ; write
    int 21h
    jc fail_close
    cmp ax, payload_end-payload
    jne fail_close

    mov byte [stage], 'Q'
    mov bx, [handle]
    mov ah, 68h                    ; commit file data
    int 21h
    jc fail_close

    mov bx, [handle]
    mov ah, 3Eh                    ; close
    int 21h
    jc fail

    mov byte [stage], 'C'
    mov dx, second_name
    xor cx, cx
    mov ah, 3Ch                    ; create a second match for FindNext
    int 21h
    jc fail
    mov bx, ax
    mov ah, 3Eh
    int 21h
    jc fail

    mov al, 0                      ; read-only
    mov dx, file_name
    mov ah, 3Dh                    ; open
    int 21h
    jc fail
    mov [handle], ax

    mov bx, ax
    mov cx, payload_end-payload
    mov dx, read_buffer
    mov ah, 3Fh                    ; read
    int 21h
    jc fail_close
    cmp ax, payload_end-payload
    jne fail_close
    mov si, payload
    mov di, read_buffer
    mov cx, payload_end-payload
    repe cmpsb
    jne fail_close

    mov bx, [handle]
    xor cx, cx
    xor dx, dx
    mov al, 2                      ; relative to end
    mov ah, 42h                    ; seek
    int 21h
    jc fail_close
    cmp ax, payload_end-payload
    jne fail_close
    or dx, dx
    jnz fail_close

    mov bx, [handle]
    mov ah, 3Eh
    int 21h
    jc fail

    mov byte [stage], 'S'
    mov al, 12h                    ; read/write, deny all
    mov dx, file_name
    mov ah, 3Dh
    int 21h
    jc fail
    mov [share_handle], ax
    mov al, 0                      ; a second reader must be denied
    mov dx, file_name
    mov ah, 3Dh
    int 21h
    jnc fail_close_unexpected
    mov [last_ax], ax
    cmp ax, 5                      ; source-shaped access-denied mapping
    jne fail
    mov bx, [share_handle]
    mov ah, 3Eh
    int 21h
    jc fail

    mov byte [stage], 'L'
    mov al, 2                      ; read/write for range lock
    mov dx, file_name
    mov ah, 3Dh
    int 21h
    jc fail
    mov [handle], ax
    mov bx, ax
    xor cx, cx
    xor dx, dx                     ; offset 0
    xor si, si
    mov di, 1                      ; length 1
    mov ax, 5C00h                  ; lock region
    int 21h
    jc fail_close
    mov ax, 5C01h                  ; unlock region
    int 21h
    jc fail_close
    mov bx, [handle]
    mov ah, 3Eh
    int 21h
    jc fail

    mov byte [stage], 'F'
    mov dx, search_name
    xor cx, cx
    mov ah, 4Eh                    ; find first
    int 21h
    jc fail
    mov byte [stage], 'N'
    mov ah, 4Fh                    ; find next
    int 21h
    jc fail

    mov byte [stage], 'T'
    mov dx, second_name
    push ds
    pop es
    mov di, renamed_name
    mov ah, 56h                    ; rename second file
    int 21h
    jc fail

    mov byte [stage], 'O'
    mov dx, fcb
    mov ah, 0Fh                    ; FCB open
    int 21h
    or al, al
    jnz fail
    mov byte [stage], 'R'
    mov dx, fcb
    mov ah, 14h                    ; FCB sequential read, one-byte record
    int 21h
    or al, al
    jz fcb_read_ok
    cmp al, 3                       ; original DOS partial final record
    jne fail_fcb
fcb_read_ok:
    mov byte [stage], 'K'
    mov dx, fcb
    mov ah, 10h                    ; FCB close
    int 21h
    or al, al
    jnz fail

    mov byte [stage], 'A'
    xor cx, cx
    mov dx, file_name
    mov ax, 4300h                  ; get attributes
    int 21h
    jc fail
    mov cx, 20h                    ; archive attribute
    mov dx, file_name
    mov ax, 4301h                  ; set attributes
    int 21h
    jc fail
    xor cx, cx
    mov dx, file_name
    mov ax, 4300h                  ; prove set/get roundtrip
    int 21h
    jc fail
    test cx, 20h
    jz fail

    mov byte [stage], 'M'
    mov dx, directory_name
    mov ah, 39h                    ; make directory
    int 21h
    jc fail
    mov dx, directory_name
    mov ah, 3Ah                    ; remove empty directory
    int 21h
    jc fail

    mov byte [stage], 'E'
    mov dx, missing_name
    mov al, 0
    mov ah, 3Dh                    ; open a known missing file
    int 21h
    jnc fail_close_unexpected
    mov [last_ax], ax
    cmp ax, 2                      ; DOS file-not-found
    jne fail

    mov byte [stage], 'D'
    mov dx, file_name
    mov ah, 41h                    ; delete
    int 21h
    jc fail
    mov dx, renamed_name
    mov ah, 41h
    int 21h
    jc fail

    mov dx, success_text
    jmp short print_exit

fail_close_unexpected:
    mov bx, ax
    mov ah, 3Eh
    int 21h
fail_close:
    mov bx, [handle]
    mov ah, 3Eh
    int 21h
fail_fcb:
    mov dx, fcb
    mov ah, 10h
    int 21h
fail:
    mov dl, [stage]
    mov ah, 02h
    int 21h
    mov ax, [last_ax]
    call print_hex_word
    mov dx, fail_text
print_exit:
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h

print_hex_word:
    push ax
    push bx
    push cx
    push dx
    mov bx, ax
    mov ch, 4
.next_nibble:
    rol bx, 4
    mov dl, bl
    and dl, 0Fh
    add dl, '0'
    cmp dl, '9'
    jbe .emit
    add dl, 'A'-'9'-1
.emit:
    mov ah, 02h
    int 21h
    dec ch
    jnz .next_nibble
    pop dx
    pop cx
    pop bx
    pop ax
    ret

handle       dw 0
share_handle dw 0
stage        db 'A'
last_ax      dw 0
file_name    db 'D33.TMP',0
second_name  db 'D33B.TMP',0
renamed_name db 'D33C.TMP',0
directory_name db 'D33DIR',0
search_name  db 'D33*.TMP',0
missing_name db 'D33.MIS',0
fcb:
             db 0
             db 'D33     ','TMP'
             dw 0                   ; current block
             dw 1                   ; record size
             dd 0                   ; file size
             dw 0,0                 ; date/time
             times 8 db 0
             db 0                   ; current record
             dd 0                   ; random record
payload      db 'DEM-S33'
payload_end:
read_buffer  times payload_end-payload db 0
success_text db 'S33_FILE_COMMIT_SHARE_LOCK_FIND_RENAME_FCB_ATTR_DIR_ERROR_MAP_OK',13,10,'$'
fail_text    db 'S33_DEM_FILE_FAIL',13,10,'$'
