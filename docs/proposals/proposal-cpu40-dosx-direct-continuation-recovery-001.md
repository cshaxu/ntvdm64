# Proposal: CPU40 DOSX direct-continuation recovery

## Decision sought

Make the two selected CPU40 `reset.c` CMOS-shutdown continuations enter the
original DOSX addresses exactly as supplied.  Preserve the historical
non-CPU40 adjustment and do not modify the global BOP convention.

## Source-first basis

P13 proves both inputs originate in original `dpmi/dxutil.asm`: shutdown code
9 restores an IRET frame returning to `DXCODE:enrm50`, and code 0Ah writes
`DXCODE:enrm45` as the BIOS far-jump target.  Both labels begin ordinary DOSX
instructions (`FCLI`), not a `C4 C4 <number>` BOP.  The smallest compatible
seam is therefore a CPU40-only selection at the recovered `reset.c` consumer;
no adapter, new provider, guest rewrite, or BOP stub is needed.

## Scope and evidence bar

Change only the two `setIP` preparation conditions in
`softpc.new/base/bios/reset.c`.  Build the selected Win32/x86 CPU40 product,
verify the source selection and clean diff, and run bounded DOSX/WOW startup
coverage before publication.  If startup fails to improve or produces an
unrelated regression, preserve the observation and stop rather than widening
into CCPU, global BOP, guest-media, or Console changes.
