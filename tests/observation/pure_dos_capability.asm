; S30 PIF-selected pure-DOS profile witness.  The companion P30.PIF names
; task-specific CONFIG.NT/AUTOEXEC.NT inputs, so DOSX/DPMI may not leak in
; from the ordinary startup profile.  The selected worker's original XMS
; handler is process-baseline state and is not used as a PIF override test.

bits 16
org 100h

start:
    mov ax, 1687h
    int 2Fh
    or ax, ax
    jz dpmi_present

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

dpmi_present:
    mov dx, dpmi_present_text
failed_print:
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h

success_text db 'S30_PURE_DOS_OK$'
failure_text db 'S30_PURE_DOS_FAIL$'
dpmi_present_text db 'S30_PURE_DOS_DPMI_PRESENT$'
