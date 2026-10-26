; Independently authored optional boot artwork receiver, not IO.SYS code.
; Native WIN.COM calls AX=160Eh/BL=0 early. DX=0 explicitly declines its
; checksum-based planar-logo takeover; its ordinary display driver owns
; the next mode. No startup delay, host image, live patch or new launcher.
; Animation contract recovered from supplied IO.SYS (SHA in README): file
; DD22-DD3C selects BMP biClrImportant; 08E3-0978 rotates DAC on even BIOS
; ticks. Retain its ring direction/rate, using NTIO's existing INT 1C chain.
; Append after all pre-existing resident data/code. The adapted Win95 IOS
; consumer has a fixed vector-state layout in that prefix; never shift it.
logo_dispatch:
 cmp ax,160eh
 je logo_multiplex
 cmp ax,0e79ch
 jne .existing
 cmp bx,095abh
 jne .existing
 push cs
 pop es
 mov di,logo_status
 xor ax,ax
 iret
.existing:
 jmp multiplex

logo_enabled db 0
logo_active db 0
logo_flag dw 0
logo_old10 dd 0
logo_old1c dd 0
logo_timer_hooked db 0       ; 0 unlinked, 1 active, 2 passive in a later chain
logo_cycle times 60 db 0      ; original logo reserves DAC 236..255
logo_handle dw 0ffffh
logo_previous db 3
logo_y dw 0
logo_bank dw 0
logo_bda times 38 db 0
logo_path times 128 db 0
logo_status:                 ; query E79C/95AB => ES:DI, six words
 dw 0                       ; queries
logo_attempted dw 0
logo_rendered dw 0
logo_handoffs dw 0
logo_error dw 0              ; 1 capability, 2 file/header, 3 read/mode
 dw 2                       ; version; original six words keep their offsets
logo_frames dw 0
logo_stopped_frames dw 0
logo_header times 54 db 0
logo_buffer times 1024 db 0 ; palette, then one input row

logo_multiplex:
 cmp byte [cs:logo_enabled],0
 je .chain
 cmp bl,0
 je .query
 cmp bl,4
 je .flag_set
 cmp bl,5
 jne .chain
 mov word [cs:logo_flag],0
 xor ax,ax
 iret
.flag_set:
 mov word [cs:logo_flag],0ffffh
 xor ax,ax
 iret
.query:
 pushf
 pushad
 push ds
 push es
 push fs
 push gs
 push cs
 pop ds
 cld
 inc word [logo_status]
 cmp word [logo_attempted],0
 jne .queried
 inc word [logo_attempted]
 sti                        ; INT 2F entry clears IF; original IO.SYS permits ticks
 call logo_show
 cli                        ; restore handler state before restoring saved flags
.queried:
 pop gs
 pop fs
 pop es
 pop ds
 popad
 popf
 mov ax,[cs:logo_flag]
 xor dx,dx                   ; no fabricated native planar checksum
 iret
.chain:
 jmp multiplex               ; retain the existing complete provider chain

logo_int10:
 pushf
 cmp byte [cs:logo_active],0
 je .chain
 cmp ah,0
 je .handoff
 cmp ax,6f05h
 jne .chain
.handoff:
 call logo_stop
 inc word [cs:logo_handoffs]
.chain:
 popf
 jmp far [cs:logo_old10]

logo_start:
 ; Install after validation/mode/palette, never for missing/invalid/disabled art.
 ; The supplied bitmap's biClrImportant is the original animation boundary.
 pushf
 cli
 push eax
 push es
 xor ax,ax
 mov es,ax
 mov eax,[es:10h*4]
 mov [logo_old10],eax
 mov word [es:10h*4],logo_int10
 mov [es:10h*4+2],cs
 cmp dword [logo_header+50],236 ; BITMAPINFOHEADER+24h, after 14-byte file header
 jne .static
 mov eax,[es:1ch*4]
 mov [logo_old1c],eax
 mov word [es:1ch*4],logo_tick
 mov [es:1ch*4+2],cs
 mov byte [logo_timer_hooked],1
.static:
 mov byte [logo_active],1
 pop es
 pop eax
 popf
 ret

logo_stop:
 ; Stop DAC writes before the new mode. Unlink only vectors still owned by
 ; us; a later hook may chain us and must never be overwritten.
 pushf
 cli
 push eax
 push es
 mov byte [cs:logo_active],0
 mov ax,[cs:logo_frames]
 mov [cs:logo_stopped_frames],ax
 cmp byte [cs:logo_timer_hooked],0
 je .vectors
 mov byte [cs:logo_timer_hooked],2 ; retire even when a later owner chains us
.vectors:
 xor ax,ax
 mov es,ax
 cmp word [es:1ch*4],logo_tick
 jne .video
 mov ax,cs
 cmp [es:1ch*4+2],ax
 jne .video
 mov eax,[cs:logo_old1c]
 mov [es:1ch*4],eax
 mov byte [cs:logo_timer_hooked],0
.video:
 cmp word [es:10h*4],logo_int10
 jne .done
 mov ax,cs
 cmp [es:10h*4+2],ax
 jne .done
 mov eax,[cs:logo_old10]
 mov [es:10h*4],eax
.done:
 pop es
 pop eax
 popf
 ret

logo_tick:
 pushf
 pushad
 push ds
 push es
 cmp byte [cs:logo_active],0
 je .chain
 xor ax,ax
 mov ds,ax
 test byte [46ch],1          ; same every-other-tick phase as original IO.SYS
 jnz .chain
 push cs
 pop ds
 push cs
 pop es
 cld
 ; Read the current ring, then write entry N+1 at N and the first at 255.
 ; Only sixty DAC bytes are touched; no DOS, BIOS, wait loop or PIC access.
 mov dx,3c7h
 mov al,236
 out dx,al
 add dx,2
 mov di,logo_cycle
 mov cx,60
 rep insb
 mov dx,3c8h
 mov al,236
 out dx,al
 inc dx
 mov si,logo_cycle+3
 mov cx,57
 rep outsb
 mov si,logo_cycle
 mov cx,3
 rep outsb
 inc word [logo_frames]
.chain:
 pop es
 pop ds
 popad
 popf
 jmp far [cs:logo_old1c]     ; preserve all pre-existing periodic work

logo_show:
 ; Validate capability and complete fixed-format file before changing mode.
 mov ax,6f00h
 int 10h
 push cs
 pop ds
 cmp bx,5637h
 jne .capability
 mov ax,3d00h
 mov dx,logo_path
 int 21h
 jc .file_error
 mov [logo_handle],ax
 mov bx,ax
 mov ax,4202h
 xor cx,cx
 xor dx,dx
 int 21h
 jc .close_error
 cmp dx,1
 jne .close_error
 cmp ax,0f836h               ; 129078 bytes
 jne .close_error
 mov ax,4200h
 xor cx,cx
 xor dx,dx
 int 21h
 jc .close_error
 mov dx,logo_header
 mov cx,54
 call logo_read
 jc .close_error
 cmp word [logo_header],4d42h
 jne .close_error
 cmp dword [logo_header+10],1078
 jne .close_error
 cmp dword [logo_header+14],40
 jne .close_error
 cmp dword [logo_header+18],320
 jne .close_error
 cmp dword [logo_header+22],400
 jne .close_error
 cmp dword [logo_header+26],00080001h
 jne .close_error
 cmp dword [logo_header+30],0
 jne .close_error
 mov dx,logo_buffer
 mov cx,1024
 call logo_read
 jc .close_error
 mov ah,0fh
 int 10h
 push cs
 pop ds
 mov [logo_previous],al
 call logo_save_bda
 mov ax,6f05h
 mov bx,66h                  ; 640x400 packed8, original image doubled in X
 int 10h
 mov ax,6f04h
 int 10h
 push cs
 pop ds
 cmp al,66h
 jne .restore_error
 mov dx,3c4h
 mov ax,0ea06h               ; enable V7 extended bank-register writes
 out dx,ax
 mov dx,3c8h
 xor al,al
 out dx,al
 inc dx
 mov si,logo_buffer
 mov cx,256
.palette:
 mov al,[si+2]
 shr al,2
 out dx,al
 mov al,[si+1]
 shr al,2
 out dx,al
 mov al,[si]
 shr al,2
 out dx,al
 add si,4
 loop .palette
 call logo_start            ; animate while normal DOS image reads continue
 mov ax,0a000h
 mov es,ax
 mov word [logo_y],399
.row:
 mov dx,logo_buffer
 mov cx,320
 call logo_read
 jc .restore_error
 movzx eax,word [logo_y]
 imul eax,eax,640
 mov di,ax
 shr eax,16
 mov [logo_bank],ax
 call logo_select_bank
 mov si,logo_buffer
 mov cx,320
.pixel:
 lodsb
 mov ah,al
 stosw
 test di,di
 jnz .next_pixel
 inc word [logo_bank]
 mov ax,[logo_bank]
 call logo_select_bank
.next_pixel:
 loop .pixel
 dec word [logo_y]
 jns .row
 xor ax,ax
 call logo_select_bank
 mov bx,[logo_handle]
 mov ah,3eh
 int 21h
 call logo_restore_bda       ; DOS/WIN.COM still see the original text state
 inc word [logo_rendered]
 ret
.restore_error:
 mov word [logo_error],3
 mov al,[logo_previous]
 xor ah,ah
 int 10h
 push cs
 pop ds
 jmp .close
.close_error:
 mov word [logo_error],2
.close:
 mov bx,[logo_handle]
 mov ah,3eh
 int 21h
 ret
.file_error:
 mov word [logo_error],2
 ret
.capability:
 mov word [logo_error],1
 ret

logo_save_bda:
 push ax
 push cx
 push si
 push di
 push ds
 push es
 xor ax,ax
 mov ds,ax
 push cs
 pop es
 mov di,logo_bda
 mov si,410h
 movsb
 mov si,449h
 mov cx,30
 rep movsb
 mov si,484h
 mov cx,7
 rep movsb
 pop es
 pop ds
 pop di
 pop si
 pop cx
 pop ax
 ret

logo_restore_bda:
 push ax
 push cx
 push si
 push di
 push ds
 push es
 push cs
 pop ds
 xor ax,ax
 mov es,ax
 mov si,logo_bda
 mov di,410h
 movsb
 mov di,449h
 mov cx,30
 rep movsb
 mov di,484h
 mov cx,7
 rep movsb
 pop es
 pop ds
 pop di
 pop si
 pop cx
 pop ax
 ret

logo_read:
 mov bx,[logo_handle]
 mov ah,3fh
 int 21h
 jc .done
 cmp ax,cx
 je .done
 stc
.done:
 ret

logo_select_bank:
 ; Existing V7 sequential-chain4 bank = (misc bit5 <<1) | F9 bit0.
 ; The 640x400 image uses banks 0..3. Preserve all other timing bits.
 push ax
 push bx
 push dx
 mov bx,ax
 and al,1
 mov ah,al
 mov al,0f9h
 mov dx,3c4h
 out dx,ax
 mov dx,3cch
 in al,dx
 and al,0dfh
 and bl,2
 shl bl,4
 or al,bl
 mov dx,3c2h
 out dx,al
 pop dx
 pop bx
 pop ax
 ret

logo_parse_options:
 ; DOS INIT request +18 holds the complete CONFIG device command line.
 ; Only a bounded /LOGO=<DOS-path> token opts into the extra vector hook.
 pushad
 push ds
 push es
 les bx,[request]
 lds si,[es:bx+18]
 mov cx,255
.scan:
 lodsb
 cmp al,13
 je .done
 test al,al
 jz .done
 cmp al,'/'
 jne .advance
 cmp cx,6
 jb .done
 mov eax,[si]
 or eax,20202020h
 cmp eax,'logo'
 jne .advance
 cmp byte [si+4],'='
 jne .advance
 add si,5
 push cs
 pop es
 mov di,logo_path
 mov cx,127
.copy:
 lodsb
 cmp al,13
 je .end
 cmp al,0
 je .end
 cmp al,' '
 je .end
 cmp al,9
 je .end
 stosb
 loop .copy
 jmp .done                   ; reject truncated path
.end:
 cmp di,logo_path
 je .done
 mov byte [es:di],0
 mov byte [cs:logo_enabled],1
 jmp .done
.advance:
 loop .scan
.done:
 pop es
 pop ds
 popad
 ret
