; Selected, mechanically preserved x86 bodies from OpenNT
; base/ntos/rtl/x86/largeint.asm.  This standalone build slice exports only
; the two reached routines; it changes neither their stack ABI nor arithmetic.

.386p
.model flat
option casemap:none

EXTERN _RtlRaiseStatus@4:PROC

PUBLIC _RtlExtendedLargeIntegerDivide@16
PUBLIC _RtlExtendedIntegerMultiply@12

_TEXT SEGMENT

_RtlExtendedLargeIntegerDivide@16 PROC
        push    esi
        push    edi
        push    ebx

        mov     eax, [esp+16]
        mov     edx, [esp+20]

lid00:  mov     ebx, [esp+24]
        or      ebx, ebx
        jz      short lid_zero

        push    ebp
        mov     ecx, 64
        xor     esi, esi

lid10:  shl     eax, 1
        rcl     edx, 1
        rcl     esi, 1
        sbb     edi, edi
        cmp     esi, ebx
        cmc
        sbb     ebp, ebp
        or      edi, ebp
        sub     eax, edi
        and     edi, ebx
        sub     esi, edi
        dec     ecx
        jnz     short lid10

        pop     ebp
        pop     ebx
        pop     edi
        mov     ecx, [esp+20]
        or      ecx, ecx
        jnz     short lid20
        pop     esi
        ret     16

lid20:  mov     [ecx], esi
        pop     esi
        ret     16

lid_zero:
        push    0C0000094h
        call    _RtlRaiseStatus@4
        pop     ebx
        pop     edi
        pop     esi
        ret     16
_RtlExtendedLargeIntegerDivide@16 ENDP

_RtlExtendedIntegerMultiply@12 PROC
        push    ebp
        mov     ebp,esp
        push    esi

        mov     esi,dword ptr [ebp+16]
        xor     esi,dword ptr [ebp+12]
        test    dword ptr [ebp+12],80000000h
        jz      short reim10
        neg     dword ptr [ebp+12]
        neg     dword ptr [ebp+8]
        sbb     dword ptr [ebp+12],0

reim10: test    dword ptr [ebp+16],80000000h
        jz      short reim20
        neg     dword ptr [ebp+16]

reim20: mov     eax,dword ptr [ebp+16]
        mul     dword ptr [ebp+8]
        push    edx
        mov     ecx, eax
        mov     eax,dword ptr [ebp+16]
        mul     dword ptr [ebp+12]
        add     eax,[esp]
        test    esi,80000000h
        jz      short reim30
        neg     eax
        neg     ecx
        sbb     eax,0

reim30: add     esp,4
        pop     esi
        mov     edx,eax
        mov     eax,ecx
        pop     ebp
        ret     12
_RtlExtendedIntegerMultiply@12 ENDP

_TEXT ENDS
END
