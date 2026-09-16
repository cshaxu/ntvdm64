# Component-boundary normalization and legacy broker retirement

## Status and objective

This is an unadmitted candidate produced by M0 T417 S5.  It receives a T
number only on admission in `CURRENT.md` and makes no source change before
then.

Normalize project-component ownership without changing the original MVDM or
OpenNT provider semantics, while deleting the proved-unused legacy in-process
broker state plane.  The desired result is fewer autonomous project mechanisms,
not a cosmetic directory reshuffle.

The governing invariant is strict: `src/mvdm` and `src/opennt-host` may not
receive a new file.  Existing mirror files may take only a minimal registered
owner-local divergence.  Modern RPC, Console, ABI, guest-memory and process
mechanics remain in a named adapter/ABI component.

## Frozen audit findings

T417 S5 identifies the following execution groups:

1. **B1 — legacy broker retirement.** `broker.c/.h`, `wire.c/.h` and
   `base_vdm_record.c/.h` total 387 lines and have no direct production source
   caller.  They are presently carried only by `broker.lib` link lists.  The
   actual product RPC route is `basesrv_entry.c` → `OpenNtBaseService*` →
   original `srvvdm.c` through `adapter-opennt-host`; the retained broker
   transport files are separate, necessary IPC mechanics.
2. **B2 — BaseSrv adapter partition.** Partition the existing
   `adapter-opennt-host/basesrv` build groups by BaseClient local binding,
   BaseSrv local binding and RPC/handle facade.  Preserve exported spelling,
   ownership and failure rules; do not move record selection or command policy
   from `opennt-host`.
3. **B3 — session extension boundary.** Replace session's untyped
   `mvdm_command_native_child` slot only with a typed private extension whose
   teardown and nested-COMMAND lifetime are demonstrated.  Do not move the
   active Console text plane or guest-memory lease until their separate
   original-owner and runtime contracts are proved.
4. **B4 — adapter-family tightening.** Give the existing Win32 and SoftPC
   families build-local subgroups (Console/presentation, wait/thread,
   process/CRT; and CCPU/SAS/mapping/worker binding respectively).  Delete a
   wrapper only when an original selected provider is already linkable.

## Sequential work

### S1 — prove and retire the legacy broker archive

Freeze all `broker.lib` graph edges, exports and source references.  Link a
fresh x86 graph without the archive.  If every product and fixture link and
the map contains no retained legacy export, delete exactly the six frozen
files and their graph edges.  If any caller appears, retain the smallest
needed source and transfer it to the appropriate group instead of deleting
blindly.

### S2 — partition BaseClient/BaseSrv adapter build ownership

Split only existing adapter source/build groups and headers.  Record every
export with original caller, modern transport dependency, state owner and
cleanup owner.  No function body moves to `app`, `broker`, `session` or either
mirror merely because it uses an OpenNT name.

### S3 — typed native-child extension

Introduce the smallest private typed extension boundary for native-child
capture state, preserving the current session teardown order.  Prove the
asynchronous guest-copy, standard stream, error, cancellation and nested
COMMAND paths.  Stop if the extension requires an ABI/lifetime policy change.

### S4 — family tightening and closure

Use dependency-checked build groups for the Win32 and SoftPC adapters.  Remove
only proved duplicate wrappers, then run the integrated regression matrix and
report separate net deletion, relocation and retained-adapter figures.

## Acceptance

Closure requires a fresh x86 `/MT` three-program graph; link-map evidence that
the legacy broker archive is absent or an exact retained-caller reason; passing
CCPU/C-VID fixtures; BaseClient/BaseSrv RPC version, death and pending-command
tests; COMMAND/MEM, nested COMMAND, EDIT keyboard/mouse and EDIT-exit→MEM;
and a documentation/diff-governance pass.  It must report mirror diff,
overlay, adapter and autonomous-project-code changes separately.  No WOW16
completion claim is part of this package.
