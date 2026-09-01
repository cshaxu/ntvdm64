# M0 T336 S1 — post-`cmdSetInfo` source/cohort admission

## Question

What original source-owned execution cohort begins when the fixed x86 runtime
returns from COMMAND BOP `54:05` (`SVC_CMDSETINFO`), and what is the first
unproven owner edge before any repair may be selected?

## Inputs and procedure

The review used the unchanged T335 fixed-container observation, selected
original sources, and the current paired formal CCPU40 product manifests.
The source walk was:

1. `softpc.new/host/src/nt_bop.c:MS_bop_4` to establish the original COMMAND
   service-byte return rule;
2. `dos/command/cmddisp.c:CmdDispatch` and `cmdmisc.c:cmdSetInfo` to establish
   service `05` ownership and the current guest-span bindings;
3. `mvdm-guest/dos/v86/doskrnl/dos/msinit.asm:346-386` to establish the first
   guest instructions after the BOP;
4. CCPU40 register/SAS definitions and the source-wide consumers of the two
   retained SCS aliases; and
5. one unchanged, console-owning, non-debug x86 observation with
   `MVDM_EXCEPTION_REPORT_PATH` set solely to determine whether the existing
   timer-thread filter saw the final process exception.

The complete per-contract result is in the adjacent
[`ledger`](../operations/m0-t336-s1-post-cmdsetinfo-cohort-ledger.tsv).

## Observations

- Original `MS_bop_4` reads one service byte from `CS:IP`, calls
  `CmdDispatch`, then executes `setIP(getIP() + 1)`.  It therefore resumes at
  the instruction following the COMMAND service byte; it does not transfer to
  a loader, an adapter-owned continuation, or another BOP.
- Original `CmdDispatch` maps service `05` to `cmdSetInfo`.
- The original NTDOS continuation immediately restores `BX`, `CX` and `DX`.
  It then saves the BIOS exchange `DI:BX` pointer, initializes DOSDATA and
  low-interrupt segment fields, saves the caller's `SS:SP`, and loads
  `SS:SP = DOSDATA:dskstack`.  There is no BOP between `54:05` and this stack
  transition.
- The current `cmdSetInfo` correctly replaces the original `pSCSInfo` /
  `pSCS_ToSync` direct alias with the bounded SCS mapping helper.  Its retained
  `pIsDosBinary` and `pFDAccess` aliases still use `GetVDMAddr`; original
  consumers occur in DEM and SoftPC disk/floppy code.  This is a shared,
  later-use mapping-lifetime issue, not evidence that `cmdSetInfo` failed the
  next guest instruction.
- The unchanged fixed container again ended with `0xC0000005`.  No
  `exception-report.txt` was emitted, so the existing
  `VdmUnhandledExceptionFilter` instrumentation did not receive this process
  exception.  That filter belongs to the original timer path and cannot be
  used to identify the CPU40 fault location.

## Disposition

The first **unproven owner edge** is CCPU40/SAS execution immediately after
the original BOP return, with the first high-risk source contract being the
guest `SS:SP` disk-stack transition.  This is deliberately not asserted to be
the faulting instruction: the present evidence cannot distinguish the three
register restores, BIOS/DOSDATA writes, segment load, stack load, or a later
instruction.

The immediate successor is therefore a bounded CCPU40/SAS observation and
source-contract packet.  It must preserve the original BOP and NTDOS paths,
observe the original instruction/stack state without changing it, and decide
whether the first failing edge is CCPU40 execution, SAS span state, or an
already-required BIOS exchange prerequisite.  It must not add a BOP provider,
guest loader, synthetic result, secondary mapper, alternate executor or Bochs
route.

## Confidence and limits

Confidence is high for the static transition and owner mapping because both
sides are selected original source and the fixed observation independently
reaches `54:05`.  Confidence is intentionally low for exact fault attribution:
the observed `0xC0000005` is process-level only and the timer-thread filter is
not on that exception path.  No claim is made for continuous DOS execution,
child-program execution, a new guest ingress, or x64 runtime execution.
