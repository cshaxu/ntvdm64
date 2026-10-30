# WOW64 SAS breakpoint address calibration

A 32-bit owned test executable contains an exported noinline `int3` function.
The exact linked map and disassembly put its breakpoint at RVA `0x1010`;
the 64-bit test debugger computes target as the process creation base plus
that RVA, and uses the same breakpoint-code/address comparison as the SAS
observer. The target arrived as first-chance `STATUS_WX86_BREAKPOINT` at
exactly that address; after the test debugger continued the intentional test
breakpoint, the child exited zero. A separate negative observer deliberately
used RVA `0x1011`; it logged no target hit, passed through the real exception,
and the child exited with `0x4000001f`. That negative observer correctly
returned failure. Both source files, outputs and input hashes are retained.

This checks address matching and basic WOW64 event handling on this host,
including ASLR base adjustment. It does not prove that the sealed NTVDM's
mapped bytes equal the pre-seal map, cover all debug events, or prove that a
particular guest exercises SAS. Only the disposable native test child contains
the intentional breakpoint; no product or guest was changed.
