; Independently authored W95COMP DOS character driver, research only.
; Shares the existing COM adapters' resident implementations; see provenance.json.
; RMD collection is deferred until first query, after CONFIG device loading.
bits 16
org 0
 dd 0ffffffffh
 dw 8000h
 dw strategy, interrupt
 db 'W95COMP$'
request dd 0
initialized db 0
rmd_ready db 0
rmd_error db 0
strategy:
 mov [cs:request],bx
 mov [cs:request+2],es
 retf
interrupt:
 pushf
 pushad
 push ds
 push es
 push fs
 push gs
 cld
 push cs
 pop ds
 les bx,[request]
 cmp byte [es:bx+2],0
 je init
 mov word [es:bx+3],8103h ; unsupported command, not a false I/O success
 jmp interrupt_return
interrupt_return:
 pop gs
 pop fs
 pop es
 pop ds
 popad
 popf
 retf
multiplex:
 cmp ax,0e79ah
 jne .not_disk_query
 push cs
 pop es
 xor ax,ax
 iret
.not_disk_query:
 cmp ax,0e79bh
 jne .not_tsr_query
 push cs
 pop es
 mov bx,d_tsr_table
 mov ds,[cs:d_dos_segment]
 xor ax,ax
 iret
.not_tsr_query:
 cmp ax,0e799h
 jne .ordinary
 cmp bx,095abh
 jne .ordinary
 push cs
 pop es
 mov di,d_tsr_table
 xor ax,ax
 iret
.ordinary:
 cmp ah,13h
 je d_swap
 cmp ax,1690h
 je .ensure
 cmp ax,0e797h
 jne .dispatch
 cmp bx,095abh
 jne .dispatch
.ensure:
 pushf
 pushad
 push ds
 push es
 push fs
 push cs
 pop ds
 cld
 cmp byte [rmd_ready],0
 jne .ready
 call collect_rmd
.ready:
 pop fs
 pop es
 pop ds
 popad
 popf
 cmp byte [cs:rmd_ready],0
 jne .dispatch
 mov ax,1
 push bp
 mov bp,sp
 or word [ss:bp+6],1
 pop bp
 iret
.dispatch:
 jmp k_int2f
; Independently authored experimental guest compatibility service.
; Compatibility: 1613h a_pathname; opt-in /2 supplies 1612h INT13/A20 callback.
; Diagnostics: observe original 1605h results and 21h/4Ch termination caller.
; Interface corroborated with Ralf Brown's Interrupt List entry D-2F1613.
; No Win95/NTDOS instruction or structure is patched.



a_old_2f: dd 0
a_old_21: dd 0
a_exit_record: dw 0,0,0,0       ; count, terminating AX, return IP, return CS
a_startup_record: times 9 dw 0 ; count, CX in/out, AX out, DI in, BX/ES, SI/DS out
a_query1612_record: dw 0,0      ; diagnostic count, unchanged underlying AX result
a_callback_record: dw 0,0,0    ; calls, A20 enable failures, actual enable attempts
a_enable1612: db 0
a_old_13: dd 0
a_xms_entry: dd 0
a_dos7_descriptor: dw 1,a_bios_a20_callback,0
a_pathname: db 'O:\WINDOWS\SYSTEM.DAT',0
a_path_bytes equ $-a_pathname

a_handler:
    pushf
    cmp ax,0e795h
    jne a_ordinary
    cmp bx,095abh
    jne a_ordinary
    mov ax,cs
    mov es,ax
    mov bx,a_exit_record
    xor ax,ax
    popf
    iret
a_ordinary:
    cmp ax,1605h
    je a_observe_startup
    cmp ax,1612h
    je a_observe_1612
    cmp ax,1613h
    jne a_chain
    cmp cx,a_path_bytes
    jb a_chain                    ; do not invent success for a short buffer
    push ds
    push si
    push di
    push cs
    pop ds
    mov si,a_pathname
    mov cx,a_path_bytes
    cld
    rep movsb                   ; caller ES:DI, bounded actual a_pathname
    pop di
    pop si
    pop ds
    mov cx,a_path_bytes
    xor ax,ax
    popf
    iret
a_chain:
    popf
    jmp far [cs:a_old_2f]
a_observe_startup:
    inc word [cs:a_startup_record]
    mov [cs:a_startup_record+2],cx
    mov [cs:a_startup_record+8],di
    popf
    pushf
    call far [cs:a_old_2f]
    mov [cs:a_startup_record+4],cx
    mov [cs:a_startup_record+6],ax
    mov [cs:a_startup_record+10],bx
    mov [cs:a_startup_record+12],es
    mov [cs:a_startup_record+14],si
    mov [cs:a_startup_record+16],ds
    jmp a_return_observed_flags
a_observe_1612:
    inc word [cs:a_query1612_record]
    cmp byte [cs:a_enable1612],0
    je a_observe_original_1612
    mov ax,cs
    mov es,ax
    mov bx,a_dos7_descriptor
    xor ax,ax
    mov [cs:a_query1612_record+2],ax
    popf
    iret
a_observe_original_1612:
    popf
    pushf
    call far [cs:a_old_2f]
    mov [cs:a_query1612_record+2],ax
a_return_observed_flags:
    push bp
    mov bp,sp
    pushf
    pop word [ss:bp+6]
    pop bp
    iret
a_exit_observer:
    pushf
    cmp ah,4ch
    jne a_exit_chain
    push bp
    mov bp,sp
    push ax
    inc word [cs:a_exit_record]
    mov [cs:a_exit_record+2],ax
    mov ax,[ss:bp+4]
    mov [cs:a_exit_record+4],ax
    mov ax,[ss:bp+6]
    mov [cs:a_exit_record+6],ax
    pop ax
    pop bp
a_exit_chain:
    popf
    jmp far [cs:a_old_21]
a_bios_a20_callback:
    ; Caller DS is descriptor's segment, as required by DOS7's contract.
    ; Delegate to the captured original BIOS vector, never the live VTD hook.
    pushf
    cli
    call far [cs:d_readvec]
    pushf
    pushad
    push ds
    push es
    inc word [cs:a_callback_record]
    ; A20 observation follows unchanged NTDOS EnsureA20On/IsA20Off in
    ; doskrnl/bios/msbio1.asm, not HIMEM's cached A20 flag alone.
    xor ax,ax
    mov es,ax
    mov di,80h
    dec ax
    mov ds,ax
    mov si,90h
    mov cx,8
    cld
    repe cmpsw
    jne a_callback_done
    inc word [cs:a_callback_record+4]
    mov ah,5
    call far [cs:a_xms_entry]
    cmp ax,1
    je a_callback_done
    inc word [cs:a_callback_record+2]
a_callback_done:
    pop es
    pop ds
    popad
    push cs
    pop ds
    popf
    retf
; Independent research RMD transport. Metadata derives from real NTDOS devmark
; records and current device headers, not guessed addresses or an empty chain.
; Win95 DDK July 1995 r_rmd.h / int2fapi.h, packed 16-bit far pointers.



r_old2f dd 0
r_status:
 r_calls dw 0
 r_count dw 0
 r_dos_segment dw 0
 r_himem_header dd 0
 r_install_next dd 0
 r_query_next times 8 dd 0
r_rss:
 r_rrs_rmd dw r_rmd,0
 r_rrs_next dd 0
r_rmd:
 db 7                       ; RMD_TYPE_IOSYS
 dd 0                       ; no further RMD type in this provider
 r_num_entries db 0
r_entries: times 32*20 db 0   ; fixed 20-byte entry with one 5-byte address packet
r_handler:
 cmp ax,0e797h
 jne .ordinary
 cmp bx,095abh
 jne .ordinary
 push cs
 pop es
 mov bx,r_status
 xor ax,ax
 iret
.ordinary:
 cmp ax,1690h
 jne .chain
 push eax
 push fs
 push si
 push di
 mov di,[cs:r_calls]
 cmp di,8
 jae .observed
 shl di,2
 lfs si,[cs:r_himem_header]
 mov eax,[fs:si]
 mov [cs:r_query_next+di],eax
.observed:
 pop di
 pop si
 pop fs
 pop eax
 inc word [cs:r_calls]
 mov [cs:r_rrs_next],bx
 mov [cs:r_rrs_next+2],es
 push cs
 pop es
 mov bx,r_rss
 push bp
 mov bp,sp
 and word [ss:bp+6],0fffeh
 pop bp
 iret
.chain:
 jmp far [cs:r_old2f]
; Independently authored DOS7 VKD return-hook ABI adapter.
; Private original IO.SYS 325695...: file 3D90h returns ES:DI to file A3Dh
; (resident offset 043Dh). Original VKD checks 9C 2E FF 0E xx xx 75 01 90 9D.
; Wrap the actual installed BIOS INT16 chain; preserve its registers/flags.
; The k_counter starts at zero, as in the original IO.SYS data at file B0Bh.
; VKD owns any subsequent k_counter updates and return-site virtualization.



k_old16 dd 0
k_old2f dd 0
k_counter dw 0
k_calls dw 0
k_int16:
 ; Keep the published return hook and subsequent IOS slots at their offsets.
 jmp k_int16_entry
 times 6-($-k_int16) db 90h
k_return_hook:
 pushf
 dec word [cs:k_counter]
 jnz .done
 nop
.done:
 popf
 retf 2
k_int2f:
 cmp ax,0e798h
 jne .callout
 cmp bx,095abh
 jne .chain
 push cs
 pop es
 mov di,k_return_hook
 mov si,k_counter
 mov dx,k_old16
 xor ax,ax
 iret
.callout:
 cmp ax,1607h
 jne .chain
 cmp bx,000dh
 jne .chain
 inc word [cs:k_calls]
 push cs
 pop es
 mov di,k_return_hook
 iret
.chain:
 jmp far [cs:k_old2f]

; Collect actual loaded character drivers from NTDOS devmarks. No fake header.
collect_rmd:
 mov byte [r_num_entries],0
 mov word [r_count],0
 mov dword [r_himem_header],0
 mov ax,5200h
 int 21h
 mov [r_dos_segment],es
 add bx,22h
 mov bp,64
.next:
 test bx,bx
 jnz .advance
 mov ax,es
 test ax,ax
 jz .fail
 dec ax
 mov fs,ax
 cmp byte [fs:0],'D'
 jne .advance
 mov ax,es
 cmp word [fs:1],ax
 jne .advance
 cmp word [fs:3],0
 je .advance
 cmp dword [fs:8],'HIME'
 jne .not_himem
 cmp byte [fs:12],'M'
 jne .not_himem
 mov [r_himem_header],bx
 mov [r_himem_header+2],es
.not_himem:
 cmp byte [r_num_entries],32
 jae .fail
 test word [es:bx+4],8000h
 jz .fail
 mov ax,[r_count]
 mov dx,20
 mul dx
 add ax,r_entries
 mov di,ax
 push es
 push cs
 pop es
 mov si,8
 mov cx,8
 fs rep movsb
 mov byte [es:di],'S'
 mov byte [es:di+1],'Y'
 mov byte [es:di+2],'S'
 add di,3
 mov word [es:di],1
 mov byte [es:di+2],0
 mov byte [es:di+3],1
 mov byte [es:di+4],1
 mov word [es:di+5],bx
 pop es
 mov ax,es
 mov [di+7],ax
 inc word [r_count]
 inc byte [r_num_entries]
.advance:
 mov eax,[es:bx]
 cmp ax,0ffffh
 je .complete
 mov bx,ax
 shr eax,16
 mov es,ax
 dec bp
 jnz .next
.fail:
 mov byte [rmd_error],1
 ret
.complete:
 cmp word [r_count],0
 je .fail
 cmp word [r_himem_header+2],0
 je .fail
 lfs si,[r_himem_header]
 mov eax,[fs:si]
 mov [r_install_next],eax
 mov byte [rmd_ready],1
 mov byte [rmd_error],0
 ret


; Only this file-oriented Win95 profile owns these vectors. No kernel patch.
; Existing audited profile has no recorded disk/IRQ TSR hooks.
; Driver owns this finite table, kernel retains only the DOS7 pointer slot.
d_tsr_canary_before db 0a5h
d_tsr_table times 256 db 0
d_tsr_canary_after db 05ah
d_altvec dd 0
d_readvec dd 0
d_restorevec dd 0
d_dos_segment dw 0
d_old13 dd 0
d_old19 dd 0
d_old25 dd 0
d_old26 dd 0
d_swap_count dw 0
d_bios_denials dw 0
d_abs_denials dw 0
d_int13:
 pushf
 cmp dl,80h
 jb .chain
 cmp ah,2
 jb .chain
 cmp ah,5
 jbe .deny
 cmp ah,0ah
 jb .chain
 cmp ah,0ch
 ja .chain
.deny:
 inc word [cs:d_bios_denials]
 mov ah,80h
 popf
 push bp
 mov bp,sp
 or word [ss:bp+6],1
 pop bp
 iret                         ; restore caller IF/DF, set only saved carry

.chain:
 popf
 jmp far [cs:d_old13]
d_absolute:
 inc word [cs:d_abs_denials]
 push bp
 mov bp,sp
 push word [ss:bp+6]
 popf                         ; caller flags, while retaining extra FLAGS word
 pop bp
 mov ax,8002h
 stc
 retf                         ; DOS INT25/26 leaves the interrupt FLAGS word
d_swap:
 ; Same DS:DX/ES:BX exchange, shared with the IOS fixed-layout consumer.
 pushf
 push ax
 push si
 push di
 mov si,ds
 mov di,es
 push cs
 pop ds
 xchg dx,[d_readvec]
 xchg si,[d_readvec+2]
 xchg bx,[d_restorevec]
 xchg di,[d_restorevec+2]
 mov ds,si
 mov es,di
 pop di
 pop si
 pop ax
 inc word [cs:d_swap_count]
 popf
 iret
d_int19:
 pushf
 push eax
 push ds
 push es
 push cs
 pop ds
 xor ax,ax
 mov es,ax
 mov eax,[d_restorevec]
 mov [es:13h*4],eax
 pop es
 pop ds
 pop eax
 popf
 jmp far [cs:d_old19]

%ifndef COMPAT95_NO_LOGO
%include "bootlogo.inc"
%endif
%include "root.inc"
k_int16_entry:
 ; The DOS7 wrapper saves IF=1 for an IRET-style BIOS return, then enters
 ; the BIOS with IF=0. A plain PUSHF here saves interrupt-entry IF=0 and
 ; lets an IRET return clear IF in the caller's polling loop.
 ; Preserve VKD's exact published return address while using this tail.
 sti
 pushf
 cli
 push cs
 push word k_return_hook
 jmp far [cs:k_old16]
resident_end:
; Discardable initialization. Do not use process-only AH31/AH49 services.
init:
 mov word [es:bx+3],810ch
 mov word [es:bx+14],0
 mov [es:bx+16],cs
 cmp byte [initialized],0
 jne .ok
 call root_parse_options
 jc .duplicate
%ifndef COMPAT95_NO_LOGO
 call logo_parse_options
%endif
 ; Reject a second instance before changing any vectors.
 mov ax,0e798h
 mov bx,095abh
 int 2fh
 test ax,ax
 jz .duplicate
 mov ax,5200h
 int 21h
 mov [d_dos_segment],es
 ; Stock NTDOS data is not written by this provider.
 mov ax,4310h
 int 2fh
 mov [a_xms_entry],bx
 mov [a_xms_entry+2],es
 mov [a_dos7_descriptor+4],cs
 mov byte [a_enable1612],1
 xor ax,ax
 mov es,ax
 pushf
 cli
 mov eax,[es:13h*4]
 mov [d_old13],eax
 mov word [es:13h*4],d_int13
 mov [es:13h*4+2],cs
 mov eax,[es:13h*4]
 mov [d_altvec],eax
 mov [d_readvec],eax
 mov [d_restorevec],eax
 mov eax,[es:19h*4]
 mov [d_old19],eax
 mov word [es:19h*4],d_int19
 mov [es:19h*4+2],cs
 mov eax,[es:25h*4]
 mov [d_old25],eax
 mov eax,[es:26h*4]
 mov [d_old26],eax
 mov word [es:25h*4],d_absolute
 mov [es:25h*4+2],cs
 mov word [es:26h*4],d_absolute
 mov [es:26h*4+2],cs
 mov eax,[es:2fh*4]
 mov [a_old_2f],eax
 mov word [r_old2f],a_handler
 mov [r_old2f+2],cs
 mov word [k_old2f],r_handler
 mov [k_old2f+2],cs
 mov eax,[es:16h*4]
 mov [k_old16],eax
 mov word [es:16h*4],k_int16
 mov [es:16h*4+2],cs
 mov word [es:2fh*4],root_dispatch
 mov [es:2fh*4+2],cs
 popf
 mov [r_rrs_rmd+2],cs
 mov byte [initialized],1
.ok:
 les bx,[request]
 mov word [es:bx+3],0100h
 mov word [es:bx+14],resident_end
 mov [es:bx+16],cs
 jmp interrupt_return
.duplicate:
 les bx,[request]
 jmp interrupt_return
