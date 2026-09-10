# T95 S6 DEM open-mode and path admission audit 001

## Question

What exact `50:12` (`SVC_DEMOPEN`) input subset can a non-invasive,
read-only adapter projection admit without exporting a host handle, ambient
current directory, write capability, or historical named-pipe behavior?

## Inputs

- OpenNT `base/mvdm/dos/dem/demfile.c:117-327` (`demOpen`).
- OpenNT `base/mvdm/dos/dem/dosdef.h:31-47` and guest
  `base/mvdm/dos/v86/inc/filemode.inc:25-41`.
- NTDOS caller and SFT storage at
  `base/mvdm/dos/v86/doskrnl/dos/file.asm:145-220` and
  `base/mvdm/dos/v86/inc/sf.inc:55-65`.
- Existing bounded copied-input and namespace boundaries in
  `t95-s5-gather-read-admission.md` and
  `t95-s4-host-namespace-projection.md`.

## Procedure

Read the service entry/exit contract together with the immediate NTDOS caller,
then compare each historical path/mode behavior with the declared CLI-owned
read-only namespace boundary. No runtime call, host open, or Bochs change was
performed.

## Observations

1. `demOpen` takes a NUL-terminated canonical guest path from `DS:SI`, mode
   from `BL`, and an EA marker from `AL`. A nonzero `AL` is not implemented by
   the historical owner. On success it returns a 32-bit handle as `AX:BP`, a
   32-bit size as `BX:CX`, and `DX=0` for a regular file. NTDOS writes that
   handle directly into its four-byte `SFT_NTHandle` field.
2. The historical owner accepts DOS access values `0` read, `1` write, `2`
   read/write, and `3` EXEC-open. Only access `0` is changed to
   `GENERIC_READ`; only access `1` becomes `GENERIC_WRITE`; accesses `2` and
   `3` retain the default read/write request. Thus `EXEC_OPEN` is not
   automatically compatible with a read-only host handle.
3. Sharing codes have source-defined meanings, but the C owner applies the
   `0x70` mask while the 16-bit include also declares `0x80` no-inherit.
   An adapter must preserve the raw mode in its request record and make an
   explicit rule; it must not use a guessed mask or infer semantics from host
   file sharing defaults.
4. A slash-normalization branch, named-pipe recognition, VDMREDIR loading,
   overlapped pipe state, and native `CreateFile`/`HANDLE` lifetime all belong
   to the historical host product. They are outside the CLI projection. The
   existing namespace can only resolve explicit admitted drive roots and
   DOS-relative 8.3 components; it has no ambient-drive or pipe owner.
5. The existing generic gather read can copy a 128-byte staged `OpenBuf`
   safely, but it must reject an absent in-range NUL. That copied input is
   enough for a first explicit-drive path decoder; no adapter component may
   follow `DS:SI` directly.
6. The retained OpenNT SoftPC host seam already establishes the historical
   native-status conversion direction: `nt_ntfun.c:19` defines
   `SETUPLASTERROR(status)` as `SetLastError(RtlNtStatusToDosError(status))`.
   `demClientError` then reports that Win32/DOS error through `AX` except for
   its separately handled hard-error class. The modern compatibility header
   exposes the same `RtlNtStatusToDosError` declaration. Therefore a future
   native-relative open/read seam must convert its own `NTSTATUS` once through
   this source-proven function and retain the resulting 16-bit value in the
   adapter outcome record; it must not leak raw `NTSTATUS` to guest registers
   or invent an ad-hoc table.

## First-profile contract consequences

The future `DEMOPEN` decoder may admit only all of the following at once:

- `AL=0`;
- a NUL-terminated, bounded copied path in explicit rooted form such as
  `C:\\DIR\\FILE.EXT`, normalized into the already admitted namespace model;
- a regular, non-reparse entry under the immutable CLI-selected drive root;
- a source-explicit read-only mode subset; and
- an adapter-issued nonzero 32-bit handle token stored in `AX:BP`, never an
  OS handle.

`OPEN_FOR_WRITE`, `OPEN_FOR_BOTH`, named pipes, devices, UNC/drive-relative
and relative paths, extended attributes, mutation, and unclosed mode bits are
rejected before an adapter host open. `EXEC_OPEN=3` is **not admitted yet**:
the historical owner asks for a read/write handle, while silently changing it
to read-only would be an intentional CLI projection policy rather than
historical-equivalent behavior. It requires a reached caller and an explicit
design decision.

Success must return the original register shape with `DX=0`, but the size is
limited to the low 32-bit source-shaped field already used by NTDOS. The token
must be private to the adapter session, invalid after close/session teardown,
and rejected on stale or forged use. This does not make SFT ownership an
adapter concern: NTDOS remains responsible for its SFT/JFN bookkeeping.

## Follow-up

Keep `50:12` listener-only until the paired read/close request and error
records specify: mode representation, token allocation/exhaustion, path
normalization, regular-file metadata, and every ordinary error mapping.
Do not solve the `EXEC_OPEN` ambiguity by broadening host write access. The
bulk-result transport and the `50:33` hard-error/retry decision remain
separate prerequisites from this input admission.
