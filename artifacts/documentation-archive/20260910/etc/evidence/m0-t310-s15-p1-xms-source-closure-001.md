# M0 T310 S15 P1 — original XMS source-closure baseline

The selected original XMS manifest has six common translation units:
`xms.c`, `xmsa20.c`, `xmsblock.c`, `xmsdisp.c`, `xmsmisc.c` and `xmsumb.c`.
They compile as one formal MSVC Win32/x86 `/MT` static archive for the selected
`CPU_40_STYLE` CCPU40 configuration. The formal graph also compiles the selected same-shaped
`adapter-mvdm-host-out/softpc/mvdm_xms_memory.c` backend.

The direct `i386/xmsmem86.c` backend is retained as source evidence but is
not a selected product object: it converts XMS numeric addresses to host
pointers, which cannot be preserved at the session guest-memory boundary.
The selected backend keeps original XMS control flow in the mirror and uses
the session guest-memory lease only for that native identity crossing.

This P1 establishes compile closure, not XMS runtime closure.  Remaining S15
work is to re-read the dispatcher, block/UMB/A20 contracts and provide focused
behavior evidence without selecting the excluded direct-pointer backend. x64
compatibility is recorded as deferred product debt, not a P1 requirement.
