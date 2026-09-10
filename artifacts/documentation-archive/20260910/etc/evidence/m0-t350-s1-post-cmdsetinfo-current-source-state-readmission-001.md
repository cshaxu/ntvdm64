# M0 T350 S1 — Current post-SVC_CMDSETINFO source/state re-admission

Date: 2026-09-01

## Reconciliation

T336 was a closed terminal-boundary audit, not proof of NTDOS continuation.
It explicitly left the three durable COMMAND aliases and the CPU40/SAS state
sequence open. The prior named prerequisites have since changed that state:

- T347 replaced all three aliases with the existing session-owned numeric
  guest-location/lease route;
- T340 closed the selected original FDC/INT15/ICA/heartbeat owner package;
- T345 closed ordinary selected host-drive/media bindings; and
- T349 admits only a staged MVDM root that original COMMAND can represent.

## Current original transition

Original `MS_bop_4` invokes `CmdDispatch` and advances IP by exactly one
service byte. Original `cmdSetInfo` records the three NTDOS locations. The
unchanged NTDOS `msinit.asm` continuation then restores `BX`, `CX`, `DX`,
copies the BIOS exchange pointer from `DI:BX`, initializes DOSDATA fields,
saves the caller's stack, and loads `SS:SP = DOSDATA:dskstack`. No BOP occurs
inside this sequence.

## Cohort decision

The next coherent owner is **CPU40/SAS/BIOS-exchange bootstrap mechanics**:
original CCPU40 instruction/segment/stack behavior, original SAS guest access,
and the existing BIOS exchange state supplied before NTDOS continuation.
FDC waits, child execution, BOP providers, media staging and host drive policy
are not members unless the source-defined bootstrap path actually reaches
them.

S2 may recover only this complete cohort through original source or an
existing same-shaped adapter/registered smallest overlay. It may not select a
repair from a trace hit or alter guest bytes. The full source/state table is
`m0-t350-s1-post-cmdsetinfo-bootstrap-cohort-ledger.tsv`.
