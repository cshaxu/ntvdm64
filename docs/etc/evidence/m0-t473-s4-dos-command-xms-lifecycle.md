# M0 T473 S4 — DOS/COMMAND/XMS execution and retirement closure

## Decision

The selected original DOS/COMMAND/XMS route remains complete at the modern
host boundary.  This review found no autonomous command parser, COMSPEC
policy, XMS allocator, descriptor parser or guest byte store to remove; nor
an omitted original user-mode owner that can be safely reintroduced.  No
production source, build selection, package, installed image or guest media
changes.

## COMMAND chain

1. Original `cmddisp.c` dispatches original `cmdExec` or `cmdExecComspec32`.
2. Original `cmdExec` decides whether its original AH branch constructs
   `COMSPEC /c <tail>`; `cmdExecComspec32` selects interactive COMSPEC.  The
   project never classifies a shell token or command name.
3. `mvdm_command_native_child` snapshots only the bounded original guest
   command/environment/standard-handle inputs before `cmdExec32` starts its
   detached host process thread.  The snapshot is bound to one active session
   epoch and is invalid outside that session/thread.
4. Original `cmdCreateProcess` performs its original process/count/reentry
   sequence.  It calls `mvdm_command_native_child_finish` after CreateProcess
   consumes the copied values and before the child can cause a later command
   request.  Error and pre-create exits call the same abort/finish clear.
5. Session teardown owns the allocation's sole finalizer.  It clears the
   session pointer before zero/free, so child failure, nested command entry,
   worker retirement and epoch reuse cannot leave a valid alternate command
   state.

`command_process_compat` supplies the admissible public CreateProcess/
Console transport only.  It reads the mode already selected by original
COMMAND and delegates to `run16`; it never parses COMSPEC, redirects a named
command, or sets original return flags.

## XMS chain

The selected original `xms.486/xmsblock.c::xmsMoveBlock` reads its original
SS:BP descriptor fields, applies the original word-to-byte conversion and
uses original **forward** overwrite semantics.  The bounded twelve-byte copy
only replaces the unavailable durable `GetVDMAddr` alias.  The transfer then
goes directly to selected CCPU SAS through
`mvdm_softpc_guest_memory_copy_forward`; the adapter owns range/lease failure
only, not copy direction, allocation or XMS status policy.  Original XMS
commit/decommit and allocator sources remain selected and authoritative.

## Failure/retirement assessment

| Case | Owner and result |
| --- | --- |
| Invalid guest command/standard/environment location | bounded capture rejects before original `cmdExec32`; original caller receives its existing error result. |
| Native child CreateProcess failure | original child thread records the error; snapshot is cleared before it returns to original VDM reentry accounting. |
| Nested COMMAND/native request | prior capture is cleared as soon as CreateProcess has consumed it; the new capture is not blocked by a stale singleton. |
| Worker/session teardown | session teardown callback clears the epoch-owned snapshot once; no host pointer crosses the session boundary. |
| XMS descriptor overflow or invalid numeric span | original move cannot be performed; adapter requests the existing cancellation path rather than continuing with a raw alias. |

## Evidence and result

The exact sources remain selected by `New-T310OriginalSoftpcNinja.ps1`:
original COMMAND/XMS libraries plus the two small command boundary objects.
The retained T452 whole-acceptance record documents a refreshed
`original-external-memory-test` pass for original allocation/free/SAS/EMS/DIB
behavior.  Existing COMMAND guest-state/native-child fixtures cover scalar
epoch validation, bounded inputs and finish/abort paths.  This packet changed
none of those inputs; it does not relabel historic proof as a fresh rerun.

No source change is justified.  Replacing either boundary with raw pointers
would violate the active session lifetime; importing the x86 raw-alias XMS
path would restore an unavailable NT kernel address model.  S5 therefore
continues with the independent SoftPC device cohort.
