# M0 T313 S2 P5 — MVDM host startup mirror cleanup

Date: 2026-08-30  
Packet: M0 T313 S2  
Profile: selected Win32/x86 CCPU40.

## Cleanup

`softpc.new/host/src/nt_msscs.c::InitialiseDosEmulation` had gained an
explicit `int` result spelling solely to satisfy a prior cross-width cleanup.
The selected product route is now x86 CCPU40 and the original source's
implicit-result definition compiles there.  The source body has therefore
been returned to its original spelling.

The required selected-provider declarations remain at the original support
call boundary in `softpc.new/base/support/main.c`; these are separately needed
to expose the CCPU40-selected call form and do not modify the startup body or
its call ordering.

## Verification

The exact affected formal candidate rebuild completed:

```text
[1/2] ... nt_msscs.c
[2/2] ... /out:original-softpc-host-roots.lib ...
```

The original implicit-result warning is deliberately visible.  There is no
compiler/linker error, no source-body substitute, and no warning suppression.
