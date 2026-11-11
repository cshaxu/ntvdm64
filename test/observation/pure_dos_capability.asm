; S30 PIF-selected pure-DOS profile witness.  The companion P30.PIF names
; task-specific CONFIG.NT/AUTOEXEC.NT inputs, so DOSX/DPMI may not leak in
; from the ordinary startup profile.  The selected worker's original XMS
; handler is process-baseline state and is not used as a PIF override test.
; Also reject DOS in HMA: otherwise a consumed CONFIG override can fall back
; to default DOS=HIGH while the separate AUTOEXEC still excludes DOSX.

bits 16
org 100h

start:
    mov ax, 1687h
    int 2Fh
    or ax, ax
    jz dpmi_present

    ; Original doskrnl/dos/msdisp.asm returns DosHasHMA in DH bit 4;
    ; dos/v86/inc/versiona.inc defines DOSINHMA as 00010000B.
    mov ax, 3306h
    int 21h
    test dh, 10h
    jnz dos_high

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
    jmp failed_print
dos_high:
    mov dx, dos_high_text
failed_print:
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h

success_text db 'S30_PURE_DOS_OK$'
failure_text db 'S30_PURE_DOS_FAIL$'
dpmi_present_text db 'S30_PURE_DOS_DPMI_PRESENT$'
dos_high_text db 'S30_PURE_DOS_UNEXPECTED_HMA$'
