; Selected, mechanically preserved x86 body from OpenNT
; base/ntos/rtl/x86/movemem.asm.  The original contract ignores the low two
; length bits and requires a four-byte-aligned destination.

.386p
.model flat
option casemap:none

PUBLIC _RtlFillMemoryUlong@12

_TEXT SEGMENT

_RtlFillMemoryUlong@12 PROC
        push    edi
        mov     edi,[esp+8]
        mov     ecx,[esp+12]
        mov     eax,[esp+16]
        shr     ecx,2
        rep     stosd
        pop     edi
        ret     12
_RtlFillMemoryUlong@12 ENDP

_TEXT ENDS
END
