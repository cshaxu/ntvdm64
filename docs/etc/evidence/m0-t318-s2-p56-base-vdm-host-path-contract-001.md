# M0 T318 S2 P56 — Base VDM host-path contract closure

## Scope

This record closes one source/link owner cluster: the app-owned installed-media
path supplied to the original Base VDM/COMMAND contract.  It does not claim
NTIO, NTDOS, EXEC, parent return, or a successful runtime observation.

## Original contract and finding

The selected original COMMAND caller, `cmdmisc.c::cmdGetNextCmd`, sets
`VDMINFO.CurDirectoryLen` to `MAX_PATH + 1` and later applies the returned
host current directory with `SetCurrentDirectory`.  BaseClient also treats an
application path as a host `MAX_PATH` value; these are distinct from the
guest-visible `MAXIMUM_VDM_CURRENT_DIR` and DOS path fields.

The current fixed package root is 76 bytes long.  The local Base VDM record
had incorrectly rejected that valid host path as a 64-byte DOS directory, and
then, after that carrier was widened, the original COMMAND receiving buffer
was only `MAXIMUM_VDM_CURRENT_DIR + 4` despite advertising `MAX_PATH + 1`.
That source mismatch would overwrite the COMMAND stack when the local broker
returned the installed host path.

## Recovery

- The local Base VDM carrier now uses `MAX_PATH` for `application` and
  `MAXIMUM_VDM_PATH_STRING` for `current_directory`; its publish and validity
  checks use the same capacities.
- The app declaration uses the matching host carriers while preserving the
  original command, environment and current-directory byte contracts.
- Original `cmdGetNextCmd` now allocates `MAX_PATH + 1` for the field for
  which it already requests that capacity.  This is registered as
  `MVDM-HOST-DIV-160`; no command/guest/host-directory behavior was changed.
- `session::termination_escape` is explicitly first in the session ABI so its
  required x64 alignment is not implicit compiler padding.

The focused existing Base VDM fixture now composes the real fixed-container
DOS root and `COMMAND.COM` path through `app_launch_declaration_publish`.
It passed after the carrier repair under both x86 and x64.

## Verification

After the final source changes, formal selected CPU40 final-link rows passed:

- x86: rebuilt `base_vdm_local`, `cmdmisc`, their affected libraries, and
  `original-softpc-process.exe`;
- x64: rebuilt the same affected members and linked
  `original-softpc-process.exe`.

The prior fixed-container p56--p59 reports remain retained research evidence:
p56--p58 show the successive app-side rejection removals; p59 exits with
`0xC0000005` after publishing the declared host path.  The report cannot prove
which original caller consumed that path, so the receiving-buffer mismatch is
a source-proven safety repair, not a runtime-attributed crash root.

The next valid runtime action is exactly one newly linked x86 product in the
unchanged `runtime-r33-config-system-media` container, subject to the fixed
observation policy.
