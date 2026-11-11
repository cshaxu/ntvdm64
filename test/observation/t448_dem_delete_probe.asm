; Disposable guest probe: exceed the original 128 retained-buffer limit,
; delete the first search's prefetched next file, then continue that search.
bits 16
org 100h
    push cs
    pop ds
    push cs
    pop es
    mov dx,name_a
    call create
    jc failure
    mov dx,name_b
    call create
    jc failure
    mov dx,name_c
    call create
    jc failure
    ; First search is the one to resume; a second search identifies its
    ; next file without advancing it (DEM retains one-item lookahead).
    mov di,dtas
    mov cx,2
.initial:
    push cx
    mov dx,di
    mov ah,1ah
    int 21h
    mov dx,pattern
    xor cx,cx
    mov ah,4eh
    int 21h
    pop cx
    jc failure
    add di,43
    loop .initial
    mov dx,dtas+43
    mov ah,1ah
    int 21h
    mov ah,4fh
    int 21h
    jc failure
    mov si,dtas+43+30
    mov di,deleted_name
.copy:
    lodsb
    stosb
    test al,al
    jnz .copy
    mov di,dtas+86
%ifdef NO_PRESSURE
    mov cx,1
%else
    mov cx,140
%endif
.first_loop:
    push cx
    mov dx,di
    mov ah,1ah
    int 21h
    mov dx,pattern
    xor cx,cx
    mov ah,4eh
    int 21h
    pop cx
    jc failure
    add di,43
    loop .first_loop
%ifndef NO_DELETE
    mov dx,deleted_name
    mov ah,41h
    int 21h
    jc failure
%endif
    mov dx,dtas
    mov ah,1ah
    int 21h
    mov ah,4fh
    int 21h
    jnc continued
    cmp ax,18
    jne failure
    mov dx,lost
    mov ah,9
    int 21h
    mov byte [result],1
    jmp cleanup
continued:
    mov dx,deleted_msg
    mov ah,9
    int 21h
    mov si,deleted_name
    call print_name
    mov dx,returned_msg
    mov ah,9
    int 21h
    mov si,dtas+30
    call print_name
    mov dx,newline
    mov ah,9
    int 21h
    ; A cached lookahead may still return the deleted entry. A further call
    ; must reach the third file if the reset really retained continuation.
    mov ah,4fh
    int 21h
    jnc .third
    cmp ax,18
    jne failure
    mov dx,lost
    mov ah,9
    int 21h
    mov byte [result],1
    jmp cleanup
.third:
    mov dx,passed
    mov ah,9
    int 21h
    jmp cleanup
failure:
    mov dx,failed
    mov ah,9
    int 21h
    mov byte [result],2
cleanup:
    mov dx,name_a
    call remove
    mov dx,name_b
    call remove
    mov dx,name_c
    call remove
    mov al,[result]
    mov ah,4ch
    int 21h
create:
    xor cx,cx
    mov ah,3ch
    int 21h
    jc .done
    mov bx,ax
    mov ah,3eh
    int 21h
.done:
    ret
remove:
    mov ah,41h
    int 21h
    ret
print_name:
    lodsb
    test al,al
    jz .done
    mov dl,al
    mov ah,2
    int 21h
    jmp print_name
.done:
    ret
name_a db 'S12A.TMP',0
name_b db 'S12B.TMP',0
name_c db 'S12C.TMP',0
pattern db 'S12?.TMP',0
deleted_name times 13 db 0
result db 0
deleted_msg db 'ANCHOR=', '$'
returned_msg db ' RETURNED=', '$'
newline db 13,10,'$'
passed db 'DEM-RESET-CONTINUED',13,10,'$'
lost db 'DEM-RESET-NO-MORE',13,10,'$'
failed db 'DEM-RESET-PROBE-FAILED',13,10,'$'
dtas times 142*43 db 0
