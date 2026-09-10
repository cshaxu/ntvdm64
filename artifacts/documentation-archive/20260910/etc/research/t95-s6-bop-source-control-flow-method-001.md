# T95 S6 BOP source-control-flow method 001

## Question

How should a reached `C4 C4 selector service` observation be used to decide
whether the next missing element is a Bochs mechanical precondition, an
adapter contract, or an unavailable host capability?

## Inputs

- The default-off, generic BOP listener observation rooted at
  `artifacts/analysis/t95-s6-r30-cli-root-release-023-20260811-001`.
- OpenNT selector definitions in `base/mvdm/inc/cmdsvc.h` and `dossvc.h`.
- Guest caller `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm` and historical
  owner `base/mvdm/dos/command/cmdconf.c`.
- The existing CONFIG.SYS capability audit
  `t95-s6-cmd-getconfigsys-capability-audit-001.md`.

## Procedure

For every reached BOP, work in this order:

1. Identify the selector and service only from the copied instruction window.
2. Locate its OpenNT definition, historical dispatch owner, and immediate
   guest caller.
3. Read the caller after the BOP to establish the required result: registers,
   flags, guest writes, later DOS operation, or non-returning failure.
4. Classify every prerequisite by owner: Bochs mechanics, adapter transport,
   source-derived host capability, or guest DOS flow.
5. Admit implementation only when the complete original contract and its
   failure behavior are bounded.  Otherwise keep the listener-only result.

## Observation: `54:0C`

`sysinit1.asm:1158-1170` sets `DS:DX` to the guest `config` descriptor,
issues `CMDSVC SVC_GETCONFIGSYS`, restores `DS`, and immediately calls the
existing `doconf` parser.  `cmdconf.c:51-82` shows that the historical owner
first materializes an expanded temporary CONFIG.SYS file, then writes its OEM
pathname (maximum 64 bytes) to that guest buffer.  Its documented failure
does not return a DOS `CF/AX` pair: it terminates the VDM.

Therefore the observed service is not a CPU instruction failure and not a
request to copy CONFIG.SYS bytes directly into RAM.  It is a request for a
reachable guest filename whose subsequent ordinary NTDOS open/read path sees
the corresponding materialized file.  The missing closure is an adapter-owned
read-only namespace plus paired configuration materialization and cleanup;
Bochs is not missing a CONFIG.SYS device or special instruction behavior.

The later `54:0D` AUTOEXEC request has the same shape.  It must be admitted
as the same paired capability, never as an unrelated successful BOP shim.

## Result

The listener remains correct for `54:0C` and `54:0D`: log the exact call and
fall through until the paired host-capability contract is admitted.  The same
source-first method applies to later BOPs, including `DEMREAD`: source may
prove a need for a generic bounded RAM result transfer, but it cannot justify
placing file, handle, DOS, IRQ, or BOP semantics in Bochs.

## Follow-up

Use this record together with the BOP catalogue for the next reached service.
Only a source-proven Bochs mechanical need may enter the exception register;
all service behavior stays in the adapter host-service plane.
