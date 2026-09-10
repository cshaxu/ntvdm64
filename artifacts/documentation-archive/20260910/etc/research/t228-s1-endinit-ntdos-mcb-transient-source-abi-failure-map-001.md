# T228 S1 P19 — COMMAND EndInit / NTDOS MCB Transient Lifecycle Source-ABI-Failure Map

## Scope and Current Fact

The controlled-large source-built run reaches original COMMAND `EndInit` and
then has no child lifecycle; P18 corrected the source-derived `54:0F`
provider contract but retained the bounded `0C41:1DC6 -> 4444:4444` outcome.
The remaining candidate is not a BOP service.  It is one guest-owned package:
COMMAND transient allocation/move/reload plus NTDOS MCB allocation.

## Original Owner Flow

| Stage | Original source | Preserved contract and failure path |
| --- | --- | --- |
| Environment resize | `cmd/command/rdata.asm:EndInit` | COMMAND invokes DOS `SETBLOCK`; an insufficient initial-environment result reallocates the guest environment and retries the existing host service. Failure follows `Alloc_error`; no adapter selects the guest segment. |
| Initial transient placement | `rdata.asm:EndInit` | COMMAND asks DOS `ALLOC` with `BX=ffffh`, allocates the returned largest block, derives `TrnSeg` at its top, performs `STD; REP MOVSB; CLD` from `RESGROUP:TranStart` through `TRANGROUP:TranSpaceEnd`, then calls DOS `DEALLOC`. |
| MCB service implementation | `doskrnl/dos/alloc.asm:$ALLOC,$SETBLOCK,$DEALLOC`; `msinit.asm` | NTDOS creates and scans the arena from `arena_head`, checks MCB signatures/ownership, coalesces and splits blocks, reports largest available paragraphs on allocation failure, and marks a freed MCB owner zero. These are guest RAM semantics. |
| Final allocation and relocation | `cmd/command/command2.asm:LodCom..LodCom1` | COMMAND again queries and allocates the largest block. When its segment differs from `TrnSeg`, it copies the whole transient forward or backward, then stores the new `TrnSeg`. Low memory reaches `BadMemErr`. |
| Integrity/reload | `command2.asm:ChkSum,LoadCom,ReadCom,WrongCom` | Only checksum mismatch opens COMSPEC, seeks `TranStart`, reads the transient to `TrnSeg:100h`, closes it and retries/presents the original failure path. This is the only downstream point that may use the existing DEM file lifecycle. |
| Return to code | `command2.asm:HavCom,TJmp` | COMMAND copies `TranVars` to `TrnSeg:HeadCall`, updates the resident PDB block length, then far-jumps through `Trans`. |

## ABI and Ownership Boundary

All allocation, MCB headers, `TrnSeg`, string moves, checksum state and far
transfer stay in the immutable guest COMMAND/NTDOS images and ordinary guest
RAM.  `bx-core` owns instruction/interrupt mechanics and RAM; `bx-mantle`
owns their selected machine lifecycle.  `bx-vdm` may only provide already
reached host services through checked transactions.  It may not implement
INT 21 allocation, synthesize an MCB, seed transient bytes, or choose a
transient segment.

The existing Direct/Readonly file provider is relevant only if original
COMMAND has actually reached `LoadCom`; neither P17 nor P18 proves that
branch.  No host capability decision or BOP selector can therefore repair the
present boundary.

## Four-Rung Recovery Ledger

| Rung | Disposition |
| --- | --- |
| Original source reuse | Required and already active: the exact historical-toolchain COMMAND.COM and NTDOS.SYS images execute the original package. Their 16-bit guest translation units cannot be linked into the MSVC x64 process ABI. |
| Smallest adapter/shim | No behavioral shim is admissible. The only potentially valid follow-on is a selector-blind, fixed-width machine observation of already executed interrupt/control facts; it must not supply allocation or transient semantics. |
| External-code intrusion | Not selected. Bochs has an existing CPU interrupt path but no admitted general observation seam for this owner package. Any addition requires its own registered, default-off bx-core/bx-mantle exception and whole-package fixture. |
| Newly authored behavior | Rejected: an adapter allocator, MCB model, transient loader, synthetic copy, or host-process surrogate would replace original guest behavior. |

## Current Diagnostic Gap and Follow-On Constraint

P12 already excludes the generic reverse-copy/far-pointer mechanics group.
The remaining unknown is which original guest stage first diverges: allocation
failure, MCB mutation, initial copy, final relocation, checksum/reload, or
transfer.  Existing traces record BOP and scalar CS transitions, but do not
record the guest INT 21 allocation lifecycle.  `src/bx-vdm/bx_ntvdm_startup_machine_interrupt_v1.c` is not reusable: it recognizes only BOP selectors `02h` and `06h` and performs their registered machine actions.  The only `soft_int.cc` INT 21 reference is an optional `SHOW_EXIT_STATUS` debug log for `AH=4Ch`; it carries no typed record and is explicitly function-specific.  Neither is a generic interrupt observation seam.

If a follow-on is admitted, it must be a default-off bx-core/bx-mantle
mechanical event record for **all** real/V86 software interrupts or an equally
neutral lifecycle boundary.  The record may copy fixed-width CPU state already
at the machine boundary and must be selector-blind: it cannot recognize INT
21 function numbers, BOP, COMMAND, NTDOS symbols, guest addresses, memory
contents or opcodes.  The consumer may correlate the finite event sequence
with this map only after the entire observation profile is complete.

## Result

No repair is selected by P19.  The source-first route is exhausted through a
non-composable original guest owner; the only candidate next step is a
separately admitted selector-blind mechanical observation profile.  The
COMMAND/NTDOS package remains the sole owner domain, and P18 remains valid as
a narrow source-derived host-provider correction.
