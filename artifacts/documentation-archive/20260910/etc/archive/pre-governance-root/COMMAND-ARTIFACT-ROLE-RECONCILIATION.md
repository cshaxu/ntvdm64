# Command Artifact Role Reconciliation

Status: M23 artifact/source role reconciliation, 2026-08-08.

## Scope And Counting Correction

`docs/NT4-GUEST-ARTIFACT-CLOSURE.md` establishes a controlled payload of 38
artifacts: 27 source-built artifacts, seven static companion/data inputs, and
four explicitly prebuilt OpenNT `binplace` inputs. Five of the 38 are WOW16
device drivers. This is the direct DOS payload and declared WOW16 driver
scope, not a claim that the whole MVDM source tree is rebuilt.

The separate 75-entry MVDM inventory is a build-descriptor census. It includes
host components, tools, samples, and broader WOW runtime families. It must not
be used as a count of unbuilt first-command DOS files.

This record classifies the 38 controlled artifacts against the M22 startup
route. It does not claim that the four historical prebuilt inputs are
source-rebuilt, nor does it convert any artifact into a repository-distributed
runtime payload.

## First Command Cut

The following roles are mandatory before an owned DOS command can be attempted:

| Role | Fixed artifact / input | Evidence | M23 disposition |
| --- | --- | --- | --- |
| NTIO bootstrap image | `NTIO.SYS` | M15-M17 | hard BYOB role; source-built evidence exists in `ntio-tools16-opennt-v1` |
| NTDOS runtime image | `NTDOS.SYS` | M16-M21 | hard BYOB role; source-built evidence exists in `ntdos-tools16-historical-v1` |
| permanent command interpreter | `COMMAND.COM` | M22 `sysinit1.asm:1517-1620` | hard BYOB role; source-built evidence exists in `command-tools16-opennt-v1` |
| selected program/data | user-selected owned DOS command plus its declared files | M22 `SVC_CMDGETNEXTCMD` then DOS open/EXEC | hard profile input; it is deliberately not substituted by an arbitrary historical utility |

The runner additionally needs three **declared non-artifact inputs**:

- a contained namespace with the above roles at their profile paths;
- a bounded environment, first-command record, and standard-stream policy;
- a configuration/AUTOEXEC policy. These may materialize safe session-local
  virtual files, but must not consult a host system directory or user startup
  files.

The M22 source record proves `COMMAND.COM` is loaded with DOS `INT 21h`
open/seek/EXEC operations. Therefore the hard-role list is not enough by
itself: it creates a requirement for a later contained filesystem contract,
not for FDC/HDC controller emulation.

## Policy-Controlled Artifacts

The following controlled DOS artifacts may be admitted only when a profile
explicitly selects the corresponding CONFIG/AUTOEXEC/command behavior:

| Artifact(s) | Admission condition | First-cut status |
| --- | --- | --- |
| `COUNTRY.SYS`, `NLSFUNC.EXE` | `COUNTRY=` or NLS profile policy | deferred |
| `HIMEM.SYS` | `DEVICE=` / `DOS=HIGH` policy | deferred; implies later HMA/XMS evidence |
| `ANSI.SYS`, `KEYBOARD.SYS`, `KB16.COM` | device/keyboard policy | deferred |
| `REDIR.EXE` | explicit redirector/network policy | deferred |
| `SHARE.EXE`, `FASTOPEN.EXE`, `SETVER.EXE`, `APPEND.EXE` | explicitly invoked utility or configuration policy | deferred |
| `GRAPHICS.COM`, `GRAPHICS.PRO` | graphics policy | deferred |

An empty or absent session-local CONFIG/AUTOEXEC policy is a legitimate first
cut only if it follows the original service-to-DOS-open sequence documented in
M22. It must reject `SHELL=`, `DEVICE=`, `INSTALL=`, and `DOS=HIGH` rather
than silently discovering extra dependencies.

## Deferred Command And Development Artifacts

`DEBUG.EXE`, `EDLIN.EXE`, `EXE2BIN.EXE`, `MEM.EXE`, `LOADFIX.COM`,
`DOSX.EXE`, `QUIT.COM`, `BACKUP.EXE`, `QBASIC.EXE`, `EDIT.COM`, `MSHERC.COM`,
and the static help/demo files are controlled historical payload artifacts,
but none is a prerequisite for booting the permanent command interpreter or
injecting one owned DOS command. They become inputs only when a selected smoke
program or later compatibility profile names them.

`DOSX.EXE` specifically is important for a later DPMI/protected-mode profile,
but its presence must not create a premature requirement for DPMI, V86, or
host-native process handoff in the conventional first-command cut.

## WOW16 Driver Separation

`MOUSE.DRV`, `VGA.DRV`, `KEYBOARD.DRV`, `COMM.DRV`, and `SOUND.DRV` are
independently rebuilt, controlled guest-adjacent artifacts. They are not DOS
system startup roles and are excluded from the first command profile. No
interactive display, mouse, serial, or sound device can be inferred from their
successful historical build.

## Artifact Gaps And Result

Within the declared direct DOS payload scope, there is no remaining
source-build gap for the hard bootstrap roles: `NTIO.SYS`, `NTDOS.SYS`, and
`COMMAND.COM` each have controlled source-build evidence. The selected owned
command is intentionally profile-owned and remains to be supplied as a future
smoke fixture.

The remaining gap is runtime evidence, not another guest compiler campaign:
the project has not yet provided an executable original-chain or core-backed
implementation of the contained namespace, command-service bridge, streams,
DOS file/process semantics, or execution backend. The next task must derive
those contracts from the actual historical callers before implementation.
