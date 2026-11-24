# T484 S4 ForceDOS preparation

## Change

P26 registers the existing independent forcedos_parent_probe.c in the native
x86 fixture graph. guest-forcedos now depends on that parent and the existing
wow-generic-normal provider. The integration runner defaults to these cached
inputs while retaining explicit caller-supplied NativeParent/GenericHostDll
options. No duplicate native implementation or compiler wrapper is introduced.

Use the existing CMD alias owner for DUAL/FCHILD/GENHOST/PARENT, then pass the
complete input set to the existing CMD physical stage. Remove runtime Copy-Item
preparation and record the verified package manifest. Result extraction remains
test observation. Existing CreateProcessA/W, marker-count, WOW report, native
exit-code and sequence assertions remain unchanged.

## Verification

Owned Userenv process22488,2026-11-24T19:18:55Z to19:23:27Z, exit0, runs
build/t484-s4-forcedos/run.ps1. Cached full product build and native parent
compilation pass. Nine actual isolated/private-desktop cases pass: forced,
normal, nested and native with both Unicode and ANSI, plus the Unicode sequence
of forced then ordinary launch. Sequence is not duplicated as ANSI because
the existing parent explicitly selects Unicode for that mode.

Forced DOS returns53 with one DOS marker; nested returns55 with two and no
Win16 report. Ordinary WOW produces three GEN1 phases with zero errors and
exit0. Native PE keeps exit90. Sequence proves the first DOS request's marker
followed by successful ordinary WOW execution; flags do not leak into request2.
This uses the x86 parent; it does not claim a newly tested x64 parent variant.

All11 raw product images and22 dual-published hashes remain unchanged. Reuse
the identical publication; no new Full or product behavior change is claimed.
Original guest/media and assets/winnt.zip are unchanged. Governance, parser
and diff checks gate delivery. CURRENT's P1-P11 history was compacted to an
evidence table without dropping links, limitations or active obligations.

## Progress

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease7 to6. S4 and final clean S5 acceptance remain open.
