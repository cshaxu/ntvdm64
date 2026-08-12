# T95 S6 DOS read-handle closure audit 001

## Question

Which BOP contracts make the filename returned by COMMAND `54:0C` and `54:0D`
reachable to NTDOS/COMMAND, and can a configuration-file handler be admitted
before those contracts?

## Inputs

- OpenNT `base/mvdm/inc/dossvc.h:88-110` and
  `base/mvdm/dos/dem/demdisp.c:98-123`.
- NTDOS open path `base/mvdm/dos/v86/doskrnl/dos/file.asm:205-220`.
- NTDOS read and close paths
  `base/mvdm/dos/v86/doskrnl/dos/handle.asm:132-145,458-474`.
- Historical host owners `base/mvdm/dos/dem/demfile.c:117-280` and
  `base/mvdm/dos/dem/demhndl.c:28-210`.
- The configuration callers already recorded in
  `t95-s6-cmd-getconfigsys-capability-audit-001.md`.

## Observations

| BOP | Historical owner | Guest contract role |
| --- | --- | --- |
| `50:00` `SVC_DEMCHGFILEPTR` | `demChgFilePtr` | NTDOS supplies the split handle, signed 32-bit displacement `CX:DX`, and method `BL` (`0` begin, `1` current, `2` end); success returns the resulting 32-bit position in `DX:AX` with CF clear. |
| `50:12` `SVC_DEMOPEN` | `demOpen` | NTDOS supplies canonical path at `DS:SI`, mode in `BL` and EA marker in `AL`; success returns a 32-bit opaque host handle split `AX:BP`, 32-bit size `BX:CX`, pipe flag `DX`, and CF clear. NTDOS stores the handle in its SFT. |
| `50:16` `SVC_DEMREAD` | `demRead` | NTDOS supplies that split handle, byte count `CX`, guest destination `DS:DX`, current offset `BX:SI`, and seek state in ZF; success writes returned bytes, sets `AX=bytes read`, and clears CF. |
| `50:02` `SVC_DEMCLOSE` | `demClose` | NTDOS supplies the split handle plus optional final offset; success clears CF and releases the host-owned handle. |

`HRDSVC` is a DEM `SVC` wrapper that preserves hard-error handling around
these calls; it does not change their BOP identity.  COMMAND's `54:0D` caller
then uses normal DOS OPEN, and `doconf` parses the `54:0C` filename through
the same DOS filesystem route.  Thus both config boot files share this read
handle sequence.

The actual `doconf` boot path proves that this is not a theoretical future
extension: after opening CONFIG.SYS it seeks to end to discover the size,
seeks back to beginning, then reads and closes (`sysconf.asm:537-613`). The
read-only capability family is therefore **Open/Seek/Read/Close**, with
`50:00`, `50:12`, `50:16`, and `50:02` admitted or rejected together. Its
first configuration use needs methods 0 and 2; the source also defines method
1, so a complete family contract must decide it rather than accidentally
leaving it as an unhandled special case.

There is one important split in that sequence. NTDOS handles method `0`
(absolute seek) locally by updating `SFT_Position`; it invokes `50:00` only
for method `2` (seek from end). A changed local position sets `sf_nt_seek`.
`50:16` then receives ZF clear and its historical owner seeks the native
handle to the supplied `BX:SI` position before `ReadFile`; with ZF set it
continues at the native handle's existing position (`handle.asm:666-716`,
`demhndl.c:188-205`). Thus the adapter-private token record must retain a
private sequential offset, and `DEMREAD` must apply the source ZF rule. It
would be incorrect to treat a guest SFT update as a host seek, or to ignore a
later read's explicit offset after an NTDOS-local seek.

The historical owners pass native Windows `HANDLE` values through the
16-bit SFT as two words and directly obtain guest pointers with `GetVDMAddr`.
That representation is prohibited by the present fixed-width adapter ABI: a
host handle must remain adapter-local and guest RAM must be changed only via
a checked, bounded transaction.

## Interpretation

The next host capability is a **single read-only DOS handle projection**,
not an independent configuration BOP handler and not the previously designed
search-only capability.  Its eventual source-derived contract must provide:

1. bounded copied DOS-path input rather than an adapter memory dereference;
2. validation against the CLI-admitted namespace and a private opaque handle
   table, never a native `HANDLE` or ambient path in the SFT;
3. read-only open modes and precise source-shaped size/result/error fields;
4. bounded atomic guest writes for `DEMREAD`, with no partial result;
5. private seek/offset/close lifecycle and explicit stale-handle failure; and
6. preservation of NTDOS's own SFT, DOS error and hard-error control flow.

This is not a Bochs concern.  It also cannot be merged into the current
`54:04` root result or activated as an ad-hoc `50:12` implementation.  Before
runtime code, it needs a complete paired Open/Read/Close ABI and error table,
plus a defined relation to the existing search namespace/session model.

## Follow-up

Keep `54:0C`, `54:0D`, `50:00`, `50:12`, `50:16`, and `50:02` listener-only.  The next
admission proposal should close the read-only handle family as one capability,
then decide whether CLI-selected configuration content is an admitted source
within that same projection.  Mutation, pipes, named devices, PIF processing,
extended attributes, general file write, and Bochs changes remain out of
scope.

## Error and hard-error finding

`demerror.c:91-181` gives the common failure contract. Ordinary Win32/DOS
errors set `AX` and CF. Disk-class errors from `ERROR_WRITE_PROTECT` through
`ERROR_GEN_FAILURE`, plus `ERROR_WRONG_DISK`, instead set the registered VHE
packet's INT 24 flag, drive and translated error before returning CF set. The
existing earlier `50:1B` registration evidence captures the VHE location, but
it does not itself authorize a later service to write it.

Therefore the read-handle capability must not flatten every failure to
`CF=1, AX=error`: it needs an explicit ordinary-error / hard-error outcome
record and an atomic VHE guest-write only after the complete source mapping is
closed. Named pipes and `ERROR_BROKEN_PIPE` are excluded from the first
read-only profile; the historical special EOF success path is not inherited
without a reached caller.

The VHE packet is exactly three bytes in source order: `fbInt24`, translated
INT 24 error code, then drive number (`dossvc.h:13-17`; mirrored by
`DOSSVC.INC:2-6`).  This makes its write bounded, but not optional: DOS checks
the first byte and may issue `50:33` (`SVC_DEMRETRY`) after the hard-error
dialogue.  `demRetry` restores the original DEM register frame and re-enters
the saved service (`demerror.c:41-89,359-397`).  A first read-only projection
therefore has only two coherent choices: implement a private retryable
operation record together with `50:33`, or reject disk-class failures before
they become guest-visible.  Returning a VHE hard error while leaving `50:33`
listener-only would create a broken source-visible control-flow branch.

## Token and close-lifecycle finding

`demClose` gives zero its own source-defined meaning: an `AX:BP` value of zero
returns CF clear without touching a native handle (`demhndl.c:51-56`). The
adapter must reserve token zero for exactly that no-op behavior and never
allocate it. For a nonzero token, a `CX:DX` other than `ffff:ffff` requests a
seek-to-absolute-position before close; otherwise close releases the native
resource. NTDOS supplies that position only when `sf_nt_seek` is set
(`handle.asm:122-143`).

An unknown, exhausted, already-closed, or forged nonzero token has no native
counterpart to call. The source-shaped adapter outcome is therefore ordinary
`ERROR_INVALID_HANDLE` (`6`), CF set, with no VHE mutation; the NTDOS error
table explicitly lists that error for Close, Read and LSeek
(`mstable.asm:108-112`). A token must consequently be session-private,
nonzero, valid at most once for Close, and never reissued while an old SFT can
still present its value. Reuse requires an unambiguous generation component
inside the 32-bit value, or exhaustion must fail with the source-shaped
too-many-open-files outcome. A native handle remains only adapter-local and
is released on session teardown even if guest Close never arrives.

## Seek contract finding

`demChgFilePtr` treats `CX:DX` as one signed 32-bit displacement and accepts
only the source-aligned method values `BL=0` (begin), `1` (current), and `2`
(end); its compile-time assertion records that the historical DOS and Win32
method values are equal (`demhndl.c:342-388`). It returns the resulting
unsigned 32-bit position as `DX:AX`, with CF clear. The adapter token record
must implement those three methods over its private native file state and
update its sequential offset only after the seek succeeds. Invalid method,
or a malformed direct BOP window is not a request the source caller issues:
NTDOS rejects methods above `2` before its file path. It must remain
listener/pass-through rather than being assigned an invented adapter result.
Invalid token, negative/out-of-range result, and native seek failure are
ordinary CF/AX outcomes through the established status conversion, not a
guest-memory write or a Bochs concern.

Although the reached CONFIG.SYS trace requires only seek-from-end, the adapter
service contract must retain all three defined methods: selector `50:00` is a
single historical service, and partially implementing it by method would turn
ordinary later guest input into an unexplained #UD path. This does not admit
write, pipes, devices, or a generic filesystem; it completes the bounded
offset semantics of an already-open read-only token.
