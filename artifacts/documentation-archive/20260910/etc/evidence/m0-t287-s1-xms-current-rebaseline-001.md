# M0 T287 S1 — current XMS source, ABI and owner rebaseline

## Question

Can the queue-head XMS package proceed as a whole original-owner recovery,
using the current component layout rather than pre-rebootstrap `bx-vdm`
evidence or a new per-service implementation?

## Source identity

All fifteen paths under `src/mvdm-host/xms.486` are byte-identical to both
canonical source counterparts:

- `O:\repos.external\OpenNT\base\mvdm\xms.486`;
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm\xms.486`.

The complete selection and final per-file disposition are in
[`m0-t287-s1-xms-file-rebaseline-ledger-002.tsv`](../operations/m0-t287-s1-xms-file-rebaseline-ledger-002.tsv).
There is no edition conflict and no local source divergence to repair.

## Current package boundary

The literal `sources` file selects six common provider units.  Its `i386`
subdirectory selects `xmsmem86.c`; this historical unit treats `ULONG` guest
addresses as host `PVOID` values and calls NT virtual-memory/Rtl memory APIs.
It is therefore retained as an exact mirror and interface evidence, but cannot
be linked as a direct backend on either supported product architecture.
`xmsmemr.c` and the Alpha/MIPS/PPC descriptions are retained evidence, not
selected product translation units.

The current owner map is complete:

- `mvdm-host/xms.486` owns source algorithms, service table, result ordering,
  UMB list policy and original unavailable failures;
- `mvdm-support/suballoc` owns the original allocator algorithm;
- `adapter-mvdm-host-out/softpc` owns same-shaped register, guest-lease,
  A20, IVT and backing-contract facades;
- `session` owns the one `guest_memory` mapping-manager instance and call
  epoch; it never exports an unbounded pointer;
- `adapter-bochs` owns typed ordinary/backing RAM, A20, IVT and opaque
  physical-span mechanics only;
- `adapter-mvdm-host-in` remains selector-blind and cannot own the XMS table
  or service meanings.

The nine interface groups, their original forms, and their next owning
subtask are recorded in
[`m0-t287-s1-xms-interface-rebaseline-ledger.tsv`](../operations/m0-t287-s1-xms-interface-rebaseline-ledger.tsv).

## BOP and dependency coverage

The audit covers original `52:00` through `52:0B` dispatcher slots and the
XMS dependency set: common state/dispatch, blocks/suballoc, A20, UMB,
system/INT15 notification, the direct-address backend and the common
guest-pointer mapping family.  Earlier entries that name `src/bx-vdm` are
historical evidence only; no current source, build or runtime input uses that
root.

The only material direct-source exclusion is the `xmsmem86.c` host-pointer
backend.  It has a specific same-shaped successor (interface I-05), not a
free-standing reimplementation.  The only source-proven pending capability is
a physical UMB success span; until it exists, the original empty-map failures
remain the required behavior.

## Result and successor

S1 closes its rebaseline gate: no XMS source or BOP row is ownerless, all
current files are exact mirrors, and every external interface has a named
owner.  S2 may now compose only the complete directly composable original
common package as a selector-disabled x86/x64 static island.  It must not
enable a `52:xx` route, import `src.old`, or bind the direct-address backend.

The earlier unnumbered file ledger is retained only as a draft record.  The
`-002` ledger is the S1 authority because it lists all fifteen physical paths
individually and records their measured SHA-256 values.
