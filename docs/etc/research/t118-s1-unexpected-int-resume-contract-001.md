# T118 S1 unexpected-int resume contract 001

OpenNT source proves that `C4 C4 02` is a three-byte machine BOP followed by
guest-owned `jmp iret_com` or `jmp DOIRET`. `unexpected_int` performs only
PIC/BDA side effects; it does not own the guest continuation or IRET.

The only admissible future handled result is therefore: after the synchronous
original handler succeeds, preserve all CPU registers/flags, set resume RIP to
the copied fault RIP plus three, and re-enter the existing Bochs decode loop.
The following guest jump and IRET remain guest code. This result is neither an
adapter response nor an emulated BIOS return.

Evidence: T110 records the caller forms and selector provenance; `bios.c`
binds `02h` to `unexpected_int`; `exception.cc` already resumes accepted
generic results through `RIP = response.resume_rip` plus `longjmp`.

No source, exception entry, handler call, RIP modification, executable or
guest runtime occurred in T118.
