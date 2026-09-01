# M0 T338 — Minimal DOS guest execution and controlled return

## Purpose

Execute one declared immutable DOS `.COM` or minimal `.EXE` through the selected
original CPU40 SoftPC, Base VDM, DEM/COMMAND and unified executable-relative
`mvdm` media root.  A successful delivery must show the original child exit
through the original NTDOS parent/COMMAND return contract; a source-defined
earliest prerequisite transfer is also a valid bounded exit.

## Current predecessor

- T336 attributes the present fixed-container stop to original
  `50:3D -> demExitVDM`, not to guest-media absence or a BOP leaf.
- T337 now stages immutable system media at `mvdm`, system files at
  `mvdm/system32`, and SoftPC firmware at `mvdm/softpc`.
- Original NTDOS `EXEC -> PSP/arena/JFN/environment -> parent restore ->
  54:0B` evidence remains the controlling return contract.

## S plan

### S1 — Original child/parent-return precondition contract

Audit the complete original source path from the declared command record and
`cmdSetInfo`/`demExitVDM` through the first NTDOS child load/EXEC edge and
back to the original parent/COMMAND result edge.  Classify every reached
precondition as direct original source, same-shaped existing adapter,
unavailable original boundary, or later owner.  Do not change production
behavior in S1.

### S2 — Fixed post-configuration continuation attribution

Use one unchanged fixed x86 container and the existing external observer to
identify the first source owner after successful original configuration-shell
construction.  If that owner is a finite source-shaped prerequisite, record
the smallest recovery candidate for S3; otherwise record its exact owner
transfer.  No invented BOP, guest loader, device success, or alternate
executor is permitted.

### S3 — Original configuration-error predicate and earliest recovery

First identify the exact original `cmdconf.c::ExpandConfigFiles` fatal
predicate reached by S2, without changing packaged guest media or bypassing
the original configuration service.  Recover only an S2-proven same-shaped
binding prerequisite, then run one fixed console-owning x86 container using
unchanged staged media and the declared DOS program.  Rebuild/link the
selected product under x86 and x64, then record original child/parent return
or the exact remaining original owner transfer.

## Exit

The package closes only with fixed-container proof of the declared DOS child
returning through the original parent/COMMAND contract, or one narrow,
source-defined earlier prerequisite transfer.  Both formal architecture rows
must compile/link; x64 link closure does not claim x64 guest execution.
